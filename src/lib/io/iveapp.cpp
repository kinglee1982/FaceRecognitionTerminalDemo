#include "iveapp.h"

#include "ive.h"

using namespace suanzi;

bool IveApp::copyImage(MmzImage *pDestImage, MmzImage *pSrcImage) {
  int w = 1920;
  int h = 1080;
  int offset = 0;
  pSrcImage->setSize(w, h);
  pDestImage->setSize(w, h);
  Ive *pIve = Ive::getInstance();
  for (int i = 0; i < 4; i++) {
    offset = w * (i & 1) + pSrcImage->width * h * (i >> 1);
    pDestImage->setOffset(offset);
    pSrcImage->setOffset(offset);
    pIve->copyImage(pDestImage, pSrcImage, true);
  }
  return true;
}

bool IveApp::copyGrayImage(MmzImage *pDestImage, HI_U64 srcAddr) {
  int realWidth = pDestImage->width;
  int realHeight = pDestImage->height;
  int w = 1920;
  int h = 1080;
  int offset = 0;
  pDestImage->setSize(w, h);
  Ive *pIve = Ive::getInstance();
  for (int i = 0; i < 4; i++) {
    offset = w * (i & 1) + realWidth * h * (i >> 1);
    pDestImage->setOffset(offset);
    pIve->copyImage(pDestImage, srcAddr, true);
  }
  pDestImage->setSize(realWidth, realHeight);
  return true;
}

bool IveApp::copyYuvImage(MmzImage *pDestImage, HI_U64 srcAddr) {
  int realWidth = pDestImage->width;
  int realHeight = pDestImage->height;
  int w = realWidth;
  int h = realHeight;
  pDestImage->setSize(w, h);
  Ive *pIve = Ive::getInstance();
  pDestImage->setOffset(0);
  pIve->copyImage(pDestImage, srcAddr, true);

  pDestImage->setSize(w, h / 2);
  pDestImage->setOffset(w * h);
  pIve->copyImage(pDestImage, srcAddr, true);

  pDestImage->setSize(realWidth, realHeight);
  return true;
}
