#ifndef _ISP_H_
#define _ISP_H_

#include "hi_comm_isp.h"
#include "hi_sns_ctrl.h"

typedef enum _SensorType {

  SONY_IMX327_MIPI_2M_30FPS_12BIT,
  SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1,
  SONY_IMX327_2L_MIPI_2M_30FPS_12BIT,
  SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1,
  SONY_IMX307_MIPI_2M_30FPS_12BIT,
  SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1,
  SONY_IMX307_2L_MIPI_2M_30FPS_12BIT,
  SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1,
  SONY_IMX335_MIPI_5M_30FPS_12BIT,
  SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1,
  SONY_IMX335_MIPI_4M_30FPS_12BIT,
  SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1,
  SONY_IMX458_MIPI_8M_30FPS_10BIT,
  SONY_IMX458_MIPI_12M_20FPS_10BIT,
  SONY_IMX458_MIPI_4M_60FPS_10BIT,
  SONY_IMX458_MIPI_4M_40FPS_10BIT,
  SONY_IMX458_MIPI_2M_90FPS_10BIT,
  SONY_IMX458_MIPI_1M_129FPS_10BIT,
  SMART_SC4210_MIPI_3M_30FPS_12BIT,
  SMART_SC4210_MIPI_3M_30FPS_10BIT_WDR2TO1,
  PANASONIC_MN34220_LVDS_2M_30FPS_12BIT,
  OMNIVISION_OS04B10_MIPI_4M_30FPS_10BIT,
  OMNIVISION_OS05A_MIPI_4M_30FPS_12BIT,
  OMNIVISION_OS05A_MIPI_4M_30FPS_10BIT_WDR2TO1,
  OMNIVISION_OS08A10_MIPI_8M_30FPS_10BIT,
  GALAXYCORE_GC2053_MIPI_2M_30FPS_10BIT,
  OMNIVISION_OV12870_MIPI_1M_240FPS_10BIT,
  OMNIVISION_OV12870_MIPI_2M_120FPS_10BIT,
  OMNIVISION_OV12870_MIPI_8M_30FPS_10BIT,
  OMNIVISION_OV12870_MIPI_12M_30FPS_10BIT,
  SONY_IMX415_MIPI_8M_30FPS_12BIT,
  SONY_IMX415_MIPI_8M_20FPS_12BIT,
  SAMPLE_SNS_TYPE_BUTT,
} SensorType;

class Isp {
 public:
  static Isp *getInstance();
  bool start(VI_PIPE viPipe, SensorType sensorType, HI_S8 busId);
  void getSizeBySensor(SensorType sensorType, int &width, int &height);

 private:
  Isp();
  void getIspAttrBySensor(SensorType sensorType, ISP_PUB_ATTR_S *pstPubAttr);
  ISP_SNS_OBJ_S *getSensorObj(SensorType sensorType);
  bool regiterSensorCallback(VI_PIPE viPipe, SensorType sensorType);
  bool unregiterSensorCallback(VI_PIPE viPipe, SensorType sensorType);
  bool registerAelibCallback(VI_PIPE viPipe);
  bool unregisterAelibCallback(VI_PIPE viPipe);
  bool registerAwblibCallback(VI_PIPE viPipe);
  bool unregisterAwblibCallback(VI_PIPE viPipe);
  bool bindSensor(VI_PIPE viPipe, SensorType sensorType, HI_S8 busId);
  bool startRun(VI_PIPE viPipe);
  void stop(VI_PIPE viPipe, SensorType sensorType);
  bool setSensorSaturationZero(VI_PIPE viPipe);

  static void *ispThread(void *param);

 private:
  pthread_t pid_;
};

#endif
