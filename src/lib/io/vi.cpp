#include "vi.h"

#include "hi_buffer.h"
#include "hi_comm_3a.h"
#include "hi_comm_vi.h"
#include "hi_mipi.h"
#include "hi_securec.h"
#include "lib/logger.h"
#include "mipi.h"
#include "mpi_sys.h"
#include "mpi_vi.h"
#include "mpp.h"

static VI_DEV_ATTR_S DEV_ATTR_IMX327_2M_BASE = {
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFF00000, 0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg */
     VI_VSYNC_PULSE,
     VI_VSYNC_NEG_LOW,
     VI_HSYNC_VALID_SINGNAL,
     VI_HSYNC_NEG_HIGH,
     VI_VSYNC_VALID_SINGAL,
     VI_VSYNC_VALID_NEG_HIGH,

     /*hsync_hfb    hsync_act    hsync_hhb*/
     {0, 1280, 0,
      /*vsync0_vhb vsync0_act vsync0_hhb*/
      0, 720, 0,
      /*vsync1_vhb vsync1_act vsync1_hhb*/
      0, 0, 0}},
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {1920, 1080},
    {{
         {1920, 1080},

     },
     {VI_REPHASE_MODE_NONE, VI_REPHASE_MODE_NONE}},
    {WDR_MODE_NONE, 1080},
    DATA_RATE_X1};

static VI_DEV_ATTR_S DEV_ATTR_IMX307_2M_BASE = {
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFF00000, 0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg */
     VI_VSYNC_PULSE,
     VI_VSYNC_NEG_LOW,
     VI_HSYNC_VALID_SINGNAL,
     VI_HSYNC_NEG_HIGH,
     VI_VSYNC_VALID_SINGAL,
     VI_VSYNC_VALID_NEG_HIGH,

     /*hsync_hfb    hsync_act    hsync_hhb*/
     {0, 1280, 0,
      /*vsync0_vhb vsync0_act vsync0_hhb*/
      0, 720, 0,
      /*vsync1_vhb vsync1_act vsync1_hhb*/
      0, 0, 0}},
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {1920, 1080},
    {{
         {1920, 1080},

     },
     {VI_REPHASE_MODE_NONE, VI_REPHASE_MODE_NONE}},
    {WDR_MODE_NONE, 1080},
    DATA_RATE_X1};

static VI_DEV_ATTR_S DEV_ATTR_MN34220_2M_BASE = {
    VI_MODE_LVDS,
    VI_WORK_MODE_1Multiplex,
    {0xFFF00000, 0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg */
     VI_VSYNC_PULSE,
     VI_VSYNC_NEG_LOW,
     VI_HSYNC_VALID_SINGNAL,
     VI_HSYNC_NEG_HIGH,
     VI_VSYNC_VALID_SINGAL,
     VI_VSYNC_VALID_NEG_HIGH,

     /*hsync_hfb    hsync_act    hsync_hhb*/
     {0, 1280, 0,
      /*vsync0_vhb vsync0_act vsync0_hhb*/
      0, 720, 0,
      /*vsync1_vhb vsync1_act vsync1_hhb*/
      0, 0, 0}},
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {1920, 1080},
    {{
         {1920, 1080},
     },
     {VI_REPHASE_MODE_NONE, VI_REPHASE_MODE_NONE}},
    {WDR_MODE_NONE, 1080},
    DATA_RATE_X1};

static VI_DEV_ATTR_S DEV_ATTR_IMX335_5M_BASE = {
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFF00000, 0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg */
     VI_VSYNC_PULSE,
     VI_VSYNC_NEG_LOW,
     VI_HSYNC_VALID_SINGNAL,
     VI_HSYNC_NEG_HIGH,
     VI_VSYNC_VALID_SINGAL,
     VI_VSYNC_VALID_NEG_HIGH,

     /*hsync_hfb    hsync_act    hsync_hhb*/
     {0, 1280, 0,
      /*vsync0_vhb vsync0_act vsync0_hhb*/
      0, 720, 0,
      /*vsync1_vhb vsync1_act vsync1_hhb*/
      0, 0, 0}},
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {2592, 1944},
    {{
         {2592, 1944},
     },
     {VI_REPHASE_MODE_NONE, VI_REPHASE_MODE_NONE}},
    {WDR_MODE_NONE, 1944},
    DATA_RATE_X1};

static VI_DEV_ATTR_S DEV_ATTR_IMX335_4M_BASE = {
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFF00000, 0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg */
     VI_VSYNC_PULSE,
     VI_VSYNC_NEG_LOW,
     VI_HSYNC_VALID_SINGNAL,
     VI_HSYNC_NEG_HIGH,
     VI_VSYNC_VALID_SINGAL,
     VI_VSYNC_VALID_NEG_HIGH,

     /*hsync_hfb    hsync_act    hsync_hhb*/
     {0, 1280, 0,
      /*vsync0_vhb vsync0_act vsync0_hhb*/
      0, 720, 0,
      /*vsync1_vhb vsync1_act vsync1_hhb*/
      0, 0, 0}},
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {2592, 1536},
    {{
         {2592, 1536},
     },
     {VI_REPHASE_MODE_NONE, VI_REPHASE_MODE_NONE}},
    {WDR_MODE_NONE, 1536},
    DATA_RATE_X1};

static VI_DEV_ATTR_S DEV_ATTR_SC4210_3M_BASE = {
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFF00000, 0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg */
     VI_VSYNC_PULSE,
     VI_VSYNC_NEG_LOW,
     VI_HSYNC_VALID_SINGNAL,
     VI_HSYNC_NEG_HIGH,
     VI_VSYNC_VALID_SINGAL,
     VI_VSYNC_VALID_NEG_HIGH,

     /*hsync_hfb    hsync_act    hsync_hhb*/
     {0, 1280, 0,
      /*vsync0_vhb vsync0_act vsync0_hhb*/
      0, 720, 0,
      /*vsync1_vhb vsync1_act vsync1_hhb*/
      0, 0, 0}},
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {2560, 1440},
    {{
         {2560, 1440},
     },
     {VI_REPHASE_MODE_NONE, VI_REPHASE_MODE_NONE}},
    {WDR_MODE_NONE, 1440},
    DATA_RATE_X1};

static VI_DEV_ATTR_S DEV_ATTR_IMX458_8M_BASE = {
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFC00000, 0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg */
     VI_VSYNC_PULSE,
     VI_VSYNC_NEG_LOW,
     VI_HSYNC_VALID_SINGNAL,
     VI_HSYNC_NEG_HIGH,
     VI_VSYNC_VALID_SINGAL,
     VI_VSYNC_VALID_NEG_HIGH,

     /*hsync_hfb    hsync_act    hsync_hhb*/
     {0, 1280, 0,
      /*vsync0_vhb vsync0_act vsync0_hhb*/
      0, 720, 0,
      /*vsync1_vhb vsync1_act vsync1_hhb*/
      0, 0, 0}},
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {3840, 2160},
    {{
         {3840, 2160},
     },
     {VI_REPHASE_MODE_NONE, VI_REPHASE_MODE_NONE}},
    {WDR_MODE_NONE, 2160},
    DATA_RATE_X1};

