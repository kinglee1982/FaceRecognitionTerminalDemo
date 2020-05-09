#include "ive.h"

#include <assert.h>
#include <unistd.h>

#include "lib/logger.h"
#include "mpi_ive.h"

Ive *Ive::getInstance() {
  static Ive self_;
  return &self_;
}

Ive::Ive() {
  ccbBlobsMem_.u32Size = sizeof(IVE_CCBLOB_S);
  HI_U32 s32Ret = HI_MPI_SYS_MmzAlloc(&ccbBlobsMem_.u64PhyAddr,
                                      (HI_VOID **)&ccbBlobsMem_.u64VirAddr,
                                      NULL, HI_NULL, sizeof(IVE_CCBLOB_S));
  assert(s32Ret == HI_SUCCESS);

  pCcbRegions_ = NULL;
  regionSize_ = 0;

  // resize tmp buff
  // pstResizeCtrl→stMem 内存至少需要 25*U8C1_NUM + 49 * (pstResizeCtrl->u16Num
  // – U8C1_NUM)字节， 其中 U8C1_NUM 为混合图像数组中 U8C1 图像的数目
  resizeCtrl_.enMode = IVE_RESIZE_MODE_LINEAR;
  resizeCtrl_.u16Num = 1;
  resizeCtrl_.stMem.u32Size = 25 * 4;
  s32Ret = HI_MPI_SYS_MmzAlloc(&resizeCtrl_.stMem.u64PhyAddr,
                               (HI_VOID **)&resizeCtrl_.stMem.u64VirAddr, NULL,
                               HI_NULL, resizeCtrl_.stMem.u32Size);
  assert(s32Ret == HI_SUCCESS);

  SZ_LOG_INFO("ive init success!");
}

Ive::~Ive() {
  HI_MPI_SYS_MmzFree(ccbBlobsMem_.u64PhyAddr,
                     (HI_VOID *)(HI_UL)ccbBlobsMem_.u64VirAddr);
  HI_MPI_SYS_MmzFree(resizeCtrl_.stMem.u64PhyAddr,
                     (HI_VOID *)(HI_UL)resizeCtrl_.stMem.u64VirAddr);
}

void Ive::waitFinish() {
  HI_BOOL bFinish = HI_FALSE;
  HI_S32 ret = 0;
  while (HI_ERR_IVE_QUERY_TIMEOUT ==
         (ret = HI_MPI_IVE_Query(handle_, &bFinish, HI_TRUE))) {
    usleep(1);
  }
}

bool Ive::threshold(MmzImage *pDst, const MmzImage *pSrc,
                    unsigned char threshold, unsigned char maxValue,
                    bool bBlock) {
  assert(pDst->width == pSrc->width && pDst->height == pSrc->height &&
         pDst->type == SZ_IMAGETYPE_GRAY);

  IVE_IMAGE_S srcImpl = *((IVE_IMAGE_S *)pSrc->pImplData);
  IVE_IMAGE_S dstImpl = *((IVE_IMAGE_S *)pDst->pImplData);
  srcImpl.au64PhyAddr[0] += pSrc->offset;
  srcImpl.au64VirAddr[0] += pSrc->offset;
  srcImpl.enType = IVE_IMAGE_TYPE_U8C1;

  dstImpl.au64PhyAddr[0] += pDst->offset;
  dstImpl.au64VirAddr[0] += pDst->offset;
  dstImpl.enType = IVE_IMAGE_TYPE_U8C1;

  // only implement CV_THRESH_BINARY
  IVE_THRESH_CTRL_S stCtrl;
  stCtrl.enMode = IVE_THRESH_MODE_BINARY;
  stCtrl.u8MinVal = 0;
  stCtrl.u8MaxVal = maxValue;
  stCtrl.u8LowThr = threshold;
  HI_U32 ret =
      HI_MPI_IVE_Thresh(&handle_, &srcImpl, &dstImpl, &stCtrl, (HI_BOOL)bBlock);
  if (HI_SUCCESS != ret) {
    SZ_LOG_ERROR("HI_MPI_IVE_Thresh failed: {:x}", ret);
    return false;
  }

  if (bBlock) {
    waitFinish();
  }
  return true;
}

