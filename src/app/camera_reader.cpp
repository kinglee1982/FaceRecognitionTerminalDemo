#include "camera_reader.h"

#include <opencv2/opencv.hpp>

#include "lib/cvxtext.h"
#include "lib/io/mmzimage.h"
#include "lib/logger.h"
#include "sz_common.h"
#include "sz_image_module.h"
#include <signal.h>


using namespace suanzi;

CameraReader::CameraReader(Quface *parent) {
  parent_ = parent;

  pVi_ = new Vi(0, 0, SONY_IMX307_MIPI_2M_30FPS_12BIT);
  pVpss_ = new Vpss(0, 1920, 1080);
  pVi_Vpss_ = new Vi_Vpss(pVi_, pVpss_);

  if (parent->disType_ == 0)
    pVo_ = new Vo(0, VO_INTF_MIPI, 800, 1280);
  else
    pVo_ = new Vo(0, VO_INTF_HDMI, kDisplayWidth, kDisplayHeight);
  pVo_->start(VO_MODE_1MUX);
  on_not_detected();

  avatar_bg_ = cv::imread("background.jpg");
  if (avatar_bg_.empty()) {
	SZ_LOG_ERROR("avatar_bg_ image read failed!");
	raise(SIGSEGV);
  }
  cv::resize(avatar_bg_, avatar_bg_, {kTargetWidth, kTargetHeight});

  avatar_ = cv::imread("avatar_unknown.jpg");
  if (avatar_.empty()) {
	SZ_LOG_ERROR("avatar_unknown image read failed!");
	raise(SIGSEGV);
  }
  cv::resize(avatar_, avatar_, {kAvatarWidth, kAvatarHeight});
}

void CameraReader::on_detected(DetectionResult &detection) {
  std::unique_lock<std::mutex> lock(detection_mutex_);

  SZ_FACE_QUALITY quality = detection.quality;
  if (quality.yaw > 45 || quality.yaw < -45 || quality.pitch > 45 ||
      quality.pitch < -25) {
    SZ_LOG_WARN("Bad head pose, yaw={:.2f}, pitch={:.2f}, roll={:.2f}",
                quality.yaw, quality.pitch, quality.roll);
    return;
  }

  // Remove detection around target border
  SZ_INT32 x1 = detection.rect.rect.x;
  SZ_INT32 x2 = x1 + detection.rect.rect.width;
  SZ_INT32 y1 = detection.rect.rect.y;
  SZ_INT32 y2 = y1 + detection.rect.rect.height;

  if (x1 <= 10 || x2 >= kTargetWidth - 10 || y1 <= 10 ||
      y2 >= kTargetHeight - 10) {
    detection_.imageCtx = NULL;
    if (parent_) parent_->extractor_->post({NULL, detection_.rect});
    return;
  }

  // Remove detection with large pose

  detection_.imageCtx = detection.imageCtx;
  detection_.rect = detection.rect;

  if (parent_) parent_->extractor_->post({detection.imageCtx, detection.rect});
}

void CameraReader::on_not_detected() {
  std::unique_lock<std::mutex> lock(detection_mutex_);
  detection_.imageCtx = NULL;

  if (parent_) parent_->extractor_->post({NULL, detection_.rect});
}

void CameraReader::on_extracted(ExtractionResult &extraction) {
  std::unique_lock<std::mutex> lock(recognition_mutex_);

  if (extraction.person != PERSON_NOT_DETECTED &&
      extraction.person.id != recognition_.person.id) {
    if (extraction.person == PERSON_UNKNOWN) {
      avatar_.release();
      avatar_ = cv::imread("avatar_unknown.jpg");
    } else {
      avatar_.release();

      SZ_IMAGE_CTX *imageCtx;
      SZ_INT32 width, height;
      if (SZ_RETCODE_OK != parent_->get_avatar(extraction.person.id, &imageCtx, &width, &height))
	  	return;

      SZ_BYTE *imageData;
      SZ_IMAGE_copyData(imageCtx, &imageData);
      avatar_ = cv::Mat(height, width, CV_8UC3, imageData);
    }

    cv::resize(avatar_, avatar_, {kAvatarWidth, kAvatarHeight});
  }
  recognition_ = extraction;
}

