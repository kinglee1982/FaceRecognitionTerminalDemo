#ifndef _MPP_H_
#define _MPP_H_

#include "hi_comm_vb.h"
#include "hi_common.h"

class Mpp {
 public:
  static Mpp *getInstance();
  bool init(const VB_CONFIG_S *pVbConf);
  bool viBindVpss(VI_PIPE viPipe, VI_CHN ViCh, VPSS_GRP vpssGrp);
  bool vpssBindVo(VPSS_GRP vpssGrp, VPSS_CHN vpssCh, VO_LAYER voLayer,
                  VO_CHN voCh);
  bool vdecBindVpss(VDEC_CHN vdecCh, VPSS_GRP vpssGrp);
  bool vdecUnBindVpss(VDEC_CHN vdecCh, VPSS_GRP vpssGrp);
  bool vpssUnBindVo(VPSS_GRP vpssGrp, VPSS_CHN vpssChn, VO_LAYER voLayer,
                    VO_CHN voChn);
  bool viUnBindVpss(VI_PIPE viPipe, VI_CHN viCh, VPSS_GRP vpssGrp);

 private:
  Mpp();
};

#endif
