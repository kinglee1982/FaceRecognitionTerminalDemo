#ifndef _VI_H_
#define _VI_H_

#include "hi_comm_vb.h"
#include "hi_comm_vi.h"
#include "hi_common.h"
#include "isp.h"

class Vi {
 public:
  explicit Vi(VI_DEV viDev, VI_PIPE viPipe, SensorType sensorType);
  bool start();
  void getVbConfig(VB_CONFIG_S *pVbConfig);
  VI_PIPE getViPipe();

 private:
  bool setParam(VI_PIPE viPipe);
  void getDevAttrBySensor(SensorType sensorType, VI_DEV_ATTR_S *pViDevAttr);
  void getPipeAttrBySensor(SensorType sensorType, VI_PIPE_ATTR_S *pstPipeAttr);
  void getChAttrBySensor(SensorType sensorType, VI_CHN_ATTR_S *pstChnAttr);
  bool startDev(VI_DEV viDev, SensorType sensorType);
  bool devBindPipe(VI_DEV viDev, VI_PIPE viPipe);
  bool startPipe(VI_PIPE viPipe, SensorType sensorType);
  bool startCh(VI_PIPE viPipe, VI_CHN viCh, SensorType sensorType);
  bool startVi(VI_DEV viDev, VI_PIPE viPipe, VI_CHN viCh,
               SensorType sensorType);

  bool stopCh(VI_PIPE viPipe, VI_CHN viCh);
  bool stopPipe(VI_PIPE viPipe);
  bool stopDev(VI_DEV viDev);

 private:
  VI_DEV viDev_;
  VI_PIPE viPipe_;
  SensorType sensorType_;
};

#endif
