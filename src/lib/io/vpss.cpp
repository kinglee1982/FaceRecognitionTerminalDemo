#include "vpss.h"

#include <assert.h>

#include "lib/logger.h"
#include "sz_common.h"

using namespace suanzi;

Vpss::Vpss(VPSS_GRP vpssGrp, int maxWidth, int maxHeight)
    : vpssGrp_(vpssGrp), maxWidth_(maxWidth), maxHeight_(maxHeight) {
  pIveApp_ = new IveApp();
  pMmzYuvImage_ = new MmzImage(maxWidth, maxHeight, SZ_IMAGETYPE_NV21);
  chNum_ = 0;
}

Vpss::~Vpss() {
  delete pIveApp_;
  if (pMmzYuvImage_ != NULL) {
    delete pMmzYuvImage_;
  }
}

bool Vpss::start(const cv::Size *pVpssChSizes, int chNum) {
  assert(pVpssChSizes != NULL && chNum < VPSS_MAX_CHN_NUM);

  VPSS_GRP_ATTR_S vpssGrpAttr;
  getDefaultGrpAttr(vpssGrpAttr, maxWidth_, maxHeight_, true);
  HI_U32 s32Ret = HI_MPI_VPSS_CreateGrp(vpssGrp_, &vpssGrpAttr);
  if (s32Ret != HI_SUCCESS) {
    SZ_LOG_ERROR("HI_MPI_VPSS_CreateGrp(grp:{0}) failed with {1:X}!", vpssGrp_,
                 s32Ret);
    return false;
  }

  VPSS_CHN_ATTR_S vpssChAttr;
  for (int i = 0; i < chNum; i++) {
    cv::Size *pSize = (cv::Size *)&pVpssChSizes[i];
    assert(pSize->width <= maxWidth_ && pSize->height <= maxHeight_);
    getDefaultChAttr(vpssChAttr, pSize->width, pSize->height);
    vpssChSizes_[i] = *pSize;

    s32Ret = HI_MPI_VPSS_SetChnAttr(vpssGrp_, i, &vpssChAttr);
    if (s32Ret != HI_SUCCESS) {
      SZ_LOG_ERROR("HI_MPI_VPSS_SetChnAttr failed with {:X}", s32Ret);
      return false;
    }

    s32Ret = HI_MPI_VPSS_EnableChn(vpssGrp_, i);
    if (s32Ret != HI_SUCCESS) {
      SZ_LOG_ERROR("HI_MPI_VPSS_EnableChn failed with {:X}", s32Ret);
      return false;
    }
  }
  chNum_ = chNum;

  s32Ret = HI_MPI_VPSS_StartGrp(vpssGrp_);
  if (s32Ret != HI_SUCCESS) {
    SZ_LOG_ERROR("HI_MPI_VPSS_StartGrp failed with {:X}", s32Ret);
    return false;
  }
  return true;
}

bool Vpss::stop(int vpssChNum) {
  for (int i = 0; i < vpssChNum; i++) {
    HI_U32 s32Ret = HI_MPI_VPSS_DisableChn(vpssGrp_, i);
    if (s32Ret != HI_SUCCESS) {
      SZ_LOG_ERROR("HI_MPI_VPSS_DisableChn failed with {:X}!", s32Ret);
      return false;
    }
  }

  HI_U32 s32Ret = HI_MPI_VPSS_StopGrp(vpssGrp_);
  if (s32Ret != HI_SUCCESS) {
    SZ_LOG_ERROR("HI_MPI_VPSS_StopGrp failed with {:X}!", s32Ret);
    return false;
  }

  s32Ret = HI_MPI_VPSS_DestroyGrp(vpssGrp_);
  if (s32Ret != HI_SUCCESS) {
    SZ_LOG_ERROR("HI_MPI_VPSS_DestroyGrp failed with {:X}!", s32Ret);
    return false;
  }
  return true;
}

