#include "vdecoder.h"

#include <stdio.h>
#include <unistd.h>

#include "hi_buffer.h"
#include "hi_comm_video.h"
#include "hi_defines.h"
#include "hi_math.h"
#include "lib/logger.h"
#include "mpi_sys.h"
#include "mpi_vb.h"
#include "mpi_vdec.h"
#include "utility.h"
#include "vo.h"

FILE *fp_out;

#define MAX_DECODE_CH_NUM 4
#define VDEC_REF_FRAME_NUM 5  // 3
#define VDEC_DIS_FRAME_NUM 11
#define VDEC_FRAME_BUF_NUM (VDEC_REF_FRAME_NUM + VDEC_DIS_FRAME_NUM + 1)
#define VDEC_TMV_BUF_NUM (VDEC_REF_FRAME_NUM + 1)
#define MAX_VPSS_CH_NUM 2
#define VDEC_IMAGE_WIDTH 3840
#define VDEC_IMAGE_HEIGHT 2160
#define NNIE_IMAGE_WIDTH 544
#define NNIE_IMAGE_HEIGHT 544

using namespace suanzi;

bool VDecoder::bInit_ = false;

VDecoder::VDecoder(int chIndex) : chIndex_(chIndex) {
  pIveApp_ = new IveApp();
  if (bInit_) return;
  bInit_ = true;
}

