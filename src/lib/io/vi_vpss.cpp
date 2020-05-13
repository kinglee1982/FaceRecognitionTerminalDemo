#include "vi_vpss.h"

#include "mpp.h"
#include "vi.h"
#include "vpss.h"

using namespace suanzi;

Vi_Vpss::Vi_Vpss(Vi *pVi, Vpss *pVpss) : pVpss_(pVpss) {
  Mpp *pMpp = Mpp::getInstance();
  static bool bInit = false;
  if (!bInit) {
    VB_CONFIG_S vbConfig;
    pVi->getVbConfig(&vbConfig);
    if (!pMpp->init(&vbConfig)) return;
    bInit = true;
  }

  if (!pVi->start()) return;

  cv::Size vpssChSizes[1] = {{1920, 1080}};
  if (!pVpss->start(vpssChSizes, 1)) return;

  VI_PIPE viPipe = pVi->getViPipe();
  VPSS_GRP vpssGrp = pVpss->getVpssGrp();
  if (!pMpp->viBindVpss(viPipe, 0, vpssGrp)) return;
}

Vi_Vpss::~Vi_Vpss() {}

VPSS_GRP Vi_Vpss::getVpssGrp() { return pVpss_->getVpssGrp(); }