bool Ive::dilate(MmzImage *pDst, const MmzImage *pSrc, unsigned char *pMask,
                 bool bBlock) {
  assert(pDst->type == pSrc->type && pDst->width == pSrc->width &&
         pDst->height == pSrc->height && (pDst->type == SZ_IMAGETYPE_GRAY));

  IVE_IMAGE_S srcImpl = *((IVE_IMAGE_S *)pSrc->pImplData);
  IVE_IMAGE_S dstImpl = *((IVE_IMAGE_S *)pDst->pImplData);
  srcImpl.au64PhyAddr[0] += pSrc->offset;
  srcImpl.au64VirAddr[0] += pSrc->offset;
  srcImpl.enType = IVE_IMAGE_TYPE_U8C1;

  dstImpl.au64PhyAddr[0] += pDst->offset;
  dstImpl.au64VirAddr[0] += pDst->offset;
  dstImpl.enType = IVE_IMAGE_TYPE_U8C1;

  // 5 * 5 template
  IVE_DILATE_CTRL_S dilateCtrl;
  memcpy(dilateCtrl.au8Mask, pMask, sizeof(dilateCtrl.au8Mask));
  HI_S32 ret = HI_MPI_IVE_Dilate(&handle_, &srcImpl, &dstImpl, &dilateCtrl,
                                 (HI_BOOL)bBlock);
  if (HI_SUCCESS != ret) {
    SZ_LOG_ERROR("HI_MPI_IVE_Dilate failed: %x \n", ret);
    return false;
  }

  if (bBlock) {
    waitFinish();
  }
  return true;
}

bool Ive::erode(MmzImage *pDst, const MmzImage *pSrc, bool bBlock) {
  assert(pDst->type == pSrc->type && pDst->width == pSrc->width &&
         pDst->height == pSrc->height &&
         (pDst->type == SZ_IMAGETYPE_GRAY || pDst->type == SZ_IMAGETYPE_NV21));

  IVE_IMAGE_S *pSrcImpl = (IVE_IMAGE_S *)pSrc->pImplData;
  IVE_IMAGE_S *pDstImpl = (IVE_IMAGE_S *)pDst->pImplData;
  pSrcImpl->enType = IVE_IMAGE_TYPE_U8C1;
  pDstImpl->enType = IVE_IMAGE_TYPE_U8C1;

  // 5 * 5 template
  IVE_ERODE_CTRL_S erodeCtrl;
  memset(erodeCtrl.au8Mask, 0xff, sizeof(erodeCtrl.au8Mask));
  if (HI_SUCCESS != HI_MPI_IVE_Erode(&handle_, pSrcImpl, pDstImpl, &erodeCtrl,
                                     (HI_BOOL)bBlock))
    return false;

  if (bBlock) {
    waitFinish();
  }
  return true;
}

