//By AlSch092 @github
#pragma once
#include "../Network/NetClient.hpp"
#include "../Common/Settings.hpp"

#define MAX_DLLS 256 
#define MAX_FILE_PATH_LENGTH 256
#define USER_SHARED_DATA ((KUSER_SHARED_DATA * const)0x7FFE0000)

namespace Debugger
{
    enum Detections
    {
        WINAPI_DEBUGGER = 1,
        PEB,
        HARDWARE_REGISTERS,
        HEAP_FLAG,
        INT3,
        INT2C,
        INT2D,
        CLOSEHANDLE,
        DEBUG_OBJECT,
        VEH_DEBUGGER,
        KERNEL_DEBUGGER,
        TRAP_FLAG,
        DEBUG_PORT,
        PROCESS_DEBUG_FLAGS,
    };

    class AntiDebug
    {
    public:
        
        AntiDebug(Settings* s, NetClient* netClient)
        {
            this->netClient = netClient;

            if (!PreventWindowsDebuggers())
            {
                Logger::logf("UltimateAnticheat.log", Warning, "Failed to apply anti-debugging technique @ AntiDebug()");
            }

            if (s != nullptr)
                this->Config = s;
        }

        ~AntiDebug()
        {
            delete this->DetectionThread;
        }
        
        list<Detections> GetDebuggerMethodsDetected() { return DebuggerMethodsDetected; }
    
        Thread* GetDetectionThread() { return this->DetectionThread; }
        HANDLE GetDetectionThreadHandle() { if (this->DetectionThread != NULL) return this->DetectionThread->handle; else return INVALID_HANDLE_VALUE; }
        void SetDetectionThread(HANDLE h) { this->DetectionThread->handle = h; }

        void StartAntiDebugThread();

        static void CheckForDebugger(LPVOID AD); //thread function, pass AntiDebug* member as `AD`

        inline bool _IsDebuggerPresent() { return IsDebuggerPresent(); }
        inline bool _IsDebuggerPresent_HeapFlags();
        inline bool _IsDebuggerPresent_CloseHandle();
        inline bool _IsDebuggerPresent_RemoteDebugger();
        inline bool _IsDebuggerPresent_Int2c();
        inline bool _IsDebuggerPresent_Int2d();
        inline bool _IsDebuggerPresent_VEH();
        inline bool _IsDebuggerPresent_DbgBreak();
        inline bool _IsDebuggerPresent_PEB();
        inline bool _IsDebuggerPresent_DebugPort();
        inline bool _IsDebuggerPresent_ProcessDebugFlags();
        inline bool _IsKernelDebuggerPresent();
        inline bool _IsKernelDebuggerPresent_SharedKData();
        static void _IsHardwareDebuggerPresent(LPVOID AD); //we need this particular routine threaded due to requirement of suspending threads to properly fetching thread context, if this is not threaded it means one thread (the anti-debugger one) will be lacking DR checks

        static bool PreventWindowsDebuggers(); //patch DbgBreakpoint + DbgUiRemoteBreakin

        bool AddDetectedFlag(Detections f);
        bool Flag(Detections flag);

        NetClient* GetNetClient() { return this->netClient; }

        Settings* GetSettings() { return this->Config; }

    private:       
        list<Detections> DebuggerMethodsDetected;

        Thread* DetectionThread = NULL;

        NetClient* netClient = nullptr;

        Settings* Config = nullptr;
    };
}

typedef struct _KSYSTEM_TIME
{
    ULONG LowPart;
    LONG High1Time;
    LONG High2Time;
} KSYSTEM_TIME, * PKSYSTEM_TIME;

typedef enum _NT_PRODUCT_TYPE
{
    NtProductWinNt = 1,
    NtProductLanManNt = 2,
    NtProductServer = 3
} NT_PRODUCT_TYPE;

typedef enum _ALTERNATIVE_ARCHITECTURE_TYPE
{
    StandardDesign = 0,
    NEC98x86 = 1,
    EndAlternatives = 2
} ALTERNATIVE_ARCHITECTURE_TYPE;

