#ifndef PERSON_INFO_H
#define PERSON_INFO_H

#include <string>

#include "sz_common.h"

namespace suanzi {
typedef struct _PersonInfo {
  SZ_INT32 id;
  std::string name;
  SZ_FLOAT score;

  bool operator==(const _PersonInfo &p);
  bool operator!=(const _PersonInfo &p);

} PersonInfo;

const static PersonInfo PERSON_UNKNOWN = {-1, "PERSON_UNKNOWN", 0};
const static PersonInfo PERSON_NOT_DETECTED = {-2, "PERSON_NOT_DETECTED", 0};

}  // namespace suanzi

#endif
