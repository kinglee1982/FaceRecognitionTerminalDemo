#include "face_detector.h"

#include <chrono>

#include "lib/logger.h"

using namespace suanzi;

FaceDetector::FaceDetector(SZ_FACE_CTX *dnn) { dnn_ = dnn; }

void FaceDetector::set_callback(FaceDetector::Callback::Ptr callback) {
  callback_ = callback;
}

SZ_RETCODE FaceDetector::detect(SZ_IMAGE_CTX *imageCtx, DetectionResult &face) {
  face.imageCtx = imageCtx;

  SZ_INT32 count = 0;

  auto start = std::chrono::system_clock::now();
  SZ_RETCODE ret = SZ_FACE_detect(dnn_, imageCtx, &count);
  auto end = std::chrono::system_clock::now();

  if (ret != SZ_RETCODE_OK || count <= 0) {
    // SZ_LOG_WARN("SZ_FACE_detect found no faces");
    return SZ_RETCODE_FAILED;
  }

  SZ_LOG_DEBUG("detection elapsed={:.2f}ms",
               (double)(std::chrono::duration_cast<std::chrono::milliseconds>(
                            end - start)
                            .count()));

  SZ_INT32 largest_area = -1, largest_idx = -1, area;
  for (SZ_INT32 idx = 0; idx < count; idx++) {
    ret = SZ_FACE_getDetectInfo(dnn_, idx, &face.rect);
    if (ret != SZ_RETCODE_OK) {
      SZ_LOG_ERROR("SZ_FACE_getDetectInfo({}) failed", idx);
      return SZ_RETCODE_FAILED;
    }

    if ((area = face.rect.rect.height * face.rect.rect.width) > largest_area) {
      largest_area = area;
      largest_idx = idx;
    }
  }

  if (largest_idx >= 0 &&
      SZ_FACE_getDetectInfo(dnn_, largest_idx, &face.rect) == SZ_RETCODE_OK) {
    SZ_FACE_QUALITY quality;
    if (SZ_FACE_evaluate(dnn_, imageCtx, largest_idx, &face.quality) !=
        SZ_RETCODE_OK) {
      return SZ_RETCODE_FAILED;
    }
    return SZ_RETCODE_OK;
  } else
    return SZ_RETCODE_FAILED;
}

void FaceDetector::handle(DetectionMessage &message) {
  static DetectionResult detection_result;
  if (SZ_RETCODE_OK == detect(message.imageCtx, detection_result)) {
    if (callback_) callback_->on_detected(detection_result);
  } else {
    if (callback_) callback_->on_not_detected();
  }
}