typedef struct _KUSER_SHARED_DATA  //https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/ns-ntddk-kuser_shared_data
{
    ULONG                         TickCountLowDeprecated;
    ULONG                         TickCountMultiplier;
    KSYSTEM_TIME                  InterruptTime;
    KSYSTEM_TIME                  SystemTime;
    KSYSTEM_TIME                  TimeZoneBias;
    USHORT                        ImageNumberLow;
    USHORT                        ImageNumberHigh;
    WCHAR                         NtSystemRoot[260];
    ULONG                         MaxStackTraceDepth;
    ULONG                         CryptoExponent;
    ULONG                         TimeZoneId;
    ULONG                         LargePageMinimum;
    ULONG                         AitSamplingValue;
    ULONG                         AppCompatFlag;
    ULONGLONG                     RNGSeedVersion;
    ULONG                         GlobalValidationRunlevel;
    LONG                          TimeZoneBiasStamp;
    ULONG                         NtBuildNumber;
    NT_PRODUCT_TYPE               NtProductType;
    BOOLEAN                       ProductTypeIsValid;
    BOOLEAN                       Reserved0[1];
    USHORT                        NativeProcessorArchitecture;
    ULONG                         NtMajorVersion;
    ULONG                         NtMinorVersion;
    BOOLEAN                       ProcessorFeatures[64];
    ULONG                         Reserved1;
    ULONG                         Reserved3;
    ULONG                         TimeSlip;
    ALTERNATIVE_ARCHITECTURE_TYPE AlternativeArchitecture;
    ULONG                         BootId;
    LARGE_INTEGER                 SystemExpirationDate;
    ULONG                         SuiteMask;
    BOOLEAN                       KdDebuggerEnabled;
    union {
        UCHAR MitigationPolicies;
        struct {
            UCHAR NXSupportPolicy : 2;
            UCHAR SEHValidationPolicy : 2;
            UCHAR CurDirDevicesSkippedForDlls : 2;
            UCHAR Reserved : 2;
        };
    };
    USHORT                        CyclesPerYield;
    ULONG                         ActiveConsoleId;
    ULONG                         DismountCount;
    ULONG                         ComPlusPackage;
    ULONG                         LastSystemRITEventTickCount;
    ULONG                         NumberOfPhysicalPages;
    BOOLEAN                       SafeBootMode;
    union {
        UCHAR VirtualizationFlags;
        struct {
            UCHAR ArchStartedInEl2 : 1;
            UCHAR QcSlIsSupported : 1;
        };
    };
    UCHAR                         Reserved12[2];
    union 
    {
        ULONG SharedDataFlags;
        struct 
        {
            ULONG DbgErrorPortPresent : 1;
            ULONG DbgElevationEnabled : 1;
            ULONG DbgVirtEnabled : 1;
            ULONG DbgInstallerDetectEnabled : 1;
            ULONG DbgLkgEnabled : 1;
            ULONG DbgDynProcessorEnabled : 1;
            ULONG DbgConsoleBrokerEnabled : 1;
            ULONG DbgSecureBootEnabled : 1;
            ULONG DbgMultiSessionSku : 1;
            ULONG DbgMultiUsersInSessionSku : 1;
            ULONG DbgStateSeparationEnabled : 1;
            ULONG SpareBits : 21;
        } Dbg;
    } DbgUnion;
    ULONG                         DataFlagsPad[1];
    ULONGLONG                     TestRetInstruction;
    LONGLONG                      QpcFrequency;
    ULONG                         SystemCall;
    ULONG                         Reserved2;
    ULONGLONG                     FullNumberOfPhysicalPages;
    ULONGLONG                     SystemCallPad[1];
    union 
    {
        KSYSTEM_TIME TickCount;
        ULONG64      TickCountQuad;
        struct 
        {
            ULONG ReservedTickCountOverlay[3];
            ULONG TickCountPad[1];
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME3;
    ULONG                         Cookie;
    ULONG                         CookiePad[1];
    LONGLONG                      ConsoleSessionForegroundProcessId;
    ULONGLONG                     TimeUpdateLock;
    ULONGLONG                     BaselineSystemTimeQpc;
    ULONGLONG                     BaselineInterruptTimeQpc;
    ULONGLONG                     QpcSystemTimeIncrement;
    ULONGLONG                     QpcInterruptTimeIncrement;
    UCHAR                         QpcSystemTimeIncrementShift;
    UCHAR                         QpcInterruptTimeIncrementShift;
    USHORT                        UnparkedProcessorCount;
    ULONG                         EnclaveFeatureMask[4];
    ULONG                         TelemetryCoverageRound;
    USHORT                        UserModeGlobalLogger[16];
    ULONG                         ImageFileExecutionOptions;
    ULONG                         LangGenerationCount;
    ULONGLONG                     Reserved4;
    ULONGLONG                     InterruptTimeBias;
    ULONGLONG                     QpcBias;
    ULONG                         ActiveProcessorCount;
    UCHAR                         ActiveGroupCount;
    UCHAR                         Reserved9;
    union 
    {
        USHORT QpcData;
        struct 
        {
            UCHAR QpcBypassEnabled;
            UCHAR QpcReserved;
        };
    };
    LARGE_INTEGER                 TimeZoneBiasEffectiveStart;
    LARGE_INTEGER                 TimeZoneBiasEffectiveEnd;
    XSTATE_CONFIGURATION          XState;
    KSYSTEM_TIME                  FeatureConfigurationChangeStamp;
    ULONG                         Spare;
    ULONG64                       UserPointerAuthMask;
    XSTATE_CONFIGURATION          XStateArm64;
    ULONG                         Reserved10[210];
} KUSER_SHARED_DATA, * PKUSER_SHARED_DATA;