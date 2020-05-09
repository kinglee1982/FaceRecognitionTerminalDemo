#include "isp.h"

#include <pthread.h>
#include <sys/prctl.h>

#include "hi_ae_comm.h"
#include "hi_awb_comm.h"
#include "lib/logger.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "mpi_isp.h"

static ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX327_2M_30FPS = {
    {0, 0, 1920, 1080}, {1920, 1080}, 30, BAYER_RGGB, WDR_MODE_NONE, 0,
};

static ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX327_MIPI_2M_30FPS_WDR2TO1_LINE = {
    {0, 0, 1920, 1080}, {1920, 1080}, 30, BAYER_RGGB, WDR_MODE_2To1_LINE, 0,
};

static ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX307_MIPI_2M_30FPS = {
    {0, 0, 1920, 1080}, {1920, 1080}, 30, BAYER_RGGB, WDR_MODE_NONE, 0,
};

static ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX307_MIPI_2M_30FPS_WDR2TO1_LINE = {
    {0, 0, 1920, 1080}, {1920, 1080}, 30, BAYER_RGGB, WDR_MODE_2To1_LINE, 0,
};

static ISP_PUB_ATTR_S ISP_PUB_ATTR_MN34220_LVDS_2M_30FPS = {
    {0, 0, 1920, 1080}, {1920, 1080}, 30, BAYER_GRBG, WDR_MODE_NONE, 0,
};

static ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX335_MIPI_5M_30FPS = {
    {0, 0, 2592, 1944}, {2592, 1944}, 30, BAYER_RGGB, WDR_MODE_NONE, 0,
};

static ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX335_MIPI_5M_30FPS_WDR2TO1 = {
    {0, 0, 2592, 1944}, {2592, 1944}, 30, BAYER_RGGB, WDR_MODE_2To1_LINE, 0,
};

static ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX335_MIPI_4M_30FPS = {
    {0, 0, 2592, 1536}, {2592, 1944}, 30, BAYER_RGGB, WDR_MODE_NONE, 0,
};

static ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX335_MIPI_4M_30FPS_WDR2TO1 = {
    {0, 0, 2592, 1536}, {2592, 1944}, 30, BAYER_RGGB, WDR_MODE_2To1_LINE, 0,
};

static ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX458_MIPI_8M_30FPS = {
    {0, 0, 3840, 2160}, {3840, 2160}, 30, BAYER_RGGB, WDR_MODE_NONE, 0,
};

static ISP_PUB_ATTR_S ISP_PUB_ATTR_SC4210_MIPI_3M_30FPS = {
    {0, 0, 2560, 1440}, {2560, 1440}, 30, BAYER_BGGR, WDR_MODE_NONE, 0,
};

static ISP_PUB_ATTR_S ISP_PUB_ATTR_SC4210_MIPI_3M_30FPS_WDR2TO1 = {
    {0, 0, 2560, 1440}, {2560, 1440}, 30, BAYER_BGGR, WDR_MODE_2To1_LINE, 0,
};

static ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX458_MIPI_12M_20FPS = {
    {0, 0, 4000, 3000}, {4000, 3000}, 19.98, BAYER_RGGB, WDR_MODE_NONE, 0,
};

static ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX458_MIPI_4M_60FPS = {
    {0, 0, 2716, 1524}, {2716, 1524}, 59.94, BAYER_RGGB, WDR_MODE_NONE, 0,
};

static ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX458_MIPI_4M_40FPS = {
    {0, 0, 2716, 1524}, {2716, 1524}, 40.62, BAYER_RGGB, WDR_MODE_NONE, 1,
};

static ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX458_MIPI_2M_90FPS = {
    {0, 0, 1920, 1080}, {1920, 1080}, 90.11, BAYER_RGGB, WDR_MODE_NONE, 0,
};

static ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX458_MIPI_1M_129FPS = {
    {0, 0, 1280, 720}, {1280, 720}, 128.80, BAYER_RGGB, WDR_MODE_NONE, 0,
};

static ISP_PUB_ATTR_S ISP_PUB_ATTR_OS04B10_MIPI_4M_30FPS = {
    {0, 0, 2560, 1440}, {2560, 1440}, 30, BAYER_GBRG, WDR_MODE_NONE, 0,
};

