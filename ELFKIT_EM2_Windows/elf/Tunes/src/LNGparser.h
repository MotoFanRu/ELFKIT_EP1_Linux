#define DEBUG

#ifndef LNGPARSER_H
#define LNGPARSER_H

#include <apps.h>
#include <filesystem.h>

typedef enum
{
    LNG_PLAYLISTS,
    LNG_PLAYLIST_NAME,
    LNG_AUTOCREATE_PLAYLISTS,
    LNG_FBROWSER,
    LNG_SKINS,
    LNG_SETTING,
    LNG_AUTONEXT,
    LNG_PLAY_AFTER_CALL,
    LNG_RANDOM,
    LNG_REPEAT,
    LNG_SHOW_NAME,
    LNG_SHOW_TIME,
    LNG_LOCK_KB,
    LNG_READ_TAGS,
    LNG_FADING_VOL,
    LNG_GROTH_VOL,
    LNG_QUEUE_SAVE,
    LNG_AFTER_REBOOT,
    LNG_ACTIVE_DISPLAY,

    LNG_ONE,
    LNG_CURRENT_TRACK,
    LNG_ADD_IN_QUEUE,
    LNG_CREATE_PLAYLIST,

    LNG_QUEUE,
    LNG_RANDOMIZE,

    LNG_MAX

} LNG_RES_ID_T;

typedef struct 
{
    WCHAR   *res;
    WCHAR   *str[LNG_MAX];  
} LNG_RES_T;

extern LNG_RES_T  Lang; 

UINT32 ReadLang(WCHAR *uri);
void CloseLang(void);

#endif

