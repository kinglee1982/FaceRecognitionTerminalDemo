#ifndef FACE_DETECTOR_H
#define FACE_DETECTOR_H

#include "lib/handler.h"
#include "sz_face_module.h"

namespace suanzi {

typedef struct _DetectionMessage {
  SZ_IMAGE_CTX *imageCtx;
} DetectionMessage;

typedef struct _DetectionResult {
  SZ_IMAGE_CTX *imageCtx;
  SZ_FACE_DETECTION rect;
  SZ_FACE_QUALITY quality;
} DetectionResult;

class FaceDetector : public Handler<DetectionMessage> {
 public:
  class Callback {
   public:
    typedef Callback *Ptr;
    virtual void on_detected(DetectionResult &result) = 0;
    virtual void on_not_detected() = 0;
  };

  FaceDetector(SZ_FACE_CTX* dnn);
  void set_callback(Callback::Ptr callback);
  SZ_RETCODE detect(SZ_IMAGE_CTX *imageCtx, DetectionResult &face);

 private:
  void handle(DetectionMessage &message);

  SZ_FACE_CTX* dnn_;
  Callback::Ptr callback_;
};

}  // namespace suanzi
#endif