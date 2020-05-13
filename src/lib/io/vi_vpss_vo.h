#ifndef _VI_VPSS_VO_H_
#define _VI_VPSS_VO_H_

#include "vi_vpss.h"
#include "vo.h"

namespace suanzi {

class Vi_Vpss_Vo {
 public:
  Vi_Vpss_Vo(Vi_Vpss *pVi_Vpss, Vo *pVo);
  Vi_Vpss_Vo(Vi_Vpss *pVi_Vpss0, Vi_Vpss *pVi_Vpss1, Vo *pVo);
  ~Vi_Vpss_Vo();

 private:
};

}  // namespace suanzi

#endif