static VI_DEV_ATTR_S DEV_ATTR_IMX458_12M_BASE = {
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFC00000, 0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg */
     VI_VSYNC_PULSE,
     VI_VSYNC_NEG_LOW,
     VI_HSYNC_VALID_SINGNAL,
     VI_HSYNC_NEG_HIGH,
     VI_VSYNC_VALID_SINGAL,
     VI_VSYNC_VALID_NEG_HIGH,

     /*hsync_hfb    hsync_act    hsync_hhb*/
     {0, 1280, 0,
      /*vsync0_vhb vsync0_act vsync0_hhb*/
      0, 720, 0,
      /*vsync1_vhb vsync1_act vsync1_hhb*/
      0, 0, 0}},
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {4000, 3000},
    {{
         {4000, 3000},
     },
     {VI_REPHASE_MODE_NONE, VI_REPHASE_MODE_NONE}},
    {WDR_MODE_NONE, 3000},
    DATA_RATE_X1};

static VI_DEV_ATTR_S DEV_ATTR_IMX458_4M_BASE = {
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFC00000, 0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg */
     VI_VSYNC_PULSE,
     VI_VSYNC_NEG_LOW,
     VI_HSYNC_VALID_SINGNAL,
     VI_HSYNC_NEG_HIGH,
     VI_VSYNC_VALID_SINGAL,
     VI_VSYNC_VALID_NEG_HIGH,

     /*hsync_hfb    hsync_act    hsync_hhb*/
     {0, 1280, 0,
      /*vsync0_vhb vsync0_act vsync0_hhb*/
      0, 720, 0,
      /*vsync1_vhb vsync1_act vsync1_hhb*/
      0, 0, 0}},
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {2716, 1524},
    {{
         {2716, 1524},
     },
     {VI_REPHASE_MODE_NONE, VI_REPHASE_MODE_NONE}},
    {WDR_MODE_NONE, 1524},
    DATA_RATE_X1};

static VI_DEV_ATTR_S DEV_ATTR_IMX458_2M_BASE = {
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFC00000, 0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg */
     VI_VSYNC_PULSE,
     VI_VSYNC_NEG_LOW,
     VI_HSYNC_VALID_SINGNAL,
     VI_HSYNC_NEG_HIGH,
     VI_VSYNC_VALID_SINGAL,
     VI_VSYNC_VALID_NEG_HIGH,

     /*hsync_hfb    hsync_act    hsync_hhb*/
     {0, 1280, 0,
      /*vsync0_vhb vsync0_act vsync0_hhb*/
      0, 720, 0,
      /*vsync1_vhb vsync1_act vsync1_hhb*/
      0, 0, 0}},
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {1920, 1080},
    {{
         {1920, 1080},
     },
     {VI_REPHASE_MODE_NONE, VI_REPHASE_MODE_NONE}},
    {WDR_MODE_NONE, 1080},
    DATA_RATE_X1};

static VI_DEV_ATTR_S DEV_ATTR_IMX458_1M_BASE = {
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFC00000, 0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg */
     VI_VSYNC_PULSE,
     VI_VSYNC_NEG_LOW,
     VI_HSYNC_VALID_SINGNAL,
     VI_HSYNC_NEG_HIGH,
     VI_VSYNC_VALID_SINGAL,
     VI_VSYNC_VALID_NEG_HIGH,

     /*hsync_hfb    hsync_act    hsync_hhb*/
     {0, 1280, 0,
      /*vsync0_vhb vsync0_act vsync0_hhb*/
      0, 720, 0,
      /*vsync1_vhb vsync1_act vsync1_hhb*/
      0, 0, 0}},
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {1280, 720},
    {{
         {1280, 720},
     },
     {VI_REPHASE_MODE_NONE, VI_REPHASE_MODE_NONE}},
    {WDR_MODE_NONE, 720},
    DATA_RATE_X1};

static VI_DEV_ATTR_S DEV_ATTR_OS04B10_4M_BASE = {
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFF00000, 0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg */
     VI_VSYNC_PULSE,
     VI_VSYNC_NEG_LOW,
     VI_HSYNC_VALID_SINGNAL,
     VI_HSYNC_NEG_HIGH,
     VI_VSYNC_VALID_SINGAL,
     VI_VSYNC_VALID_NEG_HIGH,

     /*hsync_hfb    hsync_act    hsync_hhb*/
     {0, 1280, 0,
      /*vsync0_vhb vsync0_act vsync0_hhb*/
      0, 720, 0,
      /*vsync1_vhb vsync1_act vsync1_hhb*/
      0, 0, 0}},
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {2560, 1440},
    {{
         {2560, 1440},
     },
     {VI_REPHASE_MODE_NONE, VI_REPHASE_MODE_NONE}},
    {WDR_MODE_NONE, 1440},
    DATA_RATE_X1};

static VI_DEV_ATTR_S DEV_ATTR_OS05A_4M_BASE = {
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFF00000, 0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg */
     VI_VSYNC_PULSE,
     VI_VSYNC_NEG_LOW,
     VI_HSYNC_VALID_SINGNAL,
     VI_HSYNC_NEG_HIGH,
     VI_VSYNC_VALID_SINGAL,
     VI_VSYNC_VALID_NEG_HIGH,

     /*hsync_hfb    hsync_act    hsync_hhb*/
     {0, 1280, 0,
      /*vsync0_vhb vsync0_act vsync0_hhb*/
      0, 720, 0,
      /*vsync1_vhb vsync1_act vsync1_hhb*/
      0, 0, 0}},
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {2688, 1536},
    {{
         {2688, 1536},
     },
     {VI_REPHASE_MODE_NONE, VI_REPHASE_MODE_NONE}},
    {WDR_MODE_NONE, 1536},
    DATA_RATE_X1};

static VI_DEV_ATTR_S DEV_ATTR_OS08A10_8M_BASE = {
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFC00000, 0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg */
     VI_VSYNC_PULSE,
     VI_VSYNC_NEG_LOW,
     VI_HSYNC_VALID_SINGNAL,
     VI_HSYNC_NEG_HIGH,
     VI_VSYNC_VALID_SINGAL,
     VI_VSYNC_VALID_NEG_HIGH,

     /*hsync_hfb    hsync_act    hsync_hhb*/
     {0, 1280, 0,
      /*vsync0_vhb vsync0_act vsync0_hhb*/
      0, 720, 0,
      /*vsync1_vhb vsync1_act vsync1_hhb*/
      0, 0, 0}},
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {3840, 2160},
    {{
         {3840, 2160},
     },
     {VI_REPHASE_MODE_NONE, VI_REPHASE_MODE_NONE}},
    {WDR_MODE_NONE, 2160},
    DATA_RATE_X1};

static VI_DEV_ATTR_S DEV_ATTR_GC2053_2M_BASE = {
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFC00000, 0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg */
     VI_VSYNC_PULSE,
     VI_VSYNC_NEG_LOW,
     VI_HSYNC_VALID_SINGNAL,
     VI_HSYNC_NEG_HIGH,
     VI_VSYNC_VALID_SINGAL,
     VI_VSYNC_VALID_NEG_HIGH,

     /*hsync_hfb    hsync_act    hsync_hhb*/
     {0, 1280, 0,
      /*vsync0_vhb vsync0_act vsync0_hhb*/
      0, 720, 0,
      /*vsync1_vhb vsync1_act vsync1_hhb*/
      0, 0, 0}},
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {1920, 1080},
    {{
         {1920, 1080},
     },
     {VI_REPHASE_MODE_NONE, VI_REPHASE_MODE_NONE}},
    {WDR_MODE_NONE, 1080},
    DATA_RATE_X1};

