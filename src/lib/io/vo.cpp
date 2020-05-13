#include "vo.h"

#include "hdmi.h"
#include "hi_buffer.h"
#include "lib/logger.h"
#include "mipi_tx.h"
#include "rp-lcd-mipi-8inch-800x1280.h"
#include <signal.h>

#define COLOR_RGB_RED 0xFF0000
#define COLOR_RGB_GREEN 0x00FF00
#define COLOR_RGB_BLUE 0x0000FF
#define COLOR_RGB_BLACK 0x000000
#define COLOR_RGB_YELLOW 0xFFFF00
#define COLOR_RGB_CYN 0x00ffff
#define COLOR_RGB_WHITE 0xffffff

using namespace suanzi;

bool Vo::bInit = false;
VB_POOL Vo::poolId_ = 0;
VB_BLK Vo::vbBlk_ = 0;

Vo::Vo(VO_DEV voDev, VO_INTF_TYPE_E intfType, int width, int height)
    : voDev_(voDev), intfType_(intfType), width_(width), height_(height) {
  if (bInit) return;
  bInit = true;
  signal(SIGINT, exitAbnormal);
  signal(SIGTERM, exitAbnormal);
  signal(SIGSEGV, exitAbnormal);
}

Vo::~Vo() {}

VO_INTF_SYNC_E Vo::getIntfSync(VO_INTF_TYPE_E intfType, int width, int height) {
  if (intfType == VO_INTF_MIPI) return VO_OUTPUT_USER;
  if (width == 1920 && height == 1080) {
    return VO_OUTPUT_1080P25;
  } else if (width == 1280 && height == 800) {
    return VO_OUTPUT_1280x800_60;
  } else if (width == 3840 && height == 2160) {
    return VO_OUTPUT_3840x2160_25;
  } else {
    SZ_LOG_WARN("getIntfSync failed, can't support width={} height={}", width,
                height);
    return VO_OUTPUT_1080P25;
  }
}

HI_VOID Vo::exitAbnormal(HI_S32 signo) {
  if (SIGINT == signo || SIGTSTP == signo || SIGTERM == signo || SIGSEGV == signo) {
    HI_U32 ret0 = HI_MPI_VB_ReleaseBlock(vbBlk_);
    HI_U32 ret = HI_MPI_VB_DestroyPool(poolId_);
    SZ_LOG_WARN("\033[0;31mprogram exit abnormally!\033[0;39m {0:X}  {1:X}",
                ret0, ret);
  }
  exit(0);
}

void Vo::initVb(int width, int height) {
  HI_U64 u64BlkSize =
      COMMON_GetPicBufferSize(width, height, PIXEL_FORMAT_YVU_SEMIPLANAR_420,
                              DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 0);
  u64BlkSize = ALIGN_UP(u64BlkSize, 4096);

  VB_POOL_CONFIG_S vbPoolCfg;
  memset(&vbPoolCfg, 0, sizeof(VB_POOL_CONFIG_S));
  vbPoolCfg.u64BlkSize = u64BlkSize;
  vbPoolCfg.u32BlkCnt = 1;
  vbPoolCfg.enRemapMode = VB_REMAP_MODE_NONE;
  poolId_ = HI_MPI_VB_CreatePool(&vbPoolCfg);
  if (VB_INVALID_POOLID == poolId_) {
    SZ_LOG_ERROR("HI_MPI_VB_CreatePool fail!");
  }
  vbBlk_ = HI_MPI_VB_GetBlock(poolId_, u64BlkSize, NULL);

  SZ_LOG_INFO("poolId={}   vbBlk={}", poolId_, vbBlk_);
}

bool Vo::startDevice(VO_DEV voDev, VO_INTF_TYPE_E intfType,
                     VO_INTF_SYNC_E intfSync) {
  /********************************
   * Set and start VO device VoDev#.
   *********************************/
  /*VO_INTF_SYNC_E intfSync = VO_OUTPUT_3840x2160_30;
  if (height < 2160)
          intfSync = VO_OUTPUT_1080P25;*/

  VO_PUB_ATTR_S stVoPubAttr = {0};
  stVoPubAttr.enIntfType = intfType;  //接口类型VO_INTF_HDMI
  stVoPubAttr.enIntfSync = intfSync;  //接口时序VO_OUTPUT_1080P25
  stVoPubAttr.u32BgColor = COLOR_RGB_BLUE;
  HI_U32 s32Ret = HI_MPI_VO_SetPubAttr(voDev, &stVoPubAttr);
  if (s32Ret != HI_SUCCESS) {
    SZ_LOG_ERROR("HI_MPI_VO_SetPubAttr failed with {:X}!", s32Ret);
    return false;
  }

  s32Ret = HI_MPI_VO_Enable(voDev);
  if (s32Ret != HI_SUCCESS) {
    SZ_LOG_ERROR("HI_MPI_VO_Enable failed with {:X}", s32Ret);
    return false;
  }

  return true;
}

