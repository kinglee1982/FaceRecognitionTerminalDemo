#include "mipi.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "hi_buffer.h"
#include "hi_mipi.h"
#include "hi_securec.h"
#include "lib/logger.h"

#define MIPI_DEV_NODE "/dev/hi_mipi"

static combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX327_12BIT_2M_NOWDR_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {.mipi_attr = {DATA_TYPE_RAW_12BIT, HI_MIPI_WDR_MODE_NONE, {0, 1, 2, 3}}}};

static combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX327_12BIT_2M_WDR2to1_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {.mipi_attr = {DATA_TYPE_RAW_12BIT, HI_MIPI_WDR_MODE_DOL, {0, 1, 2, 3}}}};

static combo_dev_attr_t MIPI_2lane_CHN0_SENSOR_IMX327_12BIT_2M_NOWDR_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {.mipi_attr = {
         DATA_TYPE_RAW_12BIT, HI_MIPI_WDR_MODE_NONE, {0, 2, -1, -1}}}};

static combo_dev_attr_t MIPI_2lane_CHN1_SENSOR_IMX327_12BIT_2M_NOWDR_ATTR = {
    .devno = 1,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {.mipi_attr = {
         DATA_TYPE_RAW_12BIT, HI_MIPI_WDR_MODE_NONE, {0, 1, -1, -1}
         //{1, 3, -1, -1}
     }}};

static combo_dev_attr_t MIPI_2lane_CHN0_SENSOR_IMX327_12BIT_2M_WDR2to1_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {.mipi_attr = {DATA_TYPE_RAW_12BIT, HI_MIPI_WDR_MODE_DOL, {0, 2, -1, -1}}}};

static combo_dev_attr_t MIPI_2lane_CHN1_SENSOR_IMX327_12BIT_2M_WDR2to1_ATTR = {
    .devno = 1,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {.mipi_attr = {DATA_TYPE_RAW_12BIT, HI_MIPI_WDR_MODE_DOL, {1, 3, -1, -1}}}};

static combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX307_12BIT_2M_NOWDR_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {.mipi_attr = {DATA_TYPE_RAW_12BIT, HI_MIPI_WDR_MODE_NONE, {0, 1, 2, 3}}}};

static combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX307_12BIT_2M_WDR2to1_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {.mipi_attr = {DATA_TYPE_RAW_12BIT, HI_MIPI_WDR_MODE_DOL, {0, 1, 2, 3}}}};

static combo_dev_attr_t MIPI_2lane_CHN0_SENSOR_IMX307_12BIT_2M_NOWDR_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {.mipi_attr = {
         DATA_TYPE_RAW_12BIT, HI_MIPI_WDR_MODE_NONE, {0, 2, -1, -1}}}};

static combo_dev_attr_t MIPI_2lane_CHN1_SENSOR_IMX307_12BIT_2M_NOWDR_ATTR = {
    .devno = 1,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {.mipi_attr = {
         DATA_TYPE_RAW_12BIT, HI_MIPI_WDR_MODE_NONE, {1, 3, -1, -1}}}};

static combo_dev_attr_t MIPI_2lane_CHN0_SENSOR_IMX307_12BIT_2M_WDR2to1_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {.mipi_attr = {DATA_TYPE_RAW_12BIT, HI_MIPI_WDR_MODE_DOL, {0, 2, -1, -1}}}};

static combo_dev_attr_t MIPI_2lane_CHN1_SENSOR_IMX307_12BIT_2M_WDR2to1_ATTR = {
    .devno = 1,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {.mipi_attr = {DATA_TYPE_RAW_12BIT, HI_MIPI_WDR_MODE_DOL, {1, 3, -1, -1}}}};

