#ifndef DSMA_H
#define DSMA_H

#include <typedefs.h>

// protocols
#define PROTOCOL_TCP      1	//+++
#define PROTOCOL_SSL      2	//+++
#define PROTOCOL_UDP      3	//+++
#define PROTOCOL_IP       4	//+++

#define USE_CSD           1	//+++
#define USE_GPRS          2	//+++

#define LINK_GPRS         0	//+++
#define LINK_CSD          5	//+++

#define LINK_IS_SHAREABLE 1	//+++
#define PROXY_NULL        0	//+++

#define EV_DSMA_PROXY_CREATE	0x82257
#define EV_DSMA_PROXY_STATE	0x82258
#define EV_DSMA_BUFF_STATE	0x82259
#define EV_DSMA_PROXY_ATTR	0x8225A

typedef UINT32 CMD_NUM;

// proxy_state
enum
{
	PROXY_STATE_ACTIVE = 0,			// +++ ��������
	PROXY_STATE_INACTIVE,			// +++ ��������� (����� ���� �����������)
	PROXY_STATE_FAILED,			// +++ ������
	PROXY_STATE_LINK_AVAILABLE,		// +++ Matching datalink activated
	PROXY_STATE_DEFAULT_LINK_UPDATED,	// Default link has being updated.
	PROXY_STATE_LINK_ATTACH_COUNT,		// Number of clients using matching link changed
	PROXY_STATE_GPRS_SUSPEND,
	PROXY_STATE_GPRS_RESUME,
	PROXY_STATE_NONE = 0xff
};
typedef UINT8 PROXY_STATE_T;

// buf_state
enum
{
	BUFFER_STATE_XON = 0,		// +++ ����� �������� ������
	BUFFER_STATE_XOFF,		// +++ ������ �������� ������ - ����� ���������
	BUFFER_STATE_DATA,		// +++ ����� ������ - ����� �������
	BUFFER_STATE_NONE = 0xff
};
typedef UINT8 BUFFER_STATE_T;

// ��������
enum
{
	DSMA_ATTR_NULL = 0,

	DSMA_ATTR_TRANS_PROT = 0x1000,	//++++
	DSMA_ATTR_LOCAL_PORT,		//++++
	DSMA_ATTR_REMOTE_PORT,		//++++
	DSMA_ATTR_PRIORITY,

	DSMA_ATTR_NET_PROT = 0x2000,	//++++
	DSMA_ATTR_LOCAL_IP,		//++++
	DSMA_ATTR_REMOTE_IP,		//++++

	DSMA_ATTR_LINK_PROT = 0x3000,	//++++
	DSMA_ATTR_USER_NAME,		//++++
	DSMA_ATTR_PW,			//++++
	DSMA_ATTR_AUTH_TYPE,
	DSMA_ATTR_DNS_1,		//++++
	DSMA_ATTR_DNS_2,		//++++

	DSMA_ATTR_PHYS_PROT = 0x4000,	//++++
	DSMA_ATTR_APN,			//++++

	DSMA_ATTR_OTHER_BEARER_PARAMETERS = 0x4004,

	DSMA_ATTR_LINK_IS_SHAREABLE = 0x4104,
	DSMA_ATTR_LINK_IS_DEFAULT,
	DSMA_ATTR_LINK_IS_PERSISTANT
};
typedef UINT16 DSMA_ATTR_T;


//���������
typedef struct
{
	DSMA_ATTR_T		attr;		//size = 2, offset = 0x00 +++
	UINT16			unk1;		//size = 2, offset = 0x02 ???
	UINT32			link_desc_ind;	//size = 4, offset = 0x04 +++
	void			*value;		//size = 4, offset = 0x08 +++
	UINT32			size;		//size = 4, offset = 0x0C +++
	UINT32			result;		//size = 4, offset = 0x10 +++
} DSMA_ATTRIBUTE_T;			//size = 0x14 b


//���������
typedef struct
{
	UINT8		source;
	UINT8		appCallCause;
	UINT8		dlSigCallCause;
	UINT8		networkCallCause;
	UINT8		gprsError;
	INT8		dfspError;
	UINT8		dsmaCause;
} DSMA_CAUSE_T;