bool Ive::findContours(const MmzImage *pImage, SZ_RECT **pRegions,
                       int &regionCnt) {
  assert(pImage->type == SZ_IMAGETYPE_GRAY ||
         pImage->type == SZ_IMAGETYPE_NV21);

  IVE_CCL_CTRL_S stCclCtrl;
  stCclCtrl.enMode = IVE_CCL_MODE_8C;  // 8连通
  stCclCtrl.u16Step = 20;              // 2
  stCclCtrl.u16InitAreaThr = 4;        // 4

  IVE_IMAGE_S imgImpl = *((IVE_IMAGE_S *)pImage->pImplData);
  imgImpl.au64PhyAddr[0] += pImage->offset;
  imgImpl.au64VirAddr[0] += pImage->offset;
  imgImpl.enType = IVE_IMAGE_TYPE_U8C1;

  HI_U32 ret =
      HI_MPI_IVE_CCL(&handle_, &imgImpl, &ccbBlobsMem_, &stCclCtrl, HI_TRUE);
  if (HI_SUCCESS != ret) {
    SZ_LOG_ERROR("HI_MPI_IVE_CCL failed {:X}", ret);
    return false;
  }

  waitFinish();

  IVE_CCBLOB_S *pCcBlobs = (IVE_CCBLOB_S *)(HI_UL)ccbBlobsMem_.u64VirAddr;
  if (pCcBlobs->u8RegionNum > regionSize_) {
    regionSize_ = pCcBlobs->u8RegionNum;
    pCcbRegions_ =
        (SZ_RECT *)realloc(pCcbRegions_, sizeof(SZ_RECT) * regionSize_);
  }

  SZ_LOG_DEBUG("u16CurAreaThr={}  u8RegionNum={}", pCcBlobs->u16CurAreaThr,
               pCcBlobs->u8RegionNum);
  for (int i = 0; i < pCcBlobs->u8RegionNum; i++) {
    pCcbRegions_[i].x = pCcBlobs->astRegion[i].u16Left;
    pCcbRegions_[i].y = pCcBlobs->astRegion[i].u16Top;
    pCcbRegions_[i].width =
        pCcBlobs->astRegion[i].u16Right - pCcBlobs->astRegion[i].u16Left;
    pCcbRegions_[i].height =
        pCcBlobs->astRegion[i].u16Bottom - pCcBlobs->astRegion[i].u16Top;

    /*printf("(%d %d %d %d %d) ", pCcBlobs->astRegion[i].u32Area,
            pCcBlobs->astRegion[i].u16Left, pCcBlobs->astRegion[i].u16Top,
            pCcBlobs->astRegion[i].u16Right,
       pCcBlobs->astRegion[i].u16Bottom);*/
  }
  regionCnt = pCcBlobs->u8RegionNum;
  return true;
}

bool Ive::medianBlur(MmzImage *pDst, const MmzImage *pSrc, bool bBlock) {
  assert(pDst->type == pSrc->type && pDst->width == pSrc->width &&
         pDst->height == pSrc->height &&
         (pDst->type == SZ_IMAGETYPE_GRAY || pDst->type == SZ_IMAGETYPE_NV21));

  IVE_IMAGE_S *pSrcImpl = (IVE_IMAGE_S *)pSrc->pImplData;
  IVE_IMAGE_S *pDstImpl = (IVE_IMAGE_S *)pDst->pImplData;
  pSrcImpl->enType = IVE_IMAGE_TYPE_U8C1;
  pDstImpl->enType = IVE_IMAGE_TYPE_U8C1;

  IVE_ORD_STAT_FILTER_CTRL_S filterCtrl;
  filterCtrl.enMode = IVE_ORD_STAT_FILTER_MODE_MEDIAN;

  if (HI_SUCCESS != HI_MPI_IVE_OrdStatFilter(&handle_, pSrcImpl, pDstImpl,
                                             &filterCtrl, (HI_BOOL)bBlock))
    return false;

  if (bBlock) {
    waitFinish();
  }
  return true;
}

bool Ive::resize(MmzImage *pDestImage, const MmzImage *pSrcImage, bool bBlock) {
  assert(pDestImage->type == pSrcImage->type);

  IVE_IMAGE_TYPE_E iveType = IVE_IMAGE_TYPE_U8C1;
  if (pDestImage->type == SZ_IMAGETYPE_BGR)
    iveType = IVE_IMAGE_TYPE_U8C3_PLANAR;

  //最大支持16倍缩放
  IVE_IMAGE_S *pSrcImageImpl = (IVE_IMAGE_S *)pSrcImage->pImplData;
  pSrcImageImpl->enType = iveType;
  IVE_IMAGE_S *pDestImageImpl = (IVE_IMAGE_S *)pDestImage->pImplData;
  pDestImageImpl->enType = iveType;
  if (HI_SUCCESS != HI_MPI_IVE_Resize(&handle_, pSrcImageImpl, pDestImageImpl,
                                      &resizeCtrl_, (HI_BOOL)bBlock))
    return false;

  if (bBlock) {
    waitFinish();
  }
  return true;
}