static VI_DEV_ATTR_S DEV_ATTR_OV12870_8M_BASE = {
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFC00000, 0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg */
     VI_VSYNC_PULSE,
     VI_VSYNC_NEG_LOW,
     VI_HSYNC_VALID_SINGNAL,
     VI_HSYNC_NEG_HIGH,
     VI_VSYNC_VALID_SINGAL,
     VI_VSYNC_VALID_NEG_HIGH,

     /*hsync_hfb    hsync_act    hsync_hhb*/
     {0, 1280, 0,
      /*vsync0_vhb vsync0_act vsync0_hhb*/
      0, 720, 0,
      /*vsync1_vhb vsync1_act vsync1_hhb*/
      0, 0, 0}},
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {3840, 2160},
    {{
         {3840, 2160},
     },
     {VI_REPHASE_MODE_NONE, VI_REPHASE_MODE_NONE}},
    {WDR_MODE_NONE, 2160},
    DATA_RATE_X1};

static VI_DEV_ATTR_S DEV_ATTR_OV12870_12M_BASE = {
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFC00000, 0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg */
     VI_VSYNC_PULSE,
     VI_VSYNC_NEG_LOW,
     VI_HSYNC_VALID_SINGNAL,
     VI_HSYNC_NEG_HIGH,
     VI_VSYNC_VALID_SINGAL,
     VI_VSYNC_VALID_NEG_HIGH,

     /*hsync_hfb    hsync_act    hsync_hhb*/
     {0, 1280, 0,
      /*vsync0_vhb vsync0_act vsync0_hhb*/
      0, 720, 0,
      /*vsync1_vhb vsync1_act vsync1_hhb*/
      0, 0, 0}},
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {4000, 3000},
    {{
         {4000, 3000},
     },
     {VI_REPHASE_MODE_NONE, VI_REPHASE_MODE_NONE}},
    {WDR_MODE_NONE, 3000},
    DATA_RATE_X1};

static VI_DEV_ATTR_S DEV_ATTR_OV12870_2M_BASE = {
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFC00000, 0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg */
     VI_VSYNC_PULSE,
     VI_VSYNC_NEG_LOW,
     VI_HSYNC_VALID_SINGNAL,
     VI_HSYNC_NEG_HIGH,
     VI_VSYNC_VALID_SINGAL,
     VI_VSYNC_VALID_NEG_HIGH,

     /*hsync_hfb    hsync_act    hsync_hhb*/
     {0, 1280, 0,
      /*vsync0_vhb vsync0_act vsync0_hhb*/
      0, 720, 0,
      /*vsync1_vhb vsync1_act vsync1_hhb*/
      0, 0, 0}},
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {1920, 1080},
    {{
         {1920, 1080},
     },
     {VI_REPHASE_MODE_NONE, VI_REPHASE_MODE_NONE}},
    {WDR_MODE_NONE, 1080},
    DATA_RATE_X1};

static VI_DEV_ATTR_S DEV_ATTR_OV12870_1M_BASE = {
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFC00000, 0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg */
     VI_VSYNC_PULSE,
     VI_VSYNC_NEG_LOW,
     VI_HSYNC_VALID_SINGNAL,
     VI_HSYNC_NEG_HIGH,
     VI_VSYNC_VALID_SINGAL,
     VI_VSYNC_VALID_NEG_HIGH,

     /*hsync_hfb    hsync_act    hsync_hhb*/
     {0, 1280, 0,
      /*vsync0_vhb vsync0_act vsync0_hhb*/
      0, 720, 0,
      /*vsync1_vhb vsync1_act vsync1_hhb*/
      0, 0, 0}},
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {1280, 720},
    {{
         {1280, 720},
     },
     {VI_REPHASE_MODE_NONE, VI_REPHASE_MODE_NONE}},
    {WDR_MODE_NONE, 720},
    DATA_RATE_X1};

static VI_DEV_ATTR_S DEV_ATTR_IMX415_8M_BASE = {
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFF00000, 0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg */
     VI_VSYNC_PULSE,
     VI_VSYNC_NEG_LOW,
     VI_HSYNC_VALID_SINGNAL,
     VI_HSYNC_NEG_HIGH,
     VI_VSYNC_VALID_SINGAL,
     VI_VSYNC_VALID_NEG_HIGH,

     /*hsync_hfb    hsync_act    hsync_hhb*/
     {0, 1280, 0,
      /*vsync0_vhb vsync0_act vsync0_hhb*/
      0, 720, 0,
      /*vsync1_vhb vsync1_act vsync1_hhb*/
      0, 0, 0}},
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {3840, 2160},
    {{
         {3840, 2160},
     },
     {VI_REPHASE_MODE_NONE, VI_REPHASE_MODE_NONE}},
    {WDR_MODE_NONE, 2160},
    DATA_RATE_X1};

static VI_PIPE_ATTR_S PIPE_ATTR_1920x1080_RAW12_420 = {
    /* bBindDev bYuvSkip */
    VI_PIPE_BYPASS_NONE,
    HI_FALSE,
    HI_FALSE,
    1920,
    1080,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_LINE,
    DATA_BITWIDTH_12,
    HI_FALSE,
    {PIXEL_FORMAT_YVU_SEMIPLANAR_420, DATA_BITWIDTH_8, VI_NR_REF_FROM_RFR,
     COMPRESS_MODE_NONE},
    HI_FALSE,
    {-1, -1}};

static VI_PIPE_ATTR_S PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR = {
    VI_PIPE_BYPASS_NONE,
    HI_FALSE,
    HI_FALSE,
    1920,
    1080,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_NONE,
    DATA_BITWIDTH_12,
    HI_FALSE,
    {PIXEL_FORMAT_YVU_SEMIPLANAR_420, DATA_BITWIDTH_8, VI_NR_REF_FROM_RFR,
     COMPRESS_MODE_NONE},
    HI_FALSE,
    {-1, -1}};

static VI_PIPE_ATTR_S PIPE_ATTR_1920x1080_RAW12_420_3DNR_CHN0 = {
    VI_PIPE_BYPASS_NONE,
    HI_FALSE,
    HI_FALSE,
    1920,
    1080,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_LINE,
    DATA_BITWIDTH_12,
    HI_TRUE,
    {PIXEL_FORMAT_YVU_SEMIPLANAR_420, DATA_BITWIDTH_8, VI_NR_REF_FROM_CHN0,
     COMPRESS_MODE_NONE},
    HI_FALSE,
    {-1, -1}};

static VI_PIPE_ATTR_S PIPE_ATTR_2592x1944_RAW12_420_3DNR_RFR = {
    VI_PIPE_BYPASS_NONE,
    HI_FALSE,
    HI_FALSE,
    2592,
    1944,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_LINE,
    DATA_BITWIDTH_12,
    HI_FALSE,
    {PIXEL_FORMAT_YVU_SEMIPLANAR_420, DATA_BITWIDTH_8, VI_NR_REF_FROM_RFR,
     COMPRESS_MODE_NONE},
    HI_FALSE,
    {-1, -1}};

static VI_PIPE_ATTR_S PIPE_ATTR_2592x1944_RAW10_420_3DNR_RFR = {
    VI_PIPE_BYPASS_NONE,
    HI_FALSE,
    HI_FALSE,
    2592,
    1944,
    PIXEL_FORMAT_RGB_BAYER_10BPP,
    COMPRESS_MODE_LINE,
    DATA_BITWIDTH_10,
    HI_FALSE,
    {PIXEL_FORMAT_YVU_SEMIPLANAR_420, DATA_BITWIDTH_8, VI_NR_REF_FROM_RFR,
     COMPRESS_MODE_NONE},
    HI_FALSE,
    {-1, -1}};