static ISP_PUB_ATTR_S ISP_PUB_ATTR_OS05A_MIPI_4M_30FPS = {
    {0, 0, 2688, 1536}, {2688, 1944}, 30, BAYER_BGGR, WDR_MODE_NONE, 0,
};

static ISP_PUB_ATTR_S ISP_PUB_ATTR_OS05A_MIPI_4M_30FPS_WDR2TO1 = {
    {0, 0, 2688, 1536}, {2688, 1944}, 30, BAYER_BGGR, WDR_MODE_2To1_LINE, 0,
};

static ISP_PUB_ATTR_S ISP_PUB_ATTR_OS08A10_MIPI_8M_30FPS = {
    {0, 0, 3840, 2160}, {3840, 2160}, 30, BAYER_BGGR, WDR_MODE_NONE, 0,
};

static ISP_PUB_ATTR_S ISP_PUB_ATTR_GC2053_MIPI_2M_30FPS = {
    {0, 0, 1920, 1080}, {1920, 1080}, 30, BAYER_RGGB, WDR_MODE_NONE, 0,
};

static ISP_PUB_ATTR_S ISP_PUB_ATTR_OV12870_MIPI_1M_240FPS = {
    {0, 0, 1280, 720}, {1280, 720}, 240, BAYER_BGGR, WDR_MODE_NONE, 0,
};

static ISP_PUB_ATTR_S ISP_PUB_ATTR_OV12870_MIPI_2M_120FPS = {
    {0, 0, 1920, 1080}, {1920, 1080}, 120, BAYER_BGGR, WDR_MODE_NONE, 0,
};

static ISP_PUB_ATTR_S ISP_PUB_ATTR_OV12870_MIPI_8M_30FPS = {
    {0, 0, 3840, 2160}, {3840, 2160}, 30, BAYER_BGGR, WDR_MODE_NONE, 0,
};

static ISP_PUB_ATTR_S ISP_PUB_ATTR_OV12870_MIPI_12M_30FPS = {
    {0, 0, 4000, 3000}, {4000, 3000}, 30, BAYER_BGGR, WDR_MODE_NONE, 0,
};

static ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX415_MIPI_8M_30FPS = {
    {0, 0, 3840, 2160}, {3840, 2160}, 30, BAYER_GBRG, WDR_MODE_NONE, 0,
};

static ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX415_MIPI_8M_20FPS = {
    {0, 0, 3840, 2160}, {3840, 2160}, 20, BAYER_GBRG, WDR_MODE_NONE, 1,
};

void Isp::getSizeBySensor(SensorType sensorType, int &width, int &height) {
  switch (sensorType) {
    case SONY_IMX327_MIPI_2M_30FPS_12BIT:
    case SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1:
    case SONY_IMX307_MIPI_2M_30FPS_12BIT:
    case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
    case PANASONIC_MN34220_LVDS_2M_30FPS_12BIT:
    case SONY_IMX458_MIPI_2M_90FPS_10BIT:
    case OMNIVISION_OV12870_MIPI_2M_120FPS_10BIT:
      width = 1920;
      height = 1080;
      break;

    case SONY_IMX335_MIPI_5M_30FPS_12BIT:
    case SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1:
      width = 2592;
      height = 1944;
      break;

    case SONY_IMX335_MIPI_4M_30FPS_12BIT:
    case SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1:
      width = 2592;
      height = 1536;
      break;

    case SMART_SC4210_MIPI_3M_30FPS_12BIT:
    case SMART_SC4210_MIPI_3M_30FPS_10BIT_WDR2TO1:
      width = 2560;
      height = 1440;
      break;

    case OMNIVISION_OS08A10_MIPI_8M_30FPS_10BIT:
    case SONY_IMX458_MIPI_8M_30FPS_10BIT:
    case OMNIVISION_OV12870_MIPI_8M_30FPS_10BIT:
    case SONY_IMX415_MIPI_8M_30FPS_12BIT:
    case SONY_IMX415_MIPI_8M_20FPS_12BIT:
      width = 3840;
      height = 2160;
      break;
    case SONY_IMX458_MIPI_12M_20FPS_10BIT:
    case OMNIVISION_OV12870_MIPI_12M_30FPS_10BIT:
      width = 4000;
      height = 3000;
      break;
    case SONY_IMX458_MIPI_4M_60FPS_10BIT:
    case SONY_IMX458_MIPI_4M_40FPS_10BIT:
      width = 2716;
      height = 1524;
      break;

    case SONY_IMX458_MIPI_1M_129FPS_10BIT:
    case OMNIVISION_OV12870_MIPI_1M_240FPS_10BIT:
      width = 1280;
      height = 720;
      break;

    case OMNIVISION_OS04B10_MIPI_4M_30FPS_10BIT:
      width = 2560;
      height = 1440;
      break;

    case OMNIVISION_OS05A_MIPI_4M_30FPS_12BIT:
    case OMNIVISION_OS05A_MIPI_4M_30FPS_10BIT_WDR2TO1:
      width = 2688;
      height = 1536;
      break;
    default:
      width = 1920;
      height = 1080;
      break;
  }
}