/**
//VIDEO_FRAME_INFO_S *pstFrameInfo
//stSrcData.u64VirAddr = pstFrameInfo->stVFrame.u64VirAddr[0];
stSrcData.u64PhyAddr = pstFrameInfo->stVFrame.u64PhyAddr[0];
stSrcData.u32Width	= pstFrameInfo->stVFrame.u32Width;
stSrcData.u32Height	= pstFrameInfo->stVFrame.u32Height;
stSrcData.u32Stride	= pstFrameInfo->stVFrame.u32Stride[0];
*/
bool Ive::copyImage(MmzImage *pDestImage, const MmzImage *pSrcImage,
                    bool bBlock) {
  // bgr or rgb packed image
  assert(pDestImage->width == pSrcImage->width &&
         pDestImage->height == pSrcImage->height &&
         pDestImage->type == pSrcImage->type &&
         (pSrcImage->type == SZ_IMAGETYPE_BGR ||
          pSrcImage->type == SZ_IMAGETYPE_RGB ||
          pSrcImage->type == SZ_IMAGETYPE_GRAY));

  int chNum = 3;
  if (pSrcImage->type == SZ_IMAGETYPE_GRAY) chNum = 1;

  IVE_SRC_DATA_S stSrcData;
  IVE_DST_DATA_S stDstData;
  IVE_DMA_CTRL_S stCtrl = {IVE_DMA_MODE_DIRECT_COPY, 0};

  IVE_IMAGE_S *pDest = (IVE_IMAGE_S *)pDestImage->pImplData;
  IVE_IMAGE_S *pSrc = (IVE_IMAGE_S *)pSrcImage->pImplData;
  stSrcData.u32Width = pSrc->u32Width * chNum;
  stSrcData.u32Height = pSrc->u32Height;
  stDstData.u32Width = pDest->u32Width * chNum;
  stDstData.u32Height = pDest->u32Height;

  // fill src
  // stSrcData.u64VirAddr = pSrc->au64VirAddr;
  stSrcData.u64PhyAddr = pSrc->au64PhyAddr[0] + pSrcImage->offset;
  stSrcData.u32Stride = pSrc->au32Stride[0] * chNum;

  // fill dst
  // stDstData.u64VirAddr = pDst->au64VirAddr;
  stDstData.u64PhyAddr = pDest->au64PhyAddr[0] + pDestImage->offset;
  stDstData.u32Stride = pDest->au32Stride[0] * chNum;

  HI_U32 s32Ret = HI_MPI_IVE_DMA(&handle_, &stSrcData, &stDstData, &stCtrl,
                                 (HI_BOOL)bBlock);
  if (s32Ret != HI_SUCCESS) {
    SZ_LOG_ERROR("Error({:X}),HI_MPI_IVE_DMA failed!\n", s32Ret);
    return false;
  }

  if (HI_TRUE == bBlock) {
    waitFinish();
  }
  return true;
}

bool Ive::copyImage(MmzImage *pDestImage, HI_U64 srcAddr, bool bBlock) {
  IVE_SRC_DATA_S stSrcData;
  IVE_DST_DATA_S stDstData;
  IVE_DMA_CTRL_S stCtrl = {IVE_DMA_MODE_DIRECT_COPY, 0};

  IVE_IMAGE_S *pDest = (IVE_IMAGE_S *)pDestImage->pImplData;
  stSrcData.u32Width = pDest->u32Width;
  stSrcData.u32Height = pDest->u32Height;
  stDstData.u32Width = pDest->u32Width;
  stDstData.u32Height = pDest->u32Height;
  // fill src
  // stSrcData.u64VirAddr = pSrc->au64VirAddr;
  stSrcData.u64PhyAddr = srcAddr + pDestImage->offset;
  stSrcData.u32Stride = pDest->au32Stride[0];

  // fill dst
  // stDstData.u64VirAddr = pDst->au64VirAddr;
  stDstData.u64PhyAddr = pDest->au64PhyAddr[0] + pDestImage->offset;
  stDstData.u32Stride = pDest->au32Stride[0];

  HI_S32 s32Ret = HI_MPI_IVE_DMA(&handle_, &stSrcData, &stDstData, &stCtrl,
                                 (HI_BOOL)bBlock);
  if (s32Ret != HI_SUCCESS) {
    SZ_LOG_ERROR("Error({:X}),HI_MPI_IVE_DMA failed! {}  {}", s32Ret,
                 pDest->u32Width, pDest->u32Height);
    return false;
  }

  if (HI_TRUE == bBlock) {
    waitFinish();
  }
  return true;
}