static VI_PIPE_ATTR_S PIPE_ATTR_2592x1536_RAW12_420_3DNR_RFR = {
    VI_PIPE_BYPASS_NONE,
    HI_FALSE,
    HI_FALSE,
    2592,
    1536,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_LINE,
    DATA_BITWIDTH_12,
    HI_FALSE,
    {PIXEL_FORMAT_YVU_SEMIPLANAR_420, DATA_BITWIDTH_8, VI_NR_REF_FROM_RFR,
     COMPRESS_MODE_NONE},
    HI_FALSE,
    {-1, -1}};

static VI_PIPE_ATTR_S PIPE_ATTR_2592x1536_RAW10_420_3DNR_RFR = {
    VI_PIPE_BYPASS_NONE,
    HI_FALSE,
    HI_FALSE,
    2592,
    1536,
    PIXEL_FORMAT_RGB_BAYER_10BPP,
    COMPRESS_MODE_LINE,
    DATA_BITWIDTH_10,
    HI_FALSE,
    {PIXEL_FORMAT_YVU_SEMIPLANAR_420, DATA_BITWIDTH_8, VI_NR_REF_FROM_RFR,
     COMPRESS_MODE_NONE},
    HI_FALSE,
    {-1, -1}};

static VI_PIPE_ATTR_S PIPE_ATTR_2560x1440_RAW12_420_3DNR_RFR = {
    VI_PIPE_BYPASS_NONE,
    HI_FALSE,
    HI_FALSE,
    2560,
    1440,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_NONE,
    DATA_BITWIDTH_12,
    HI_FALSE,
    {PIXEL_FORMAT_YVU_SEMIPLANAR_420, DATA_BITWIDTH_8, VI_NR_REF_FROM_RFR,
     COMPRESS_MODE_NONE},
    HI_FALSE,
    {-1, -1}};

static VI_PIPE_ATTR_S PIPE_ATTR_2560x1440_RAW10_420_3DNR_RFR = {
    VI_PIPE_BYPASS_NONE,
    HI_FALSE,
    HI_FALSE,
    2560,
    1440,
    PIXEL_FORMAT_RGB_BAYER_10BPP,
    COMPRESS_MODE_NONE,
    DATA_BITWIDTH_10,
    HI_FALSE,
    {PIXEL_FORMAT_YVU_SEMIPLANAR_420, DATA_BITWIDTH_8, VI_NR_REF_FROM_RFR,
     COMPRESS_MODE_NONE},
    HI_FALSE,
    {-1, -1}};

static VI_PIPE_ATTR_S PIPE_ATTR_3840x2160_RAW10_420_3DNR_RFR = {
    VI_PIPE_BYPASS_NONE,
    HI_FALSE,
    HI_FALSE,
    3840,
    2160,
    PIXEL_FORMAT_RGB_BAYER_10BPP,
    COMPRESS_MODE_LINE,
    DATA_BITWIDTH_10,
    HI_TRUE,
    {PIXEL_FORMAT_YVU_SEMIPLANAR_420, DATA_BITWIDTH_8, VI_NR_REF_FROM_RFR,
     COMPRESS_MODE_NONE},
    HI_FALSE,
    {-1, -1}};

static VI_PIPE_ATTR_S PIPE_ATTR_3840x2160_RAW12_420_3DNR_RFR = {
    VI_PIPE_BYPASS_NONE,
    HI_FALSE,
    HI_FALSE,
    3840,
    2160,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_LINE,
    DATA_BITWIDTH_12,
    HI_TRUE,
    {PIXEL_FORMAT_YVU_SEMIPLANAR_420, DATA_BITWIDTH_8, VI_NR_REF_FROM_RFR,
     COMPRESS_MODE_NONE},
    HI_FALSE,
    {-1, -1}};

static VI_PIPE_ATTR_S PIPE_ATTR_4000x3000_RAW10_420_3DNR_RFR = {
    VI_PIPE_BYPASS_NONE,
    HI_FALSE,
    HI_FALSE,
    4000,
    3000,
    PIXEL_FORMAT_RGB_BAYER_10BPP,
    COMPRESS_MODE_LINE,
    DATA_BITWIDTH_10,
    HI_TRUE,
    {PIXEL_FORMAT_YVU_SEMIPLANAR_420, DATA_BITWIDTH_8, VI_NR_REF_FROM_RFR,
     COMPRESS_MODE_NONE},
    HI_FALSE,
    {-1, -1}};

static VI_PIPE_ATTR_S PIPE_ATTR_2716x1524_RAW10_420_3DNR_RFR = {
    VI_PIPE_BYPASS_NONE,
    HI_FALSE,
    HI_FALSE,
    2716,
    1524,
    PIXEL_FORMAT_RGB_BAYER_10BPP,
    COMPRESS_MODE_LINE,
    DATA_BITWIDTH_10,
    HI_TRUE,
    {PIXEL_FORMAT_YVU_SEMIPLANAR_420, DATA_BITWIDTH_8, VI_NR_REF_FROM_RFR,
     COMPRESS_MODE_NONE},
    HI_FALSE,
    {-1, -1}};

static VI_PIPE_ATTR_S PIPE_ATTR_1920x1080_RAW10_420_3DNR_RFR = {
    VI_PIPE_BYPASS_NONE,
    HI_FALSE,
    HI_FALSE,
    1920,
    1080,
    PIXEL_FORMAT_RGB_BAYER_10BPP,
    COMPRESS_MODE_LINE,
    DATA_BITWIDTH_10,
    HI_TRUE,
    {PIXEL_FORMAT_YVU_SEMIPLANAR_420, DATA_BITWIDTH_8, VI_NR_REF_FROM_RFR,
     COMPRESS_MODE_NONE},
    HI_FALSE,
    {-1, -1}};

static VI_PIPE_ATTR_S PIPE_ATTR_1280x720_RAW10_420_3DNR_RFR = {
    VI_PIPE_BYPASS_NONE,
    HI_FALSE,
    HI_FALSE,
    1280,
    720,
    PIXEL_FORMAT_RGB_BAYER_10BPP,
    COMPRESS_MODE_LINE,
    DATA_BITWIDTH_10,
    HI_TRUE,
    {PIXEL_FORMAT_YVU_SEMIPLANAR_420, DATA_BITWIDTH_8, VI_NR_REF_FROM_RFR,
     COMPRESS_MODE_NONE},
    HI_FALSE,
    {-1, -1}};

static VI_PIPE_ATTR_S PIPE_ATTR_2688x1536_RAW12_420_3DNR_RFR = {
    VI_PIPE_BYPASS_NONE,
    HI_FALSE,
    HI_FALSE,
    2688,
    1536,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_LINE,
    DATA_BITWIDTH_12,
    HI_FALSE,
    {PIXEL_FORMAT_YVU_SEMIPLANAR_420, DATA_BITWIDTH_8, VI_NR_REF_FROM_RFR,
     COMPRESS_MODE_NONE},
    HI_FALSE,
    {-1, -1}};

