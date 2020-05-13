#ifndef _VO_H_
#define _VO_H_
#include <opencv2/opencv.hpp>

#include "mpi_sys.h"
#include "mpi_vb.h"
#include "mpi_vo.h"

namespace suanzi {

typedef enum VoMode_ {
  VO_MODE_1MUX,
  VO_MODE_2MUX,
  VO_MODE_4MUX,
  VO_MODE_8MUX,
  VO_MODE_9MUX,
  VO_MODE_16MUX,
  VO_MODE_25MUX,
  VO_MODE_36MUX,
  VO_MODE_49MUX,
  VO_MODE_64MUX,
  VO_MODE_2X4,
  VO_MODE_BUTT

} VoMode;

class Vo {
 public:
  Vo(VO_DEV voDev, VO_INTF_TYPE_E intfType, int width, int height);
  ~Vo();

  bool start(VoMode voMode);
  bool sendYuvFrame(const unsigned char *pData, int width, int height);
  bool sendFrame(const cv::Mat &image);

 private:
  VO_INTF_SYNC_E getIntfSync(VO_INTF_TYPE_E intfType, int width, int height);
  void initVb(int width, int height);
  bool startDevice(VO_DEV voDev, VO_INTF_TYPE_E intfType,
                   VO_INTF_SYNC_E intfSync);
  bool startLayer(VO_LAYER voLayer, int width, int height, int disFrameRate);
  bool startCh(VO_LAYER voLayer, VoMode voMode, int width, int height);

  bool stopCh(VO_LAYER voLayer, VoMode voMode);
  bool stopLayer(VO_LAYER voLayer);
  bool stopDev(VO_DEV voDev);

 private:
  static HI_VOID exitAbnormal(HI_S32 signo);
  static bool bInit;
  static VB_POOL poolId_;
  static VB_BLK vbBlk_;

  VO_DEV voDev_;
  VO_INTF_TYPE_E intfType_;
  int width_;
  int height_;
};

}  // namespace suanzi

#endif
