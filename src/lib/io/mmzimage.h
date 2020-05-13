#ifndef _MMZ_IMAGE_H_
#define _MMZ_IMAGE_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hi_comm_svp.h"
#include "lib/logger.h"
#include "mpi_sys.h"
#include "sz_common.h"

#define IMAGE_ALIGN 16

typedef struct MmzImage_ {
  int type;
  int width;
  int height;
  int dataLen;
  unsigned char *pData;
  unsigned char *pImplData;
  int offset;

  explicit MmzImage_(int width, int height, SZ_IMAGETYPE type) {
    int len = width * height;
    switch (type) {
      case SZ_IMAGETYPE_BGR:
      case SZ_IMAGETYPE_RGB:
        len *= 3;
        break;
      case SZ_IMAGETYPE_NV21:
        len *= 3 / 2;
        break;
      case SZ_IMAGETYPE_GRAY:
        break;
      default:
        break;
    }
    this->width = width;
    this->height = height;
    this->dataLen = len;
    this->type = type;
    this->offset = 0;
    this->pImplData = (unsigned char *)malloc(sizeof(SVP_IMAGE_S));
    createImage(this);
  }

  bool setData(const unsigned char *pData, int width, int height) {
    int len = setSize(width, height);
    if (len == 0) return false;
    memcpy(this->pData, pData, len);
    return true;
  }

  void setOffset(int offset) { this->offset = offset; }

  int setSize(int width, int height) {
    int len = 0;
    switch (type) {
      case SZ_IMAGETYPE_BGR:
      case SZ_IMAGETYPE_RGB:
        len = width * height * 3;
        break;
      case SZ_IMAGETYPE_GRAY:
        len = width * height;
        break;
      case SZ_IMAGETYPE_NV21:
        len = width * height * 3 / 2;
        break;
      default:
        return 0;
        break;
    }
    this->width = width;
    this->height = height;
    if (len > dataLen) {
      destroyImage(this);
      createImage(this);
      dataLen = len;
    } else {
      setImplSize(width, height);
    }
    return len;
  }

  ~MmzImage_() {
    if (pImplData != NULL) {
      destroyImage(this);
      free(pImplData);
      pImplData = NULL;
    }
  }

 private:
  int getStride(int width) {
    return (width + IMAGE_ALIGN - 1) / IMAGE_ALIGN * IMAGE_ALIGN;
  }

  void setImplSize(int width, int height) {
    SVP_IMAGE_S *pImageImpl = (SVP_IMAGE_S *)pImplData;
    pImageImpl->u32Width = width;
    pImageImpl->u32Height = height;
    // pImageImpl->au32Stride[0] = getStride(width);

    switch (type) {
      case SZ_IMAGETYPE_GRAY:
        break;
      case SZ_IMAGETYPE_BGR:
      case SZ_IMAGETYPE_RGB:
        pImageImpl->au32Stride[1] = pImageImpl->au32Stride[0];
        pImageImpl->au32Stride[2] = pImageImpl->au32Stride[0];
        break;
      case SZ_IMAGETYPE_NV21:
        pImageImpl->au32Stride[1] = pImageImpl->au32Stride[0];
        break;
    }
  }

  void createImage(MmzImage_ *pImage) {
    assert(pImage != NULL && pImage->pImplData != NULL &&
           (pImage->type == SZ_IMAGETYPE_BGR ||
            pImage->type == SZ_IMAGETYPE_RGB ||
            pImage->type == SZ_IMAGETYPE_GRAY ||
            pImage->type == SZ_IMAGETYPE_NV21));

    SVP_IMAGE_S *pImageImpl = (SVP_IMAGE_S *)pImage->pImplData;
    pImageImpl->u32Width = pImage->width;
    pImageImpl->u32Height = pImage->height;
    pImageImpl->au32Stride[0] = getStride(pImage->width);

    HI_U32 u32Size = 0;
    HI_S32 s32Ret = 0;
    int chLen = pImageImpl->au32Stride[0] * pImageImpl->u32Height;
    switch (pImage->type) {
      case SZ_IMAGETYPE_GRAY:
        u32Size = chLen;
        s32Ret = HI_MPI_SYS_MmzAlloc_Cached(pImageImpl->au64PhyAddr,
                                            (HI_VOID **)pImageImpl->au64VirAddr,
                                            NULL, HI_NULL, u32Size);
        if (s32Ret != HI_SUCCESS) {
          SZ_LOG_ERROR("HI_MPI_SYS_MmzAlloc fail,Error({:X})", s32Ret);
          return;
        }
        break;

      case SZ_IMAGETYPE_BGR:
      case SZ_IMAGETYPE_RGB:
        u32Size = chLen * 3;
        s32Ret = HI_MPI_SYS_MmzAlloc_Cached(pImageImpl->au64PhyAddr,
                                            (HI_VOID **)pImageImpl->au64VirAddr,
                                            NULL, HI_NULL, u32Size);
        if (s32Ret != HI_SUCCESS) {
          SZ_LOG_ERROR("Error({:X}):HI_MPI_SYS_MmzAlloc failed!", s32Ret);
          return;
        }
        pImageImpl->au32Stride[1] = pImageImpl->au32Stride[0];
        pImageImpl->au32Stride[2] = pImageImpl->au32Stride[0];

        pImageImpl->au64VirAddr[1] = pImageImpl->au64VirAddr[0] + chLen;
        pImageImpl->au64VirAddr[2] = pImageImpl->au64VirAddr[1] + chLen;
        pImageImpl->au64PhyAddr[1] = pImageImpl->au64PhyAddr[0] + chLen;
        pImageImpl->au64PhyAddr[2] = pImageImpl->au64PhyAddr[1] + chLen;
        /*
        pImageImpl->au64VirAddr[1] = pImageImpl->au64VirAddr[0] + 1;
pImageImpl->au64VirAddr[2] = pImageImpl->au64VirAddr[1] + 1;
pImageImpl->au64PhyAddr[1] = pImageImpl->au64PhyAddr[0] + 1;
pImageImpl->au64PhyAddr[2] = pImageImpl->au64PhyAddr[1] + 1;*/

        break;

      case SZ_IMAGETYPE_NV21:
        u32Size = pImageImpl->au32Stride[0] * pImageImpl->u32Height * 3 / 2;
        s32Ret = HI_MPI_SYS_MmzAlloc_Cached(pImageImpl->au64PhyAddr,
                                            (HI_VOID **)pImageImpl->au64VirAddr,
                                            NULL, HI_NULL, u32Size);
        // s32Ret = HI_MPI_SYS_MmzAlloc(pImageImpl->au64PhyAddr,
        // (HI_VOID**)pImageImpl->au64VirAddr, NULL, HI_NULL, u32Size);
        if (s32Ret != HI_SUCCESS) {
          SZ_LOG_ERROR("HI_MPI_SYS_MmzAlloc_Cached fail,Error({:X})", s32Ret);
          return;
        }
        pImageImpl->au32Stride[1] = pImageImpl->au32Stride[0];
        pImageImpl->au64PhyAddr[1] = pImageImpl->au64PhyAddr[0] + chLen;
        pImageImpl->au64VirAddr[1] = pImageImpl->au64VirAddr[0] + chLen;
        break;
    }
    this->pData = (HI_U8 *)(pImageImpl->au64VirAddr[0]);
  }

  void destroyImage(MmzImage_ *pImage) {
    SVP_IMAGE_S *pImageImpl = (SVP_IMAGE_S *)pImage->pImplData;
    HI_MPI_SYS_MmzFree(pImageImpl->au64PhyAddr[0],
                       (HI_VOID *)(HI_UL)pImageImpl->au64VirAddr);
  }
} MmzImage;

#endif