static VI_PIPE_ATTR_S PIPE_ATTR_2688x1536_RAW10_420_3DNR_RFR = {
    VI_PIPE_BYPASS_NONE,
    HI_FALSE,
    HI_FALSE,
    2688,
    1536,
    PIXEL_FORMAT_RGB_BAYER_10BPP,
    COMPRESS_MODE_LINE,
    DATA_BITWIDTH_10,
    HI_FALSE,
    {PIXEL_FORMAT_YVU_SEMIPLANAR_420, DATA_BITWIDTH_8, VI_NR_REF_FROM_RFR,
     COMPRESS_MODE_NONE},
    HI_FALSE,
    {-1, -1}};

static VI_CHN_ATTR_S CHN_ATTR_1920x1080_422_SDR8_LINEAR = {
    {1920, 1080},
    PIXEL_FORMAT_YVU_SEMIPLANAR_422,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    HI_FALSE,
    HI_FALSE,
    0,
    {-1, -1}};

static VI_CHN_ATTR_S CHN_ATTR_1920x1080_420_SDR8_LINEAR = {
    {1920, 1080},
    PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    HI_FALSE,
    HI_FALSE,
    0,
    {-1, -1}};

static VI_CHN_ATTR_S CHN_ATTR_1920x1080_400_SDR8_LINEAR = {{1920, 1080},
                                                           PIXEL_FORMAT_YUV_400,
                                                           DYNAMIC_RANGE_SDR8,
                                                           VIDEO_FORMAT_LINEAR,
                                                           COMPRESS_MODE_NONE,
                                                           HI_FALSE,
                                                           HI_FALSE,
                                                           0,
                                                           {-1, -1}};

static VI_CHN_ATTR_S CHN_ATTR_2592x1944_420_SDR8_LINEAR = {
    {2592, 1944},
    PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    HI_FALSE,
    HI_FALSE,
    0,
    {-1, -1}};

static VI_CHN_ATTR_S CHN_ATTR_2592x1536_420_SDR8_LINEAR = {
    {2592, 1536},
    PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    HI_FALSE,
    HI_FALSE,
    0,
    {-1, -1}};

static VI_CHN_ATTR_S CHN_ATTR_2560x1440_420_SDR8_LINEAR = {
    {2560, 1440},
    PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    HI_FALSE,
    HI_FALSE,
    0,
    {-1, -1}};

static VI_CHN_ATTR_S CHN_ATTR_4000x3000_420_SDR8_LINEAR = {
    {4000, 3000},
    PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    HI_FALSE,
    HI_FALSE,
    0,
    {-1, -1}};

static VI_CHN_ATTR_S CHN_ATTR_3840x2160_420_SDR8_LINEAR = {
    {3840, 2160},
    PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    HI_FALSE,
    HI_FALSE,
    0,
    {-1, -1}};

static VI_CHN_ATTR_S CHN_ATTR_2716x1524_420_SDR8_LINEAR = {
    {2716, 1524},
    PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    HI_FALSE,
    HI_FALSE,
    0,
    {-1, -1}};

static VI_CHN_ATTR_S CHN_ATTR_1280x720_420_SDR8_LINEAR = {
    {1280, 720},
    PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    HI_FALSE,
    HI_FALSE,
    0,
    {-1, -1}};

static VI_CHN_ATTR_S CHN_ATTR_2688x1536_420_SDR8_LINEAR = {
    {2688, 1536},
    PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    HI_FALSE,
    HI_FALSE,
    0,
    {-1, -1}};

bool Vi::setParam(VI_PIPE viPipe) {
  VI_VPSS_MODE_S stVIVPSSMode;
  HI_U32 s32Ret = HI_MPI_SYS_GetVIVPSSMode(&stVIVPSSMode);
  if (HI_SUCCESS != s32Ret) {
    SZ_LOG_ERROR("Get VI-VPSS mode Param failed with {:X}!", s32Ret);
    return false;
  }
  stVIVPSSMode.aenMode[viPipe] = VI_OFFLINE_VPSS_OFFLINE;
  // stVIVPSSMode.aenMode[2] = VI_OFFLINE_VPSS_OFFLINE;
  s32Ret = HI_MPI_SYS_SetVIVPSSMode(&stVIVPSSMode);
  if (HI_SUCCESS != s32Ret) {
    SZ_LOG_ERROR("Set VI-VPSS mode Param failed with {0:X}  {1}!", s32Ret,
                 viPipe);
    return false;
  }
  return true;
}

void Vi::getDevAttrBySensor(SensorType sensorType, VI_DEV_ATTR_S *pViDevAttr) {
  switch (sensorType) {
    case SONY_IMX327_MIPI_2M_30FPS_12BIT:
    case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT:
    case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
      memcpy_s(pViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX327_2M_BASE,
               sizeof(VI_DEV_ATTR_S));
      break;

    case SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1:
      memcpy_s(pViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX327_2M_BASE,
               sizeof(VI_DEV_ATTR_S));
      pViDevAttr->au32ComponentMask[0] = 0xFFC00000;
      break;

    case SONY_IMX307_MIPI_2M_30FPS_12BIT:
    case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT:
      memcpy_s(pViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX307_2M_BASE,
               sizeof(VI_DEV_ATTR_S));
      break;

    case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
    case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
      memcpy_s(pViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX307_2M_BASE,
               sizeof(VI_DEV_ATTR_S));
      pViDevAttr->au32ComponentMask[0] = 0xFFC00000;
      break;

    case PANASONIC_MN34220_LVDS_2M_30FPS_12BIT:
      memcpy_s(pViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_MN34220_2M_BASE,
               sizeof(VI_DEV_ATTR_S));
      break;

    case SONY_IMX335_MIPI_5M_30FPS_12BIT:
      memcpy_s(pViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX335_5M_BASE,
               sizeof(VI_DEV_ATTR_S));
      break;

    case SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1:
      memcpy_s(pViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX335_5M_BASE,
               sizeof(VI_DEV_ATTR_S));
      pViDevAttr->au32ComponentMask[0] = 0xFFC00000;
      break;

    case SONY_IMX335_MIPI_4M_30FPS_12BIT:
      memcpy_s(pViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX335_4M_BASE,
               sizeof(VI_DEV_ATTR_S));
      break;

    case SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1:
      memcpy_s(pViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX335_4M_BASE,
               sizeof(VI_DEV_ATTR_S));
      pViDevAttr->au32ComponentMask[0] = 0xFFC00000;
      break;

    case SMART_SC4210_MIPI_3M_30FPS_12BIT:
    case SMART_SC4210_MIPI_3M_30FPS_10BIT_WDR2TO1:
      memcpy_s(pViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_SC4210_3M_BASE,
               sizeof(VI_DEV_ATTR_S));
      break;

    case SONY_IMX458_MIPI_8M_30FPS_10BIT:
      memcpy(pViDevAttr, &DEV_ATTR_IMX458_8M_BASE, sizeof(VI_DEV_ATTR_S));
      break;
    case SONY_IMX458_MIPI_12M_20FPS_10BIT:
      memcpy(pViDevAttr, &DEV_ATTR_IMX458_12M_BASE, sizeof(VI_DEV_ATTR_S));
      break;
    case SONY_IMX458_MIPI_4M_60FPS_10BIT:
    case SONY_IMX458_MIPI_4M_40FPS_10BIT:
      memcpy(pViDevAttr, &DEV_ATTR_IMX458_4M_BASE, sizeof(VI_DEV_ATTR_S));
      break;
    case SONY_IMX458_MIPI_2M_90FPS_10BIT:
      memcpy(pViDevAttr, &DEV_ATTR_IMX458_2M_BASE, sizeof(VI_DEV_ATTR_S));
      break;
    case SONY_IMX458_MIPI_1M_129FPS_10BIT:
      memcpy(pViDevAttr, &DEV_ATTR_IMX458_1M_BASE, sizeof(VI_DEV_ATTR_S));
      break;
    case GALAXYCORE_GC2053_MIPI_2M_30FPS_10BIT:
      memcpy_s(pViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_GC2053_2M_BASE,
               sizeof(VI_DEV_ATTR_S));
      pViDevAttr->au32ComponentMask[0] = 0xFFC00000;
      break;
    case OMNIVISION_OV12870_MIPI_8M_30FPS_10BIT:
      memcpy(pViDevAttr, &DEV_ATTR_OV12870_8M_BASE, sizeof(VI_DEV_ATTR_S));
      break;
    case OMNIVISION_OV12870_MIPI_12M_30FPS_10BIT:
      memcpy(pViDevAttr, &DEV_ATTR_OV12870_12M_BASE, sizeof(VI_DEV_ATTR_S));
      break;
    case OMNIVISION_OV12870_MIPI_2M_120FPS_10BIT:
      memcpy(pViDevAttr, &DEV_ATTR_OV12870_2M_BASE, sizeof(VI_DEV_ATTR_S));
      break;
    case OMNIVISION_OV12870_MIPI_1M_240FPS_10BIT:
      memcpy(pViDevAttr, &DEV_ATTR_OV12870_1M_BASE, sizeof(VI_DEV_ATTR_S));
      break;

    case OMNIVISION_OS04B10_MIPI_4M_30FPS_10BIT:
      memcpy_s(pViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_OS04B10_4M_BASE,
               sizeof(VI_DEV_ATTR_S));
      break;

    case OMNIVISION_OS05A_MIPI_4M_30FPS_12BIT:
      memcpy_s(pViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_OS05A_4M_BASE,
               sizeof(VI_DEV_ATTR_S));
      break;
    case OMNIVISION_OS05A_MIPI_4M_30FPS_10BIT_WDR2TO1:
      memcpy_s(pViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_OS05A_4M_BASE,
               sizeof(VI_DEV_ATTR_S));
      pViDevAttr->au32ComponentMask[0] = 0xFFC00000;
      break;

    case OMNIVISION_OS08A10_MIPI_8M_30FPS_10BIT:
      memcpy_s(pViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_OS08A10_8M_BASE,
               sizeof(VI_DEV_ATTR_S));
      pViDevAttr->au32ComponentMask[0] = 0xFFC00000;
      break;

    case SONY_IMX415_MIPI_8M_30FPS_12BIT:
    case SONY_IMX415_MIPI_8M_20FPS_12BIT:
      memcpy(pViDevAttr, &DEV_ATTR_IMX415_8M_BASE, sizeof(VI_DEV_ATTR_S));
      break;

    default:
      memcpy_s(pViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX327_2M_BASE,
               sizeof(VI_DEV_ATTR_S));
  }
}