static combo_dev_attr_t LVDS_4lane_CHN0_SENSOR_MN34220_12BIT_2M_NOWDR_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_LVDS,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {.lvds_attr = {.input_data_type = DATA_TYPE_RAW_12BIT,
                   .wdr_mode = HI_WDR_MODE_NONE,
                   .sync_mode = LVDS_SYNC_MODE_SOF,
                   .vsync_attr = {LVDS_VSYNC_NORMAL, 0, 0},
                   .fid_attr = {LVDS_FID_NONE, HI_TRUE},
                   .data_endian = LVDS_ENDIAN_BIG,
                   .sync_code_endian = LVDS_ENDIAN_BIG,
                   .lane_id = {0, 2, 1, 3},
                   .sync_code = {
                       {{0x002, 0x003, 0x000, 0x001},  // lane 0
                        {0x002, 0x003, 0x000, 0x001},
                        {0x002, 0x003, 0x000, 0x001},
                        {0x002, 0x003, 0x000, 0x001}},

                       {{0x012, 0x013, 0x010, 0x011},  // lane 1
                        {0x012, 0x013, 0x010, 0x011},
                        {0x012, 0x013, 0x010, 0x011},
                        {0x012, 0x013, 0x010, 0x011}},

                       {{0x006, 0x007, 0x004, 0x005},  // lane2
                        {0x006, 0x007, 0x004, 0x005},
                        {0x006, 0x007, 0x004, 0x005},
                        {0x006, 0x007, 0x004, 0x005}},

                       {{0x016, 0x017, 0x014, 0x015},  // lane3
                        {0x016, 0x017, 0x014, 0x015},
                        {0x016, 0x017, 0x014, 0x015},
                        {0x016, 0x017, 0x014, 0x015}},
                   }}}};

static combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX335_12BIT_5M_NOWDR_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 2592, 1944},

    {.mipi_attr = {DATA_TYPE_RAW_12BIT, HI_MIPI_WDR_MODE_NONE, {0, 1, 2, 3}}}};

static combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX335_10BIT_5M_WDR2TO1_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 2592, 1944},

    {.mipi_attr = {DATA_TYPE_RAW_10BIT, HI_MIPI_WDR_MODE_VC, {0, 1, 2, 3}}}};

static combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX335_12BIT_4M_NOWDR_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 204, 2592, 1536},

    {.mipi_attr = {DATA_TYPE_RAW_12BIT, HI_MIPI_WDR_MODE_NONE, {0, 1, 2, 3}}}};

static combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX335_10BIT_4M_WDR2TO1_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 204, 2592, 1536},

    {.mipi_attr = {DATA_TYPE_RAW_10BIT, HI_MIPI_WDR_MODE_VC, {0, 1, 2, 3}}}};

static combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_SC4210_12BIT_3M_NOWDR_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 2560, 1440},

    {.mipi_attr = {DATA_TYPE_RAW_12BIT, HI_MIPI_WDR_MODE_NONE, {0, 1, 2, 3}}}};

static combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_SC4210_10BIT_3M_WDR2TO1_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 2560, 1440},

    {.mipi_attr = {DATA_TYPE_RAW_10BIT, HI_MIPI_WDR_MODE_VC, {0, 1, 2, 3}}}};

static combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX458_10BIT_8M_NOWDR_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 3840, 2160},

    {.mipi_attr = {DATA_TYPE_RAW_10BIT, HI_MIPI_WDR_MODE_NONE, {0, 1, 2, 3}}}};

static combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX458_10BIT_12M_NOWDR_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 4000, 3000},

    {.mipi_attr = {DATA_TYPE_RAW_10BIT, HI_MIPI_WDR_MODE_NONE, {0, 1, 2, 3}}}};

static combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX458_10BIT_4M_NOWDR_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 2716, 1524},

    {.mipi_attr = {DATA_TYPE_RAW_10BIT, HI_MIPI_WDR_MODE_NONE, {0, 1, 2, 3}}}};

static combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX458_10BIT_2M_NOWDR_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {.mipi_attr = {DATA_TYPE_RAW_10BIT, HI_MIPI_WDR_MODE_NONE, {0, 1, 2, 3}}}};

static combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX458_10BIT_1M_NOWDR_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1280, 720},

    {.mipi_attr = {DATA_TYPE_RAW_10BIT, HI_MIPI_WDR_MODE_NONE, {0, 1, 2, 3}}}};

static combo_dev_attr_t MIPI_2lane_CHN0_SENSOR_OS04B10_10BIT_4M_NOWDR_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 2560, 1440},

    {.mipi_attr = {
         DATA_TYPE_RAW_10BIT, HI_MIPI_WDR_MODE_NONE, {0, 1, -1, -1}}}};

static combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_OS05A_12BIT_4M_NOWDR_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 204, 2688, 1536},

    {.mipi_attr = {DATA_TYPE_RAW_12BIT, HI_MIPI_WDR_MODE_NONE, {0, 1, 2, 3}}}};

static combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_OS05A_10BIT_4M_WDR2TO1_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 204, 2688, 1536},

    {.mipi_attr = {DATA_TYPE_RAW_10BIT, HI_MIPI_WDR_MODE_VC, {0, 1, 2, 3}}}};

static combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_OS08A10_10BIT_8M_NOWDR_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 3840, 2160},

    {.mipi_attr = {DATA_TYPE_RAW_10BIT, HI_MIPI_WDR_MODE_NONE, {0, 1, 2, 3}}}};

static combo_dev_attr_t MIPI_2lane_CHN0_SENSOR_GC2053_10BIT_2M_NOWDR_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {.mipi_attr = {
         DATA_TYPE_RAW_10BIT, HI_MIPI_WDR_MODE_NONE, {0, 1, -1, -1}}}};

static combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_OV12870_10BIT_8M_NOWDR_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 3840, 2160},

    {.mipi_attr = {DATA_TYPE_RAW_10BIT, HI_MIPI_WDR_MODE_NONE, {0, 1, 2, 3}}}};

static combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_OV12870_10BIT_12M_NOWDR_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 4000, 3000},

    {.mipi_attr = {DATA_TYPE_RAW_10BIT, HI_MIPI_WDR_MODE_NONE, {0, 1, 2, 3}}}};

static combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_OV12870_10BIT_2M_NOWDR_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {.mipi_attr = {DATA_TYPE_RAW_10BIT, HI_MIPI_WDR_MODE_NONE, {0, 1, 2, 3}}}};

static combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_OV12870_10BIT_1M_NOWDR_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1280, 720},

    {.mipi_attr = {DATA_TYPE_RAW_10BIT, HI_MIPI_WDR_MODE_NONE, {0, 1, 2, 3}}}};

static combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX415_12BIT_8M_NOWDR_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 3840, 2160},

    {.mipi_attr = {DATA_TYPE_RAW_12BIT, HI_MIPI_WDR_MODE_NONE, {0, 1, 2, 3}}}};

bool Mipi::enableSensorClock(sns_clk_source_t sensorIndex) {
  HI_S32 fd = open(MIPI_DEV_NODE, O_RDWR);
  if (fd < 0) {
    SZ_LOG_ERROR("open hi_mipi dev failed");
    return false;
  }

  HI_U32 s32Ret = ioctl(fd, HI_MIPI_ENABLE_SENSOR_CLOCK, &sensorIndex);
  if (HI_SUCCESS != s32Ret) {
    SZ_LOG_ERROR("HI_MIPI_ENABLE_SENSOR_CLOCK {} failed {:X}", sensorIndex,
                 s32Ret);
  }
  close(fd);
  return HI_SUCCESS == s32Ret;
}

bool Mipi::resetSensor(sns_rst_source_t sensorIndex) {
  HI_S32 fd = open(MIPI_DEV_NODE, O_RDWR);
  if (fd < 0) {
    SZ_LOG_ERROR("open hi_mipi dev failed");
    return false;
  }

  HI_U32 s32Ret = ioctl(fd, HI_MIPI_RESET_SENSOR, &sensorIndex);
  if (HI_SUCCESS != s32Ret) {
    SZ_LOG_ERROR("HI_MIPI_RESET_SENSOR {} failed {:X}", sensorIndex, s32Ret);
  }
  close(fd);
  return HI_SUCCESS == s32Ret;
}