bool Vpss::parseFrame(MmzImage *pImage, const VIDEO_FRAME_S *pVBuf,
                      int vpssChIndex) {
  PIXEL_FORMAT_E enPixelFormat = pVBuf->enPixelFormat;
  if (PIXEL_FORMAT_YVU_SEMIPLANAR_420 != enPixelFormat) {
    SZ_LOG_ERROR("This YUV format is not support!\n");
    return false;
  }
  HI_U32 s32Ysize = (pVBuf->u64VirAddr[1] - pVBuf->u64VirAddr[0]);
  HI_U32 u32Size = s32Ysize * 3 / 2;
  HI_U32 u32UvHeight = pVBuf->u32Height / 2;
  HI_U64 phy_addr = pVBuf->u64PhyAddr[0];
  pIveApp_->copyYuvImage(pImage, phy_addr);
  return true;
}

void Vpss::getDefaultGrpAttr(VPSS_GRP_ATTR_S &vpssGrpAttr, int maxWidth,
                             int maxHeight, bool bNrEn) {
  hi_memset(&vpssGrpAttr, sizeof(VPSS_GRP_ATTR_S), 0, sizeof(VPSS_GRP_ATTR_S));
  vpssGrpAttr.stFrameRate.s32SrcFrameRate = -1;
  vpssGrpAttr.stFrameRate.s32DstFrameRate = -1;
  vpssGrpAttr.enDynamicRange = DYNAMIC_RANGE_SDR8;
  vpssGrpAttr.enPixelFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
  vpssGrpAttr.u32MaxW = maxWidth;
  vpssGrpAttr.u32MaxH = maxHeight;
  if (bNrEn) {
    vpssGrpAttr.bNrEn = HI_TRUE;
    vpssGrpAttr.stNrAttr.enCompressMode = COMPRESS_MODE_FRAME;
    vpssGrpAttr.stNrAttr.enNrMotionMode = NR_MOTION_MODE_NORMAL;
  }
}

void Vpss::getDefaultChAttr(VPSS_CHN_ATTR_S &chAttr, int width, int height) {
  chAttr.u32Width = width;
  chAttr.u32Height = height;
  chAttr.enChnMode = VPSS_CHN_MODE_USER;
  chAttr.enCompressMode = COMPRESS_MODE_NONE;
  chAttr.enDynamicRange = DYNAMIC_RANGE_SDR8;
  chAttr.enVideoFormat = VIDEO_FORMAT_LINEAR;
  chAttr.enPixelFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
  chAttr.stFrameRate.s32SrcFrameRate = -1;  // 30 -1
  chAttr.stFrameRate.s32DstFrameRate = -1;  // 30 -1
  chAttr.u32Depth = 1;                      // usr image queue size
  chAttr.bMirror = HI_FALSE;
  chAttr.bFlip = HI_FALSE;
  chAttr.stAspectRatio.enMode = ASPECT_RATIO_NONE;
}

bool Vpss::getYuvFrame(MmzImage *pImage, int vpssChIndex) {
  bool bOk = false;
  VIDEO_FRAME_INFO_S stVFrameInfo;
  HI_U32 s32Ret =
      HI_MPI_VPSS_GetChnFrame(vpssGrp_, vpssChIndex, &stVFrameInfo, 0);
  if (HI_SUCCESS != s32Ret) {
    if (HI_ERR_VPSS_BUF_EMPTY != s32Ret)
      SZ_LOG_ERROR("HI_MPI_VPSS_GetChnFrame failed {:X}", s32Ret);
  } else {
    bOk = parseFrame(pImage, &stVFrameInfo.stVFrame, vpssChIndex);
    HI_MPI_VPSS_ReleaseChnFrame(vpssGrp_, vpssChIndex, &stVFrameInfo);
  }
  return bOk;
}

bool Vpss::getFrame(cv::Mat &image, int vpssChIndex) {
  assert(vpssChIndex < chNum_);
  int width = vpssChSizes_[vpssChIndex].width;
  int height = vpssChSizes_[vpssChIndex].height;
  assert(image.cols == width && image.rows == height);

  pMmzYuvImage_->setSize(width, height);
  if (!getYuvFrame(pMmzYuvImage_, vpssChIndex)) return false;

  cv::Mat tmpYuvImage(height * 3 / 2, width, CV_8UC1);
  memcpy(tmpYuvImage.data, pMmzYuvImage_->pData, width * height * 3 / 2);
  cvtColor(tmpYuvImage, image, CV_YUV2BGR_NV21);
  return true;
}

VPSS_GRP Vpss::getVpssGrp() { return vpssGrp_; }