void Isp::getIspAttrBySensor(SensorType sensorType,
                             ISP_PUB_ATTR_S *pstPubAttr) {
  switch (sensorType) {
    case SONY_IMX327_MIPI_2M_30FPS_12BIT:
    case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT:
      memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX327_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
      break;

    case SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1:
    case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
      memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX327_MIPI_2M_30FPS_WDR2TO1_LINE,
             sizeof(ISP_PUB_ATTR_S));
      break;

    case SONY_IMX307_MIPI_2M_30FPS_12BIT:
    case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT:
      memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX307_MIPI_2M_30FPS,
             sizeof(ISP_PUB_ATTR_S));
      break;

    case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
    case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
      memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX307_MIPI_2M_30FPS_WDR2TO1_LINE,
             sizeof(ISP_PUB_ATTR_S));
      break;

    case SONY_IMX335_MIPI_5M_30FPS_12BIT:
      memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX335_MIPI_5M_30FPS,
             sizeof(ISP_PUB_ATTR_S));
      break;

    case SONY_IMX335_MIPI_4M_30FPS_12BIT:
      memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX335_MIPI_4M_30FPS,
             sizeof(ISP_PUB_ATTR_S));
      break;

    case SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1:
      memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX335_MIPI_5M_30FPS_WDR2TO1,
             sizeof(ISP_PUB_ATTR_S));
      break;

    case SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1:
      memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX335_MIPI_4M_30FPS_WDR2TO1,
             sizeof(ISP_PUB_ATTR_S));
      break;

    case SMART_SC4210_MIPI_3M_30FPS_12BIT:
      memcpy(pstPubAttr, &ISP_PUB_ATTR_SC4210_MIPI_3M_30FPS,
             sizeof(ISP_PUB_ATTR_S));
      break;

    case SMART_SC4210_MIPI_3M_30FPS_10BIT_WDR2TO1:
      memcpy(pstPubAttr, &ISP_PUB_ATTR_SC4210_MIPI_3M_30FPS_WDR2TO1,
             sizeof(ISP_PUB_ATTR_S));
      break;

    case SONY_IMX458_MIPI_8M_30FPS_10BIT:
      memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX458_MIPI_8M_30FPS,
             sizeof(ISP_PUB_ATTR_S));
      break;

    case SONY_IMX458_MIPI_12M_20FPS_10BIT:
      memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX458_MIPI_12M_20FPS,
             sizeof(ISP_PUB_ATTR_S));
      break;

    case SONY_IMX458_MIPI_4M_60FPS_10BIT:
      memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX458_MIPI_4M_60FPS,
             sizeof(ISP_PUB_ATTR_S));
      break;

    case SONY_IMX458_MIPI_4M_40FPS_10BIT:
      memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX458_MIPI_4M_40FPS,
             sizeof(ISP_PUB_ATTR_S));
      break;

    case SONY_IMX458_MIPI_2M_90FPS_10BIT:
      memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX458_MIPI_2M_90FPS,
             sizeof(ISP_PUB_ATTR_S));
      break;

    case SONY_IMX458_MIPI_1M_129FPS_10BIT:
      memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX458_MIPI_1M_129FPS,
             sizeof(ISP_PUB_ATTR_S));
      break;

    case PANASONIC_MN34220_LVDS_2M_30FPS_12BIT:
      memcpy(pstPubAttr, &ISP_PUB_ATTR_MN34220_LVDS_2M_30FPS,
             sizeof(ISP_PUB_ATTR_S));
      break;

    case OMNIVISION_OS04B10_MIPI_4M_30FPS_10BIT:
      memcpy(pstPubAttr, &ISP_PUB_ATTR_OS04B10_MIPI_4M_30FPS,
             sizeof(ISP_PUB_ATTR_S));
      break;

    case OMNIVISION_OS05A_MIPI_4M_30FPS_12BIT:
      memcpy(pstPubAttr, &ISP_PUB_ATTR_OS05A_MIPI_4M_30FPS,
             sizeof(ISP_PUB_ATTR_S));
      break;

    case OMNIVISION_OS05A_MIPI_4M_30FPS_10BIT_WDR2TO1:
      memcpy(pstPubAttr, &ISP_PUB_ATTR_OS05A_MIPI_4M_30FPS_WDR2TO1,
             sizeof(ISP_PUB_ATTR_S));
      break;

    case OMNIVISION_OS08A10_MIPI_8M_30FPS_10BIT:
      memcpy(pstPubAttr, &ISP_PUB_ATTR_OS08A10_MIPI_8M_30FPS,
             sizeof(ISP_PUB_ATTR_S));
      break;

    case GALAXYCORE_GC2053_MIPI_2M_30FPS_10BIT:
      memcpy(pstPubAttr, &ISP_PUB_ATTR_GC2053_MIPI_2M_30FPS,
             sizeof(ISP_PUB_ATTR_S));
      break;
    case OMNIVISION_OV12870_MIPI_1M_240FPS_10BIT:
      memcpy(pstPubAttr, &ISP_PUB_ATTR_OV12870_MIPI_1M_240FPS,
             sizeof(ISP_PUB_ATTR_S));
      break;
    case OMNIVISION_OV12870_MIPI_2M_120FPS_10BIT:
      memcpy(pstPubAttr, &ISP_PUB_ATTR_OV12870_MIPI_2M_120FPS,
             sizeof(ISP_PUB_ATTR_S));
      break;
    case OMNIVISION_OV12870_MIPI_8M_30FPS_10BIT:
      memcpy(pstPubAttr, &ISP_PUB_ATTR_OV12870_MIPI_8M_30FPS,
             sizeof(ISP_PUB_ATTR_S));
      break;
    case OMNIVISION_OV12870_MIPI_12M_30FPS_10BIT:
      memcpy(pstPubAttr, &ISP_PUB_ATTR_OV12870_MIPI_12M_30FPS,
             sizeof(ISP_PUB_ATTR_S));
      break;

    case SONY_IMX415_MIPI_8M_30FPS_12BIT:
      memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX415_MIPI_8M_30FPS,
             sizeof(ISP_PUB_ATTR_S));
      break;
    case SONY_IMX415_MIPI_8M_20FPS_12BIT:
      memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX415_MIPI_8M_20FPS,
             sizeof(ISP_PUB_ATTR_S));
      break;

    default:
      memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX327_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
      break;
  }
}

