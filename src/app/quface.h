#ifndef QUFACE_H
#define QUFACE_H

#include <memory>
#include <string>
#include <vector>

#include "camera_reader.h"
#include "face_detector.h"
#include "face_extractor.h"
#include "lib/handler.h"
#include "lib/thread.h"
#include "person_info.h"
#include "sz_database_module.h"
#include "sz_face_module.h"
#include "sz_face_server_module.h"
#include "sz_license_module.h"
#include "sz_net_module.h"

namespace suanzi {

class CameraReader;

class Quface {
 private:
  const char *kClientID = "QufaceHisiDemo";
  const char *kDbName = "Quface-db";
  const char *kLicenseStoragePath = ".";

  const SZ_INT32 kMaxPersonInfoLength = 1000;

 public:
  SZ_RETCODE initialize(const std::string &device_file,
                        const std::string &model_file, int disType);
  void release();

  SZ_RETCODE add_person(const std::string &name, SZ_INT32 face_id,
                        SZ_IMAGE_CTX *imageCtx);

  SZ_RETCODE get_avatar(SZ_INT32 face_id, SZ_IMAGE_CTX **imageCtx, SZ_INT32 *width,
                        SZ_INT32 *height);

  bool license_avaliable() { return license_avaliable_; }

  bool network_avaliable() { return network_avaliable_; }

  void handle(SZ_IMAGE_CTX *imageCtx);
  void run();

  FaceDetector *detector_;
  FaceExtractor *extractor_;
  CameraReader *camera_;
  int disType_;

 private:
  SZ_RETCODE initialize_net(const std::string &device_file);
  SZ_RETCODE initialize_license();
  SZ_RETCODE initialize_database();
  SZ_RETCODE initialize_dnn(const std::string &model_file);

  bool license_avaliable_ = false;
  bool network_avaliable_ = false;

  SZ_NET_CTX *network_ = NULL;
  SZ_LICENSE_CTX *license_ = NULL;
  SZ_FACE_CTX *dnn_ = NULL;
  SZ_DATABASE_CTX *db_ = NULL;
  SZ_FACE_SERVER_CTX *db_sync_ = NULL;

  std::vector<PersonInfo> history_;
  PersonInfo determined_person_ = PERSON_NOT_DETECTED;
  bool person_changed_ = false;
};
}  // namespace suanzi

#endif