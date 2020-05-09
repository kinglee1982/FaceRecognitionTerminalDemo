#include "mpp.h"

#include <assert.h>

#include "lib/logger.h"
#include "mpi_sys.h"
#include "mpi_vb.h"

Mpp *Mpp::getInstance() {
  static Mpp self;
  return &self;
}

Mpp::Mpp() { HI_MPI_SYS_Exit(); }

bool Mpp::init(const VB_CONFIG_S *pVbConf) {
  HI_MPI_VB_Exit();
  HI_U32 s32Ret = HI_MPI_VB_SetConfig(pVbConf);
  if (HI_SUCCESS != s32Ret) {
    SZ_LOG_ERROR("HI_MPI_VB_SetConf failed! {:X}", s32Ret);
    return false;
  }

  s32Ret = HI_MPI_VB_Init();
  if (HI_SUCCESS != s32Ret) {
    SZ_LOG_ERROR("HI_MPI_VB_Init failed! {:X}", s32Ret);
    return false;
  }

  s32Ret = HI_MPI_SYS_Init();
  if (HI_SUCCESS != s32Ret) {
    SZ_LOG_ERROR("HI_MPI_SYS_Init failed! {:X}", s32Ret);
    return false;
  }
  return true;
}

bool Mpp::viBindVpss(VI_PIPE viPipe, VI_CHN viCh, VPSS_GRP vpssGrp) {
  MPP_CHN_S stSrcChn;
  MPP_CHN_S stDestChn;
  stSrcChn.enModId = HI_ID_VI;
  stSrcChn.s32DevId = viPipe;
  stSrcChn.s32ChnId = viCh;

  stDestChn.enModId = HI_ID_VPSS;
  stDestChn.s32DevId = vpssGrp;
  stDestChn.s32ChnId = 0;

  HI_U32 ret = HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
  if (HI_SUCCESS != ret) {
    SZ_LOG_ERROR("HI_MPI_SYS_Bind(VI-VPSS) failed with {:X}", ret);
    return false;
  }
  return true;
}

bool Mpp::vpssBindVo(VPSS_GRP vpssGrp, VPSS_CHN vpssCh, VO_LAYER voLayer,
                     VO_CHN voCh) {
  MPP_CHN_S stSrcChn;
  MPP_CHN_S stDestChn;
  stSrcChn.enModId = HI_ID_VPSS;
  stSrcChn.s32DevId = vpssGrp;
  stSrcChn.s32ChnId = vpssCh;

  stDestChn.enModId = HI_ID_VO;
  stDestChn.s32DevId = voLayer;
  stDestChn.s32ChnId = voCh;

  HI_U32 ret = HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
  if (HI_SUCCESS != ret) {
    SZ_LOG_ERROR("HI_MPI_SYS_Bind(VPSS-VO) failed with {:X}", ret);
    return false;
  }
  return true;
}

bool Mpp::vdecBindVpss(VDEC_CHN vdecCh, VPSS_GRP vpssGrp) {
  MPP_CHN_S stSrcChn;
  MPP_CHN_S stDestChn;
  stSrcChn.enModId = HI_ID_VDEC;
  stSrcChn.s32DevId = 0;
  stSrcChn.s32ChnId = vdecCh;

  stDestChn.enModId = HI_ID_VPSS;
  stDestChn.s32DevId = vpssGrp;
  stDestChn.s32ChnId = 0;

  HI_U32 ret = HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
  if (ret != HI_SUCCESS) {
    SZ_LOG_ERROR("HI_MPI_SYS_Bind(VDEC-VPSS) failed {:X}", ret);
    return false;
  }
  return true;
}

bool Mpp::vdecUnBindVpss(VDEC_CHN vdecCh, VPSS_GRP vpssGrp) {
  MPP_CHN_S stSrcChn;
  MPP_CHN_S stDestChn;
  stSrcChn.enModId = HI_ID_VDEC;
  stSrcChn.s32DevId = 0;
  stSrcChn.s32ChnId = vdecCh;

  stDestChn.enModId = HI_ID_VPSS;
  stDestChn.s32DevId = vpssGrp;
  stDestChn.s32ChnId = 0;

  HI_U32 ret = HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
  if (ret != HI_SUCCESS) {
    SZ_LOG_ERROR("HI_MPI_SYS_UnBind(VDEC-VPSS) failed {:X}", ret);
    return false;
  }
  return true;
}

bool Mpp::vpssUnBindVo(VPSS_GRP vpssGrp, VPSS_CHN vpssChn, VO_LAYER voLayer,
                       VO_CHN voChn) {
  MPP_CHN_S stSrcChn;
  MPP_CHN_S stDestChn;
  stSrcChn.enModId = HI_ID_VPSS;
  stSrcChn.s32DevId = vpssGrp;
  stSrcChn.s32ChnId = vpssChn;

  stDestChn.enModId = HI_ID_VO;
  stDestChn.s32DevId = voLayer;
  stDestChn.s32ChnId = voChn;

  HI_U32 ret = HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
  if (ret != HI_SUCCESS) {
    SZ_LOG_ERROR("HI_MPI_SYS_UnBind(VPSS-VO) failed {:X}", ret);
    return false;
  }

  return true;
}

bool Mpp::viUnBindVpss(VI_PIPE viPipe, VI_CHN viCh, VPSS_GRP vpssGrp) {
  MPP_CHN_S stSrcChn;
  MPP_CHN_S stDestChn;
  stSrcChn.enModId = HI_ID_VI;
  stSrcChn.s32DevId = viPipe;
  stSrcChn.s32ChnId = viCh;
  stDestChn.enModId = HI_ID_VPSS;
  stDestChn.s32DevId = vpssGrp;
  stDestChn.s32ChnId = 0;

  HI_U32 ret = HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
  if (ret != HI_SUCCESS) {
    SZ_LOG_ERROR("HI_MPI_SYS_UnBind(VI-VPSS) failed {:X}", ret);
    return false;
  }
  return true;
}
