#include "quface.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>

#include "lib/logger.h"

using namespace suanzi;

SZ_RETCODE Quface::initialize(const std::string& device_file,
                              const std::string& model_file, int disType) {
  SZ_RETCODE ret = SZ_RETCODE_FAILED;
  disType_ = disType;
  ret = initialize_net(device_file);
  if (ret != SZ_RETCODE_OK) {
    SZ_LOG_ERROR("initialize mqtt failed, error_code = {}", ret);
    release();
    return ret;
  }

  ret = initialize_license();
  if (ret != SZ_RETCODE_OK) {
    SZ_LOG_ERROR("initialize license failed, error_code = {}", ret);
    release();
    return ret;
  }

  ret = initialize_dnn(model_file);
  if (ret != SZ_RETCODE_OK) {
    SZ_LOG_ERROR("initialize dnn failed, error_code = {}", ret);
    release();
    return ret;
  }

  ret = initialize_database();
  if (ret != SZ_RETCODE_OK) {
    SZ_LOG_ERROR("initialize database failed, error_code = {}", ret);
    release();
    return ret;
  }

  return ret;
}

void Quface::release() {
  if (network_ != NULL) SZ_NET_CTX_release(network_);

  if (license_ != NULL) SZ_LICENSE_CTX_release(license_);

  if (db_ != NULL) SZ_DATABASE_CTX_release(db_);

  if (dnn_ != NULL) SZ_FACE_CTX_release(dnn_);
}

SZ_RETCODE Quface::add_person(const std::string& name, SZ_INT32 face_id,
                              SZ_IMAGE_CTX* imageCtx) {
  DetectionResult face;
  if (SZ_RETCODE_OK != detector_->detect(imageCtx, face)) {
    return SZ_RETCODE_FAILED;
  }

  char person_info_data[kMaxPersonInfoLength];
  memset(person_info_data, 0, kMaxPersonInfoLength);

  nlohmann::json person_info = {{"name", name}};
  std::string person_info_json = person_info.dump();

  if (person_info_json.length() >= kMaxPersonInfoLength)
    SZ_LOG_WARN("assert person_info.length < kMaxPersonInfoLength failed");

  memcpy(person_info_data, person_info_json.c_str(),
         std::min((int)person_info_json.length(), kMaxPersonInfoLength - 1));

  SZ_FACE_FEATURE* feature;
  SZ_INT32 feature_length;
  if (SZ_RETCODE_OK !=
      extractor_->extract(imageCtx, &face.rect, &feature, &feature_length)) {
    return SZ_RETCODE_FAILED;
  }

  if (SZ_RETCODE_OK != SZ_DATABASE_add(db_, face_id, feature, person_info_data,
                                       kMaxPersonInfoLength)) {
    return SZ_RETCODE_FAILED;
  }
  return SZ_RETCODE_OK;
}

SZ_RETCODE Quface::get_avatar(SZ_INT32 face_id, SZ_IMAGE_CTX** imageCtx,
                              SZ_INT32* width, SZ_INT32* height) {
  return SZ_DATABASE_getAvatar(db_, face_id, imageCtx, width, height);
}

void Quface::handle(SZ_IMAGE_CTX* imageCtx) { detector_->post({imageCtx}); }

void Quface::run() {
  if (license_avaliable_ && network_avaliable_) SZ_NET_detach(network_);

  camera_ = new CameraReader(this);
  detector_->set_callback(camera_);
  extractor_->set_callback(camera_);

  Thread* thread_reader = new Thread(camera_);
  Thread* thread_detector = new Thread(detector_);
  Thread* thread_extractor = new Thread(extractor_);

  thread_reader->join();
}

