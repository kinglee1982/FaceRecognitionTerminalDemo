#ifndef __RP_LCD_MIPI_8INCH_800X1280_H__
#define __RP_LCD_MIPI_8INCH_800X1280_H__
//#include "sample_comm.h"
#include <sys/ioctl.h>

#include "hi_comm_vo.h"
#include "hi_mipi_tx.h"
#include "hi_type.h"

#define SAMPLE_PRT(fmt...)                       \
  do {                                           \
    printf("[%s]-%d: ", __FUNCTION__, __LINE__); \
    printf(fmt);                                 \
  } while (0)

/*============================= mipi 8 inch 800x1280 lcd config
 * ====================================*/
typedef struct lcd_resoluton {
  HI_U32 pu32W;
  HI_U32 pu32H;
  HI_U32 pu32Frm;
} lcd_resoluton_t;

#ifdef __cplusplus
extern "C" {
#endif

HI_S32 start_mipi_8inch_800x1280_userSyncInfo(VO_DEV voDev);
void start_mipi_8inch_800x1280_mipiTx(VO_INTF_SYNC_E enVoIntfSync);

#ifdef __cplusplus
}
#endif

#endif /* End of #ifndef __RP_LCD_MIPI_8INCH_800X1280_H__*/
