#include "utils.h"

#include <opencv2/opencv.hpp>

#include "logger.h"

using namespace suanzi;

SZ_IMAGE_CTX* Utils::load_image(const std::string& image_path) {
  cv::Mat cv_image = cv::imread(image_path);
  if (cv_image.empty()) {
    SZ_LOG_ERROR("Utils::load_image",
                 "cv::imread(\"" + image_path + "\") failed");
    return NULL;
  }

  SZ_IMAGE_CTX* ctx =
      SZ_IMAGE_CTX_create(cv_image.cols, cv_image.rows, SZ_IMAGETYPE_BGR);
  if (ctx == NULL) {
    SZ_LOG_ERROR("Utils::load_image", "SZ_IMAGE_CTX_create failed");
    return NULL;
  }

  SZ_IMAGE_setData(ctx, cv_image.ptr(), cv_image.cols, cv_image.rows);

  return ctx;
}

void Utils::release_image(SZ_IMAGE_CTX* ctx) {
  if (ctx != NULL) {
    SZ_IMAGE_CTX_release(ctx);
  }
}