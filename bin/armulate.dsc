;; ARMulator configuration file type 3
;; - armulate.dsc -
;; Copyright (c) 1996-2001 ARM Limited. All Rights Reserved.

;; RCS $Revision: 1.65.2.49 $
;; Checkin $Date: 2002/03/18 15:11:20 $
;; Revising $Author: dsinclai $
 
;;
;; This is the configuration file for ARMulator 
;;
ARMULATE_VERSION=120

;; Comment this out for benchmarking
; For the moment we assume that if no clock speed has been set on the
; command-line, the user wishes to use a wall-clock for timing
#if !CPUSPEED
Clock=Real
#endif


;; This line controls whether (some) models give more useful descriptions
;; of what they are on startup, and during running.
;Verbose=False

;; To enable faster watchpoints, set "WatchpointsEnabled"
WatchpointsEnabled=False

;; ARM966E-S HiVector boot control
;; Uncomment next line if you wish to boot ARM966E-S with hivecs
;ARM966BootHiVectors


;; *****************************************************************
;; ARMulator Peripheral Models
;; Central list of peripherals 
;; Use this list to enable/disable peripherals
;; *****************************************************************
;; To enable a peripheral change the rhs to TRUE
;; To disable a peripheral change the rhs to FALSE

;This is False by default.
WDogEnabled=False

; Note that DCC is enabled by default if running on a processor
; with an EmbeddedICE macrocell

;; end of peripheral list
;; *****************************************************************


;; ARMulator can tell you how much stack your program uses (at a
;; substantial runtime cost)
TrackStack=False



{ PeripheralSets

{ Processors_Common_ARMULATE=Processors_Common_No_Peripherals

;; This probably has to be loaded after after Flatmem (for now),
;; so we load it in Default_Common_Peipherals.
; Load code-sequences.
;{ Codeseq=Codeseq
;}

; The BUS model.
{ Flatmem=Flatmem
{Peripherals=Common_Peripherals
}
}

;End Processors_Common_ARMULATE
}

;End PeripheralSets
}


;;
;; This is the list of all processors supported by ARMulator.
;;

