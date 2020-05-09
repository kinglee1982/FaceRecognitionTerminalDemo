#include "mipi_tx.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "hi_mipi_tx.h"
#include "rp-lcd-mipi-8inch-800x1280.h"

Mipi_Tx::Mipi_Tx() {}

Mipi_Tx *Mipi_Tx::getInstance() {
  static Mipi_Tx self;
  return &self;
}

bool Mipi_Tx::startMipiTx(VO_INTF_SYNC_E voIntfSync) {
  start_mipi_8inch_800x1280_mipiTx(voIntfSync);
  return true;
}

bool Mipi_Tx::startDeviceWithUserSyncInfo(VO_DEV voDev) {
  start_mipi_8inch_800x1280_userSyncInfo(voDev);
  return true;
}
