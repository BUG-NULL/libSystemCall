//
//  z_ptrace.h
//  SystemCall
//
//  Created by NULL
//

#ifndef z_ptrace_h
#define z_ptrace_h

#include <SystemCall/z_types.h>

#define PT_TRACE_ME     0       /* child declares it's being traced */
#define PT_READ_I       1       /* read word in child's I space */
#define PT_READ_D       2       /* read word in child's D space */
#define PT_READ_U       3       /* read word in child's user structure */
#define PT_WRITE_I      4       /* write word in child's I space */
#define PT_WRITE_D      5       /* write word in child's D space */
#define PT_WRITE_U      6       /* write word in child's user structure */
#define PT_CONTINUE     7       /* continue the child */
#define PT_KILL         8       /* kill the child process */
#define PT_STEP         9       /* single step the child */
#define PT_ATTACH       10      /* trace some running process */
#define PT_DETACH       11      /* stop tracing a process */
#define PT_SIGEXC       12      /* signals as exceptions for current_proc */
#define PT_THUPDATE     13      /* signal for thread# */
#define PT_ATTACHEXC    14      /* attach to running process with signal exception */
#define PT_FORCEQUOTA   30      /* Enforce quota for root */
#define PT_DENY_ATTACH  31
#define PT_FIRSTMACH    32      /* for machine-specific requests */

typedef union _z_instructions
{
    z_uint64    ins_long;
    z_uint8     ins_char[8];
} z_instructions;

z_int32 z_ptrace(z_int32 request, z_int32 pid, z_void *addr, z_int64 data);

#endif /* z_ptrace_h */