void Vi::getPipeAttrBySensor(SensorType sensorType,
                             VI_PIPE_ATTR_S *pstPipeAttr) {
  switch (sensorType) {
    case SONY_IMX327_MIPI_2M_30FPS_12BIT:
    case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT:
    case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
      memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
               &PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
      break;

    case SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1:
      memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
               &PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
      pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_10BPP;
      pstPipeAttr->enBitWidth = DATA_BITWIDTH_10;
      break;

    case SONY_IMX307_MIPI_2M_30FPS_12BIT:
    case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT:
      memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
               &PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
      break;

    case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
    case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
      memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
               &PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
      pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_10BPP;
      pstPipeAttr->enBitWidth = DATA_BITWIDTH_10;
      break;

    case PANASONIC_MN34220_LVDS_2M_30FPS_12BIT:
      memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
               &PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
      break;

    case SONY_IMX335_MIPI_5M_30FPS_12BIT:
      memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
               &PIPE_ATTR_2592x1944_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
      break;

    case SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1:
      memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
               &PIPE_ATTR_2592x1944_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
      break;

    case SONY_IMX335_MIPI_4M_30FPS_12BIT:
      memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
               &PIPE_ATTR_2592x1536_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
      break;

    case SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1:
      memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
               &PIPE_ATTR_2592x1536_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
      break;

    case SMART_SC4210_MIPI_3M_30FPS_12BIT:
      memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
               &PIPE_ATTR_2560x1440_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
      break;

    case SMART_SC4210_MIPI_3M_30FPS_10BIT_WDR2TO1:
      memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
               &PIPE_ATTR_2560x1440_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
      break;

    case OMNIVISION_OS08A10_MIPI_8M_30FPS_10BIT:
    case SONY_IMX458_MIPI_8M_30FPS_10BIT:
      memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
               &PIPE_ATTR_3840x2160_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
      break;
    case SONY_IMX458_MIPI_12M_20FPS_10BIT:
      memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
               &PIPE_ATTR_4000x3000_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
      break;
    case SONY_IMX458_MIPI_4M_60FPS_10BIT:
    case SONY_IMX458_MIPI_4M_40FPS_10BIT:
      memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
               &PIPE_ATTR_2716x1524_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
      break;
    case SONY_IMX458_MIPI_2M_90FPS_10BIT:
      memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
               &PIPE_ATTR_1920x1080_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
      break;
    case SONY_IMX458_MIPI_1M_129FPS_10BIT:
      memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
               &PIPE_ATTR_1280x720_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
      break;
    case GALAXYCORE_GC2053_MIPI_2M_30FPS_10BIT:
      memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
               &PIPE_ATTR_1920x1080_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
      break;
    case OMNIVISION_OV12870_MIPI_8M_30FPS_10BIT:
      memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
               &PIPE_ATTR_3840x2160_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
      break;
    case OMNIVISION_OV12870_MIPI_12M_30FPS_10BIT:
      memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
               &PIPE_ATTR_4000x3000_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
      break;
    case OMNIVISION_OV12870_MIPI_2M_120FPS_10BIT:
      memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
               &PIPE_ATTR_1920x1080_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
      break;
    case OMNIVISION_OV12870_MIPI_1M_240FPS_10BIT:
      memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
               &PIPE_ATTR_1280x720_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
      break;

    case OMNIVISION_OS04B10_MIPI_4M_30FPS_10BIT:
      memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
               &PIPE_ATTR_2560x1440_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
      break;

    case OMNIVISION_OS05A_MIPI_4M_30FPS_12BIT:
      memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
               &PIPE_ATTR_2688x1536_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
      break;
    case OMNIVISION_OS05A_MIPI_4M_30FPS_10BIT_WDR2TO1:
      memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
               &PIPE_ATTR_2688x1536_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
      break;

    case SONY_IMX415_MIPI_8M_30FPS_12BIT:
    case SONY_IMX415_MIPI_8M_20FPS_12BIT:
      memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
               &PIPE_ATTR_3840x2160_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
      break;

    default:
      memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
               &PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
  }
}

