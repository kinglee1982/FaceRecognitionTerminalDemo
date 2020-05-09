#ifndef _VPSS_H_
#define _VPSS_H_

#include <opencv2/opencv.hpp>

#include "iveapp.h"
#include "mmzimage.h"
#include "mpi_vpss.h"

namespace suanzi {

class Vpss {
 public:
  Vpss(VPSS_GRP vpssGrp, int maxWidth, int maxHeight);
  ~Vpss();
  bool start(const cv::Size *pVpssChSizes, int chNum);
  bool getYuvFrame(MmzImage *pImage, int vpssChIndex);
  bool getFrame(cv::Mat &image, int vpssChIndex);
  VPSS_GRP getVpssGrp();

 private:
  bool parseFrame(MmzImage *pImage, const VIDEO_FRAME_S *pVBuf,
                  int vpssChIndex);
  bool stop(int vpssChNum);
  void getDefaultGrpAttr(VPSS_GRP_ATTR_S &vpssGrpAttr, int maxWidth,
                         int maxHeight, bool bNrEn);
  void getDefaultChAttr(VPSS_CHN_ATTR_S &chAttr, int width, int height);

 private:
  IveApp *pIveApp_;
  MmzImage *pMmzYuvImage_;
  VPSS_GRP vpssGrp_;
  int maxWidth_;
  int maxHeight_;
  int chNum_;
  cv::Size vpssChSizes_[VPSS_MAX_CHN_NUM];
};

}  // namespace suanzi

#endif
