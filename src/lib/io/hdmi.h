#ifndef _HDMI_H_
#define _HDMI_H_

#include "hi_comm_hdmi.h"

class Hdmi {
 public:
  static Hdmi *getInstance();
  bool start(HI_HDMI_ID_E hdmiId, int width, int height);
  void stop(HI_HDMI_ID_E hdmiId);

 private:
  Hdmi();
  HI_HDMI_VIDEO_FMT_E getHdmiVideoFmt(int width, int height);
  int getDisFrameRate(HI_HDMI_VIDEO_FMT_E hdmiVideoFmt);

 private:
};

#endif
