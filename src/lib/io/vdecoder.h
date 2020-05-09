#ifndef __VDECODER__
#define __VDECODER__

#include <opencv2/opencv.hpp>

#include "hi_comm_vdec.h"
#include "hi_comm_video.h"
#include "hi_types.h"
#include "iveapp.h"
#include "mmzimage.h"
#include "vpss.h"

namespace suanzi {

class VDecoder {
 public:
  VDecoder(int chIndex);
  ~VDecoder();
  bool getFrame(MmzImage *pImage, int vpssChIndex);
  int sendStream(const unsigned char *pu8Buf, int len, bool bStreamOfEnd);

  static void getVbConfig(int decodeChNum, int width, int height,
                          VB_CONFIG_S *pVbConfig);
  static void start(int decodeChNum, int width, int height,
                    PAYLOAD_TYPE_E payloadType, VIDEO_DEC_MODE_E decMode);

 private:
  static bool setModuleBufPool(const VB_CONFIG_S *pVbConf, VB_UID_E vbUid);
  static bool initVbPool(int decodeChNum, int width, int height,
                         PAYLOAD_TYPE_E payloadType, VIDEO_DEC_MODE_E decMode);
  static bool startRecvStream(int decodeChNum, int width, int height,
                              PAYLOAD_TYPE_E payloadType,
                              VIDEO_DEC_MODE_E decMode);
  bool stopRecvStream(int chId);
  void stopRecvStreams(int chNum);
  bool parseFrame(MmzImage *pImage, const VIDEO_FRAME_S *pVBuf,
                  int vpssChIndex);

 private:
  static bool bInit_;
  int chIndex_;
  IveApp *pIveApp_;
};

}  // namespace suanzi
#endif