bool Vo::startLayer(VO_LAYER voLayer, int width, int height, int disFrameRate) {
  /******************************
   * Set and start layer VoDev#.
   ********************************/
  VO_VIDEO_LAYER_ATTR_S stLayerAttr = {0};
  stLayerAttr.stDispRect.u32Width = width;
  stLayerAttr.stDispRect.u32Height = height;
  stLayerAttr.u32DispFrmRt = 30;  // disFrameRate;//30;
  stLayerAttr.bClusterMode = HI_FALSE;
  stLayerAttr.bDoubleFrame = HI_FALSE;
  stLayerAttr.enPixFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
  stLayerAttr.stDispRect.s32X = 0;
  stLayerAttr.stDispRect.s32Y = 0;

  stLayerAttr.stImageSize.u32Width = width;
  stLayerAttr.stImageSize.u32Height = height;
  stLayerAttr.enDstDynamicRange = DYNAMIC_RANGE_SDR8;

  VO_PART_MODE_E enVoPartMode = VO_PART_MODE_SINGLE;
  HI_S32 s32Ret = HI_FAILURE;
  if (VO_PART_MODE_MULTI == enVoPartMode) {
    s32Ret = HI_MPI_VO_SetVideoLayerPartitionMode(voLayer, enVoPartMode);
    if (HI_SUCCESS != s32Ret) {
      SZ_LOG_ERROR("HI_MPI_VO_SetVideoLayerPartitionMode failed!");
      return false;
    }
  }

  // multi ch ouptut disBufLen can't be 0.
  int disBufLen = 3;
  if (disBufLen != 0) {
    s32Ret = HI_MPI_VO_SetDisplayBufLen(voLayer, disBufLen);
    if (HI_SUCCESS != s32Ret) {
      SZ_LOG_ERROR("HI_MPI_VO_SetDisplayBufLen failed with {:X}!", s32Ret);
      return false;
    }
  }

  s32Ret = HI_MPI_VO_SetVideoLayerAttr(voLayer, &stLayerAttr);
  if (s32Ret != HI_SUCCESS) {
    SZ_LOG_ERROR("HI_MPI_VO_SetVideoLayerAttr failed with {:X}!", s32Ret);
    return false;
  }

  s32Ret = HI_MPI_VO_EnableVideoLayer(voLayer);
  if (s32Ret != HI_SUCCESS) {
    SZ_LOG_ERROR("HI_MPI_VO_EnableVideoLayer failed with {:X}!", s32Ret);
    return false;
  }
  return true;
}

