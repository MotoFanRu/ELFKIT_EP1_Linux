;; ARMulator configuration file type 3
;; - XScale.dsc -
;; Copyright (c) 1996-2002 ARM Limited. All Rights Reserved.

;; RCS $Revision: 1.4.2.4 $
;; Checkin $Date: 2002/04/26 14:05:21 $
;; Revising $Author: hbullman $
 
;;
;; This is a non-user-edittable configuration file for ARMulator
;;

;;
;; This file has been updated by the Multi-ICE 2.2 installer
;;

;;
;; This is the list of XScale processors supported by ARMISS.
;;

{ Processors

{XScale=Processors_Common_ARMISS
; The 'meta_sordi_dll' tag in ADS 1.1 renamed to 'MODEL_DLL_FILENAME'
; for ADS 1.2.
meta_sordi_dll=ARMISS
MODEL_DLL_FILENAME=ARMISS
ARMulator=XSCALE
Architecture=5TE
CP15_IDREGVALUE=0x69052000
LSU_Width=32
PFU_Width=32
BUS_Width=64

; This tells the main memory model to ignore CP15.
;ENDIANNESS=L
; L means main memory is fixed LittleEnd (R+W)
; C means listen to BIGEND from CP15.
ENDIANNESS=C

{RDIMSVR=RDIMSVR
target_controller_type=armulate
}

{ meta_moduleserver_processor=XScale-80200
XScale_DSP=true
XScale_InterruptCtrl=true
XScale_Performance=true
XScale_SoftwareDebug=false
XScale_ClockAndPower=false
XScale_SystemControl=true
}

;Tell Pagetab how to treat XScale.
HASMMU=True


;End XScale
}

;End Processors
}

;; EOF XScale.dsc








