
#include <typedefs.h>
#include <mem.h>
#include <utilities.h>
#include <stdargs.h>
#include <filesystem.h>

#include "defines.h"


typedef struct
{
	int cpsr;		// +0
	int r4;			// +4
	int r5;			// +8
	int r6;			// +c
	int r7;			// +10
	int r8;			// +14
	int r9;			// +18
	int r10;		// +1c
	int r11;		// +20
	int r12;		// +24
	void (*pc)(void);	// + 28		// su_INT_exit
	
} vmc_frame; // 0x2c = 44


typedef struct
{
	vmc_frame	regs;		// +0
	int			sig;		// + 2c		//0xFEEDFACE
	int			task_arg;	// +30
	int			unk[4];		// +34
	int 		spsr;		// +44
	void		(*task_entry)(void);	// +48
	void		(*task_return)(void);	// +4c	// vmc_userexit
	
} vmc_init_frame; // 0x50 = 80

/* task control block entry */
typedef struct
{
	unsigned char tstate;      /* task state */
	unsigned char tprio;       /* task priority */
	unsigned char prio_next;   /* Next in the priority list */
	unsigned char prio_prev;   /* Prev in the priority list */

	vmc_init_frame	*pregs;      /* processor registers */
	unsigned long timeout;     /* timeout if on the delta list */
	unsigned char time_next;   /* Next in the delta list */
	unsigned char time_prev;   /* Prev in the delta list */

	unsigned char workq_next;  /* Next task on the work queue    */
	char          pad;         /* Padding to align - for speedup */

	union
	{
		struct {
			unsigned long tfmask;      /* event flags mask */
			unsigned char tfevgroup;   /* event flags group */
			unsigned char tfopt;       /* option */
		} event; // 6
		struct {
			char          *message;    /* Message posted here when blocked */
			char          **mboxp;     /* mailbox address */
		} mbox; // 8
		struct {
			char          *message;    /* Message posted here when blocked */
			unsigned char qid;         /* qid of the queue */
		} queue; // 5
	} object; // 8
	
} tentry; // 4+4+4+4+8 = 24



/*
sizeof(frame) = 0x2C
f		0xD3 (cpsr)
f+0x4	R4
f+0x8	R5
f+0xC	R6
f+0x10	R7
f+0x14	R8
f+0x18	R9
f+0x1C	R10
f+0x20	R11
f+0x24	R12
f+0x28  su_INT_exit (pc)
sizeof(startup) = 0x24
f+0x2C s+0	0xfeedface
f+0x30 s+0x4	arg
f+0x34 s+0x8	?
f+0x38 s+0xC	?
f+0x3C s+0x10	?
f+0x40 s+0x14	?
f+0x44 s+0x18	spsr
f+0x48 s+0x1C	entry+4
f+0x4C s+0x20	vmc_userexit+1
*/


// кол-во стандартных тасков = sizeof(vmc_tasktab_glb)/24
//#define NTASKS 0x3A // 58

// дополнительное место от su_stacklocations_glb, su_stacksizes_glb, su_zombie_free_count_glb
//#define TBLEXT_SIZE 0x1DC // хватит на 19 тасков
// итого 0x4D тасков, это значение прописать в патче sc_tsuspend_fix.fpa

#define MAX_USR_TASKS min(32, TBLEXT_SIZE/sizeof(tentry))
//(TBLEXT_SIZE-sizeof(nusrtasks))/(sizeof(tentry)+sizeof(char*)))

extern	tentry vmc_tasktab_glb[];


//UINT32	bitmask;
UINT32	nusrtasks = 0;
char*	usrstacks[MAX_USR_TASKS] = {0}; // служит также индикатором занятости записи в таблице

typedef void (*TASK_ENTRY_FN)(void *);

//INT32	vmc_tcreate( TASK_ENTRY_FN entry, char* stk, int stksize, int priority, int arg, int state);
extern	void	vmc_taskinit(tentry *, char *, int, TASK_ENTRY_FN, int);
extern int vmc_ready_insert(int, UINT32);
extern UINT32 suGetSelfHandle(void);
extern void sc_tsuspend(int tid, int code, INT32 *err);
void sc_lock(void);
void sc_unlock(void);
UINT32	suDisableInt(void);
UINT32	suSetInt(UINT32 cpsr);