void VDecoder::getVbConfig(int decodeChNum, int width, int height,
                           VB_CONFIG_S *pVbConfig) {
  hi_memset(pVbConfig, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
  pVbConfig->u32MaxPoolCnt = 1;
  pVbConfig->astCommPool[0].u32BlkCnt = 10 * decodeChNum;
  pVbConfig->astCommPool[0].u64BlkSize =
      COMMON_GetPicBufferSize(width, height, PIXEL_FORMAT_YVU_SEMIPLANAR_420,
                              DATA_BITWIDTH_8, COMPRESS_MODE_SEG, 0);
}

void VDecoder::start(int decodeChNum, int width, int height,
                     PAYLOAD_TYPE_E payloadType, VIDEO_DEC_MODE_E decMode) {
  initVbPool(decodeChNum, width, height, payloadType, decMode);
  startRecvStream(decodeChNum, width, height, payloadType, decMode);
}

bool VDecoder::setModuleBufPool(const VB_CONFIG_S *pVbConf, VB_UID_E vbUid) {
  // VB_SOURCE_MODULE    VB_UID_VDEC
  HI_MPI_VB_ExitModCommPool(vbUid);
  HI_S32 ret = HI_MPI_VB_SetModPoolConfig(vbUid, pVbConf);
  if (ret != HI_SUCCESS) {
    SZ_LOG_ERROR("HI_MPI_VB_SetModPoolConfig failed {:X}", ret);
    return false;
  }

  ret = HI_MPI_VB_InitModCommPool(vbUid);
  if (HI_SUCCESS != ret) {
    SZ_LOG_ERROR("HI_MPI_VB_InitModCommPool fail for {:X}", ret);
    HI_MPI_VB_ExitModCommPool(vbUid);
    return false;
  }
  return true;
}

bool VDecoder::initVbPool(int decodeChNum, int width, int height,
                          PAYLOAD_TYPE_E payloadType,
                          VIDEO_DEC_MODE_E decMode) {
  HI_U32 picBufSize = 0;
  HI_U32 tmvBufSize = 0;
  if (PT_H265 == payloadType) {
    picBufSize = VDEC_GetPicBufferSize(payloadType, width, height,
                                       PIXEL_FORMAT_YVU_SEMIPLANAR_420,
                                       DATA_BITWIDTH_8, 0);
    tmvBufSize = VDEC_GetTmvBufferSize(payloadType, width, height);
  } else if (PT_H264 == payloadType) {
    picBufSize = VDEC_GetPicBufferSize(payloadType, width, height,
                                       PIXEL_FORMAT_YVU_SEMIPLANAR_420,
                                       DATA_BITWIDTH_8, 0);
    if (VIDEO_DEC_MODE_IPB == decMode) {
      tmvBufSize = VDEC_GetTmvBufferSize(payloadType, width, height);
    }
  } else {
    picBufSize = VDEC_GetPicBufferSize(payloadType, width, height,
                                       PIXEL_FORMAT_YVU_SEMIPLANAR_420,
                                       DATA_BITWIDTH_8, 0);
  }

  // VB_SOURCE_MODULE config
  VB_CONFIG_S stVbConf;
  memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
  stVbConf.astCommPool[0].u64BlkSize = picBufSize;
  stVbConf.astCommPool[0].u32BlkCnt = VDEC_FRAME_BUF_NUM * decodeChNum;
  stVbConf.u32MaxPoolCnt = 1;
  if (tmvBufSize != 0) {
    stVbConf.astCommPool[1].u64BlkSize = tmvBufSize;
    stVbConf.astCommPool[1].u32BlkCnt = VDEC_TMV_BUF_NUM * decodeChNum;
    stVbConf.u32MaxPoolCnt = 2;
  }
  return setModuleBufPool(&stVbConf, VB_UID_VDEC);
}

bool VDecoder::startRecvStream(int decodeChNum, int width, int height,
                               PAYLOAD_TYPE_E payloadType,
                               VIDEO_DEC_MODE_E decMode) {
  VDEC_MOD_PARAM_S stModParam;
  HI_S32 s32Ret = HI_MPI_VDEC_GetModParam(&stModParam);
  if (HI_SUCCESS != s32Ret) {
    SZ_LOG_ERROR("HI_MPI_VDEC_GetModParam failed {:X}", s32Ret);
    // return false;
  }

  stModParam.enVdecVBSource = VB_SOURCE_MODULE;
  s32Ret = HI_MPI_VDEC_SetModParam(&stModParam);
  if (HI_SUCCESS != s32Ret) {
    SZ_LOG_ERROR("HI_MPI_VDEC_GetModParam failed {:X}", s32Ret);
    // return false;
  }
  // VDEC_CHN_POOL_S stPool;
  VDEC_CHN_PARAM_S stChnParam;
  VDEC_CHN_ATTR_S stChnAttr[MAX_DECODE_CH_NUM];
  for (int i = 0; i < decodeChNum; i++) {
    stChnAttr[i].enType = payloadType;
    stChnAttr[i].enMode = VIDEO_MODE_FRAME;
    stChnAttr[i].u32PicWidth = width;
    stChnAttr[i].u32PicHeight = height;
    stChnAttr[i].u32StreamBufSize = width * height;  //>=32k
    stChnAttr[i].u32FrameBufCnt = VDEC_FRAME_BUF_NUM;

    if (PT_H264 == payloadType || PT_H265 == payloadType) {
      stChnAttr[i].stVdecVideoAttr.u32RefFrameNum = VDEC_REF_FRAME_NUM;  // 5 16
      stChnAttr[i].stVdecVideoAttr.bTemporalMvpEnable = HI_TRUE;
      if (PT_H264 == payloadType && VIDEO_DEC_MODE_IPB != decMode) {
        stChnAttr[i].stVdecVideoAttr.bTemporalMvpEnable = HI_FALSE;
      }
      stChnAttr[i].u32FrameBufSize = VDEC_GetPicBufferSize(
          payloadType, width, height, PIXEL_FORMAT_YVU_SEMIPLANAR_420,
          DATA_BITWIDTH_8, 0);
      if (stChnAttr[i].stVdecVideoAttr.bTemporalMvpEnable)
        stChnAttr[i].stVdecVideoAttr.u32TmvBufSize =
            VDEC_GetTmvBufferSize(payloadType, width, height);
    } else if (PT_JPEG == payloadType || PT_MJPEG == payloadType) {
      stChnAttr[i].enMode = VIDEO_MODE_FRAME;
      stChnAttr[i].u32FrameBufSize = VDEC_GetPicBufferSize(
          payloadType, width, height, PIXEL_FORMAT_YVU_SEMIPLANAR_420,
          DATA_BITWIDTH_8, 0);
    }

    HI_U32 s32Ret = HI_MPI_VDEC_CreateChn(i, &stChnAttr[i]);
    if (s32Ret != HI_SUCCESS) {
      SZ_LOG_ERROR("HI_MPI_VDEC_CreateChn failed {:X}\n", s32Ret);
      return false;
    }

    s32Ret = HI_MPI_VDEC_GetChnParam(i, &stChnParam);
    if (HI_SUCCESS != s32Ret) {
      SZ_LOG_ERROR("HI_MPI_VDEC_GetChnParam failed {:X}", s32Ret);
      return false;
    }

    if (PT_H264 == payloadType || PT_H265 == payloadType) {
      stChnParam.stVdecVideoParam.enDecMode = decMode;
      stChnParam.stVdecVideoParam.enCompressMode = COMPRESS_MODE_TILE;
      stChnParam.stVdecVideoParam.enVideoFormat = VIDEO_FORMAT_TILE_64x16;
      if (VIDEO_DEC_MODE_IPB == decMode) {
        stChnParam.stVdecVideoParam.enOutputOrder = VIDEO_OUTPUT_ORDER_DISP;
      } else {
        stChnParam.stVdecVideoParam.enOutputOrder = VIDEO_OUTPUT_ORDER_DEC;
      }
    } else {
      stChnParam.stVdecPictureParam.enPixelFormat =
          PIXEL_FORMAT_YVU_SEMIPLANAR_420;
      stChnParam.stVdecPictureParam.u32Alpha = 255;
    }
    stChnParam.u32DisplayFrameNum = VDEC_DIS_FRAME_NUM;
    s32Ret = HI_MPI_VDEC_SetChnParam(i, &stChnParam);
    if (HI_SUCCESS != s32Ret) {
      SZ_LOG_ERROR("HI_MPI_VDEC_GetChnParam failed {:X}", s32Ret);
      return false;
    }

    s32Ret = HI_MPI_VDEC_StartRecvStream(i);
    if (HI_SUCCESS != s32Ret) {
      SZ_LOG_ERROR("HI_MPI_VDEC_StartRecvStream failed {:X}", s32Ret);
      return false;
    }
  }
  return true;
}

#if 0
bool VDecoder::startVpss() {

    VPSS_GRP_ATTR_S stVpssGrpAttr;
    stVpssGrpAttr.u32MaxW = 3840;
    stVpssGrpAttr.u32MaxH = 2160;
    stVpssGrpAttr.stFrameRate.s32SrcFrameRate = -1;
    stVpssGrpAttr.stFrameRate.s32DstFrameRate = -1;
    stVpssGrpAttr.enDynamicRange = DYNAMIC_RANGE_SDR8;
    stVpssGrpAttr.enPixelFormat  = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    stVpssGrpAttr.bNrEn   = HI_FALSE;

	VPSS_CHN_ATTR_S astVpssChnAttr[MAX_VPSS_CH_NUM];
	for (int i = 0; i < MAX_VPSS_CH_NUM; i++) {
		if (i == 0) {
			astVpssChnAttr[0].u32Width                    = VDEC_IMAGE_WIDTH;
    		astVpssChnAttr[0].u32Height                   = VDEC_IMAGE_HEIGHT;
		}
		else {
			astVpssChnAttr[i].u32Width                    = NNIE_IMAGE_WIDTH;
    		astVpssChnAttr[i].u32Height                   = NNIE_IMAGE_HEIGHT;
		}
	    astVpssChnAttr[i].enChnMode                   = VPSS_CHN_MODE_USER;
	    astVpssChnAttr[i].enCompressMode              = COMPRESS_MODE_NONE;
	    astVpssChnAttr[i].enDynamicRange              = DYNAMIC_RANGE_SDR8;
	    astVpssChnAttr[i].enPixelFormat               = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
	    astVpssChnAttr[i].stFrameRate.s32SrcFrameRate = -1;
	    astVpssChnAttr[i].stFrameRate.s32DstFrameRate = -1;
	    astVpssChnAttr[i].u32Depth                    = 8;
	    astVpssChnAttr[i].bMirror                     = HI_FALSE;
	    astVpssChnAttr[i].bFlip                       = HI_FALSE;
	    astVpssChnAttr[i].stAspectRatio.enMode        = ASPECT_RATIO_NONE;
	    astVpssChnAttr[i].enVideoFormat               = VIDEO_FORMAT_LINEAR;
	}

	Vpss *pVpss = Vpss::getInstance();
    for(int i = 0; i < MAX_DECODE_CH_NUM; i++) {
		if (!pVpss->start(i, &stVpssGrpAttr, astVpssChnAttr, MAX_VPSS_CH_NUM)) {
            return false;
		}

		if (!vdecBindVpss(i, i)) {
			return false;
		}
    }

	return true;
}


bool VDecoder::startVo() {
	Vo *pVo = Vo::getInstance();
	if (!pVo->start(0, 3840, 2160))
		return false;

	if (!vpssBindVo(0, 0, 0, 0)) {
		return false;
	}

	return true;
}

#endif
bool VDecoder::stopRecvStream(int chId) {
  HI_U32 ret = HI_MPI_VDEC_StopRecvStream(chId);
  if (HI_SUCCESS != ret) {
    SZ_LOG_ERROR("HI_MPI_VDEC_StopRecvStream failed {:X}", ret);
    return false;
  }

  ret = HI_MPI_VDEC_DestroyChn(chId);
  if (HI_SUCCESS != ret) {
    SZ_LOG_ERROR("HI_MPI_VDEC_DestroyChn failed {:X}", ret);
    return false;
  }
  return true;
}

void VDecoder::stopRecvStreams(int chNum) {
  for (int i = 0; i < chNum; i++) {
    stopRecvStream(i);
  }
}

int VDecoder::sendStream(const unsigned char *pu8Buf, int len,
                         bool bStreamOfEnd) {
  PAYLOAD_TYPE_E payloadType = PT_H264;
  assert(payloadType == PT_H264 || payloadType == PT_H265 ||
         payloadType == PT_MJPEG || payloadType == PT_JPEG);

  HI_S32 i;
  HI_U32 s32Ret;
  HI_BOOL bFindStart = HI_FALSE;
  HI_BOOL bFindEnd = HI_FALSE;
  HI_S32 u32Start = 0;
  HI_S32 s32ReadLen = 0;
  // VIDEO_MODE_FRAME
  if (payloadType == PT_H264) {
    for (i = 0; i < len - 8; i++) {
      int tmp = pu8Buf[i + 3] & 0x1F;
      if (pu8Buf[i] == 0 && pu8Buf[i + 1] == 0 && pu8Buf[i + 2] == 1 &&
          (((tmp == 0x5 || tmp == 0x1) && ((pu8Buf[i + 4] & 0x80) == 0x80)) ||
           (tmp == 20 && (pu8Buf[i + 7] & 0x80) == 0x80))) {
        bFindStart = HI_TRUE;
        i += 8;
        break;
      }
    }

    for (; i < len - 8; i++) {
      int tmp = pu8Buf[i + 3] & 0x1F;
      if (pu8Buf[i] == 0 && pu8Buf[i + 1] == 0 && pu8Buf[i + 2] == 1 &&
          (tmp == 15 || tmp == 7 || tmp == 8 || tmp == 6 ||
           ((tmp == 5 || tmp == 1) && ((pu8Buf[i + 4] & 0x80) == 0x80)) ||
           (tmp == 20 && (pu8Buf[i + 7] & 0x80) == 0x80))) {
        bFindEnd = HI_TRUE;
        break;
      }
    }

    if (i > 0) s32ReadLen = i;

    if (bFindStart == HI_FALSE) {
      SZ_LOG_ERROR("chn {0} can not find H264 start code!s32ReadLen {1}!",
                   chIndex_, s32ReadLen);
    }

    if (bFindEnd == HI_FALSE) {
      s32ReadLen = i + 8;
    }
  }  // VIDEO_MODE_FRAME
  else if (payloadType == PT_H265) {
    bool bNewPic = false;
    for (i = 0; i < len - 6; i++) {
      HI_U32 tmp = (pu8Buf[i + 3] & 0x7E) >> 1;
      bNewPic =
          (pu8Buf[i + 0] == 0 && pu8Buf[i + 1] == 0 && pu8Buf[i + 2] == 1 &&
           (tmp >= 0 && tmp <= 21) && ((pu8Buf[i + 5] & 0x80) == 0x80));
      if (bNewPic) {
        bFindStart = HI_TRUE;
        i += 6;
        break;
      }
    }

    for (; i < len - 6; i++) {
      HI_U32 tmp = (pu8Buf[i + 3] & 0x7E) >> 1;
      bNewPic =
          (pu8Buf[i + 0] == 0 && pu8Buf[i + 1] == 0 && pu8Buf[i + 2] == 1 &&
           (tmp == 32 || tmp == 33 || tmp == 34 || tmp == 39 || tmp == 40 ||
            ((tmp >= 0 && tmp <= 21) && (pu8Buf[i + 5] & 0x80) == 0x80)));
      if (bNewPic) {
        bFindEnd = HI_TRUE;
        break;
      }
    }

    if (i > 0) s32ReadLen = i;

    if (bFindStart == HI_FALSE) {
      SZ_LOG_ERROR("chn {0} can not find H265 start code! s32ReadLen {1}.!",
                   chIndex_, s32ReadLen);
    }

    if (bFindEnd == HI_FALSE) {
      s32ReadLen = i + 6;
    }
  } else if (payloadType == PT_MJPEG || payloadType == PT_JPEG) {
    for (i = 0; i < len - 1; i++) {
      if (pu8Buf[i] == 0xFF && pu8Buf[i + 1] == 0xD8) {
        u32Start = i;
        bFindStart = HI_TRUE;
        i = i + 2;
        break;
      }
    }

    for (; i < len - 3; i++) {
      if ((pu8Buf[i] == 0xFF) && (pu8Buf[i + 1] & 0xF0) == 0xE0) {
        HI_U32 u32Len = (pu8Buf[i + 2] << 8) + pu8Buf[i + 3];
        i += 1 + u32Len;
      } else {
        break;
      }
    }

    for (; i < len - 1; i++) {
      if (pu8Buf[i] == 0xFF && pu8Buf[i + 1] == 0xD9) {
        bFindEnd = HI_TRUE;
        break;
      }
    }
    s32ReadLen = i + 2;
    if (bFindStart == HI_FALSE) {
      SZ_LOG_ERROR("chn {0} can not find JPEG start code!s32ReadLen {1} !",
                   chIndex_, s32ReadLen);
    }
  }

  VDEC_STREAM_S stStream;
  stStream.u64PTS = 0;
  stStream.pu8Addr = (HI_U8 *)pu8Buf + u32Start;
  stStream.u32Len = s32ReadLen;
  stStream.bEndOfFrame = HI_TRUE;
  stStream.bEndOfStream = HI_FALSE;
  stStream.bDisplay = HI_TRUE;
  while (HI_SUCCESS !=
         (s32Ret = HI_MPI_VDEC_SendStream(chIndex_, &stStream, 0))) {
    SZ_LOG_ERROR("HI_MPI_VDEC_SendStream {:X}!", s32Ret);
    usleep(1000);
  }

  if (bStreamOfEnd) {
    /* send the flag of stream end */
    memset(&stStream, 0, sizeof(VDEC_STREAM_S));
    stStream.bEndOfStream = HI_TRUE;
    HI_MPI_VDEC_SendStream(chIndex_, &stStream, -1);
  }

  return s32ReadLen + u32Start;
}

bool VDecoder::parseFrame(MmzImage *pImage, const VIDEO_FRAME_S *pVBuf,
                          int vpssChIndex) {
  PIXEL_FORMAT_E enPixelFormat = pVBuf->enPixelFormat;
  if (PIXEL_FORMAT_YVU_SEMIPLANAR_420 != enPixelFormat) {
    SZ_LOG_ERROR("This YUV format is not support!");
    return false;
  }

  HI_U32 s32Ysize = (pVBuf->u64VirAddr[1] - pVBuf->u64VirAddr[0]);
  HI_U32 u32Size = s32Ysize * 3 / 2;
  HI_U32 u32UvHeight = pVBuf->u32Height / 2;
  HI_U64 phy_addr = pVBuf->u64PhyAddr[0];
  // HI_U8 *pY_map = (HI_U8*) HI_MPI_SYS_Mmap(phy_addr, u32Size);
  /*HI_U8 *pY_map = (HI_U8*)pVBuf->u64VirAddr[0];
  if (HI_NULL == pY_map) {
      printf("HI_MPI_SYS_Mmap for pY_map fail!!\n");
      return false;
  }*/

  // printf("%d  %d   %d  %d   %d \n", pVBuf->u32Width, pVBuf->u32Height,
  // vpssChIndex, pVBuf->u32Stride[0], pVBuf->u32Stride[1]);
#if 0
	int offset = 0;
	//static cv::Mat image(2160, 3840, CV_8UC3);
	static unsigned char *pData = (unsigned char *)malloc(2160 * 3840 * 3 / 2);
    for (int h = 0; h < pVBuf->u32Height; h++, offset += pVBuf->u32Width) {
        HI_U8 *pYRowContent = pY_map + h * pVBuf->u32Stride[0];
		//memcpy(pImage->pData + offset, pYRowContent, pVBuf->u32Width);
		memcpy(pData + offset, pYRowContent, pVBuf->u32Width);
    }


	HI_U8 *pC_map = pY_map + s32Ysize;
    for (int h = 0; h < u32UvHeight; h++, offset += pVBuf->u32Width) {
        HI_U8 *pVuRowContent = pC_map + h * pVBuf->u32Stride[1];
		//memcpy(pImage->pData + offset, pVuRowContent, pVBuf->u32Width);
		memcpy(pData + offset, pVuRowContent, pVBuf->u32Width);
    }

#endif
  if (vpssChIndex == 0) {
    pIveApp_->copyGrayImage(pImage, phy_addr);

    static bool bSave = false;
    static int cnt = 0;
    if (!bSave) {
      if (cnt == 10) {
        /*cv::Mat image(2160, 3840, CV_8UC1);
        memcpy(image.data, pImage->pData, 2160 * 3840);
        cv::imwrite(("yuv" + std::to_string(cnt) + "_"  +
        std::to_string(chIndex_) + ".png" ).c_str(), image);*/

        HI_U8 *pY_map = (HI_U8 *)HI_MPI_SYS_Mmap(phy_addr, u32Size);
        fwrite(pY_map, 1, pVBuf->u32Width * pVBuf->u32Height * 3 / 2, fp_out);
        HI_MPI_SYS_Munmap(pY_map, u32Size);
      }

      if (++cnt > 10) {
        fclose(fp_out);
        bSave = true;
      }
    }
  } else {
    pIveApp_->copyYuvImage(pImage, phy_addr);
    // pIveApp_->copyRgbImage(pImage, phy_addr);
  }
  // HI_MPI_SYS_Munmap(pY_map, u32Size);
  return true;
}

bool VDecoder::getFrame(MmzImage *pImage, int vpssChIndex) {
  VIDEO_FRAME_INFO_S stVFrameInfo;
  struct timeval tv1, tv2;
  struct timezone tz1, tz2;
  bool bOk = false;
  gettimeofday(&tv1, &tz1);
  int vpssGrpIndex = chIndex_;
  HI_S32 s32Ret =
      HI_MPI_VPSS_GetChnFrame(vpssGrpIndex, vpssChIndex, &stVFrameInfo, 0);
  if (HI_SUCCESS != s32Ret) {
    if (HI_ERR_VPSS_BUF_EMPTY != s32Ret)
      SZ_LOG_ERROR("HI_MPI_VPSS_GetChnFrame failed {:X}", s32Ret);
  } else {
    bOk = parseFrame(pImage, &stVFrameInfo.stVFrame, vpssChIndex);
    HI_MPI_VPSS_ReleaseChnFrame(vpssGrpIndex, vpssChIndex, &stVFrameInfo);
  }
  gettimeofday(&tv2, &tz2);
  float ms = getElapse(&tv1, &tv2);
  if (ms > 2)
    SZ_LOG_ERROR("getFrame time={0:03.2f}  {1}  {2}", ms, bOk, chIndex_);
  return bOk;
}

VDecoder::~VDecoder() {
  delete pIveApp_;

  if (!bInit_) return;
  bInit_ = false;

  stopRecvStreams(MAX_DECODE_CH_NUM);

  HI_S32 s32Ret = HI_MPI_VB_ExitModCommPool(VB_UID_VDEC);
  if (HI_SUCCESS != s32Ret) {
    SZ_LOG_ERROR("HI_MPI_VB_ExitModCommPool failed! {:X}", s32Ret);
  }
}
