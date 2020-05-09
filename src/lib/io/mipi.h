#ifndef _MIPI_H_
#define _MIPI_H_

#include "hi_mipi.h"
#include "isp.h"

class Mipi {
 public:
  static Mipi* getInstance();
  bool start(combo_dev_t mipiDev, sns_clk_source_t sensorIndex,
             SensorType sensorType);

 private:
  Mipi();
  bool enableSensorClock(sns_clk_source_t sensorIndex);
  bool resetSensor(sns_rst_source_t sensorIndex);
  bool unresetSensor(sns_rst_source_t sensorIndex);
  bool setMipiHsMode(SensorType sensorType);
  bool enableMipiClock(combo_dev_t mipiDev);
  bool resetMipi(combo_dev_t mipiDev);
  void getComboAttrBySensor(combo_dev_t mipiDev, SensorType sensorType,
                            combo_dev_attr_t* pstComboAttr);
  bool setMipiAttr(combo_dev_t mipiDev, SensorType sensorType);
  bool unresetMipi(combo_dev_t mipiDev);

 private:
};

#endif
