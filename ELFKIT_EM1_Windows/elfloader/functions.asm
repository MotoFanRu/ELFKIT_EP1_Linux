	.equ APP_Register, 0x1040AFEA
	.export APP_Register
	.equ APP_InitAppData, 0x1040ABD0
	.export APP_InitAppData
	.equ APP_HandleEvent, 0x1040A7B4
	.export APP_HandleEvent
	.equ APP_HandleEventPrepost, 0x1040A9D0
	.export APP_HandleEventPrepost
	.equ APP_Start, 0x1040B084
	.export APP_Start
	.equ APP_Exit, 0x1040A54C
	.export APP_Exit
	.equ APP_UtilChangeState, 0x105D0076
	.export APP_UtilChangeState
	.equ APP_UtilHistSaveAndChangeState, 0x105D0410
	.export APP_UtilHistSaveAndChangeState
	.equ APP_UtilHistGoToPrevState, 0x105D0318
	.export APP_UtilHistGoToPrevState
	.equ AFW_InquireRoutingStackByRegId, 0x102F6668
	.export AFW_InquireRoutingStackByRegId
	.equ UIS_CreateColorCanvas, 0x108C8DC0
	.export UIS_CreateColorCanvas
	.equ UIS_CanvasDrawPixel, 0x108CC69C
	.export UIS_CanvasDrawPixel
	.equ UIS_CanvasDrawLine, 0x108CC80C
	.export UIS_CanvasDrawLine
	.equ UIS_CanvasFillRect, 0x108CCABC
	.export UIS_CanvasFillRect
	.equ UIS_CanvasDrawRect, 0x108CC978
	.export UIS_CanvasDrawRect
	.equ UIS_CanvasDrawRoundRect, 0x108CCF40
	.export UIS_CanvasDrawRoundRect
	.equ UIS_CanvasDrawArc, 0x108CCC78
	.export UIS_CanvasDrawArc
	.equ UIS_CanvasDrawPoly, 0x108CCDB0
	.export UIS_CanvasDrawPoly
	.equ UIS_CanvasDrawColorText, 0x108CBE84
	.export UIS_CanvasDrawColorText
	.equ UIS_CanvasSetFont, 0x108CB440
	.export UIS_CanvasSetFont
	.equ UIS_CanvasGetFontStyle, 0x108CFBCC
	.export UIS_CanvasGetFontStyle
	.equ UIS_CanvasSetFontStyle, 0x108CFAFC
	.export UIS_CanvasSetFontStyle
	.equ UIS_CanvasGetStringSize, 0x108C99C8
	.export UIS_CanvasGetStringSize
	.equ UIS_CanvasGetFontSize, 0x108C9698
	.export UIS_CanvasGetFontSize
	.equ UIS_CanvasDrawImage, 0x108CD87E
	.export UIS_CanvasDrawImage
	.equ UIS_CanvasDrawWallpaper, 0x108CE710
	.export UIS_CanvasDrawWallpaper
	.equ UIS_CanvasDrawColorBitmap, 0x108D1048
	.export UIS_CanvasDrawColorBitmap
	.equ UIS_CanvasOpenImage, 0x108CDB98
	.export UIS_CanvasOpenImage
	.equ UIS_CanvasOpenImageAndGetSize, 0x108CDC38
	.export UIS_CanvasOpenImageAndGetSize
	.equ UIS_CanvasCloseImage, 0x108CE4DC
	.export UIS_CanvasCloseImage
	.equ UIS_CanvasResizeImage, 0x108CDFFE
	.export UIS_CanvasResizeImage
	.equ UIS_CanvasGetImageSize, 0x108CE540
	.export UIS_CanvasGetImageSize
	.equ UIS_CanvasSetImageAttribute, 0x108CE5BC
	.export UIS_CanvasSetImageAttribute
	.equ UIS_CanvasGetImageAttribute, 0x108CE624
	.export UIS_CanvasGetImageAttribute
	.equ UIS_CanvasRenderImageToBuffer, 0x108CDD18
	.export UIS_CanvasRenderImageToBuffer
	.equ UIS_CanvasDecodeImageToPointer, 0x108CF6B0
	.export UIS_CanvasDecodeImageToPointer
	.equ UIS_CanvasInsert, 0x108CF730
	.export UIS_CanvasInsert
	.equ UIS_CanvasMoveRegion, 0x108D02C8
	.export UIS_CanvasMoveRegion
	.equ UIS_CanvasSetFillColor, 0x108CBB58
	.export UIS_CanvasSetFillColor
	.equ UIS_CanvasGetFillColor, 0x108CBB08
	.export UIS_CanvasGetFillColor
	.equ UIS_CanvasSetForegroundColor, 0x108CB9E6
	.export UIS_CanvasSetForegroundColor
	.equ UIS_CanvasGetForegroundColor, 0x108CB998
	.export UIS_CanvasGetForegroundColor
	.equ UIS_CanvasSetBackgroundColor, 0x108CBA7E
	.export UIS_CanvasSetBackgroundColor
	.equ UIS_CanvasGetBackgroundColor, 0x108CBA2E
	.export UIS_CanvasGetBackgroundColor
	.equ UIS_CanvasSetLineWidth, 0x108CBC50
	.export UIS_CanvasSetLineWidth
	.equ UIS_CanvasGetLineWidth, 0x108CBC24
	.export UIS_CanvasGetLineWidth
	.equ UIS_CanvasRefreshDisplayRegion, 0x108C9064
	.export UIS_CanvasRefreshDisplayRegion
	.equ UIS_CanvasRefreshDisplayBuffer, 0x108C9034
	.export UIS_CanvasRefreshDisplayBuffer
	.equ UIS_CanvasGetColorDepth, 0x108CB408
	.export UIS_CanvasGetColorDepth
	.equ UIS_CanvasGetDisplaySize, 0x108CB904
	.export UIS_CanvasGetDisplaySize
	.equ UIS_CanvasSetBackLight, 0x108CB956
	.export UIS_CanvasSetBackLight
	.equ DL_AudPlayTone, 0x10785B22
	.export DL_AudPlayTone
	.equ PFprintf, 0x1079F912
	.export PFprintf
	.equ DL_DbFeatureGetCurrentState, 0x1071DFD8
	.export DL_DbFeatureGetCurrentState
	.equ DL_DbFeatureStoreState, 0x1071E058
	.export DL_DbFeatureStoreState
	.equ DL_DbFeatureGetValueString, 0x1071E024
	.export DL_DbFeatureGetValueString
	.equ SEEM_ELEMENT_DATA_read, 0x101D1958
	.export SEEM_ELEMENT_DATA_read
	.equ SEEM_ELEMENT_DATA_write, 0x101D1B84
	.export SEEM_ELEMENT_DATA_write
	.equ SEEM_FDI_OUTSIDE_SEEM_ACCESS_read, 0x101D3030
	.export SEEM_FDI_OUTSIDE_SEEM_ACCESS_read
	.equ SEEM_FDI_OUTSIDE_SEEM_ACCESS_write, 0x101D2D22
	.export SEEM_FDI_OUTSIDE_SEEM_ACCESS_write
	.equ DL_AudSetVolumeSetting, 0x10784EC4
	.export DL_AudSetVolumeSetting
	.equ DL_AudGetVolumeSetting, 0x10784FA0
	.export DL_AudGetVolumeSetting
	.equ UIS_SetBacklightWithIntensity, 0x108C5D28
	.export UIS_SetBacklightWithIntensity
	.equ DL_KeyUpdateKeypadBacklight, 0x10730FCA
	.export DL_KeyUpdateKeypadBacklight
	.equ DAL_DisableDisplay, 0x108246FA
	.export DAL_DisableDisplay
	.equ DAL_EnableDisplay, 0x10824748
	.export DAL_EnableDisplay
	.equ pu_main_powerdown, 0x101AA68C
	.export pu_main_powerdown
	.equ DL_SigRegQuerySignalStrength, 0x1073EDB2
	.export DL_SigRegQuerySignalStrength
	.equ DL_PwrGetActiveBatteryPercent, 0x1070EDB2
	.export DL_PwrGetActiveBatteryPercent
	.equ DL_PwrGetChargingMode, 0x1070ECF0
	.export DL_PwrGetChargingMode
	.equ DL_SigCallGetCallStates, 0x1073E3BC
	.export DL_SigCallGetCallStates
	.equ APP_MMC_Util_IsVoiceCall, 0x1049B70C
	.export APP_MMC_Util_IsVoiceCall
	.equ APP_MMC_Util_IsVideoCall, 0x1049B760
	.export APP_MMC_Util_IsVideoCall
	.equ DL_SigMsgSendTextMsgReq, 0x1073E914
	.export DL_SigMsgSendTextMsgReq
	.equ DL_KeyInjectKeyPress, 0x10730F66
	.export DL_KeyInjectKeyPress
	.equ DL_DbShortcutCreateRecord, 0x10726BE4
	.export DL_DbShortcutCreateRecord
	.equ DL_DbShortcutUpdateRecord, 0x10726B4A
	.export DL_DbShortcutUpdateRecord
	.equ DL_DbShortcutDeleteRecord, 0x10726B22
	.export DL_DbShortcutDeleteRecord
	.equ DL_DbShortcutGetRecordByRecordId, 0x10726ACC
	.export DL_DbShortcutGetRecordByRecordId
	.equ DL_DbShortcutGetNumOfRecordsAvailable, 0x10726AF4
	.export DL_DbShortcutGetNumOfRecordsAvailable
	.equ DL_DbShortcutGetNumOfRecordsUsed, 0x10726AF8
	.export DL_DbShortcutGetNumOfRecordsUsed
	.equ DL_DbGetFirstAvailableNumber, 0x10726B1E
	.export DL_DbGetFirstAvailableNumber
	.equ DL_DbShortcutGetNumOfURLRecordsUsed, 0x10726C70
	.export DL_DbShortcutGetNumOfURLRecordsUsed
	.equ DL_DbShortcutGetURLByURLId, 0x10726C74
	.export DL_DbShortcutGetURLByURLId
	.equ DL_DbShortcutGetshortcutType, 0x10726CCA
	.export DL_DbShortcutGetshortcutType
	.equ DL_DbShortcutGetNumOfURLRecordsAvailable, 0x10726CD2
	.export DL_DbShortcutGetNumOfURLRecordsAvailable
	.equ DL_SimMgrGetPinStatus, 0x1073FFC4
	.export DL_SimMgrGetPinStatus
	.equ DL_SigRegGetCellID, 0x1073EDBA
	.export DL_SigRegGetCellID
	.equ SetFlashLight, 0x101A6BB6
	.export SetFlashLight
	.equ AFW_GetEv, 0x102F5E7C
	.export AFW_GetEv
	.equ AFW_GetEvSeqn, 0x102F5EA8
	.export AFW_GetEvSeqn
	.equ AFW_AddEvNoD, 0x102F5592
	.export AFW_AddEvNoD
	.equ AFW_AddEvEvD, 0x102F55C4
	.export AFW_AddEvEvD
	.equ AFW_AddEvAux, 0x102F55F6
	.export AFW_AddEvAux
	.equ AFW_AddEvAuxD, 0x102F5624
	.export AFW_AddEvAuxD
	.equ AFW_CreateInternalQueuedEvAux, 0x102F5AC6
	.export AFW_CreateInternalQueuedEvAux
	.equ APP_ConsumeEv, 0x1040A4E8
	.export APP_ConsumeEv
	.equ APP_UtilAddEvUISListData, 0x105CFFBA
	.export APP_UtilAddEvUISListData
	.equ APP_UtilAddEvUISListChange, 0x105CFF40
	.export APP_UtilAddEvUISListChange
	.equ APP_UtilAddEvUISGetDataForItem, 0x105CFF1C
	.export APP_UtilAddEvUISGetDataForItem
	.equ DL_FsOpenFile, 0x10770800
	.export DL_FsOpenFile
	.equ DL_FsCloseFile, 0x107708EA
	.export DL_FsCloseFile
	.equ DL_FsReadFile, 0x10770A70
	.export DL_FsReadFile
	.equ DL_FsWriteFile, 0x10770BB2
	.export DL_FsWriteFile
	.equ DL_FsFSeekFile, 0x10770C78
	.export DL_FsFSeekFile
	.equ DL_FsGetFileSize, 0x10770FC2
	.export DL_FsGetFileSize
	.equ DL_FsSGetFileSize, 0x10771084
	.export DL_FsSGetFileSize
	.equ DL_FsDirExist, 0x10773A08
	.export DL_FsDirExist
	.equ DL_FsFFileExist, 0x10770EDC
	.export DL_FsFFileExist
	.equ DL_FsFGetPosition, 0x10771194
	.export DL_FsFGetPosition
	.equ DL_FsSRenameFile, 0x10771EF0
	.export DL_FsSRenameFile
	.equ DL_FsRenameFile, 0x10771E32
	.export DL_FsRenameFile
	.equ DL_FsDeleteFile, 0x10770DA4
	.export DL_FsDeleteFile
	.equ DL_FsFMoveFile, 0x10772202
	.export DL_FsFMoveFile
	.equ DL_FsMkDir, 0x107737BC
	.export DL_FsMkDir
	.equ DL_FsRmDir, 0x10773872
	.export DL_FsRmDir
	.equ DL_FsFSetAttr, 0x10771D1C
	.export DL_FsFSetAttr
	.equ DL_FsGetAttr, 0x107718DA
	.export DL_FsGetAttr
	.equ DL_FsSGetAttr, 0x1077197C
	.export DL_FsSGetAttr
	.equ DL_FsSSearch, 0x10772410
	.export DL_FsSSearch
	.equ DL_FsSearchResults, 0x1077272C
	.export DL_FsSearchResults
	.equ DL_FsSearchClose, 0x107728AE
	.export DL_FsSearchClose
	.equ DL_FsFlush, 0x10773BBE
	.export DL_FsFlush
	.equ DL_FsVolumeEnum, 0x10772C50
	.export DL_FsVolumeEnum
	.equ suAllocMem, 0x10040310
	.export suAllocMem
	.equ suFreeMem, 0x10040338
	.export suFreeMem
	.equ DRM_CreateResource, 0x102F9BD4
	.export DRM_CreateResource
	.equ DRM_GetResource, 0x102F950C
	.export DRM_GetResource
	.equ DRM_GetResourceLength, 0x102F96EC
	.export DRM_GetResourceLength
	.equ DRM_SetResource, 0x102F9AEC
	.export DRM_SetResource
	.equ DRM_ClearResource, 0x102F9C7A
	.export DRM_ClearResource
	.equ DRM_GetCurrentLanguage, 0x102F98A4
	.export DRM_GetCurrentLanguage
	.equ DL_ClkGetDate, 0x10711850
	.export DL_ClkGetDate
	.equ DL_ClkGetTime, 0x10711898
	.export DL_ClkGetTime
	.equ suPalReadTime, 0x1028B0E4
	.export suPalReadTime
	.equ suPalTicksToMsec, 0x1028B194
	.export suPalTicksToMsec
	.equ APP_UtilStartTimer, 0x105D0D20
	.export APP_UtilStartTimer
	.equ APP_UtilStartCyclicalTimer, 0x105D0D56
	.export APP_UtilStartCyclicalTimer
	.equ APP_UtilStopTimer, 0x105D0DA0
	.export APP_UtilStopTimer
	.equ UIS_MakeContentFromString, 0x108C55BA
	.export UIS_MakeContentFromString
	.equ APP_HandleUITokenGranted, 0x1040AAA8
	.export APP_HandleUITokenGranted
	.equ APP_HandleUITokenRevoked, 0x1040AB18
	.export APP_HandleUITokenRevoked
	.equ APP_UtilConsumeEvChangeState, 0x105D00C6
	.export APP_UtilConsumeEvChangeState
	.equ UIS_HandleEvent, 0x108C4FA2
	.export UIS_HandleEvent
	.equ UIS_Refresh, 0x108C5B0C
	.export UIS_Refresh
	.equ UIS_ForceRefresh, 0x108C5B4A
	.export UIS_ForceRefresh
	.equ APP_UtilUISDialogDelete, 0x105D164E
	.export APP_UtilUISDialogDelete
	.equ UIS_Delete, 0x108C4B9E
	.export UIS_Delete
	.equ UIS_SetActionList, 0x108C5B7C
	.export UIS_SetActionList
	.equ UIS_SetCenterSelectAction, 0x108C5C00
	.export UIS_SetCenterSelectAction
	.equ UIS_GetActiveDialogType, 0x108C4C08
	.export UIS_GetActiveDialogType
	.equ UIS_CreateTransientNotice, 0x108C4846
	.export UIS_CreateTransientNotice
	.equ UIS_CreateConfirmation, 0x108C2E18
	.export UIS_CreateConfirmation
	.equ UIS_CreateList, 0x108C332C
	.export UIS_CreateList
	.equ UIS_CreateStaticList, 0x108C3516
	.export UIS_CreateStaticList
	.equ UIS_CreateCharacterEditor, 0x108C2D1E
	.export UIS_CreateCharacterEditor
	.equ UIS_CreateViewer, 0x108C48C8
	.export UIS_CreateViewer
	.equ UIS_CreatePictureAndTextList, 0x108C6EAC
	.export UIS_CreatePictureAndTextList
	.equ UIS_CreateSplashScreenFs, 0x108C4052
	.export UIS_CreateSplashScreenFs
	.equ UIS_CreateSplashScreen, 0x108C3FF0
	.export UIS_CreateSplashScreen
	.equ UIS_CreateProgressBar, 0x108C3ADC
	.export UIS_CreateProgressBar
	.equ UIS_SetStatus, 0x108C5EF6
	.export UIS_SetStatus
	.equ UIS_GetCurrentStatusValue, 0x108C5FD0
	.export UIS_GetCurrentStatusValue
	.equ UIS_SetWallpaper, 0x108C6232
	.export UIS_SetWallpaper
	.equ uisAllocateMemory, 0x1086D4D8
	.export uisAllocateMemory
	.equ uisFreeMemory, 0x1086DA6C
	.export uisFreeMemory
	.equ UIS_SetBacklight, 0x108C5CD6
	.export UIS_SetBacklight
	.equ UIS_SetLanguage, 0x108C5DAC
	.export UIS_SetLanguage
	.equ u_atou, 0x108D5EB0
	.export u_atou
	.equ u_strcpy, 0x108D4C6A
	.export u_strcpy
	.equ u_strcat, 0x108D4B80
	.export u_strcat
	.equ u_strlen, 0x108D4CB6
	.export u_strlen
	.equ u_strcmp, 0x108D4C04
	.export u_strcmp
	.equ u_strmakeupper, 0x108D5A74
	.export u_strmakeupper
	.equ strncpy, 0x10284B0C
	.export strncpy
	.equ strcat, 0x10284944
	.export strcat
	.equ strcmp, 0x10284984
	.export strcmp
	.equ strncmp, 0x10284ADC
	.export strncmp
	.equ sprintf, 0x10283154
	.export sprintf