bool Vo::startCh(VO_LAYER voLayer, VoMode voMode, int width, int height) {
  HI_U32 u32WndNum = 0;
  HI_U32 u32Square = 0;
  HI_U32 u32Row = 0;
  HI_U32 u32Col = 0;
  HI_U32 u32Width = width;
  HI_U32 u32Height = height;
  switch (voMode) {
    case VO_MODE_1MUX:
      u32WndNum = 1;
      u32Square = 1;
      break;
    case VO_MODE_2MUX:
      u32WndNum = 2;
      u32Square = 2;
      break;
    case VO_MODE_4MUX:
      u32WndNum = 4;
      u32Square = 2;
      break;
    case VO_MODE_8MUX:
      u32WndNum = 8;
      u32Square = 3;
      break;
    case VO_MODE_9MUX:
      u32WndNum = 9;
      u32Square = 3;
      break;
    case VO_MODE_16MUX:
      u32WndNum = 16;
      u32Square = 4;
      break;
    case VO_MODE_25MUX:
      u32WndNum = 25;
      u32Square = 5;
      break;
    case VO_MODE_36MUX:
      u32WndNum = 36;
      u32Square = 6;
      break;
    case VO_MODE_49MUX:
      u32WndNum = 49;
      u32Square = 7;
      break;
    case VO_MODE_64MUX:
      u32WndNum = 64;
      u32Square = 8;
      break;
    case VO_MODE_2X4:
      u32WndNum = 8;
      u32Square = 3;
      u32Row = 4;
      u32Col = 2;
      break;
    default:
      SZ_LOG_ERROR("not support vo mode: {}!\n", voMode);
      return false;
  }

  VO_CHN_ATTR_S stChnAttr;
  stChnAttr.u32Priority = 0;
  stChnAttr.bDeflicker = HI_FALSE;
  for (int i = 0; i < u32WndNum; i++) {
    if (voMode == VO_MODE_2X4) {
      stChnAttr.stRect.s32X = ALIGN_DOWN((u32Width / u32Col) * (i % u32Col), 2);
      stChnAttr.stRect.s32Y =
          ALIGN_DOWN((u32Height / u32Row) * (i / u32Col), 2);
      stChnAttr.stRect.u32Width = ALIGN_DOWN(u32Width / u32Col, 2);
      stChnAttr.stRect.u32Height = ALIGN_DOWN(u32Height / u32Row, 2);
    } else {
      if (u32Width > u32Height) {
        stChnAttr.stRect.s32X =
            ALIGN_DOWN((u32Width / u32Square) * (i % u32Square), 2);
        stChnAttr.stRect.s32Y =
            ALIGN_DOWN((u32Height / u32Square) * (i / u32Square), 2);
        stChnAttr.stRect.u32Width = ALIGN_DOWN(u32Width / u32Square, 2);
        stChnAttr.stRect.u32Height = ALIGN_DOWN(u32Height, 2);
      } else {
        stChnAttr.stRect.s32X =
            ALIGN_DOWN((u32Width / u32Square) * (i / u32Square), 2);
        stChnAttr.stRect.s32Y =
            ALIGN_DOWN((u32Height / u32Square) * (i % u32Square), 2);
        stChnAttr.stRect.u32Width = ALIGN_DOWN(u32Width, 2);
        stChnAttr.stRect.u32Height = ALIGN_DOWN(u32Height / u32Square, 2);
      }
    }

    SZ_LOG_INFO("s32X = {0}, s32Y = {1}, u32Width = {2}, u32Height = {3}",
                stChnAttr.stRect.s32X, stChnAttr.stRect.s32Y,
                stChnAttr.stRect.u32Width, stChnAttr.stRect.u32Height);
    HI_S32 s32Ret = HI_MPI_VO_SetChnAttr(voLayer, i, &stChnAttr);
    if (s32Ret != HI_SUCCESS) {
      SZ_LOG_ERROR("HI_MPI_VO_SetChnAttr failed with {:X}!", s32Ret);
      return false;
    }

    s32Ret = HI_MPI_VO_EnableChn(voLayer, i);
    if (s32Ret != HI_SUCCESS) {
      SZ_LOG_ERROR("HI_MPI_VO_EnableChn failed with {:X}!", s32Ret);
      return false;
    }
  }
  return true;
}

bool Vo::start(VoMode voMode) {
  initVb(width_, height_);

  VO_INTF_SYNC_E intfSync = getIntfSync(intfType_, width_, height_);
  if (intfSync == VO_OUTPUT_USER) {
    Mipi_Tx *pMipi_Tx = Mipi_Tx::getInstance();
    if (!pMipi_Tx->startDeviceWithUserSyncInfo(voDev_)) return false;
  } else {
    if (!startDevice(voDev_, intfType_, intfSync)) return false;
  }

  VO_LAYER voLayer = 0;
  int disFrameRate = 30;
  if (!startLayer(voLayer, width_, height_, disFrameRate)) return false;

  VO_CSC_S stVideoCSC;
  HI_S32 s32Ret = HI_MPI_VO_GetVideoLayerCSC(voLayer, &stVideoCSC);
  if (HI_SUCCESS != s32Ret) {
    SZ_LOG_ERROR("HI_MPI_VO_GetVideoLayerCSC failed!");
    return false;
  }

  if (VO_INTF_MIPI == intfType_) {
    stVideoCSC.enCscMatrix = VO_CSC_MATRIX_BT709_TO_RGB_PC;
    s32Ret = HI_MPI_VO_SetVideoLayerCSC(voLayer, &stVideoCSC);
    if (HI_SUCCESS != s32Ret) {
      SZ_LOG_ERROR("HI_MPI_VO_SetVideoLayerCSC failed!");
      return false;
    }
  } else {
    stVideoCSC.enCscMatrix = VO_CSC_MATRIX_IDENTITY;
    s32Ret = HI_MPI_VO_SetVideoLayerCSC(voLayer, &stVideoCSC);
    if (HI_SUCCESS != s32Ret) {
      SZ_LOG_ERROR("HI_MPI_VO_SetVideoLayerCSC failed! {:X}", s32Ret);
      return false;
    }
  }

  if (!startCh(voLayer, voMode, width_, height_)) return false;

  if (VO_INTF_HDMI & intfType_) {
    Hdmi *pHdmi = Hdmi::getInstance();
    if (!pHdmi->start(HI_HDMI_ID_0, width_, height_)) return false;
  }

  if (VO_INTF_MIPI & intfType_) {
    Mipi_Tx *pMipi_Tx = Mipi_Tx::getInstance();
    if (!pMipi_Tx->startMipiTx(intfSync)) return false;
  }
  return true;
}