bool Mipi::unresetSensor(sns_rst_source_t sensorIndex) {
  HI_S32 fd = open(MIPI_DEV_NODE, O_RDWR);
  if (fd < 0) {
    SZ_LOG_ERROR("open hi_mipi dev failed");
    return false;
  }

  HI_U32 s32Ret = ioctl(fd, HI_MIPI_UNRESET_SENSOR, &sensorIndex);
  if (HI_SUCCESS != s32Ret) {
    SZ_LOG_ERROR("HI_MIPI_UNRESET_SENSOR {} failed {:X}", sensorIndex, s32Ret);
  }
  close(fd);
  return HI_SUCCESS == s32Ret;
}

bool Mipi::setMipiHsMode(SensorType sensorType) {
  lane_divide_mode_t lane_divide_mode = LANE_DIVIDE_MODE_0;
  if (SONY_IMX327_2L_MIPI_2M_30FPS_12BIT == sensorType ||
      SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1 == sensorType) {
    lane_divide_mode = LANE_DIVIDE_MODE_1;
  } else if ((SONY_IMX307_MIPI_2M_30FPS_12BIT == sensorType) ||
             (SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1 == sensorType)) {
    lane_divide_mode = LANE_DIVIDE_MODE_1;
  }
  HI_S32 fd = open(MIPI_DEV_NODE, O_RDWR);
  if (fd < 0) {
    SZ_LOG_ERROR("open hi_mipi dev failed");
    return false;
  }

  HI_U32 s32Ret = ioctl(fd, HI_MIPI_SET_HS_MODE, &lane_divide_mode);
  if (HI_SUCCESS != s32Ret) {
    SZ_LOG_ERROR("HI_MIPI_SET_HS_MODE failed {:X}", s32Ret);
  }
  close(fd);
  return HI_SUCCESS == s32Ret;
}

bool Mipi::enableMipiClock(combo_dev_t mipiDev) {
  HI_S32 fd = open(MIPI_DEV_NODE, O_RDWR);
  if (fd < 0) {
    SZ_LOG_ERROR("open hi_mipi dev failed");
    return false;
  }

  HI_U32 s32Ret = ioctl(fd, HI_MIPI_ENABLE_MIPI_CLOCK, &mipiDev);
  if (HI_SUCCESS != s32Ret) {
    SZ_LOG_ERROR("MIPI_ENABLE_CLOCK {} failed {:X}", mipiDev, s32Ret);
  }
  close(fd);
  return HI_SUCCESS == s32Ret;
}

bool Mipi::resetMipi(combo_dev_t mipiDev) {
  HI_S32 fd = open(MIPI_DEV_NODE, O_RDWR);
  if (fd < 0) {
    SZ_LOG_ERROR("open hi_mipi dev failed");
    return false;
  }

  HI_U32 s32Ret = ioctl(fd, HI_MIPI_RESET_MIPI, &mipiDev);
  if (HI_SUCCESS != s32Ret) {
    SZ_LOG_ERROR("RESET_MIPI {} failed  {:X}", mipiDev, s32Ret);
  }
  close(fd);
  return HI_SUCCESS == s32Ret;
}

