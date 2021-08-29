#pragma once
/*
#include <TlHelp32.h>
#include <Winternl.h>

PVOID tlsBackup[64];
typedef struct _THREAD_BASIC_INFORMATION {


    NTSTATUS                ExitStatus;
    PVOID                   TebBaseAddress;
    CLIENT_ID               ClientId;
    KAFFINITY               AffinityMask;
    KPRIORITY               Priority;
    KPRIORITY               BasePriority;


} THREAD_BASIC_INFORMATION, * PTHREAD_BASIC_INFORMATION;

//possibly alter to not have unneeded variables
typedef struct _TEB_T {

    BYTE                    Reserved[0x0E10];
    PVOID                   TlsSlots[0x40];
    LIST_ENTRY              TlsLinks;
    PVOID                   Vdm;
    PVOID                   ReservedForNtRpc;
    PVOID                   DbgSsReserved[0x2];
    ULONG                   HardErrorDisabled;
    PVOID                   Instrumentation[0x10];
    PVOID                   WinSockData;
    ULONG                   GdiBatchCount;
    ULONG                   Spare2;
    ULONG                   Spare3;
    ULONG                   Spare4;
    PVOID                   ReservedForOle;
    ULONG                   WaitingOnLoaderLock;
    PVOID                   StackCommit;
    PVOID                   StackCommitMax;
    PVOID                   StackReserved;

} TEB_T, * PTEB_T;




typedef struct _PEB_T {
    BYTE InheritedAddressSpace;
    BYTE ReadImageFileExecOptions;
    BYTE BeingDebugged;
    BYTE SpareBool;
    void* Mutant;
    void* ImageBaseAddress;
    _PEB_LDR_DATA* Ldr;
    _RTL_USER_PROCESS_PARAMETERS* ProcessParameters;
    void* SubSystemData;
    void* ProcessHeap;
    _RTL_CRITICAL_SECTION* FastPebLock;
    void* FastPebLockRoutine;
    void* FastPebUnlockRoutine;
    DWORD EnvironmentUpdateCount;
    void* KernelCallbackTable;
    DWORD SystemReserved[1];
    DWORD ExecuteOptions : 2; // bit offset: 34, len=2
    DWORD SpareBits : 30; // bit offset: 34, len=30
    PVOID ApiSetMap;
    DWORD TlsExpansionCounter;
    void* TlsBitmap;
    DWORD TlsBitmapBits[2];
    void* ReadOnlySharedMemoryBase;
    void* ReadOnlySharedMemoryHeap;
    void** ReadOnlyStaticServerData;
    void* AnsiCodePageData;
    void* OemCodePageData;
    void* UnicodeCaseTableData;
    DWORD NumberOfProcessors;
    DWORD NtGlobalFlag;
    _LARGE_INTEGER CriticalSectionTimeout;
    DWORD HeapSegmentReserve;
    DWORD HeapSegmentCommit;
    DWORD HeapDeCommitTotalFreeThreshold;
    DWORD HeapDeCommitFreeBlockThreshold;
    DWORD NumberOfHeaps;
    DWORD MaximumNumberOfHeaps;
    void** ProcessHeaps;
    void* GdiSharedHandleTable;
    void* ProcessStarterHelper;
    DWORD GdiDCAttributeList;
    void* LoaderLock;
    DWORD OSMajorVersion;
    DWORD OSMinorVersion;
    WORD OSBuildNumber;
    WORD OSCSDVersion;
    DWORD OSPlatformId;
    DWORD ImageSubsystem;
    DWORD ImageSubsystemMajorVersion;
    DWORD ImageSubsystemMinorVersion;
    DWORD ImageProcessAffinityMask;
    DWORD GdiHandleBuffer[34];
    void (*PostProcessInitRoutine)();
    void* TlsExpansionBitmap;
    DWORD TlsExpansionBitmapBits[32];
    DWORD SessionId;
    _ULARGE_INTEGER AppCompatFlags;
    _ULARGE_INTEGER AppCompatFlagsUser;
    void* pShimData;
    void* AppCompatInfo;
    _UNICODE_STRING CSDVersion;
    void* ActivationContextData;
    void* ProcessAssemblyStorageMap;
    void* SystemDefaultActivationContextData;
    void* SystemAssemblyStorageMap;
    DWORD MinimumStackCommit;
} PEB_T, * PPEB_T;


ULONGLONG GetTime(FILETIME ft)
{
	return (((ULONGLONG)ft.dwHighDateTime) << 32) + ft.dwLowDateTime;;
}

DWORD GetMainThread()
{
    HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    ULONGLONG threadTimes = -1;
    DWORD threadID = 0;
    FILETIME lpCreationTime;
    FILETIME lpExitTime;
    FILETIME lpKernelTime;
    FILETIME lpUserTime;
    DWORD currentPID = GetCurrentProcessId();
    if (h != INVALID_HANDLE_VALUE)
    {
        THREADENTRY32 te;
        te.dwSize = sizeof(te);
        if (Thread32First(h, &te))
        {
            do
            {
                if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(te.th32OwnerProcessID))
                {
                    if (te.th32OwnerProcessID == currentPID)
                    {
                        
                            
                            if (threadTimes == -1 || threadTimes < te.tpBasePri)
                            {
                                threadTimes = te.tpBasePri;
                                threadID = te.th32ThreadID;


                            }
                            std::cout << te.th32ThreadID << " | " << te.tpBasePri << endl;
                            break;
                        
                            
                       
                        
                    }
                    
                }
                te.dwSize = sizeof(te);
            } while (Thread32Next(h, &te));
        }
        CloseHandle(h);
    }
    std::cout << threadID << endl;
    return threadID;
}

TEB_T* GetTEB(HANDLE hThread, HANDLE thisThread)
{
    THREAD_BASIC_INFORMATION tInf;
    std::cout << "Thread Handle : " << hThread << endl;
    bool Status = GetThreadInformation(hThread, (THREAD_INFORMATION_CLASS)0, &tInf, sizeof(tInf));
    bool s = SetThreadInformation(thisThread,(THREAD_INFORMATION_CLASS)0, &tInf, sizeof(tInf));
    std::cout << Status << endl;
    std::cout << "TEB address : " << (TEB_T*)tInf.TebBaseAddress << endl;
    CloseHandle(hThread);
    return (TEB_T*)tInf.TebBaseAddress;
}

void SwapTLS(TEB_T* targetTeb)
{
    
    //finish writing using https://www.unknowncheats.me/forum/1031151-post2.html
 
    TEB_T* thisTeb = (TEB_T*)NtCurrentTeb();
    std::cout << "TEB aquired" << endl;


    std::cout << "TLS backup allocated" << endl;
    std::cout << targetTeb->TlsSlots[0] << endl;
    for (int i = 0; i < 64; i++)
    {
        tlsBackup[i] = thisTeb->TlsSlots[i];
        //std::cout << "TLS slot " << i << " backed up: " << tlsBackup[i] << endl;
        thisTeb->TlsSlots[i] = targetTeb->TlsSlots[i]; 
        //std::cout << "TLS slot " << i << " swapped: " << thisTeb->TlsSlots[i] << endl;
    }
    std::cout << "TLS Swapped" << endl;

    


    return;

}
void ReturnTLS()
{
    TEB_T* thisTeb = (TEB_T*)NtCurrentTeb();
    for (int i = 0; i < 64; i++)
    {
        thisTeb->TlsSlots[i] = tlsBackup[i];
    }
}

*/