//���������
typedef struct
{
	UINT8		foregroundClients;
	UINT8		backgroundClientst;
	UINT8		listeningClients;
} DSMA_LINK_ATTACH_COUNT_T;


//���������
typedef struct
{
	UINT16 unk1;
	UINT16 unk2;
	UINT16 unk3;
} DSMA_QOS_INFO_T;


//������ ������ ���� 68 ���� (����������� ����: proxy_id, status, cmd, port, proxy_state, buf_state)
typedef struct
{
	UINT32				proxy_id;		// +++ offset=0x00, ID ������
	UINT8				status;			// +++ offset=0x04, ������ ����������� ������� (RESULT_OK/RESULT_FAIL)
	UINT8				unk1[3];		// +++ offset=0x05, ???
	CMD_NUM				cmd;			// +++ offset=0x08, ���������� ����� - ����� ������������ ��� �������������

	SU_PORT_T			port;			// +++ offset=0x0C, ��� �� ���� ���� (�� �������) �������� ������
	UINT8				proxy_state;		// +++ offset=0x10, ��������� ������
	BUFFER_STATE_T			buf_state;		// +++ offset=0x11, ��������� �������
	UINT8				unk2[2];		// +++ offset=0x12, ???

	UINT32				attr_num;		// +++ offset=0x14, ���-�� ���������
	DSMA_ATTRIBUTE_T  *		attr_list;		// +++ offset=0x18, ������ ���������

//��������� ��������� �� �����������
	UINT32				lp_handle;
	DSMA_CAUSE_T			cause;
	DSMA_LINK_ATTACH_COUNT_T	linkAttachCount;
	UINT32				handle;
	UINT8				sessionId;
	DSMA_QOS_INFO_T			qosInfo;
	UINT8				localPortLength;
	UINT16				*localPortInfo;
	DSMA_QOS_INFO_T			minReqQosInfo;

} DSMA_DATA_T;	// [72]

//***************************************
// ����� ������ ������� ���������� CMD_NUM � ���������� ��� � CMD_NUM �� ������ � ������ ����� ��������� ��� ��� ��� �����!!!
//***************************************


// ������� ������, ������ ����� ����� EV_DSMA_PROXY_CREATE
CMD_NUM DL_DSMA_ProxyCreate (SU_PORT_T port);

// �������/����������� ������
CMD_NUM DL_DSMA_ProxyDestroy (UINT32 proxy_id);

// ���������� ������, ������� ���������� � ������������ � �����������, ����� � EV_DSMA_PROXY_STATE
CMD_NUM DL_DSMA_ProxyActivate (UINT32 proxy_id);

// ������������ ������, ��������� ������ � ��������� ����������
CMD_NUM DL_DSMA_ProxyDeactivate (UINT32 proxy_id);

// ������������ ������, ��������� ������ (���������� ��������, �.�. ����� ������ ������������)
CMD_NUM DL_DSMA_ProxyDeactivateSocket (UINT32 proxy_id);

// ��������� ��������, ����� � EV_DSMA_PROXY_ATTR (� ������ DSMA_DATA_T)
CMD_NUM DL_DSMA_ProxyAttributeSet (UINT32 proxy_id, UINT32 attr_count, DSMA_ATTRIBUTE_T *attr_list);

// �������� ��������, ����� � EV_DSMA_PROXY_ATTR (� ������ DSMA_DATA_T)
CMD_NUM DL_DSMA_ProxyAttributeGet (UINT32 proxy_id, UINT32 attr_count, DSMA_ATTRIBUTE_T *attr_list);

// ��������� ������, ���������� ���-�� ����
UINT32 DL_DSMA_ProxyWrite (UINT32 proxy_id, BYTE *buf, UINT32 size);

// ��������� ������, ���������� ���-�� ����
UINT32 DL_DSMA_ProxyRead (UINT32 proxy_id, BYTE *buf, UINT32 size);

#endif