void CameraReader::run() {
  SZ_IMAGE_CTX *imageCtx;
  if ((imageCtx = SZ_IMAGE_CTX_create(kTargetWidth, kTargetHeight,
                                      SZ_IMAGETYPE_BGR)) == NULL) {
    SZ_LOG_ERROR("SZ_IMAGE_CTX_create failed");
    return;
  }

  cv::Mat bgr(kInputHeight, kInputWidth, CV_8UC3);
  cv::Mat display;

  while (_bRunning) {
    if (pVpss_->getFrame(bgr, kVPSSChannelIndex)) {
      cv::flip(bgr, bgr, 0);
      display = bgr(kDisplayRect).clone();

      if (parent_) {
        if (SZ_RETCODE_OK !=
            SZ_IMAGE_setData(imageCtx, display(kTargetRect).clone().ptr(),
                             kTargetWidth, kTargetHeight)) {
          SZ_LOG_WARN("SZ_IMAGE_setData failed");
        } else {
          parent_->handle(imageCtx);
        }
      }

      draw_detection(display);
      draw_recognition(display);
      draw_status(display);
      // cv::resize(bgr, display, {kDisplayWidth, kDisplayHeight});
      if (parent_->disType_ == 0) {
        cv::flip(display, display, 0);
        cv::flip(display, display, 1);
      }

      if (!pVo_->sendFrame(display)) {
        SZ_LOG_WARN("Vo::sendFrame failed");
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }
    } else {
      SZ_LOG_WARN("Vpss::getFrame failed");
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }
}

void CameraReader::draw_detection(cv::Mat &bgr) {
  if (recognition_.person == PERSON_NOT_DETECTED)
    cv::rectangle(bgr, kTargetRect, {0, 255, 0}, 2);

  std::unique_lock<std::mutex> lock(detection_mutex_);
  if (detection_.imageCtx != NULL) {
    cv::rectangle(bgr,
                  {kTargetRect.x + detection_.rect.rect.x,
                   kTargetRect.y + detection_.rect.rect.y,
                   detection_.rect.rect.width, detection_.rect.rect.height},
                  {255, 0, 0}, 2);
    // for (int i = 0; i < 5; i++)
    //   cv::circle(bgr,
    //              {kTargetRect.x + detection_.face.points.point[i].x,
    //               kTargetRect.y + detection_.face.points.point[i].y},
    //              5, {0, 0, 255}, 5);
  }
}

void CameraReader::draw_recognition(cv::Mat &bgr) {
  std::unique_lock<std::mutex> lock(recognition_mutex_);
  std::string name = "";
  if (recognition_.person == PERSON_UNKNOWN) {
    name = "访客";
  } else if (recognition_.person != PERSON_NOT_DETECTED) {
    name = recognition_.person.name;
  }
  if (recognition_.person != PERSON_NOT_DETECTED) {
    avatar_bg_.copyTo(bgr(kTargetRect));
    avatar_.copyTo(bgr(kAvatarRect));
    CvxText::getInstance()->putText(bgr, name.c_str(),
                                    {kDisplayWidth / 2 - name.length() * 7.5,
                                     kTargetRect.y + kTargetRect.height - 50},
                                    {255, 255, 255}, {40, 40});
  }
}

void CameraReader::draw_status(cv::Mat &bgr) {
  static cv::Scalar green(0, 255, 0);
  static cv::Scalar red(0, 0, 255);

  if (parent_) {
    std::string network = parent_->network_avaliable() ? "ONLINE" : "OFFLINE";
    std::string license = parent_->license_avaliable() ? "OK" : "FAILED";

    cv::putText(bgr, "NETWORK: " + network, {50, bgr.rows - 50},
                cv::FONT_HERSHEY_SIMPLEX, 1,
                parent_->network_avaliable() ? green : red, 2);
    cv::putText(bgr, "LICENSE: " + license,
                {50 + kDisplayWidth / 2, bgr.rows - 50},
                cv::FONT_HERSHEY_SIMPLEX, 1,
                parent_->license_avaliable() ? green : red, 2);
  }
}
