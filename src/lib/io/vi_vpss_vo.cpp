#include "vi_vpss_vo.h"

#include "mpp.h"
#include "vi_vpss.h"

using namespace suanzi;

Vi_Vpss_Vo::Vi_Vpss_Vo(Vi_Vpss *pVi_Vpss, Vo *pVo) {
  if (!pVo->start(VO_MODE_1MUX)) return;

  VPSS_GRP vpssGrp = pVi_Vpss->getVpssGrp();
  Mpp *pMpp = Mpp::getInstance();
  pMpp->vpssBindVo(vpssGrp, 0, 0, 0);
}

Vi_Vpss_Vo::Vi_Vpss_Vo(Vi_Vpss *pVi_Vpss0, Vi_Vpss *pVi_Vpss1, Vo *pVo) {
  if (!pVo->start(VO_MODE_2MUX)) return;

  Mpp *pMpp = Mpp::getInstance();
  VPSS_GRP vpssGrp = pVi_Vpss0->getVpssGrp();
  pMpp->vpssBindVo(vpssGrp, 0, 0, 0);

  vpssGrp = pVi_Vpss1->getVpssGrp();
  pMpp->vpssBindVo(vpssGrp, 0, 0, 1);
}

Vi_Vpss_Vo::~Vi_Vpss_Vo() {}
