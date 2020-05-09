#include "hdmi.h"

#include "lib/logger.h"
#include "mpi_hdmi.h"

Hdmi::Hdmi() {}

Hdmi *Hdmi::getInstance() {
  static Hdmi self;
  return &self;
}

bool Hdmi::start(HI_HDMI_ID_E hdmiId, int width, int height) {
  /******************************
   * Start hdmi device.
   * Note : do this after vo device started.
   ********************************/
  HI_U32 s32Ret = HI_MPI_HDMI_Init();
  if (s32Ret != HI_SUCCESS) {
    SZ_LOG_ERROR("HI_MPI_HDMI_Init failed! {:X}", s32Ret);
    return false;
  }

  s32Ret = HI_MPI_HDMI_Open(hdmiId);  // HI_HDMI_ID_0
  if (s32Ret != HI_SUCCESS) {
    SZ_LOG_ERROR("HI_MPI_HDMI_Open failed! {:X}", s32Ret);
    return false;
  }

  HI_HDMI_ATTR_S stAttr;
  s32Ret = HI_MPI_HDMI_GetAttr(hdmiId, &stAttr);
  if (s32Ret != HI_SUCCESS) {
    SZ_LOG_ERROR("HI_MPI_HDMI_GetAttr failed! {:X}", s32Ret);
    return false;
  }
  stAttr.bEnableHdmi = HI_TRUE;
  stAttr.bEnableVideo = HI_TRUE;
  stAttr.enVideoFmt = getHdmiVideoFmt(
      width,
      height);  // HI_HDMI_VIDEO_FMT_1080P_30 //HI_HDMI_VIDEO_FMT_3840X2160P_30;
  stAttr.enVidInMode = HI_HDMI_VIDEO_MODE_YCBCR444;
  stAttr.enVidOutMode = HI_HDMI_VIDEO_MODE_YCBCR444;
  stAttr.enDeepColorMode = HI_HDMI_DEEP_COLOR_24BIT;
  stAttr.bxvYCCMode = HI_FALSE;
  stAttr.enOutCscQuantization = HDMI_QUANTIZATION_LIMITED_RANGE;
  stAttr.bEnableAudio = HI_FALSE;
  stAttr.enSoundIntf = HI_HDMI_SND_INTERFACE_I2S;
  stAttr.bIsMultiChannel = HI_FALSE;
  stAttr.enBitDepth = HI_HDMI_BIT_DEPTH_16;
  stAttr.bEnableAviInfoFrame = HI_TRUE;
  stAttr.bEnableAudInfoFrame = HI_TRUE;
  stAttr.bEnableSpdInfoFrame = HI_FALSE;
  stAttr.bEnableMpegInfoFrame = HI_FALSE;
  stAttr.bDebugFlag = HI_FALSE;
  stAttr.bHDCPEnable = HI_FALSE;
  stAttr.b3DEnable = HI_FALSE;
  stAttr.enDefaultMode = HI_HDMI_FORCE_HDMI;

  s32Ret = HI_MPI_HDMI_SetAttr(hdmiId, &stAttr);
  if (s32Ret != HI_SUCCESS) {
    SZ_LOG_ERROR("HI_MPI_HDMI_SetAttr failed! {:X}", s32Ret);
    return false;
  }

  s32Ret = HI_MPI_HDMI_Start(hdmiId);
  if (s32Ret != HI_SUCCESS) {
    SZ_LOG_ERROR("HI_MPI_HDMI_Start failed! {:X}", s32Ret);
    return false;
  }

  return true;
}

HI_HDMI_VIDEO_FMT_E Hdmi::getHdmiVideoFmt(int width, int height) {
  if (width == 1920 && height == 1080) {
    return HI_HDMI_VIDEO_FMT_1080P_30;  // HI_HDMI_VIDEO_FMT_1080P_60
  } else if (width == 1280 && height == 800) {
    return HI_HDMI_VIDEO_FMT_VESA_1280X800_60;
  } else if (width == 3840 && height == 2160) {
    return HI_HDMI_VIDEO_FMT_3840X2160P_30;
  } else {
    SZ_LOG_WARN("getHdmiVideoFmt failed, can't support width={0} height={1}",
                width, height);
    return HI_HDMI_VIDEO_FMT_1080P_30;
  }
}

int Hdmi::getDisFrameRate(HI_HDMI_VIDEO_FMT_E hdmiVideoFmt) {
  switch (hdmiVideoFmt) {
    case HI_HDMI_VIDEO_FMT_1080P_30:
    case HI_HDMI_VIDEO_FMT_3840X2160P_30:
      return 30;
      break;
    case HI_HDMI_VIDEO_FMT_1080P_60:
      return 60;
      break;
  }
  return 30;
}

void Hdmi::stop(HI_HDMI_ID_E hdmiId) {
  HI_MPI_HDMI_Stop(hdmiId);
  HI_MPI_HDMI_Close(hdmiId);
  HI_MPI_HDMI_DeInit();
}