void Mipi::getComboAttrBySensor(combo_dev_t mipiDev, SensorType sensorType,
                                combo_dev_attr_t* pstComboAttr) {
  switch (sensorType) {
    case SONY_IMX327_MIPI_2M_30FPS_12BIT:
      memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t),
               &MIPI_4lane_CHN0_SENSOR_IMX327_12BIT_2M_NOWDR_ATTR,
               sizeof(combo_dev_attr_t));
      break;

    case SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1:
      memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t),
               &MIPI_4lane_CHN0_SENSOR_IMX327_12BIT_2M_WDR2to1_ATTR,
               sizeof(combo_dev_attr_t));
      break;

    case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT:
      if (0 == mipiDev) {
        memcpy(pstComboAttr, &MIPI_2lane_CHN0_SENSOR_IMX327_12BIT_2M_NOWDR_ATTR,
               sizeof(combo_dev_attr_t));
      } else if (1 == mipiDev) {
        memcpy(pstComboAttr, &MIPI_2lane_CHN1_SENSOR_IMX327_12BIT_2M_NOWDR_ATTR,
               sizeof(combo_dev_attr_t));
      }
      break;
    case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
      if (0 == mipiDev) {
        memcpy(pstComboAttr,
               &MIPI_2lane_CHN0_SENSOR_IMX327_12BIT_2M_WDR2to1_ATTR,
               sizeof(combo_dev_attr_t));
      } else if (1 == mipiDev) {
        memcpy(pstComboAttr,
               &MIPI_2lane_CHN1_SENSOR_IMX327_12BIT_2M_WDR2to1_ATTR,
               sizeof(combo_dev_attr_t));
      }

      break;

    case SONY_IMX307_MIPI_2M_30FPS_12BIT:
      if (0 == mipiDev) {
        memcpy(pstComboAttr, &MIPI_2lane_CHN0_SENSOR_IMX327_12BIT_2M_NOWDR_ATTR,
               sizeof(combo_dev_attr_t));
      } else if (1 == mipiDev) {
        memcpy(pstComboAttr, &MIPI_2lane_CHN1_SENSOR_IMX327_12BIT_2M_NOWDR_ATTR,
               sizeof(combo_dev_attr_t));
      }
      break;
      // memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t),
      // &MIPI_4lane_CHN0_SENSOR_IMX307_12BIT_2M_NOWDR_ATTR,
      // sizeof(combo_dev_attr_t));
      break;

    case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
      memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t),
               &MIPI_4lane_CHN0_SENSOR_IMX307_12BIT_2M_WDR2to1_ATTR,
               sizeof(combo_dev_attr_t));
      break;

    case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT:
      if (0 == mipiDev) {
        memcpy(pstComboAttr, &MIPI_2lane_CHN0_SENSOR_IMX307_12BIT_2M_NOWDR_ATTR,
               sizeof(combo_dev_attr_t));
      } else if (1 == mipiDev) {
        memcpy(pstComboAttr, &MIPI_2lane_CHN1_SENSOR_IMX307_12BIT_2M_NOWDR_ATTR,
               sizeof(combo_dev_attr_t));
      }

      break;

    case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
      if (0 == mipiDev) {
        memcpy(pstComboAttr,
               &MIPI_2lane_CHN0_SENSOR_IMX307_12BIT_2M_WDR2to1_ATTR,
               sizeof(combo_dev_attr_t));
      } else if (1 == mipiDev) {
        memcpy(pstComboAttr,
               &MIPI_2lane_CHN1_SENSOR_IMX307_12BIT_2M_WDR2to1_ATTR,
               sizeof(combo_dev_attr_t));
      }
      break;

    case PANASONIC_MN34220_LVDS_2M_30FPS_12BIT:
      memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t),
               &LVDS_4lane_CHN0_SENSOR_MN34220_12BIT_2M_NOWDR_ATTR,
               sizeof(combo_dev_attr_t));
      break;

    case SONY_IMX335_MIPI_5M_30FPS_12BIT:
      memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t),
               &MIPI_4lane_CHN0_SENSOR_IMX335_12BIT_5M_NOWDR_ATTR,
               sizeof(combo_dev_attr_t));
      break;

    case SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1:
      memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t),
               &MIPI_4lane_CHN0_SENSOR_IMX335_10BIT_5M_WDR2TO1_ATTR,
               sizeof(combo_dev_attr_t));
      break;

    case SONY_IMX335_MIPI_4M_30FPS_12BIT:
      memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t),
               &MIPI_4lane_CHN0_SENSOR_IMX335_12BIT_4M_NOWDR_ATTR,
               sizeof(combo_dev_attr_t));
      break;

    case SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1:
      memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t),
               &MIPI_4lane_CHN0_SENSOR_IMX335_10BIT_4M_WDR2TO1_ATTR,
               sizeof(combo_dev_attr_t));
      break;

    case SMART_SC4210_MIPI_3M_30FPS_12BIT:
      memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t),
               &MIPI_4lane_CHN0_SENSOR_SC4210_12BIT_3M_NOWDR_ATTR,
               sizeof(combo_dev_attr_t));
      break;

    case SMART_SC4210_MIPI_3M_30FPS_10BIT_WDR2TO1:
      memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t),
               &MIPI_4lane_CHN0_SENSOR_SC4210_10BIT_3M_WDR2TO1_ATTR,
               sizeof(combo_dev_attr_t));
      break;

    case SONY_IMX458_MIPI_8M_30FPS_10BIT:
      memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t),
               &MIPI_4lane_CHN0_SENSOR_IMX458_10BIT_8M_NOWDR_ATTR,
               sizeof(combo_dev_attr_t));
      break;
    case SONY_IMX458_MIPI_12M_20FPS_10BIT:
      memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t),
               &MIPI_4lane_CHN0_SENSOR_IMX458_10BIT_12M_NOWDR_ATTR,
               sizeof(combo_dev_attr_t));
      break;
    case SONY_IMX458_MIPI_4M_60FPS_10BIT:
    case SONY_IMX458_MIPI_4M_40FPS_10BIT:
      memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t),
               &MIPI_4lane_CHN0_SENSOR_IMX458_10BIT_4M_NOWDR_ATTR,
               sizeof(combo_dev_attr_t));
      break;
    case SONY_IMX458_MIPI_2M_90FPS_10BIT:
      memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t),
               &MIPI_4lane_CHN0_SENSOR_IMX458_10BIT_2M_NOWDR_ATTR,
               sizeof(combo_dev_attr_t));
      break;
    case SONY_IMX458_MIPI_1M_129FPS_10BIT:
      memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t),
               &MIPI_4lane_CHN0_SENSOR_IMX458_10BIT_1M_NOWDR_ATTR,
               sizeof(combo_dev_attr_t));
      break;

    case OMNIVISION_OS04B10_MIPI_4M_30FPS_10BIT:
      memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t),
               &MIPI_2lane_CHN0_SENSOR_OS04B10_10BIT_4M_NOWDR_ATTR,
               sizeof(combo_dev_attr_t));
      break;

    case OMNIVISION_OS05A_MIPI_4M_30FPS_12BIT:
      memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t),
               &MIPI_4lane_CHN0_SENSOR_OS05A_12BIT_4M_NOWDR_ATTR,
               sizeof(combo_dev_attr_t));
      break;
    case OMNIVISION_OS05A_MIPI_4M_30FPS_10BIT_WDR2TO1:
      memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t),
               &MIPI_4lane_CHN0_SENSOR_OS05A_10BIT_4M_WDR2TO1_ATTR,
               sizeof(combo_dev_attr_t));
      break;

    case OMNIVISION_OS08A10_MIPI_8M_30FPS_10BIT:
      memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t),
               &MIPI_4lane_CHN0_SENSOR_OS08A10_10BIT_8M_NOWDR_ATTR,
               sizeof(combo_dev_attr_t));
      break;

    case GALAXYCORE_GC2053_MIPI_2M_30FPS_10BIT:
      memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t),
               &MIPI_2lane_CHN0_SENSOR_GC2053_10BIT_2M_NOWDR_ATTR,
               sizeof(combo_dev_attr_t));
      break;
    case OMNIVISION_OV12870_MIPI_8M_30FPS_10BIT:
      memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t),
               &MIPI_4lane_CHN0_SENSOR_OV12870_10BIT_8M_NOWDR_ATTR,
               sizeof(combo_dev_attr_t));
      break;
    case OMNIVISION_OV12870_MIPI_12M_30FPS_10BIT:
      memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t),
               &MIPI_4lane_CHN0_SENSOR_OV12870_10BIT_12M_NOWDR_ATTR,
               sizeof(combo_dev_attr_t));
      break;
    case OMNIVISION_OV12870_MIPI_2M_120FPS_10BIT:
      memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t),
               &MIPI_4lane_CHN0_SENSOR_OV12870_10BIT_2M_NOWDR_ATTR,
               sizeof(combo_dev_attr_t));
      break;
    case OMNIVISION_OV12870_MIPI_1M_240FPS_10BIT:
      memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t),
               &MIPI_4lane_CHN0_SENSOR_OV12870_10BIT_1M_NOWDR_ATTR,
               sizeof(combo_dev_attr_t));
      break;

    case SONY_IMX415_MIPI_8M_30FPS_12BIT:
    case SONY_IMX415_MIPI_8M_20FPS_12BIT:
      memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t),
               &MIPI_4lane_CHN0_SENSOR_IMX415_12BIT_8M_NOWDR_ATTR,
               sizeof(combo_dev_attr_t));
      break;

    default:
      SZ_LOG_ERROR("not support enSnsType: {}", sensorType);
      memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t),
               &MIPI_4lane_CHN0_SENSOR_IMX327_12BIT_2M_NOWDR_ATTR,
               sizeof(combo_dev_attr_t));
  }
}