bool Ive::cropImage(MmzImage *pDestImage, const MmzImage *pSrcImage,
                    const SZ_RECT *pRect, bool bBlock) {
  // bgr or rgb packed image
  assert(pDestImage->width == pRect->width &&
         pDestImage->height == pRect->height &&
         pDestImage->type == pSrcImage->type &&
         pSrcImage->width >= pRect->width + pRect->x &&
         pSrcImage->height >= pRect->height + pRect->y &&
         (pSrcImage->type == SZ_IMAGETYPE_BGR ||
          pSrcImage->type == SZ_IMAGETYPE_RGB ||
          pSrcImage->type == SZ_IMAGETYPE_GRAY));

  int chNum = 3;
  if (pSrcImage->type == SZ_IMAGETYPE_GRAY) chNum = 1;

  IVE_SRC_DATA_S stSrcData;
  IVE_DST_DATA_S stDstData;
  IVE_DMA_CTRL_S stCtrl = {IVE_DMA_MODE_DIRECT_COPY, 0};

  IVE_IMAGE_S *pDest = (IVE_IMAGE_S *)pDestImage->pImplData;
  IVE_IMAGE_S *pSrc = (IVE_IMAGE_S *)pSrcImage->pImplData;

  int offset = pRect->y * pSrc->au32Stride[0] * 3 + pRect->x * 3;

  stSrcData.u32Width = pSrc->u32Width * chNum;
  stSrcData.u32Height = pSrc->u32Height;
  stDstData.u32Width = pDest->u32Width * chNum;
  stDstData.u32Height = pDest->u32Height;

  // fill src
  // stSrcData.u64VirAddr = pSrc->au64VirAddr;
  stSrcData.u64PhyAddr = pSrc->au64PhyAddr[0] + offset;
  stSrcData.u32Stride = pSrc->au32Stride[0] * chNum;

  // fill dst
  // stDstData.u64VirAddr = pDst->au64VirAddr;
  stDstData.u64PhyAddr = pDest->au64PhyAddr[0];
  stDstData.u32Stride = pDest->au32Stride[0] * chNum;

  HI_U32 s32Ret = HI_MPI_IVE_DMA(&handle_, &stSrcData, &stDstData, &stCtrl,
                                 (HI_BOOL)bBlock);
  if (s32Ret != HI_SUCCESS) {
    SZ_LOG_ERROR("Error({:X}), HI_MPI_IVE_DMA failed!", s32Ret);
    return false;
  }

  if (HI_TRUE == bBlock) {
    waitFinish();
  }
  return true;
}

bool Ive::rgbPlannar2Yuv(MmzImage *pDestImage, const MmzImage *pSrcImage,
                         bool bBlock) {
  assert(pSrcImage->type == SZ_IMAGETYPE_RGB &&
         pDestImage->type == SZ_IMAGETYPE_NV21);

  IVE_CSC_CTRL_S cscCtrl;
  cscCtrl.enMode =
      IVE_CSC_MODE_PIC_BT709_RGB2YUV;  // IVE_CSC_MODE_PIC_BT601_RGB2YUV

  IVE_IMAGE_S *pSrcImpl = (IVE_IMAGE_S *)pSrcImage->pImplData;
  IVE_IMAGE_S *pDstImpl = (IVE_IMAGE_S *)pDestImage->pImplData;
  pSrcImpl->enType = IVE_IMAGE_TYPE_U8C3_PLANAR;
  pDstImpl->enType = IVE_IMAGE_TYPE_YUV420SP;

  HI_S32 s32Ret =
      HI_MPI_IVE_CSC(&handle_, pSrcImpl, pDstImpl, &cscCtrl, (HI_BOOL)bBlock);
  if (s32Ret != HI_SUCCESS) {
    return false;
  }

  if (HI_TRUE == bBlock) {
    waitFinish();
  }
  return true;
}

