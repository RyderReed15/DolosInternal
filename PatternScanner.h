#pragma once
#include <Windows.h>
#include <string>
#include <fstream>
#include "Utils.h"
typedef struct _RECVPROP RECVPROP, * PRECVPROP;
typedef struct _RECVTABLE RECVTABLE, * PRECVTABLE;
typedef struct _CLIENTCLASS CLIENTCLASS, * PCLIENTCLASS;
typedef struct _RECVPROP {
    char* m_pVarName;
    INT32        m_RecvType;
    INT32        m_Flags;
    INT32        m_StringBufferSize;
    INT32        m_bInsideArray;
    const INT32  m_pExtraData;
    PRECVPROP    m_pArrayProp;
    INT32        m_ArrayLengthProxy;
    INT32        m_ProxyFn;
    INT32        m_DataTableProxyFn;
    PRECVTABLE   m_pDataTable;
    INT32        m_Offset;
    INT32        m_ElementStride;
    INT32        m_nElements;
    const INT32  m_pParentArrayPropName;
} RECVPROP, * PRECVPROP;

typedef struct _RECVTABLE {

    PRECVPROP   m_pProps;
    INT32       m_nProps;
    INT32       m_pDecoder;
    char*       m_pNetTableName;
    bool        m_bInitialized;
    bool        m_bInMainList;
} RECVTABLE, * PRECVTABLE;

typedef struct _CLIENTCLASS {
    INT32           m_pCreateFn;
    INT32           m_pCreateEventFn;
    INT32           m_pNetworkName;
    PRECVTABLE      m_pRecvTable;
    PCLIENTCLASS    m_pNext;
    INT32           m_ClassID;
} CLIENTCLASS, * PCLIENTCLASS;
typedef struct _SIGNATURE {
    BYTE        Pattern[48];
    CHAR        Mask[48];
    SIZE_T      PatternSize;
    BOOL        Relative;
    UINT32       Offsets[8];
    SIZE_T      OffsetSize;
    ULONG       Extra;
    CHAR        Module[16];
} SIGNATURE, * PSIGNATURE;

typedef struct _NETVAR {
    CHAR    Name[32];	    // Name to scan for
    CHAR    Table[32];      // Name of table to search in
    INT32   Offset;// Offset to add to the original netvar
} NETVAR, * PNETVAR;
bool GetConfigBuffer(PBYTE* Buffer, SIZE_T* BufferSize);
bool PatternScan();
PVOID FindSignature(SIGNATURE Signature);
SIGNATURE BuildSignature(const CHAR Name[], BYTE Buffer[], SIZE_T BufferSize);
NETVAR BuildNetvar(const CHAR Name[], BYTE Buffer[], SIZE_T BufferSize);
PVOID GetNetvarOffset(NETVAR Netvar, PRECVTABLE RecvTable);
PVOID FindNetvar(NETVAR Netvar, PCLIENTCLASS ClientClass);