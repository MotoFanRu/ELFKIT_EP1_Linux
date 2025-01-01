;; ARMulator configuration file type 3
;; - peripherals.dsc -
;; Copyright (c) 1996-2001 ARM Limited. All Rights Reserved.

;; RCS $Revision: 1.1.2.3 $
;; Checkin $Date: 2002/03/21 16:50:38 $
;; Revising $Author: dsinclai $
 
;;
;; This is a non-user-edittable configuration file for ARMulator (ADS1.2)
;;

{ Peripherals


{VFPv2
MODEL_DLL_FILENAME=Vfpv2
META_GUI_INTERFACE=FPU
;;Using the VFPv1 description causes the module-server to hang :-(
meta_moduleserver_component=VFPv2
}

{VFP
MODEL_DLL_FILENAME=VFP
META_GUI_INTERFACE=FPU
meta_moduleserver_component=VFPv1
}
{VFP_NOSUP      
MODEL_DLL_FILENAME=VFP_nosup
META_GUI_INTERFACE=FPU
meta_moduleserver_component=VFPv1
}
{ No_FPU=Nothing
META_GUI_INTERFACE=FPU
}
{VFP_FASTMODE
MODEL_DLL_FILENAME=VFP
META_GUI_INTERFACE=FPU
vfpr2=TRUE
FPSID=0x410101a0
meta_moduleserver_component=VFPv1
}

}

; EOF vfp.dsc
