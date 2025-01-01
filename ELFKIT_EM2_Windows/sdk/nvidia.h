#ifndef NVIDIA_H
#define NVIDIA_H

#include <typedefs.h>
#include <dal.h>

#define GF_DEVICE_ID_DEFAULT    0x00000000L
#define GF_DEVICE_REV_DEFAULT   0x00000000L

#define GF_SUCCESS              0x00000000L
#define GF_ERROR                0x80000000L

#define GF_EPPAPI               0x00000001L
#define GF_GXAPI                0x00000002L
#define GF_VXAPI                0x00000003L
#define GF_JXEAPI               0x00000004L
#define GF_JXDAPI               0x00000005L
#define GF_MXEAPI               0x00000006L
#define GF_MXDAPI               0x00000007L
#define GF_CPUAPI               0x00000008L
#define GF_DXAPI                0x00000009L
#define GF_IXAPI                0x0000000AL
#define GF_ISPAPI               0x0000000BL
#define GF_FDEVAPI              0x0000000CL
#define GF_BDEVSDAPI            0x0000000DL
#define GF_BDEVMDAPI            0x0000000EL
#define GF_UIAPI                0x0000000FL
#define GF_OSXAPI               0x00000010L
#define GF_I2CAPI               0x00000011L
#define GF_I2SAPI               0x00000012L
#define GF_MMPROCAPI            0x00000013L
#define GF_CAMAPI               0x00000014L
#define GF_3DAPI                0x00000015L
#define GF_INTXAPI              0x00000016L
#define GF_MXDH264API           0x00000017L
#define GF_MXEH264API           0x00000018L
#define GF_RMAPI                0x00000019L
#define GF_GXFBAPI              0x0000001AL
#define GF_MXDRV9API            0x0000001BL
#define GF_MXDVC1API            0x0000001CL

#define GF_STATE_REGISTER       0x00000001
#define GF_STATE_UNREGISTER     0x00000002
// INTERNAL USE ONLY.
#define GF_STATE_UNREGISTER_ALL 0x00000004
#define GF_STATE_ENABLE         0x00000008
#define GF_STATE_DISABLE        0x00000010
#define GF_STATE_DONE           0x00000020
// Share if possible, else create new.
#define GF_STATE_NEW_OR_SHARE   0x00000100
// Only create new.
#define GF_STATE_NEW_ONLY       0x00000200
// Always share.
#define GF_STATE_SHARE_ONLY     0x00000400
// Special Block Device (For internal use only).
#define GF_STATE_BLOCK_DEVICE   0x80000000
#define GF_STATE_DEFAULT        GF_STATE_NEW_OR_SHARE
// Masks for the regular states.
#define GF_STATE_MASK           0x000000FF

#define ISGFSUCCESS(code)       ((code) >= GF_SUCCESS)
#define ISGFERROR(code)         ((code) < GF_SUCCESS)

typedef UINT32 GF_HANDLE;
typedef UINT32 GF_RETTYPE;
typedef UINT32 GF_STATE_TYPE;

typedef struct _GFRMCOMPONENTID {
	UINT32 ComponentType;
	UINT16 DeviceID;
	UINT16 DeviceRev;
} GFRMCOMPONENTID, *PGFRMCOMPONENTID;

typedef struct _GXRECT {
	UINT16 x;
	UINT16 y;
	UINT16 w;
	UINT16 h;
} GXRECT, *PGXRECT;

typedef struct _GFPROPERTY{
	UINT32 Version; 
	UINT16 DeviceID;
	UINT16 DeviceRev;
	UINT32 BuildNumber;
	UINT32 Capability;
	UINT32 CapabilityEx;
	UINT32 ProdSkuID;
} GFPROPERTY, *PGFPROPERTY;

extern GF_HANDLE DAL_GetRmHandle(UINT8 display);
extern GF_HANDLE DAL_GetDxHandle(UINT8 display);

extern GF_RETTYPE GFRmGetProperty(GF_HANDLE RmHandle, GFPROPERTY *pRmProp);

extern GF_RETTYPE GFRmComponentGet(
	GF_HANDLE RmHandle, GFRMCOMPONENTID *pComponentID, GF_HANDLE *pComponent, GF_STATE_TYPE state
);

extern GF_RETTYPE GFRmComponentGetEx(
	GF_HANDLE RmHandle, GFRMCOMPONENTID *pComponentID, GF_HANDLE *pComponent, GF_STATE_TYPE state, GF_HANDLE CtxHandle
);

extern GF_RETTYPE GFRmOpen(GF_HANDLE RmOpenHandle);

extern GF_RETTYPE GFGxCopyColorBitmapEx(
	GF_HANDLE GxHandle, 
	INT16 dx, INT16 dy, INT16 w, INT16 h, INT16 sx, INT16 sy, 
	INT16 srcStride, UINT8 *pColorBits
);

extern GF_HANDLE RmHandle;
extern GF_HANDLE DxHandle;
extern GF_HANDLE GxHandle;

#endif // NVIDIA_H