bool Vo::sendYuvFrame(const unsigned char *pData, int width, int height) {
  VIDEO_FRAME_INFO_S stVFrame;
  memset(&stVFrame, 0, sizeof(VIDEO_FRAME_INFO_S));

  HI_U64 u64YPhyAddr = HI_MPI_VB_Handle2PhysAddr(vbBlk_);
  HI_U8 *pMap = (HI_U8 *)HI_MPI_SYS_Mmap(u64YPhyAddr, width * height * 3 / 2);
  memcpy(pMap, pData, width * height * 3 / 2);

  stVFrame.u32PoolId = poolId_;
  stVFrame.enModId = HI_ID_USER;
  stVFrame.stVFrame.u32Width = width;
  stVFrame.stVFrame.u32Height = height;
  stVFrame.stVFrame.enField = VIDEO_FIELD_FRAME;
  stVFrame.stVFrame.enPixelFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
  stVFrame.stVFrame.enVideoFormat = VIDEO_FORMAT_LINEAR;
  stVFrame.stVFrame.enCompressMode = COMPRESS_MODE_NONE;
  stVFrame.stVFrame.enDynamicRange = DYNAMIC_RANGE_SDR8;
  stVFrame.stVFrame.enColorGamut = COLOR_GAMUT_BT709;
  stVFrame.stVFrame.u32Stride[0] = width;
  stVFrame.stVFrame.u32Stride[1] = width;
  stVFrame.stVFrame.u64PhyAddr[0] = u64YPhyAddr;
  stVFrame.stVFrame.u64PhyAddr[1] = u64YPhyAddr + width * height;
  HI_MPI_SYS_Munmap(pMap, width * height * 3 / 2);

  HI_S32 s32Ret = HI_MPI_VO_SendFrame(0, 0, &stVFrame, -1);
  if (s32Ret != HI_SUCCESS) {
    SZ_LOG_ERROR("HI_MPI_VO_SendFrame failed  {:X}", s32Ret);
    return false;
  }
  return true;
}

bool Vo::sendFrame(const cv::Mat &image) {
  cv::Mat yuvImg(image.rows * 3 / 2, image.cols, CV_8UC1);
  cv::Mat yuvImg_(image.rows * 3 / 2, image.cols, CV_8UC1);
  cv::cvtColor(image, yuvImg, CV_BGR2YUV_I420);

  int yLen = image.rows * yuvImg.cols;
  memcpy(yuvImg_.data, yuvImg.data, yLen);
  uchar *pU = yuvImg.data + yLen;
  uchar *pV = yuvImg.data + (image.rows + image.rows / 4) * yuvImg.cols;
  uchar *pDst = yuvImg_.data + yLen;
  for (int i = 0; i < yLen / 2; i += 2) {
    *pDst++ = *pV++;
    *pDst++ = *pU++;
  }
  return sendYuvFrame(yuvImg_.data, image.cols, image.rows);
}

bool Vo::stopCh(VO_LAYER voLayer, VoMode voMode) {
  int wndNum = 0;
  switch (voMode) {
    case VO_MODE_1MUX:
      wndNum = 1;
      break;
    case VO_MODE_2MUX:
      wndNum = 2;
      break;
    case VO_MODE_4MUX:
      wndNum = 4;
      break;
    case VO_MODE_8MUX:
      wndNum = 8;
      break;
    default:
      SZ_LOG_ERROR("can't support mode {}!", voMode);
      return false;
  }

  for (int i = 0; i < wndNum; i++) {
    HI_U32 s32Ret = HI_MPI_VO_DisableChn(voLayer, i);
    if (s32Ret != HI_SUCCESS) {
      SZ_LOG_ERROR("HI_MPI_VO_DisableChn failed with {:X}!", s32Ret);
      return false;
    }
  }
  return true;
}

bool Vo::stopLayer(VO_LAYER voLayer) {
  HI_U32 s32Ret = HI_MPI_VO_DisableVideoLayer(voLayer);
  if (s32Ret != HI_SUCCESS) {
    SZ_LOG_ERROR("HI_MPI_VO_DisableVideoLayer failed with {:X}!", s32Ret);
    return false;
  }
  return true;
}

bool Vo::stopDev(VO_DEV voDev) {
  HI_S32 s32Ret = HI_MPI_VO_Disable(voDev);
  if (s32Ret != HI_SUCCESS) {
    SZ_LOG_ERROR("HI_MPI_VO_Disable failed with {:X}!", s32Ret);
    return false;
  }
  return true;
}