void Vi::getChAttrBySensor(SensorType sensorType, VI_CHN_ATTR_S *pChAttr) {
  switch (sensorType) {
    case SONY_IMX327_MIPI_2M_30FPS_12BIT:
    case SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1:
    case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT:
    case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
      memcpy_s(pChAttr, sizeof(VI_CHN_ATTR_S),
               &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
      break;

    case SONY_IMX307_MIPI_2M_30FPS_12BIT:
    case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
    case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT:
    case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
      memcpy_s(pChAttr, sizeof(VI_CHN_ATTR_S),
               &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
      break;

    case PANASONIC_MN34220_LVDS_2M_30FPS_12BIT:
      memcpy_s(pChAttr, sizeof(VI_CHN_ATTR_S),
               &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
      break;

    case SONY_IMX335_MIPI_5M_30FPS_12BIT:
    case SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1:
      memcpy_s(pChAttr, sizeof(VI_CHN_ATTR_S),
               &CHN_ATTR_2592x1944_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
      break;

    case SONY_IMX335_MIPI_4M_30FPS_12BIT:
    case SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1:
      memcpy_s(pChAttr, sizeof(VI_CHN_ATTR_S),
               &CHN_ATTR_2592x1536_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
      break;

    case SMART_SC4210_MIPI_3M_30FPS_12BIT:
    case SMART_SC4210_MIPI_3M_30FPS_10BIT_WDR2TO1:
      memcpy_s(pChAttr, sizeof(VI_CHN_ATTR_S),
               &CHN_ATTR_2560x1440_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
      break;

    case OMNIVISION_OS08A10_MIPI_8M_30FPS_10BIT:
    case SONY_IMX458_MIPI_8M_30FPS_10BIT:
      memcpy_s(pChAttr, sizeof(VI_CHN_ATTR_S),
               &CHN_ATTR_3840x2160_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
      break;
    case SONY_IMX458_MIPI_12M_20FPS_10BIT:
      memcpy_s(pChAttr, sizeof(VI_CHN_ATTR_S),
               &CHN_ATTR_4000x3000_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
      break;
    case SONY_IMX458_MIPI_4M_60FPS_10BIT:
    case SONY_IMX458_MIPI_4M_40FPS_10BIT:
      memcpy_s(pChAttr, sizeof(VI_CHN_ATTR_S),
               &CHN_ATTR_2716x1524_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
      break;
    case SONY_IMX458_MIPI_2M_90FPS_10BIT:
      memcpy_s(pChAttr, sizeof(VI_CHN_ATTR_S),
               &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
      break;
    case SONY_IMX458_MIPI_1M_129FPS_10BIT:
      memcpy_s(pChAttr, sizeof(VI_CHN_ATTR_S),
               &CHN_ATTR_1280x720_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
      break;
    case GALAXYCORE_GC2053_MIPI_2M_30FPS_10BIT:
      memcpy_s(pChAttr, sizeof(VI_CHN_ATTR_S),
               &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
      break;
    case OMNIVISION_OV12870_MIPI_8M_30FPS_10BIT:
      memcpy_s(pChAttr, sizeof(VI_CHN_ATTR_S),
               &CHN_ATTR_3840x2160_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
      break;
    case OMNIVISION_OV12870_MIPI_12M_30FPS_10BIT:
      memcpy_s(pChAttr, sizeof(VI_CHN_ATTR_S),
               &CHN_ATTR_4000x3000_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
      break;
    case OMNIVISION_OV12870_MIPI_2M_120FPS_10BIT:
      memcpy_s(pChAttr, sizeof(VI_CHN_ATTR_S),
               &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
      break;
    case OMNIVISION_OV12870_MIPI_1M_240FPS_10BIT:
      memcpy_s(pChAttr, sizeof(VI_CHN_ATTR_S),
               &CHN_ATTR_1280x720_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
      break;

    case OMNIVISION_OS04B10_MIPI_4M_30FPS_10BIT:
      memcpy_s(pChAttr, sizeof(VI_CHN_ATTR_S),
               &CHN_ATTR_2560x1440_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
      break;

    case OMNIVISION_OS05A_MIPI_4M_30FPS_12BIT:
    case OMNIVISION_OS05A_MIPI_4M_30FPS_10BIT_WDR2TO1:
      memcpy_s(pChAttr, sizeof(VI_CHN_ATTR_S),
               &CHN_ATTR_2688x1536_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
      break;

    case SONY_IMX415_MIPI_8M_30FPS_12BIT:
    case SONY_IMX415_MIPI_8M_20FPS_12BIT:
      memcpy_s(pChAttr, sizeof(VI_CHN_ATTR_S),
               &CHN_ATTR_3840x2160_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
      break;

    default:
      memcpy_s(pChAttr, sizeof(VI_CHN_ATTR_S),
               &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
  }
}

bool Vi::startDev(VI_DEV devIndex, SensorType sensorType) {
  VI_DEV_ATTR_S viDevAttr;
  getDevAttrBySensor(sensorType, &viDevAttr);
  viDevAttr.stWDRAttr.enWDRMode = WDR_MODE_NONE;

  VI_VPSS_MODE_E mastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
  if (VI_PARALLEL_VPSS_OFFLINE == mastPipeMode ||
      VI_PARALLEL_VPSS_PARALLEL == mastPipeMode) {
    viDevAttr.enDataRate = DATA_RATE_X2;
  }

  HI_U32 s32Ret = HI_MPI_VI_SetDevAttr(devIndex, &viDevAttr);
  if (s32Ret != HI_SUCCESS) {
    SZ_LOG_ERROR("HI_MPI_VI_SetDevAttr failed with {:X}!", s32Ret);
    return false;
  }

  s32Ret = HI_MPI_VI_EnableDev(devIndex);
  if (s32Ret != HI_SUCCESS) {
    SZ_LOG_ERROR("HI_MPI_VI_EnableDev failed with {:X}!", s32Ret);
    return false;
  }

  return true;
}

bool Vi::devBindPipe(VI_DEV viDev, VI_PIPE viPipe) {
  VI_DEV_BIND_PIPE_S stDevBindPipe = {0};
  stDevBindPipe.PipeId[0] = viPipe;
  stDevBindPipe.u32Num = 1;
  HI_U32 s32Ret = HI_MPI_VI_SetDevBindPipe(viDev, &stDevBindPipe);
  if (s32Ret != HI_SUCCESS) {
    SZ_LOG_ERROR("HI_MPI_VI_SetDevBindPipe failed with {:X}!", s32Ret);
    return false;
  }

  return true;
}

bool Vi::startPipe(VI_PIPE viPipe, SensorType sensorType) {
  VI_PIPE_ATTR_S stPipeAttr;
  getPipeAttrBySensor(sensorType, &stPipeAttr);
  HI_BOOL bBypass = HI_FALSE;
  if (bBypass) {
    stPipeAttr.bIspBypass = HI_TRUE;
    stPipeAttr.enPixFmt = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    stPipeAttr.enBitWidth = DATA_BITWIDTH_8;
  }

  if (2 == viPipe || 3 == viPipe) {
    stPipeAttr.enCompressMode = COMPRESS_MODE_NONE;
  }

  HI_U32 s32Ret = HI_MPI_VI_CreatePipe(viPipe, &stPipeAttr);
  if (s32Ret != HI_SUCCESS) {
    SZ_LOG_ERROR("HI_MPI_VI_CreatePipe failed with {:X}!", s32Ret);
    return false;
  }

  HI_BOOL bSnap = HI_FALSE;
  HI_BOOL bDoublePipe = HI_FALSE;
  VI_PIPE snapPipeIndex = 0;
  if (bSnap && bDoublePipe && viPipe == snapPipeIndex) {
  } else {
    HI_BOOL bVcNumCfged = HI_FALSE;
    HI_U32 vcNum = 1;
    if (bVcNumCfged) {
      s32Ret = HI_MPI_VI_SetPipeVCNumber(viPipe, vcNum);
      if (s32Ret != HI_SUCCESS) {
        HI_MPI_VI_DestroyPipe(viPipe);
        SZ_LOG_ERROR("HI_MPI_VI_SetPipeVCNumber failed with {:X}!", s32Ret);
        return false;
      }
    }

    s32Ret = HI_MPI_VI_StartPipe(viPipe);
    if (s32Ret != HI_SUCCESS) {
      HI_MPI_VI_DestroyPipe(viPipe);
      SZ_LOG_ERROR("HI_MPI_VI_StartPipe failed with {:X}!", s32Ret);
      return false;
    }
  }
  return true;
}

bool Vi::startCh(VI_PIPE viPipe, VI_CHN viCh, SensorType sensorType) {
  VI_CHN_ATTR_S stChnAttr;
  getChAttrBySensor(sensorType, &stChnAttr);
  stChnAttr.enDynamicRange = DYNAMIC_RANGE_SDR8;
  stChnAttr.enVideoFormat = VIDEO_FORMAT_LINEAR;
  stChnAttr.enPixelFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
  stChnAttr.enCompressMode = COMPRESS_MODE_NONE;
  HI_BOOL bNeedChn;
  WDR_MODE_E enWDRMode = WDR_MODE_NONE;
  if (WDR_MODE_NONE == enWDRMode) {
    bNeedChn = HI_TRUE;
  } else {
    bNeedChn = (viPipe > 0) ? HI_FALSE : HI_TRUE;
  }

  if (bNeedChn) {
    HI_U32 s32Ret = HI_MPI_VI_SetChnAttr(viPipe, viCh, &stChnAttr);
    if (s32Ret != HI_SUCCESS) {
      SZ_LOG_ERROR("HI_MPI_VI_SetChnAttr failed with {:X}!\n", s32Ret);
      return false;
    }

    VI_VPSS_MODE_E enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
    if (VI_OFFLINE_VPSS_OFFLINE == enMastPipeMode ||
        VI_ONLINE_VPSS_OFFLINE == enMastPipeMode ||
        VI_PARALLEL_VPSS_OFFLINE == enMastPipeMode) {
      s32Ret = HI_MPI_VI_EnableChn(viPipe, viCh);
      if (s32Ret != HI_SUCCESS) {
        SZ_LOG_ERROR("HI_MPI_VI_EnableChn failed with {:X}!\n", s32Ret);
        return false;
      }
    }
  }
  return true;
}

bool Vi::startVi(VI_DEV viDev, VI_PIPE viPipe, VI_CHN viCh,
                 SensorType sensorType) {
  if (!startDev(viDev, sensorType)) return false;

  if (!devBindPipe(viDev, viPipe)) return false;

  if (!startPipe(viPipe, sensorType)) return false;

  if (!startCh(viPipe, viCh, sensorType)) return false;

  return true;
}

bool Vi::start() {
  Mipi *pMipi = Mipi::getInstance();
  combo_dev_t mipiDev = viDev_;
  sns_clk_source_t sensorIndex = viDev_;
  if (!pMipi->start(mipiDev, sensorIndex, sensorType_)) return false;

  static bool bInit = false;
  if (!bInit) {
    if (!setParam(viPipe_)) return false;
    bInit = true;
  }

  if (!startVi(viDev_, viPipe_, 0, sensorType_)) return false;

  VI_PIPE_ATTR_S stPipeAttr;
  getPipeAttrBySensor(sensorType_, &stPipeAttr);
  if (VI_PIPE_BYPASS_BE == stPipeAttr.enPipeBypassMode) {
    return true;
  }

  Isp *pIsp = Isp::getInstance();
  HI_S8 busId = viDev_;
  if (!pIsp->start(viPipe_, sensorType_, busId)) return false;

  return true;
}

void Vi::getVbConfig(VB_CONFIG_S *pVbConfig) {
  int width = 0, height = 0;
  Isp *pIsp = Isp::getInstance();
  pIsp->getSizeBySensor(sensorType_, width, height);

  /*config vb*/
  hi_memset(pVbConfig, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
  pVbConfig->u32MaxPoolCnt = 2;

  HI_U32 u32BlkSize = COMMON_GetPicBufferSize(
      width, height, PIXEL_FORMAT_YVU_SEMIPLANAR_420, DATA_BITWIDTH_8,
      COMPRESS_MODE_SEG, DEFAULT_ALIGN);
  pVbConfig->astCommPool[0].u64BlkSize = u32BlkSize;
  pVbConfig->astCommPool[0].u32BlkCnt = 4;  // 20

  u32BlkSize = VI_GetRawBufferSize(width, height, PIXEL_FORMAT_RGB_BAYER_16BPP,
                                   COMPRESS_MODE_NONE, DEFAULT_ALIGN);
  pVbConfig->astCommPool[1].u64BlkSize = u32BlkSize;
  pVbConfig->astCommPool[1].u32BlkCnt = 2;  // 4
}

Vi::Vi(VI_DEV viDev, VI_PIPE viPipe, SensorType sensorType)
    : viDev_(viDev), viPipe_(viPipe), sensorType_(sensorType) {}

VI_PIPE Vi::getViPipe() { return viPipe_; }

bool Vi::stopCh(VI_PIPE viPipe, VI_CHN viCh) {
  HI_BOOL bNeedChn;
  WDR_MODE_E enWDRMode = WDR_MODE_NONE;
  if (WDR_MODE_NONE == enWDRMode) {
    bNeedChn = HI_TRUE;
  } else {
    bNeedChn = (viPipe > 0) ? HI_FALSE : HI_TRUE;
  }

  if (bNeedChn) {
    VI_VPSS_MODE_E enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
    if (VI_OFFLINE_VPSS_OFFLINE == enMastPipeMode ||
        VI_ONLINE_VPSS_OFFLINE == enMastPipeMode ||
        VI_PARALLEL_VPSS_OFFLINE == enMastPipeMode) {
      HI_U32 s32Ret = HI_MPI_VI_DisableChn(viPipe, viCh);
      if (s32Ret != HI_SUCCESS) {
        SZ_LOG_ERROR("HI_MPI_VI_DisableChn failed with {:X}!", s32Ret);
        return false;
      }
    }
  }
  return true;
}

bool Vi::stopPipe(VI_PIPE viPipe) {
  HI_U32 s32Ret = HI_MPI_VI_StopPipe(viPipe);
  if (s32Ret != HI_SUCCESS) {
    SZ_LOG_ERROR("HI_MPI_VI_StopPipe failed with {:X}!", s32Ret);
    return false;
  }

  s32Ret = HI_MPI_VI_DestroyPipe(viPipe);
  if (s32Ret != HI_SUCCESS) {
    SZ_LOG_ERROR("HI_MPI_VI_DestroyPipe failed with {:X}!", s32Ret);
    return false;
  }
  return true;
}

bool Vi::stopDev(VI_DEV viDev) {
  HI_U32 s32Ret = HI_MPI_VI_DisableDev(viDev);
  if (s32Ret != HI_SUCCESS) {
    SZ_LOG_ERROR("HI_MPI_VI_DisableDev failed with {:X}!", s32Ret);
    return false;
  }
  return true;
}