{ Processors

;; This isn't a processor.
Default=ARM7TDMI

;; Entries are of the form:
;
; { <processor-name>=<processor-prototype>
; ... features ...
; }
;


;; The "BASIC" RDI model provides models of the ARM2/ARM6/ARM7 and ARM8
;; families.
;;#if RDI_BASIC

;; ARM6 family

{ ARM6=Processors_Common_ARMULATE
CoprocessorInterfaceType=7
MODEL_DLL_FILENAME=armulate
;; Features:
Core=ARM6
ARMulator=BASIC
Architecture=3
{DCC=Default_DebugComms
}

}

;; Variants:
{ARM60=ARM6
}

;; Cached variants
;;  MEMORY_MMUlator -- must be defined.


{ARM600=ARM6
Memory=ARM600
CacheWords=4
CacheAssociativity=64
CacheBlocks=4
HASMMU=True
}

{ARM610=ARM6
Memory=ARM610
CacheWords=4
CacheAssociativity=64
CacheBlocks=4
HASMMU=True
}

;; ARM7 family

{ ARM7=Processors_Common_ARMULATE
CoprocessorInterfaceType=7
MODEL_DLL_FILENAME=armulate
;; Features:
Core=ARM7
ARMulator=BASIC
Architecture=3
Nexec
LateAborts
SubPage
{DCC=Default_DebugComms
}
}

;; Variants of ARM7:
{ARM70=ARM7
}

;; Cached variants
{ARM700=ARM7
Memory=ARM700
CacheWords=8
CacheAssociativity=4
CacheBlocks=64
;; MMU/PU properties - used by pagetable to determine how to start
HASMMU=True
}

{ARM704=ARM7
HASMMU=True
Memory=ARM704
CacheWords=4
CacheAssociativity=4
CacheBlocks=64
}

{ARM710=ARM7
HASMMU=True
CacheWords=4
CacheAssociativity=4
CacheBlocks=128
Memory=ARM710a
}

{ARM710a=ARM7
HASMMU=True
Memory=ARM710
CacheWords=4
CacheAssociativity=4
CacheBlocks=128
}


;; ARM7D family - ARM7D and ARM70D

{ ARM7D=Processors_Common_ARMULATE
CoprocessorInterfaceType=7
MODEL_DLL_FILENAME=armulate
;; Features
Core=ARM7
ARMulator=BASIC
Architecture=3
Nexec=True
LateAborts=True
Debug=True
{DCC=Default_DebugComms
}

}
;;Variants of ARM7D
{ARM70D=ARM7D
Debug=True
{DCC=Default_DebugComms
}
}

;; ARM7DM families
{ARM7DM=ARM7D
Architecture=3M
}

{ARM70DM=ARM7D
Architecture=3M
}



;; Thumb family

{ ARM7TDM=Processors_Common_ARMULATE
CoprocessorInterfaceType=7
MODEL_DLL_FILENAME=armulate
;; Features
meta_moduleserver_processor=ARM7TDMI
Core=ARM7
ARMulator=BASIC
Architecture=4T
Nexec=True
LateAborts=True
Debug=True
{DCC=Default_DebugComms
}

}
;; Variants of ARM7TDM
{ARM7TDMI=ARM7TDM
}

{ARM7TM=ARM7TDM
}

;; Cached variants

{ARM720T=ARM7TDM
meta_moduleserver_processor=ARM720T
Memory=ARM720T
HighExceptionVectors=True
CacheWords=4
CacheAssociativity=4
CacheBlocks=128
HASMMU=True
}

{ARM720T-REV4=ARM7TDM
meta_moduleserver_processor=ARM720T
Memory=MMU720
HighExceptionVectors=True
CacheWords=8
CacheAssociativity=4
CacheBlocks=64
HASMMU=True
}




{ARM710T=ARM7TDM
meta_moduleserver_processor=ARM710T
Memory=ARM710T
CacheWords=4
CacheAssociativity=4
CacheBlocks=128
HASMMU=True
}

{ARM740T=ARM7TDM
{meta_moduleserver_processor=ARM740T
revision=0
}
CacheWords=4
CacheAssociativity=4
CacheBlocks=128
Memory=ARM740T
HASPU=True
TBox_MemMask=TRUE
}

;; Synthesisable ARM family

{ ARM7TM-S=Processors_Common_ARMULATE
CoprocessorInterfaceType=7
MODEL_DLL_FILENAME=armulate
;; Features
Core=ARM7
ARMulator=BASIC
Architecture=4T
Nexec=True
LateAborts=True
Debug=True
{DCC=Default_DebugComms
}
}

;;Variants of ARM7TM-S

{ARM7T-S=ARM7TM-S
Architecture=4TxM
EarlySignedMultiply=True
}

{ARM7TDI-S=ARM7TM-S
Architecture=4TxM
EarlySignedMultiply=True
}

{ARM7TDMI-S=ARM7TM-S
}

;; ARM8 family

{ ARM8=Processors_Common_ARMULATE
CoprocessorInterfaceType=7
MODEL_DLL_FILENAME=armulate
;; Features:
Core=ARM8
ARMulator=BASIC
Architecture=4
Nexec=True
MultipleEarlyAborts=True
AbortsStopMultiple=True
Prefetch=True
HasBranchPrediction=True
NoLDCSTC=True
{DCC=Default_DebugComms
}
}

;; Variants of ARM8:
{ARM810=ARM8
Memory=ARM810
CacheWords=4
CacheAssociativity=64
CacheBlocks=8
HASMMU=True
}

;; Endif RDI_BASIC.
;;#endif



;; StrongARM1 family

{ StrongARM1=Processors_Common_ARMULATE
CoprocessorInterfaceType=7
MODEL_DLL_FILENAME=armulate
;; Features:
Core=StrongARM
ARMulator=STRONG
Architecture=4
Nexec=True
MultipleEarlyAborts=True
AbortsStopMultiple=True
StrongARMAware=True
NoLDCSTC=True
NoCDP=True
DEFAULT_CPUSPEED=20.24MHz
{DCC=Default_DebugComms
}
}
;;;#if MEMORY_StrongMMU
;; Variants of StrongARM:
{SA-110=StrongARM1
Memory=SA-110
ICYCLES=TRUE
HASMMU=True
}
;;;;#endif



;; ARM9 family
;

;; ARM 9E-S variants
{ ARM9E-S-REV1=Processors_Common_ARMULATE
CoprocessorInterfaceType=9
MODEL_DLL_FILENAME=armulate
;; Features:
{meta_moduleserver_processor=ARM9E-S
revision=1
}
Core=ARM9
ARMulator=ARM9ulator
Architecture=4T
Nexec=True
MultipleEarlyAborts=True
AbortsStopMultiple=True
CoreCycles=True
HighExceptionVectors=True
ARM9Extensions=True
ARM9CoprocessorInterface=True
ARMV5PEXTENSIONS=True
;;StrongARMAware
;;NoLDCSTC
;;NoCDP
{DCC=Default_DebugComms
}
}

{ ARM9E-S-REV0=Processors_Common_ARMULATE
CoprocessorInterfaceType=9
MODEL_DLL_FILENAME=armulate
;; Features:
{meta_moduleserver_processor=ARM9E-S
revision=0
}
Core=ARM9
ARMulator=ARM9ulator
Architecture=4T
Nexec=True
MultipleEarlyAborts=True
AbortsStopMultiple=True
CoreCycles=True
HighExceptionVectors=True
ARM9Extensions=True
ARM9CoprocessorInterface=True
;;StrongARMAware
;;NoLDCSTC
;;NoCDP
{DCC=Default_DebugComms
}
}



{ ARM9E-S-REV2=Processors_Common_ARMULATE
CoprocessorInterfaceType=9
{meta_moduleserver_processor=ARM9E-S
revision=2
}
Core=ARM9
ARMulator=ARM9ulator
Architecture=5T
Nexec=True
MultipleEarlyAborts=True
AbortsStopMultiple=True
CoreCycles=True
HighExceptionVectors=True
ARM9Extensions=True
ARMv5PExtensions=True
ARM9OptimizedMemory=True
ARM9OptimizedDAborts=True

ARMJavaExtensions=False
ARMJavaV1Extensions=False

ARM9CoprocessorInterface=True
{DCC=Default_DebugComms
}
}




{ ARM9EJ-S-REV1=ARM9E-S-REV2
{meta_moduleserver_processor=ARM9EJ-S
revision=1
}
;Core=ARM9
;ARMulator=ARM9ulator
;Architecture=5T
;Nexec=True
;MultipleEarlyAborts=True
;AbortsStopMultiple=True
;CoreCycles=True
;HighExceptionVectors=True
;ARM9Extensions=True
;ARMv5PExtensions=True
;ARM9OptimizedMemory=True
ARMJavaExtensions=True
ARMJavaV1Extensions=True
;ARM9CoprocessorInterface=True
;{DCC=Default_DebugComms
;}
}

{ ARM7ej-s=ARM9ej-s
VNInterface=TRUE
Memory=ARM7ej
}

{ ARM966E-S-REV0=Processors_Common_ARMULATE
CoprocessorInterfaceType=9
MODEL_DLL_FILENAME=armulate
{meta_moduleserver_processor=ARM966E-S
revision=0
}
Core=ARM9
ARMulator=ARM9ulator

;(Rev 0 is really. 5TExP, i.e. not a full 5T.)
Architecture=5T
Nexec=True
MultipleEarlyAborts=True
AbortsStopMultiple=True
CoreCycles=True
HighExceptionVectors=True
ARM9Extensions=True
;966 has neither PU nor MMU.
HASTCRAM=TRUE
HasSRAM=True
;
CHIPID=0x41049660
Memory=ARM966E_S
revision=0
IRamSize=0x10000
DRamSize=0x10000
ARM9CoprocessorInterface=True
{DCC=Default_DebugComms
}
}

;;(Rev 1 s.b. 5TE)
{ ARM966E-S-REV1=ARM966E-S-REV0
;; We'd like to be 5T.
Architecture=5T
ARM9CoprocessorInterface=True
ARMV5PEXTENSIONS=True
; Note that rev1 is an >architecture< upgrade, but still revision 0
; (and variant 0) in the CP15 ID register. 
CHIPID=0x41059660
HASTCRAM=TRUE
HasSRAM=True
Memory=ARM966E_S
revision=1
IRamSize=0x10000
DRamSize=0x10000
}


{ ARM966E-S-REV2=ARM966E-S-REV1
; We cannot yet model external TCRAM, or their timings,
; but we can model the internal 9-REV2 core.
{meta_moduleserver_processor=ARM966E-S
revision=2
}
Core=ARM9
ARMulator=ARM9ulator
Architecture=5T
Nexec=True
MultipleEarlyAborts=True
AbortsStopMultiple=True
CoreCycles=True
HighExceptionVectors=True
ARM9Extensions=True
ARMv5PExtensions=True
ARM9OptimizedMemory=True
ARMJavaExtensions=False
ARMJavaV1Extensions=False
ARM9CoprocessorInterface=True
revision=2
; This revision is reflected in the "variant" field of CHIPID.
CHIPID=0x41259660
IRamSize=0x10000
DRamSize=0x10000
{DCC=Default_DebugComms
}
}



{ ARM946E-S-REV1=Processors_Common_ARMULATE
CoprocessorInterfaceType=9
MODEL_DLL_FILENAME=armulate
{meta_moduleserver_processor=ARM946E-S
revision=1
}
Core=ARM9
ARMulator=ARM9ulator
Architecture=5T
Nexec=True
MultipleEarlyAborts=True
AbortsStopMultiple=True
CoreCycles=True
HighExceptionVectors=True
ARM9Extensions=True
ARM9CoprocessorInterface=True
ARMV5PEXTENSIONS=True
HASPU=True
CHIPID=0x41000000
REVISION=0x1
;This belongs to the memory.
;CHIPNUMBER=0x946
HASTCRAM=TRUE
HasSRAM=True
Memory=ARM946E_S
IRamSize=0x10000
DRamSize=0x10000
ICache_Associativity=4
DCache_Associativity=4

;; Use cache lines to change cache size
;; 512 = 16Kb
;; 256 =  8kb

ICache_Lines=512
DCache_Lines=512
{DCC=Default_DebugComms
}
}



{ ARM946E-S-REV0=Processors_Common_ARMULATE
CoprocessorInterfaceType=9
MODEL_DLL_FILENAME=armulate
{meta_moduleserver_processor=ARM946E-S
revision=0
}
Core=ARM9
ARMulator=ARM9ulator
Architecture=4T
Nexec=True
MultipleEarlyAborts=True
AbortsStopMultiple=True
CoreCycles=True
HighExceptionVectors=True
ARM9Extensions=True
ARM9CoprocessorInterface=True
HASPU=True
CHIPID=0x41000000
REVISION=0x0
;This belongs to the memory.
;CHIPNUMBER=0x946
HASTCRAM=True
HasSRAM=True
Memory=ARM946E_S
IRamSize=0x10000
DRamSize=0x10000
ICache_Associativity=4
DCache_Associativity=4

;; Use cache lines to change cache size
;; 512 = 16Kb
;; 256 =  8kb
ICache_Lines=512
DCache_Lines=512
{DCC=Default_DebugComms
}
}

;This is a REV2 (but REV1 Jazelle)
{ ARM926EJ-S=Processors_Common_ARMULATE
CoprocessorInterfaceType=9
;Processor=ARM926EJ-S
{meta_moduleserver_processor=ARM926EJ-S
revision=0
}
;This is read by the memory-model from the Core's config.
Revision=3
;CHIPID=0x41069262
Core=ARM9
ARMulator=ARM9ulator
Architecture=5T
Nexec=True
MultipleEarlyAborts=True
AbortsStopMultiple=True
CoreCycles=True
HighExceptionVectors=True
ARM9Extensions=True
ARMv5PExtensions=True
ARMJavaExtensions=True
ARMJavaV1Extensions=True
ARM9OptimizedMemory=True
ARM9OptimizedDAborts=True
ARM9CoprocessorInterface=True
HASMMU=True
HASTCRAM=TRUE
HasSRAM=True
Memory=ARM926EJ_S
;ARM926EJ_S Memory settings.
IRamSize=0x10000
DRamSize=0x10000
ICache_Associativity=4
DCache_Associativity=4
ICache_Lines=512
DCache_Lines=512
{DCC=Default_DebugComms
}
}

;Same as ARM9TDMI-REV0; release was bugfix.
{ ARM9TDMI-REV1=Processors_Common_ARMULATE
CoprocessorInterfaceType=9
MODEL_DLL_FILENAME=armulate
;; Features:
meta_moduleserver_processor=ARM9TDMI
Core=ARM9
ARMulator=ARM9ulator
Architecture=4T
Nexec=True
MultipleEarlyAborts=True
AbortsStopMultiple=True
CoreCycles=True
HighExceptionVectors=True
ARM9CoprocessorInterface=True
{DCC=Default_DebugComms
}

}

{ ARM9TDMI-REV2=ARM9TDMI-REV1
ARM9OptimizedDAborts=True
}


;; Variants of ARM9TDMI:


;;;#if MEMORY_ARM940CacheMPU
{ ARM940T=ARM9TDMI-REV2
{meta_moduleserver_processor=ARM940T
revision=2
}
Memory=ARM940T
;;Set the number of cache lines
ICACHE_LINES=256
DCACHE_LINES=256
;; 256 = 4 Kbytes
;; 512 = 8 Kbytes
HASPU=TRUE
}
;;;#endif
;;;#if MEMORY_ARM920CacheMMU
{ ARM920T=ARM9TDMI-REV2
meta_moduleserver_processor=ARM920T
Memory=ARM920T
HASMMU=TRUE
revision=1
}
;;;#endif

{ ARM922T=ARM9TDMI-REV2
meta_moduleserver_processor=ARM920T
Memory=ARM922T
ICACHE_LINES=256
DCACHE_LINES=256
HASMMU=True
}


;; New processors/variants can be added here.
}