bool Ive::rgbPacked2Yuv(MmzImage *pDestImage, const MmzImage *pSrcImage,
                        bool bBlock) {
  assert(pSrcImage->type == SZ_IMAGETYPE_RGB &&
         pDestImage->type == SZ_IMAGETYPE_NV21);

  IVE_CSC_CTRL_S cscCtrl;
  cscCtrl.enMode =
      IVE_CSC_MODE_PIC_BT709_RGB2YUV;  // IVE_CSC_MODE_PIC_BT601_RGB2YUV

  IVE_IMAGE_S *pSrcImpl = (IVE_IMAGE_S *)pSrcImage->pImplData;
  IVE_IMAGE_S *pDstImpl = (IVE_IMAGE_S *)pDestImage->pImplData;
  pSrcImpl->enType = IVE_IMAGE_TYPE_U8C3_PACKAGE;
  pDstImpl->enType = IVE_IMAGE_TYPE_YUV420SP;

  HI_S32 s32Ret =
      HI_MPI_IVE_CSC(&handle_, pSrcImpl, pDstImpl, &cscCtrl, (HI_BOOL)bBlock);
  if (s32Ret != HI_SUCCESS) {
    return false;
  }

  if (HI_TRUE == bBlock) {
    waitFinish();
  }
  return true;
}

bool Ive::yuv2RgbPacked(MmzImage *pDestImage, const MmzImage *pSrcImage,
                        bool bBlock) {
  assert(pSrcImage->type == SZ_IMAGETYPE_NV21 &&
         pDestImage->type == SZ_IMAGETYPE_RGB);

  IVE_CSC_CTRL_S cscCtrl;
  cscCtrl.enMode =
      IVE_CSC_MODE_PIC_BT709_YUV2RGB;  // IVE_CSC_MODE_PIC_BT601_YUV2RGB

  IVE_IMAGE_S *pSrcImpl = (IVE_IMAGE_S *)pSrcImage->pImplData;
  IVE_IMAGE_S *pDstImpl = (IVE_IMAGE_S *)pDestImage->pImplData;
  pSrcImpl->enType = IVE_IMAGE_TYPE_YUV420SP;
  pDstImpl->enType = IVE_IMAGE_TYPE_U8C3_PACKAGE;

  HI_S32 s32Ret =
      HI_MPI_IVE_CSC(&handle_, pSrcImpl, pDstImpl, &cscCtrl, (HI_BOOL)bBlock);
  if (s32Ret != HI_SUCCESS) {
    return false;
  }

  if (HI_TRUE == bBlock) {
    waitFinish();
  }
  return true;
}

bool Ive::yuv2RgbPlannar(MmzImage *pDestImage, const MmzImage *pSrcImage,
                         bool bBlock) {
  assert(pSrcImage->type == SZ_IMAGETYPE_NV21 &&
         pDestImage->type == SZ_IMAGETYPE_RGB);

  IVE_CSC_CTRL_S cscCtrl;
  cscCtrl.enMode =
      IVE_CSC_MODE_VIDEO_BT709_YUV2RGB;  // IVE_CSC_MODE_PIC_BT601_YUV2RGB;//IVE_CSC_MODE_PIC_BT709_YUV2RGB;//

  IVE_IMAGE_S *pSrcImpl = (IVE_IMAGE_S *)pSrcImage->pImplData;
  IVE_IMAGE_S *pDstImpl = (IVE_IMAGE_S *)pDestImage->pImplData;
  pSrcImpl->enType = IVE_IMAGE_TYPE_YUV420SP;
  pDstImpl->enType = IVE_IMAGE_TYPE_U8C3_PLANAR;

  HI_U32 s32Ret =
      HI_MPI_IVE_CSC(&handle_, pSrcImpl, pDstImpl, &cscCtrl, (HI_BOOL)bBlock);
  if (s32Ret != HI_SUCCESS) {
    SZ_LOG_ERROR("Error({:X}),HI_MPI_IVE_CSC failed!", s32Ret);
    return false;
  }

  if (HI_TRUE == bBlock) {
    waitFinish();
  }
  return true;
}