ISP_SNS_OBJ_S *Isp::getSensorObj(SensorType sensorType) {
  switch (sensorType) {
      /*case SONY_IMX327_MIPI_2M_30FPS_12BIT:
      case SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1:
          return &stSnsImx327Obj;*/

    case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT:
    case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
      return &stSnsImx327_2l_Obj;

    case SONY_IMX307_MIPI_2M_30FPS_12BIT:
    case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
      return &stSnsImx307Obj;

#if 0
        case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT:
        case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
            return &stSnsImx307_2l_Obj;

        case SONY_IMX335_MIPI_5M_30FPS_12BIT:
        case SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1:
        case SONY_IMX335_MIPI_4M_30FPS_12BIT:
        case SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1:
            return &stSnsImx335Obj;

        case SMART_SC4210_MIPI_3M_30FPS_12BIT:
        case SMART_SC4210_MIPI_3M_30FPS_10BIT_WDR2TO1:
            return &stSnsSc4210Obj;

        case SONY_IMX458_MIPI_8M_30FPS_10BIT:
        case SONY_IMX458_MIPI_12M_20FPS_10BIT:
        case SONY_IMX458_MIPI_4M_60FPS_10BIT:
        case SONY_IMX458_MIPI_4M_40FPS_10BIT:
        case SONY_IMX458_MIPI_2M_90FPS_10BIT:
        case SONY_IMX458_MIPI_1M_129FPS_10BIT:
            return &stSnsImx458Obj;

        case PANASONIC_MN34220_LVDS_2M_30FPS_12BIT:
            return &stSnsMn34220Obj;

        case OMNIVISION_OS04B10_MIPI_4M_30FPS_10BIT:
            return &stSnsOs04b10_2lObj;

        case OMNIVISION_OS05A_MIPI_4M_30FPS_12BIT:
        case OMNIVISION_OS05A_MIPI_4M_30FPS_10BIT_WDR2TO1:
            return &stSnsOs05aObj;

        case OMNIVISION_OS08A10_MIPI_8M_30FPS_10BIT:
            return &stSnsOS08A10Obj;

        case GALAXYCORE_GC2053_MIPI_2M_30FPS_10BIT:
            return &stSnsGc2053Obj;
        case OMNIVISION_OV12870_MIPI_1M_240FPS_10BIT:
        case OMNIVISION_OV12870_MIPI_2M_120FPS_10BIT:
        case OMNIVISION_OV12870_MIPI_8M_30FPS_10BIT:
        case OMNIVISION_OV12870_MIPI_12M_30FPS_10BIT:
            return &stSnsOv12870Obj;

        case SONY_IMX415_MIPI_8M_30FPS_12BIT:
        case SONY_IMX415_MIPI_8M_20FPS_12BIT:
            return &stSnsImx415Obj;
#endif
    default:
      return HI_NULL;
  }
}