SZ_RETCODE Quface::initialize_net(const std::string& device_file) {
  SZ_RETCODE ret = SZ_RETCODE_FAILED;

  std::ifstream device_fin;
  try {
    device_fin.open(device_file);
  } catch (...) {
    SZ_LOG_ERROR("cannot open {}", device_file);
    return ret;
  }

  nlohmann::json device_info;
  if (!device_fin.is_open()) {
    license_avaliable_ = false;
    SZ_LOG_ERROR("cannot open {}", device_file);
  } else {
    device_info = nlohmann::json::parse(device_fin);
    if (!device_info.contains("DeviceName") ||
        !device_info.contains("DeviceSecret") ||
        device_info["DeviceName"] == "" || device_info["DeviceSecret"] == "") {
      SZ_LOG_WARN("field DeviceName or DeviceSecret is missing in {}",
                  device_file);
      license_avaliable_ = false;
    } else {
      license_avaliable_ = true;
    }
    device_fin.close();
  }

  std::string product_key;
  std::string device_name;
  std::string device_secret;
  if (license_avaliable_) {
    product_key = device_info["ProductKey"];
    device_name = device_info["DeviceName"];
    device_secret = device_info["DeviceSecret"];
  } else {
    device_name = "";
    device_secret = "";
  }

  NetCreateOption opts = {
      .clientId = (char*)kClientID,
      .productKey = (char*)product_key.c_str(),
      .deviceName = (char*)device_name.c_str(),
      .deviceSecret = (char*)device_secret.c_str(),
      .storagePath = (char*)kLicenseStoragePath,
  };

  network_ = SZ_NET_CTX_create(opts);
  if (network_ == NULL) {
    SZ_LOG_ERROR("SZ_NET_CTX_create failed");
    return ret;
  }

  ret = SZ_NET_connect(network_);
  if (ret != SZ_RETCODE_OK) {
    SZ_LOG_WARN("SZ_NET_connect failed. network not avaliable");
    network_avaliable_ = false;
  } else {
    network_avaliable_ = true;
  }
  return SZ_RETCODE_OK;
}

SZ_RETCODE Quface::initialize_license() {
  license_ = SZ_LICENSE_CTX_create(network_);
  if (license_ == NULL) {
    SZ_LOG_ERROR("SZ_LICENSE_CTX_create failed");
    return SZ_RETCODE_FAILED;
  }
  SZ_RETCODE ret = SZ_LICENSE_auth(license_);
  if (ret != SZ_RETCODE_OK) {
    license_avaliable_ = false;
    SZ_LOG_ERROR("SZ_LICENSE_auth failed");
  } else {
    license_avaliable_ = true;
  }
  return SZ_RETCODE_OK;
}

SZ_RETCODE Quface::initialize_database() {
  db_ = SZ_DATABASE_CTX_create(kDbName);
  if (db_ == NULL) {
    SZ_LOG_ERROR("SZ_DATABASE_CTX_create failed");
    return SZ_RETCODE_FAILED;
  }
  db_sync_ = SZ_FACE_SERVER_CTX_create(network_, dnn_, db_);
  if (db_sync_ == NULL) {
    SZ_LOG_ERROR("SZ_FACE_SERVER_CTX_create failed");
    return SZ_RETCODE_FAILED;
  }
  detector_ = new FaceDetector(dnn_);
  extractor_ = new FaceExtractor(dnn_, db_);
  return SZ_RETCODE_OK;
}

SZ_RETCODE Quface::initialize_dnn(const std::string& model_file) {
  std::ifstream model_fin;

  try {
    model_fin.open(model_file, std::ios::binary);
  } catch (...) {
    SZ_LOG_ERROR("cannot open {}", model_file);
    return SZ_RETCODE_FAILED;
  }
  if (!model_fin.is_open()) {
    SZ_LOG_ERROR("cannot open {}", model_file);
    return SZ_RETCODE_FAILED;
  }

  std::filebuf* pbuf = model_fin.rdbuf();
  std::size_t model_length = pbuf->pubseekoff(0, model_fin.end, model_fin.in);
  pbuf->pubseekpos(0, model_fin.in);

  SZ_BYTE* model = new SZ_BYTE[model_length];
  pbuf->sgetn((char*)model, model_length);

  dnn_ = SZ_FACE_CTX_create(license_, model, model_length);
  delete[] model;

  if (dnn_ == NULL) {
    SZ_LOG_ERROR("SZ_FACE_CTX_create failed");
    model_fin.close();
    return SZ_RETCODE_FAILED;
  }

  model_fin.close();

  return SZ_RETCODE_OK;
}