void UtilLogStringData(const char*  format, ...);
void suTaskExit( void );


#pragma arm section code="suCreateTask"
INT32 suCreateTask( TASK_ENTRY_FN entry, UINT32 stacksize, UINT8 priority )
{
	INT32		status = 0;
	char		*stack;
	UINT32		cpsr;
	UINT32		tid = 0;
	tentry		*tcbp;
	
	if ( nusrtasks >= MAX_USR_TASKS )
		return 1;
		
	// Создадим стек
	stack = (char*)suAllocMem( stacksize, &status );
	if ( stack == NULL )
		return status;
	
	while ( usrstacks[ tid ] != NULL )
		tid++;

	nusrtasks++;

	usrstacks[ tid ] = stack;	// Запоминаем стек, чтобы потом освободить
	
#ifdef DEBUG2
	UtilLogStringData("NEW %d\n", tid);
#endif
	
	sc_lock(); // lock scheduler
	cpsr = suDisableInt();
	
	tid += NTASKS;
	tcbp = &vmc_tasktab_glb[ tid ];
	tcbp->tprio = priority;
	//tcbp->tstate = 0x10; // TBSREADY
	
#ifdef DEBUG2
	UtilLogStringData("ALLOC %d\n", tid);
#endif

	vmc_taskinit( tcbp, stack, stacksize, entry, 0 );
	tcbp->pregs->task_return = suTaskExit;
	
	//tcbp->frame->return_pc = suDeleteTask;
	
#ifdef DEBUG2
	UtilLogStringData("I %d\n", tid);
#endif

	vmc_ready_insert( tid, cpsr );
	//entry( (void*)tid );

#ifdef DEBUG2
	UtilLogStringData("INS %d\n", tid);
#endif

	suSetInt( cpsr );
	sc_unlock(); // reschedule
	

	return 0;
}

#pragma arm section


#pragma arm section code="UtilLogStringData"

#ifdef DEBUG2
void UtilLogStringData(const char*  format, ...)
{  
    char      		buffer[255];
    va_list   		vars;
    
#ifdef LOG_FILE
    FILE_HANDLE_T	f;
    UINT32			written;
    UINT32			sz;
    UINT32			*reboot;
#endif

    va_start(vars, format);
    vsprintf(buffer, format, vars);
    va_end(vars);

#ifdef LOG_FILE
    sz = strlen(buffer);
    buffer[sz] = '\r';
    buffer[sz+1] = '\n';
    f = DL_FsOpenFile(L"file://b/elfpack.txt", DL_FS_WRITE_MODE, 0xE);
    if(f == DL_FS_HANDLE_INVALID)
    {
        reboot = (UINT32*)0x10092000;
        *reboot = 0;
    }
    

    DL_FsWriteFile((void*)buffer, sz+2, 1, f, &written);
    DL_FsCloseFile(f);

#else
    suLogData(0, 0x5151, 1, strlen(buffer)+1, buffer);
#endif

}

#endif

#pragma arm section

#pragma arm section code="suDeleteTask"
INT32	suDeleteTask( UINT8 tid )
{
	UINT32		cpsr;
	INT32		status;

	sc_lock();
	cpsr = suDisableInt();

	if ( tid == 0 )
		tid = suGetSelfHandle() & (~0x80000000);
		
#ifdef DEBUG2
	UtilLogStringData("SUSP %d\n", tid);
#endif
	
	// Надо пропатчить vmc_ntsk_glb: sc_tsuspend_fix.fpa
	sc_tsuspend( tid, 0, &status );
	
#ifdef DEBUG2
	UtilLogStringData("FREE %d\n", tid);
#endif
	
	// Освобождаем стек
	tid -= NTASKS;
	suFreeMem( usrstacks[ tid ] );
	
	// Теперь пометим таск как удалённый
	usrstacks[ tid ] = NULL;
	
	nusrtasks--;

	suSetInt( cpsr );
	sc_unlock();
	
	return 0; 
}

void suTaskExit( void )
{
	suDeleteTask( 0 );
}
#pragma arm section
