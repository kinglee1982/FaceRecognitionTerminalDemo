#ifndef CAMERA_READER_H
#define CAMERA_READER_H

#include <mutex>
#include <opencv2/opencv.hpp>

#include "face_detector.h"
#include "face_extractor.h"
#include "lib/io/vi_vpss.h"
#include "lib/io/vo.h"
#include "lib/io/vpss.h"
#include "lib/thread.h"
#include "quface.h"

namespace suanzi {

class Quface;

class CameraReader : public AbstractRunnable,
                     public FaceDetector::Callback,
                     public FaceExtractor::Callback {
 public:
  const int kInputWidth = 1920;
  const int kInputHeight = 1080;

  const int kDisplayWidth = 800;
  const int kDisplayHeight = 1024;

  const int kTargetWidth = 550;
  const int kTargetHeight = 550;

  const int kAvatarWidth = 300;
  const int kAvatarHeight = 300;

  const int kVPSSChannelIndex = 0;
  const int kVOChannelIndex = 0;

  const cv::Rect kDisplayRect = {640, 0, kDisplayWidth, kDisplayHeight};
  const cv::Rect kTargetRect = {125, 155, kTargetWidth, kTargetHeight};
  const cv::Rect kAvatarRect = {(kDisplayWidth - kAvatarWidth) / 2, 250,
                                kAvatarWidth, kAvatarHeight};

  CameraReader(Quface *parent);

  void on_detected(DetectionResult &detection);
  void on_not_detected();

  void on_extracted(ExtractionResult &extraction);

 private:
  virtual void run();
  void draw_detection(cv::Mat &bgr);
  void draw_recognition(cv::Mat &bgr);
  void draw_status(cv::Mat &bgr);

  Quface *parent_;

  Vi_Vpss *pVi_Vpss_;
  Vi *pVi_;
  Vo *pVo_;
  Vpss *pVpss_;

  std::mutex detection_mutex_;
  DetectionResult detection_;

  std::mutex recognition_mutex_;
  ExtractionResult recognition_;

  cv::Mat avatar_bg_, avatar_;
};

}  // namespace suanzi

#endif
