;; ARMulator configuration file type 3
;; - armiss.dsc -
;; Copyright (c) 1996-2001 ARM Limited. All Rights Reserved.

;; RCS $Revision: 1.26.2.17 $
;; Checkin $Date: 2002/03/21 16:50:38 $
;; Revising $Author: dsinclai $
 
;;
;; This is a non-user-edittable configuration file for ARMulator 
;;

;; Comment this out for benchmarking
; For the moment we assume that if no clock speed has been set on the
; command-line, the user wishes to use a wall-clock for timing
#if !CPUSPEED
Clock=Real
#endif


;; This line controls whether (some) models give more useful descriptions
;; of what they are on startup, and during running.
Verbose=False


;;
;; This is the list of all processors supported by ARMISS.
;;

{ PeripheralSets

{ Processors_Common_ARMISS=Processors_Common_No_Peripherals

;All ARMISS cores have ARM10-style coprocessor-interfaces,
; in the sense that they are clocked with "corecycles".
CoprocessorInterfaceType=10

; Avoid loading code-sequences, because they are built-in.
{ Codeseq=No_Codeseq
}

; The BUS model.
{ Flatmem=Flatmem
{Peripherals=Common_Peripherals
}
}


;End Processors_Common_ARMISS
}

;End PeripheralSets
}



{ Processors

;; This is the core macro-cell (no CP15).
{ ARM10E=PROCESSORS_COMMON_ARMISS
; 0x41 = ARM, 0x05 = arch 5E, 0xA20 = part number, 0x1 = revision.
CP15_IDREGVALUE=0x4105a201
HASMMU=False
;; Features:
Architecture=5T
;;;Processor=ARM10E
{ meta_moduleserver_processor=ARM10E
revision=1
}
MODEL_DLL_FILENAME=ARMISS
; Select a (module) RDI_ProcVec
ARMulator=ARM1020E
;NB Only 64 and 32 are legal so far.
LSU_Width=64
PFU_Width=64
;This is for the AHB.
BUS_Width=64
; ARM10E has no VFP.
; L means main memory is fixed LittleEnd (R+W)
ENDIANNESS=C


{RDIMSVR=RDIMSVR
target_controller_type=armulate
}
}
;; Variants of ARM10E

;; Architecture 5T (rev 0?) ARM10
{ARM1020T=ARM10E
Architecture=5T
;; Features:
;;;Processor=ARM10TDMI
{meta_moduleserver_processor=ARM1020T
revision=1
}
CP15_IDREGVALUE=0x4104a200
;Allow Pagetab to work.
HASMMU=True
; Select a (module) RDI_ProcVec
ARMulator=ARM1020T
; ARM1020T has no VFP, but nor does ARM10E.
;{VFP=No_FPU
;}
}


{ARM10TDMI=ARM1020T
Architecture=5T
{meta_moduleserver_processor=ARM10TDMI
revision=1
}
HASMMU=False
}


{ARM10200=ARM10E
Architecture=5T

; Tell the module server that we are an ARM1020T - it will add the VFP
; itself. If we instead claim to be an "ARM10200",
; then when the VFP advertises itself later, we get 2 VFPs displayed.
{meta_moduleserver_processor=ARM1020T
revision=1
}
CP15_IDREGVALUE=0x4104a200
;Allow Pagetab to work.
HASMMU=True
; ARM10200 has a VFPv1
; !TODO: Prevent addition or removal by $FPU !
;FPU=VFP
{VFP=VFP
meta_moduleserver_component=VFPv1
}
}


{ARM10200_NOSUP=ARM10E
Architecture=5T
; See above for why not "ARM10200".
{meta_moduleserver_processor=ARM1020T
revision=1
}
CP15_IDREGVALUE=0x4104a200
;Allow Pagetab to work.
HASMMU=True
; ARM10200 has a VFPv1
{VFP_NOSUP=VFP_NOSUP
}
}


  

;Architecture 5TE (Rev1) ARM10

{ARM1020E=ARM10E
;;;Processor=ARM1020E
{meta_moduleserver_processor=ARM1020E
revision=1
}
; 0x41 = ARM, 0x05 = arch 5E, 0xA20 = part number, 0x1 = revision.
CP15_IDREGVALUE=0x4105a201
;Allow Pagetab to work.
HASMMU=True
; ARM1020E has no VFP.
{VFP=No_FPU
}
}


{ARM1022E=ARM10E
;;;Processor=ARM1022E
meta_moduleserver_processor=ARM1020E
; 0x41 = ARM, 0x05 = arch 5E, 0xA20 = part number, 0x1 = revision.
CP15_IDREGVALUE=0x4105a201
;Allow Pagetab to work.
HASMMU=True
ICache_Lines=512
DCache_Lines=512
; ARM1020E has no VFP.
{VFP=No_FPU
}
}


{ARM10200E_NOSUP=ARM10E
;;;Processor=ARM10200
; See above for why not "ARM10200E".
{meta_moduleserver_processor=ARM1020E
revision=1
}
; 0x41 = ARM, 0x05 = arch 5E, 0xA20 = part number, 0x1 = revision.
CP15_IDREGVALUE=0x4105a201
;Allow Pagetab to work.
HASMMU=True
; ARM10200E has a VFPv2.
{VFP_NOSUP=VFPv2
}
}

;Since we have no "with support code" version of VFPv2,
; the above and below are currently identical :-(


{ARM10200E=ARM10E
Architecture=5TE
; See above for why not "ARM10200E".
{meta_moduleserver_processor=ARM1020E
revision=1
}
; 0x41 = ARM, 0x05 = arch 5E, 0xA20 = part number, 0x1 = revision.
;(Provisional ARM1020E datasheet said "Rev1 v5TE ID = 0x4115A200",
; with b[23:20] being "variant".).
CP15_IDREGVALUE=0x4105a201
;Allow Pagetab to work.
HASMMU=True
; ARM10200E has a VFPv2
{VFP=VFPv2
}
}



;; New processors/variants can be added here.
}





;; EOF armiss.dsc



