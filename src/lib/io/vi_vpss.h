#ifndef _VI_VPSS_H_
#define _VI_VPSS_H_

#include "vi.h"
#include "vpss.h"

namespace suanzi {

class Vi_Vpss {
 public:
  Vi_Vpss(Vi *pVi, Vpss *pVpss);
  ~Vi_Vpss();

  VPSS_GRP getVpssGrp();

 private:
  Vpss *pVpss_;
};

}  // namespace suanzi

#endif
