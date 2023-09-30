/*
    Copyright (c) 2008 theCor3 & flash.tato

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef FUNL_H
#define FUNL_H

#include <apps.h>
#include <uis.h>
#include <dl.h>
#include <loader.h>

typedef struct
{ 
    APPLICATION_T           apt; 
} APP_FUNL_T;

typedef enum
{
    HW_STATE_ANY,
    HW_STATE_MAIN,
   
    HW_STATE_MAX
} HW_STATES_T;

UINT32 FunLStart( EVENT_STACK_T *ev_st,  REG_ID_T reg_id,  UINT32 param2 );
UINT32 FunLExit( EVENT_STACK_T *ev_st,  void *app );

UINT32 MainStateEnter( EVENT_STACK_T *ev_st,  void *app,  ENTER_STATE_TYPE_T type );

UINT32 HandleUITokenGranted( EVENT_STACK_T *ev_st,  void *app );

UINT32 CallReceived( EVENT_STACK_T *ev_st,  void *app );
UINT32 CallEnded( EVENT_STACK_T *ev_st,  void *app );

UINT32 FlipOpen( EVENT_STACK_T *ev_st,  void *app );
UINT32 FlipClose( EVENT_STACK_T *ev_st,  void *app );

UINT32 Timer( EVENT_STACK_T *ev_st,  void *app );

UINT32 HandleKeypress( EVENT_STACK_T * ev_st,  void * app );

INT32 LoadConfig( );

#endif