bool Mipi::setMipiAttr(combo_dev_t mipiDev, SensorType sensorType) {
  HI_S32 fd = open(MIPI_DEV_NODE, O_RDWR);
  if (fd < 0) {
    SZ_LOG_ERROR("open hi_mipi dev failed");
    return false;
  }

  combo_dev_attr_t comboDevAttr;
  getComboAttrBySensor(mipiDev, sensorType, &comboDevAttr);
  comboDevAttr.devno = mipiDev;
  HI_U32 s32Ret = ioctl(fd, HI_MIPI_SET_DEV_ATTR, &comboDevAttr);
  if (HI_SUCCESS != s32Ret) {
    SZ_LOG_ERROR("MIPI_SET_DEV_ATTR failed {:X}", s32Ret);
  }
  close(fd);

  return HI_SUCCESS == s32Ret;
}

bool Mipi::unresetMipi(combo_dev_t mipiDev) {
  HI_S32 fd = open(MIPI_DEV_NODE, O_RDWR);
  if (fd < 0) {
    SZ_LOG_ERROR("open hi_mipi dev failed");
    return false;
  }

  HI_U32 s32Ret = ioctl(fd, HI_MIPI_UNRESET_MIPI, &mipiDev);
  if (HI_SUCCESS != s32Ret) {
    SZ_LOG_ERROR("UNRESET_MIPI {} failed {:X}", mipiDev, s32Ret);
  }
  close(fd);

  return HI_SUCCESS == s32Ret;
}

bool Mipi::start(combo_dev_t mipiDev, sns_clk_source_t sensorIndex,
                 SensorType sensorType) {
  static bool bInit = false;
  if (!bInit) {
    if (!setMipiHsMode(sensorType)) return false;
    bInit = true;
  }
  if (!enableMipiClock(mipiDev)) return false;
  if (!resetMipi(mipiDev)) return false;
  if (!enableSensorClock(sensorIndex)) return false;
  if (!resetSensor(sensorIndex)) return false;
  if (!setMipiAttr(mipiDev, sensorType)) return false;
  if (!unresetMipi(mipiDev)) return false;
  if (!unresetSensor(sensorIndex)) return false;
  return true;
}

Mipi::Mipi() {}

Mipi* Mipi::getInstance() {
  static Mipi self;
  return &self;
}