;;
;; List of memory models
;;

{ Memories

;;
;; If there is a cache/mmu memory-model defined, then it loads the
;; memory named as Memories:Default to handle external accesses.
;;

;; Default memory model is the "Flat" model.
;; [Formerly, or the "MapFile" model if there is an armsd.map file to load.]
;; Validation suite uses the trickbox
;; but that's a SORDI plugin now.
;#if Validate && Validate==CP
;Default=TrickBox
;#endif

;;;; Default default is the flat memory map
;;Default=Flat
Default=NUL_BIU

;Future: This performs some of the duties of a cache model
; for uncached cores (E.g. ARM7TDMI).
;Currently: This increments bus_BusyUntil once per call.
{Nul_BIU
Memory=Flat
}

{AR<7EJ
Memory=Flat
}

;; The "MMUlator" provides emulation of the caches and MMU's on the
;; ARM6/ARM7 and ARM8 processor families.

;#if MEMORY_MMUlator
;; Plumbing for cached models - leave alone
ARM600=MMUlator
ARM610=MMUlator

ARM700=MMUlator
ARM704=MMUlator
ARM710=MMUlator
ARM710a=MMUlator
ARM710T=MMUlator
ARM720T=MMUlator
ARM740T=MMUlator

;; Validationmem is used by the 740T to map the top 2GB of memory onto the 
;; lower 2GB of memory. This is used during the validation process and must be
;; provided here as there is no MMU. 


;#if ARM740T && Validate
;Validationmem=1
;#endif

ARM810=MMUlator

{ MMUlator

IdleCycles
;; Uncomment this to have a byte-lane memory interface
;;Bytelanes
#if Bytelanes
Memory=BytelaneVeneer
#else
Memory=Default
#endif

;; If Track=True, the MMUlator tells you when registers are
;; written to, etc. Helps debugging.
Track=False



{ ARM600
TLBSize=32
RNG=6
WriteBufferAddrs=2
WriteBufferWords=8
CacheReplaceTicks=1
CacheWrite=WriteThrough
HasUpdateable=TRUE
BufferedSwap=TRUE
Architecture=3
CacheWriteBackInterlock
sNa=Yes
ChipNumber=0x60
Has26BitConfig=True
Replacement=Random
HasWriteBuffer
HASMMU=TRUE

ARM610:NoCoprocessorInterface
ARM610:ChipNumber=0x61

; Set core/memory clock ratio
;;MCCFG=2 ;;Moved to root.
}

ARM610=ARM600

{ ARM700

TLBSize=64
RNG=7
WriteBufferAddrs=4
WriteBufferWords=8
CacheReplaceTicks=1
CacheWrite=WriteThrough
HasRFlag
HasUpdateable=FALSE
BufferedSwap=FALSE
Architecture=3
CacheWriteBackInterlock
sNa=Yes
Replacement=Random
Has26BitConfig=True
HasWriteBuffer=True
CheckCacheWhenDisabled=True

ChipNumber=0x700

ARM710:NoCoprocessorInterface
ARM710:ChipNumber=0x0710
ARM710:Revision=0
 
ARM704:NoCoprocessorInterface
ARM704:ChipNumber=0x2710
ARM704:Revision=0
 
ARM710a:NoCoprocessorInterface
ARM710a:ChipNumber=0x4710
ARM710a:Revision=0
 

ARM710T:Architecture=4T
ARM710T:ChipNumber=0x710
ARM710T:Revision=0
ARM710T:ThumbAware=1
ARM710T:ProcessId=0
  

ARM720T:Architecture=4T
ARM720T:ChipNumber=0x720
ARM720T:Revision=0
ARM720T:ThumbAware=1
ARM720T:ProcessId=1
 

ARM740T:Architecture=4T
ARM740T:ChipNumber=0x740
ARM740T:Revision=0
ARM740T:ThumbAware=1
ARM740T:ProcessId=0
ARM740T:ProtectionUnit=1
ARM740T:LockDownTLB=0
 
; Set core/memory clock ratio
;;MCCFG=3
}

ARM710=ARM700
ARM710a=ARM700
ARM704=ARM700
ARM710T=ARM700
ARM720T=ARM700
ARM740T=ARM700

{ ARM810

TLBSize=64
RNG=6
WriteBufferAddrs=4
WriteBufferWords=8
CacheReplaceTicks=3
CacheWrite=WriteBack
HasRFlag
NoUpdateable
NoBufferedSwap
Architecture=4
CacheBlockInterlock
;; If using the PLLs
;PLLClock
;RefClkCfg=0
;PLLCfg=15
;PLLRange=0
HasBranchPrediction
Replacement=Random
HasWriteBuffer
LockDownCache
LockDownTLB
CheckCacheWhenDisabled

ChipNumber=0x810

HASMMU=TRUE

;Set core/memory clock speed ratio
;;MCCFG=9
}

Replacement=rng
WriteBufferWords=8
Revision=0xff

;; Track, if on, displays to the console all CP15 (etc.) operations
Track=Off

LRURead
LRUWrite

}
;#endif
; \ MMUlator


{ MMU720

IdleCycles
Memory=Default

;; If Track=True, the MMUlator tells you when registers are
;; written to, etc. Helps debugging.
Track=False
TLBSize=64
RNG=7
WriteBufferAddrs=8
WriteBufferWords=8
CacheReplaceTicks=1
CacheWrite=WriteThrough
HasRFlag
HasUpdateable=FALSE
BufferedSwap=FALSE
Architecture=3
CacheWriteBackInterlock
sNa=Yes
Replacement=Random
Has26BitConfig=True
HasWriteBuffer=True
CheckCacheWhenDisabled=True
Architecture=4T
ChipNumber=0x720
Revision=4
ThumbAware=1
ProcessId=1
 
; Set core/memory clock ratio
;;MCCFG=3


Replacement=rng
WriteBufferWords=8
Revision=0xff
ProcessId=1
;; Track, if on, displays to the console all CP15 (etc.) operations
Track=Off

LRURead
LRUWrite

}



;; The "StrongMMU" model provides the cache model for the SA-110

;#if MEMORY_StrongMMU
SA-110=StrongMMU

{ StrongMMU
; This selects normal (AHB-like) interface out of the cache.
; The alternative "Enhanced" selects a bytelane-interface which
; the peripheral- and memory-map decoders cannot handle yet, so
; we don't use them yet.
Config=Standard

ChipNumber=0x110
;; Clock speed controlled by three values:
;; See the SA-110 Technical Reference Manual for details.
CCLK=3.68MHz
CCCFG=0
;;MCCFG=0

HASMMU=TRUE

;; "ClockSwitching" controls whether "clock switching" is disabled (as on
;; real silicon) or enabled (to simplify benchmarking) on reset.
ClockSwitching=True

;; To enable useful timings for profiling (etc), the StrongARMulator allows
;; us to divide what would be the real core clock frequency by a factor. This
;; "slows down" StrongARM1, so we get more emulated time per real time.
Time_Scale_Factor=1

;Note that although the StrongMMU can be configured to use
; values for iCacheLines and dCachLines (must be powers of 2)
; other than the default sizes (16K/32,16K/32),
; we do not test this, and do not encourage variations
; because Intel do not.

Memory=Default
}
;#endif
; \ StrongMMU


;#if MEMORY_ARM940CacheMPU
;; The "ARM940CacheMPU" model provides the cache/PU model for the ARM940
ARM940T=ARM940CacheMPU

{ ARM940CacheMPU

ChipNumber=0x940

;; Clock speed controlled by:
;;MCCFG=1
Revision=2
HASPU=TRUE
 
Memory=Default
}
;#endif
; \ MEMORY_ARM940CacheMPU



;#if MEMORY_ARM920CacheMMU
;; The "ARM920CacheMMU" model provides the cache/MMU model for the ARM920
ARM920T=ARM920CacheMMU

{ ARM920CacheMMU

;#if Validate
;Config=Standard
;#endif
Config=Enhanced
ChipNumber=0x920
revision=1
;; Clock speed now controlled by MCCFG at top level.
;;MCCFG=8

HASMMU=TRUE
ProcessId=TRUE
Memory=Default
}
;#endif
; \MEMORY_ARM920CacheMMU

;; The "ARM922CacheMMU" model provides the cache/MMU model for the ARM922
ARM922T=ARM922CacheMMU

{ ARM922CacheMMU

;#if Validate
;Config=Standard
;#endif
Config=Enhanced
ChipNumber=0x922
ProcessID=True

;; Clock speed now controlled by MCCFG at top-level.
;;MCCFG=8

HASMMU=True

Memory=Default
}



ARM966E_S=ARM9xxE
ARM946E_S=ARM9xxE
ARM926EJ_S=ARM9xxE


{ ARM9xxE
;#if Validate
;Config=Standard
;#endif
Config=Enhanced


#if ARM9x6BootHiVectors || ARM966BootHiVectors
VIntHi=TRUE
#else 
VIntHi=FALSE
#endif                       



; A rev2 966 has wait-states on IRAM.
;;ARM966E_S:IRamwaitStates=0x1
ARM966E_S:WriteBufferWords=12
ARM966E_S:ChipNumber=0x966
ARM966E_S:Revision=0



ARM946E_S:WriteBufferWords=16
ARM946E_S:ChipNumber=0x946
ARM946E_S:Revision=0
;;Set the number of cache lines

;;;These settings are in the core, so an AMI file can change them.
;ARM926EJ_S:IRamSize=0x10000
;ARM926EJ_S:DRamSize=0x10000
;ARM926EJ_S:ICache_Associativity=4
;ARM926EJ_S:DCache_Associativity=4
;ARM926EJ_S:ICache_Lines=256
;ARM926EJ_S:DCache_Lines=256

{ARM926EJ_S
;ARM926EJ_S=ARM9xxE
;;These can stay here-we don't intend users to change these.
IuTag_Lines = 4
DuTag_Lines = 8
IuTLB_Lines = 8
DuTLB_Lines = 8
TLB_Lines = 64
TLB_Associativity=2
TLB_LockdownLines=8
WriteBufferWords=16
ChipNumber=0x926
; Revision has moved to core's config
ProcessId=True

}


;;MCCFG=1
Memory=Default            

}



#if MEMORY_BytelaneVeneer
BytelaneVeneer:Memory=Default
#endif


;; end of memories
}

;; Co-processor bus
CoprocessorBus=ARMCoprocessorBus

;;
;; Coprocessor configurations
;;

{ Coprocessors

; Here is the list of co-processors, in the form:
; Coprocessor[<n>]=Name

#if COPROCESSOR_DummyMMU && !MEMORY_MMUlator
;; By default, we install a dummy MMU on co-processor 15.
CoProcessor[15]=DummyMMU

; Here is the configuration for the co-processors.

;; The Dummy MMU can be configured to return a given Chip ID
;DummyMMU:ChipID=
#endif
}

;;
;; Basic models (ARMulator extensions)
;;

{ EarlyModels
;;
;; "EarlyModels" get run before memory initialisation, "Models" after.
;;



{ EventConverter
}

;; End of EarlyModels
}

{ Models
;; This file is no longer used for Model configuration.
;; End of Models.
}




;; EOF armulate.dsc