bool Isp::regiterSensorCallback(VI_PIPE viPipe, SensorType sensorType) {
  ISP_SNS_OBJ_S *pSensorObj = getSensorObj(sensorType);
  if (HI_NULL == pSensorObj) {
    SZ_LOG_ERROR("sensor {} not exist!\n", sensorType);
    return false;
  }

  ALG_LIB_S stAeLib;
  ALG_LIB_S stAwbLib;
  stAeLib.s32Id = viPipe;
  stAwbLib.s32Id = viPipe;
  strncpy(stAeLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
  strncpy(stAwbLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
  // strncpy(stAfLib.acLibName, HI_AF_LIB_NAME, sizeof(HI_AF_LIB_NAME));
  if (pSensorObj->pfnRegisterCallback != HI_NULL) {
    HI_U32 s32Ret =
        pSensorObj->pfnRegisterCallback(viPipe, &stAeLib, &stAwbLib);
    if (s32Ret != HI_SUCCESS) {
      SZ_LOG_ERROR("sensor_register_callback failed with {:X}!\n", s32Ret);
      return false;
    }
    return true;
  } else {
    SZ_LOG_ERROR("sensor_register_callback failed with HI_NULL!\n");
    return false;
  }
}

bool Isp::unregiterSensorCallback(VI_PIPE viPipe, SensorType sensorType) {
  const ISP_SNS_OBJ_S *pSensorObj = getSensorObj(sensorType);
  if (HI_NULL == pSensorObj) {
    return false;
  }

  ALG_LIB_S stAeLib;
  ALG_LIB_S stAwbLib;
  stAeLib.s32Id = viPipe;
  stAwbLib.s32Id = viPipe;
  strncpy(stAeLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
  strncpy(stAwbLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
  // strncpy(stAfLib.acLibName, HI_AF_LIB_NAME, sizeof(HI_AF_LIB_NAME));
  if (pSensorObj->pfnUnRegisterCallback != HI_NULL) {
    HI_U32 s32Ret =
        pSensorObj->pfnUnRegisterCallback(viPipe, &stAeLib, &stAwbLib);
    if (s32Ret != HI_SUCCESS) {
      SZ_LOG_ERROR("sensor_unregister_callback failed with {:X}!", s32Ret);
      return false;
    }
    return true;
  } else {
    SZ_LOG_ERROR("sensor_unregister_callback failed with HI_NULL!");
    return false;
  }
}

bool Isp::registerAelibCallback(VI_PIPE viPipe) {
  ALG_LIB_S stAeLib;
  stAeLib.s32Id = viPipe;
  strncpy(stAeLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
  HI_U32 ret = HI_MPI_AE_Register(viPipe, &stAeLib);
  if (HI_SUCCESS != ret) {
    SZ_LOG_ERROR("HI_MPI_AE_Register failed {:X}", ret);
    return false;
  }
  return true;
}

bool Isp::unregisterAelibCallback(VI_PIPE viPipe) {
  ALG_LIB_S stAeLib;
  stAeLib.s32Id = viPipe;
  strncpy(stAeLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
  HI_U32 ret = HI_MPI_AE_UnRegister(viPipe, &stAeLib);
  if (HI_SUCCESS != ret) {
    SZ_LOG_ERROR("HI_MPI_AE_UnRegister failed {:X}", ret);
    return false;
  }
  return true;
}

bool Isp::registerAwblibCallback(VI_PIPE viPipe) {
  ALG_LIB_S stAwbLib;
  stAwbLib.s32Id = viPipe;
  strncpy(stAwbLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));

  HI_U32 ret = HI_MPI_AWB_Register(viPipe, &stAwbLib);
  if (HI_SUCCESS != ret) {
    SZ_LOG_ERROR("HI_MPI_AWB_Register failed {:X}", ret);
    return false;
  }
  return true;
}

bool Isp::unregisterAwblibCallback(VI_PIPE viPipe) {
  ALG_LIB_S stAwbLib;
  stAwbLib.s32Id = viPipe;
  strncpy(stAwbLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
  HI_U32 ret = HI_MPI_AWB_UnRegister(viPipe, &stAwbLib);
  if (HI_SUCCESS != ret) {
    SZ_LOG_ERROR("HI_MPI_AWB_UnRegister failed {:X}", ret);
    return false;
  }
  return true;
}

void Isp::stop(VI_PIPE viPipe, SensorType sensorType) {
  if (pid_ != 0) {
    HI_MPI_ISP_Exit(viPipe);
    pthread_join(pid_, NULL);
    unregisterAwblibCallback(viPipe);
    unregisterAelibCallback(viPipe);
    unregiterSensorCallback(viPipe, sensorType);
    pid_ = 0;
  }
}

bool Isp::bindSensor(VI_PIPE viPipe, SensorType sensorType, HI_S8 busId) {
  const ISP_SNS_OBJ_S *pSensorObj = getSensorObj(sensorType);
  if (HI_NULL == pSensorObj) {
    SZ_LOG_ERROR("sensor {} not exist!\n", sensorType);
    return false;
  }

  ISP_SNS_COMMBUS_U uSnsBusInfo;
  ISP_SNS_TYPE_E busType = ISP_SNS_I2C_TYPE;
  if (ISP_SNS_I2C_TYPE == busType) {
    uSnsBusInfo.s8I2cDev = busId;
  } else {
    uSnsBusInfo.s8SspDev.bit4SspDev = busId;
    uSnsBusInfo.s8SspDev.bit4SspCs = 0;
  }

  if (HI_NULL != pSensorObj->pfnSetBusInfo) {
    HI_U32 s32Ret = pSensorObj->pfnSetBusInfo(viPipe, uSnsBusInfo);
    if (s32Ret != HI_SUCCESS) {
      SZ_LOG_ERROR("set sensor bus info failed with {:X}!", s32Ret);
      return false;
    }
    return true;
  } else {
    SZ_LOG_ERROR("not support set sensor bus info!");
    return false;
  }
}

void *Isp::ispThread(void *param) {
  VI_PIPE viPipe = (VI_PIPE)param;
  HI_CHAR szThreadName[20];
  snprintf(szThreadName, 20, "ISP%d_RUN", viPipe);
  prctl(PR_SET_NAME, szThreadName, 0, 0, 0);
  SZ_LOG_INFO("ISP Dev {} running !", viPipe);

  HI_U32 s32Ret = HI_MPI_ISP_Run(viPipe);
  if (HI_SUCCESS != s32Ret) {
    SZ_LOG_ERROR("HI_MPI_ISP_Run failed with {:X}!", s32Ret);
  }
  return NULL;
}

bool Isp::startRun(VI_PIPE viPipe) {
  pthread_attr_t *pstAttr = NULL;
  HI_U32 s32Ret = pthread_create(&pid_, pstAttr, ispThread, (HI_VOID *)viPipe);
  if (0 != s32Ret) {
    SZ_LOG_ERROR("create isp running thread failed!, error: {:X}, {}", s32Ret,
                 strerror(s32Ret));
    if (NULL != pstAttr) {
      pthread_attr_destroy(pstAttr);
    }
    return false;
  }
  return true;
}

bool Isp::start(VI_PIPE viPipe, SensorType sensorType, HI_S8 busId) {
  ISP_PUB_ATTR_S stPubAttr;
  getIspAttrBySensor(sensorType, &stPubAttr);
  stPubAttr.enWDRMode = WDR_MODE_NONE;
  HI_BOOL bNeedPipe;
  if (WDR_MODE_NONE == stPubAttr.enWDRMode) {
    bNeedPipe = HI_TRUE;
  } else {
    bNeedPipe = (viPipe > 0) ? HI_FALSE : HI_TRUE;
  }

  if (HI_TRUE != bNeedPipe) {
    return false;
  }

  if (!regiterSensorCallback(viPipe, sensorType)) {
    SZ_LOG_ERROR("register sensor {} to ISP {} failed\n", sensorType, viPipe);
    return false;
  }

  HI_BOOL bDoublePipe = HI_FALSE;
  VI_PIPE snapPipeIndex = 0;
  HI_BOOL bMultiPipe = HI_FALSE;
  HI_S8 s8BusId = busId;
  if ((bDoublePipe && snapPipeIndex == viPipe) || (bMultiPipe && viPipe > 0)) {
    s8BusId = -1;
  }
  if (!bindSensor(viPipe, sensorType, busId)) {
    return false;
  }

  if (!registerAelibCallback(viPipe)) return false;

  if (!registerAwblibCallback(viPipe)) return false;

  HI_U32 s32Ret = HI_MPI_ISP_MemInit(viPipe);
  if (s32Ret != HI_SUCCESS) {
    SZ_LOG_ERROR("HI_MPI_ISP_MemInit failed {:X}!\n", s32Ret);
    return false;
  }

  s32Ret = HI_MPI_ISP_SetPubAttr(viPipe, &stPubAttr);
  if (s32Ret != HI_SUCCESS) {
    SZ_LOG_ERROR("HI_MPI_ISP_SetPubAttr failed with {:X}!\n", s32Ret);
    return false;
  }

  s32Ret = HI_MPI_ISP_Init(viPipe);
  if (s32Ret != HI_SUCCESS) {
    SZ_LOG_ERROR("HI_MPI_ISP_Init failed with {:X}!\n", s32Ret);
    return false;
  }

  if (!startRun(viPipe)) {
    return false;
  }
  return true;
}

bool Isp::setSensorSaturationZero(VI_PIPE viPipe) {
  /* isp saturation attr */
  /* config isp saturation added by rpdzk ivy */
  ISP_SATURATION_ATTR_S stSaturationAttr;
  HI_U32 s32Ret = HI_MPI_ISP_GetSaturationAttr(viPipe, &stSaturationAttr);
  if (HI_SUCCESS != s32Ret) {
    SZ_LOG_ERROR("HI_MPI_ISP_GetSaturationAttr Failed {:X}!", s32Ret);
    return false;
  }

  stSaturationAttr.enOpType = OP_TYPE_MANUAL;
  stSaturationAttr.stManual.u8Saturation = 0x0;
  s32Ret = HI_MPI_ISP_SetSaturationAttr(viPipe, &stSaturationAttr);
  if (HI_SUCCESS != s32Ret) {
    SZ_LOG_ERROR("HI_MPI_ISP_SetSaturationAttr Failed {:X}!", s32Ret);
    return false;
  }

  return true;
}

Isp::Isp() { pid_ = 0; }

Isp *Isp::getInstance() {
  static Isp self;
  return &self;
}
