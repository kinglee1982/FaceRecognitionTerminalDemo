#ifndef _MIPI_TX_H_
#define _MIPI_TX_H_

#include "mpi_vo.h"

class Mipi_Tx {
 public:
  static Mipi_Tx *getInstance();
  bool startMipiTx(VO_INTF_SYNC_E voIntfSync);
  bool startDeviceWithUserSyncInfo(VO_DEV voDev);

 private:
  Mipi_Tx();
  bool initScreen720x1280(HI_S32 s32fd);
  bool initScreen1080x1920(HI_S32 fd);
  // void initMipiTxScreen(VO_INTF_SYNC_E enVoIntfSync, HI_S32 fd);

 private:
};

#endif
