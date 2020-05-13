#ifndef _IVE_APP_H_
#define _IVE_APP_H_

#include "mmzimage.h"

namespace suanzi {

class IveApp {
 public:
  bool copyImage(MmzImage *pDestImage, MmzImage *pSrcImage);
  bool copyGrayImage(MmzImage *pDestImage, HI_U64 srcAddr);
  bool copyYuvImage(MmzImage *pDestImage, HI_U64 srcAddr);

 private:
};

}  // namespace suanzi
#endif
