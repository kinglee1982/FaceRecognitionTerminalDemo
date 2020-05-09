#include "rtsp_vdec_vpss.h"

Rtsp_Vdec_Vpss::Rtsp_Vdec_Vpss() {}

Rtsp_Vdec_Vpss *Rtsp_Vdec_Vpss::getInstance() {
  static Rtsp_Vdec_Vpss self;
  return &self;
}
