//
//  z_uccontext.c
//  SystemCall
//
//  Created by NULL
//

#include "z_uccontext.h"
#include "z_signal.h"
#include "z_resource.h"
#include "z_error.h"
#include <stdarg.h>
#include "z_unistd.h"
#include "z_stdlib.h"

#define UCF_SWAPPED     0x80000000
#define ARG_MAX         (1024 * 1024)
#define NCARGS          ARG_MAX         /* max bytes for an exec function */

extern void z_ctx_start(struct z_ucontext *, z_int32 argc, ...);
extern z_int32 zz_setcontext(z_void *);

struct z_mcontext64 *z_getmcontext(struct z_ucontext *uctx, void *sp)
{
    struct z_mcontext64 *mctx = (struct z_mcontext64 *)&(uctx->mcontext_data);
    z_uint64 stacksize = 0;
    struct z_stack_t stack;

    uctx->uc_stack.ss_sp = sp;
    uctx->uc_stack.ss_flags = 0;

    if (0 == z_sigaltstack(Z_NULL, &stack))
    {
        if (stack.ss_flags & SS_ONSTACK)
        {
            uctx->uc_stack = stack;
            stacksize = stack.ss_size;
        }
    }

    if (stacksize == 0)
    {
        struct z_rlimit rlim;
        if (0 == z_getrlimit(RLIMIT_STACK, &rlim))
        {
            stacksize = rlim.rlim_cur;
        }
    }

    uctx->uc_stack.ss_size = stacksize;
    if (uctx->uc_mcontext != mctx)
    {
        uctx->uc_mcontext = mctx;
        uctx->uc_link = 0;
    }

    z_sigprocmask(0, Z_NULL, &uctx->uc_sigmask);
    return mctx;
}

z_void z_makecontext(struct z_ucontext *ucp, z_void (*start)(z_void), z_int32 argc, ...)
{
    va_list     ap;
    z_int8      *stack_top;
    z_int64     *argp;
    z_int32     i;

    if (ucp == Z_NULL)
    {
        return;
    }
    else if ((ucp->uc_stack.ss_sp == Z_NULL) || (ucp->uc_stack.ss_size < MINSIGSTKSZ))
    {
        /*
         * This should really return -1 with errno set to ENOMEM
         * or something, but the spec says that makecontext is
         * a void function.   At least make sure that the context
         * isn't valid so it can't be used without an error.
         */
        ucp->uc_mcsize = 0;
    }
    /* XXX - Do we want to sanity check argc? */
    else if ((argc < 0) || (argc > NCARGS))
    {
        ucp->uc_mcsize = 0;
    }
    /* Make sure the context is valid. */
    else
    {
        /*
         * Arrange the stack as follows:
         *
         *    _ctx_start()    - context start wrapper
         *    start()        - user start routine
         *     arg1            - first argument, aligned(16)
         *    ...
         *    argn
         *    ucp        - this context, %rbp/%ebp points here
         *
         * When the context is started, control will return to
         * the context start wrapper which will pop the user
         * start routine from the top of the stack.  After that,
         * the top of the stack will be setup with all arguments
         * necessary for calling the start routine.  When the
         * start routine returns, the context wrapper then sets
         * the stack pointer to %rbp/%ebp which was setup to point to
         * the base of the stack (and where ucp is stored).  It
         * will then call _ctx_done() to swap in the next context
         * (uc_link != 0) or exit the program (uc_link == 0).
         */
        struct z_mcontext64 *mc;

        stack_top = (z_int8 *)(ucp->uc_stack.ss_sp + ucp->uc_stack.ss_size - sizeof(z_int64));
        z_int32 minargc = argc;
        
        /* Give 6 stack slots to _ctx_start */
        if (minargc < 6)
        {
            minargc = 6;
        }

        /*
         * Adjust top of stack to allow for 3 pointers (return
         * address, _ctx_start, and ucp) and argc arguments.
         * We allow the arguments to be pointers also.  The first
         * argument to the user function must be properly aligned.
         */

        stack_top = stack_top - (sizeof(z_int64) * (1 + minargc));
        stack_top = (z_int8 *)((z_int64)stack_top & ~15);
        stack_top = stack_top - (2 * sizeof(z_int64));
        argp = (z_int64 *)stack_top;

        /*
         * Setup the top of the stack with the user start routine
         * followed by all of its aguments and the pointer to the
         * ucontext.  We need to leave a spare spot at the top of
         * the stack because setcontext will move rip/eip to the top
         * of the stack before returning.
         */
        *argp = (z_int64)z_ctx_start;  /* overwritten with same value */
        argp++;
        *argp = (z_int64)start;
        argp++;

        /* Add all the arguments: */
        va_start(ap, argc);
        for (i = 0; i < argc; i++)
        {
            *argp = va_arg(ap, z_int64);
            argp++;
        }
        va_end(ap);

        /* Always provide space for ctx_start to pop the parameter registers */
        for (;argc < minargc; argc++)
        {
            *argp++ = 0;
        }

        /* Keep stack aligned */
        if (argc & 1)
        {
            *argp++ = 0;
        }

        /* The ucontext is placed at the bottom of the stack. */
        *argp = (z_int64)ucp;

        /*
         * Set the machine context to point to the top of the
         * stack and the program counter to the context start
         * wrapper.  Note that setcontext() pushes the return
         * address onto the top of the stack, so allow for this
         * by adjusting the stack downward 1 slot.  Also set
         * %r12/%esi to point to the base of the stack where ucp
         * is stored.
         */
        mc = ucp->uc_mcontext;
        /* Use callee-save and match _ctx_start implementation */
        mc->ss.r12 = (z_int64)argp;
        mc->ss.rbp = 0;
        mc->ss.rsp = (z_int64)stack_top + sizeof(z_int8 *);
        mc->ss.rip = (z_int64)z_ctx_start;
    }
}

z_int32 z_setcontext(struct z_ucontext *uctx)
{
    struct z_mcontext64 *mctx = (struct z_mcontext64 *)&(uctx->mcontext_data);
    struct z_ucontext *_uctx = (struct z_ucontext *)uctx;
    if (mctx != _uctx->uc_mcontext)
    {
        _uctx->uc_mcontext = mctx;
    }
    
    z_sigprocmask(SIG_SETMASK, &uctx->uc_sigmask, Z_NULL);
    return zz_setcontext(mctx);
}

z_void z_ctx_done(struct z_ucontext *ucp)
{
    if (ucp->uc_link == Z_NULL)
    {
        z_exit(0);
    }
    else
    {
        /*
         * Since this context has finished, don't allow it
         * to be restarted without being reinitialized (via
         * setcontext or swapcontext).
         */
        ucp->uc_mcsize = 0;

        /* Set context to next one in link */
        /* XXX - what to do for error, abort? */
        z_setcontext((struct z_ucontext *)ucp->uc_link);
        __builtin_trap();    /* should never get here */
    }
}

z_int32 z_swapcontext(struct z_ucontext *oucp, struct z_ucontext *ucp)
{
    z_int32 ret = 0;

    if ((oucp == Z_NULL) || (ucp == Z_NULL))
    {
        z_errno = EINVAL;
        return -1;
    }
    
    oucp->uc_onstack &= ~UCF_SWAPPED;
    ret = z_getcontext(oucp);
    if ((ret == 0) && !(oucp->uc_onstack & UCF_SWAPPED))
    {
        oucp->uc_onstack |= UCF_SWAPPED;
        ret = z_setcontext(ucp);
    }
    
    asm(""); // Prevent tailcall <rdar://problem/12581792>
    return ret;
}
