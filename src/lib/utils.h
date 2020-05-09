#ifndef UTILS_H
#define UTILS_H

#include <string>

#include "sz_image_module.h"

namespace suanzi {

class Utils {
 public:
  static SZ_IMAGE_CTX* load_image(const std::string &image_path);
  static void release_image(SZ_IMAGE_CTX* handle);
};

};  // namespace suanzi

#endif
