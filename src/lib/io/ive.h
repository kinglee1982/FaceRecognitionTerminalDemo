#ifndef __IVE_H__
#define __IVE_H__

#include "hi_ive.h"
#include "mmzimage.h"

class Ive {
 public:
  static Ive *getInstance();

  ~Ive();
  bool threshold(MmzImage *pDst, const MmzImage *pSrc, unsigned char threshold,
                 unsigned char maxValue, bool bBlock);
  bool dilate(MmzImage *pDst, const MmzImage *pSrc, unsigned char *pMask,
              bool bBlock);
  bool erode(MmzImage *pDst, const MmzImage *pSrc, bool bBlock);
  bool findContours(const MmzImage *pImage, SZ_RECT **pRegions, int &regionCnt);
  bool medianBlur(MmzImage *pDst, const MmzImage *pSrc, bool bBlock);
  bool resize(MmzImage *pDestImage, const MmzImage *pSrcImage, bool bBlock);
  bool copyImage(MmzImage *pDestImage, const MmzImage *pSrcImage, bool bBlock);
  bool copyImage(MmzImage *pDestImage, HI_U64 srcAddr, bool bBlock);
  bool cropImage(MmzImage *pDestImage, const MmzImage *pSrcImage,
                 const SZ_RECT *pRect, bool bBlock);
  bool rgbPlannar2Yuv(MmzImage *pDestImage, const MmzImage *pSrcImage,
                      bool bBlock);
  bool rgbPacked2Yuv(MmzImage *pDestImage, const MmzImage *pSrcImage,
                     bool bBlock);
  bool yuv2RgbPacked(MmzImage *pDestImage, const MmzImage *pSrcImage,
                     bool bBlock);
  bool yuv2RgbPlannar(MmzImage *pDestImage, const MmzImage *pSrcImage,
                      bool bBlock);

 private:
  Ive();
  void waitFinish();
  void bgr2RgbAlign(MmzImage *pDestImage, const MmzImage *pSrcImage);
  void rgbAlign2Bgr(MmzImage *pDestImage, const MmzImage *pSrcImage);

 private:
  IVE_HANDLE handle_;

  IVE_MEM_INFO_S ccbBlobsMem_;
  SZ_RECT *pCcbRegions_;  //连通区域
  int regionSize_;

  IVE_RESIZE_CTRL_S resizeCtrl_;
};

#endif
