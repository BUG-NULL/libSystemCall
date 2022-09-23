//
//  z_uccontext.h
//  SystemCall
//
//  Created by NULL
//

#ifndef z_uccontext_h
#define z_uccontext_h

#include <SystemCall/z_types.h>
#include <SystemCall/z_signal.h>

#define FP_PREC_24B     0
#define FP_PREC_53B     2
#define FP_PREC_64B     3
#define FP_RND_NEAR     0
#define FP_RND_DOWN     1
#define FP_RND_UP       2
#define FP_CHOP         3

struct x86_exception_state64
{
    z_uint16    trapno;
    z_uint16    cpu;
    z_uint32    err;
    z_uint64    faultvaddr;
};

struct x86_thread_state64
{
    z_uint64    rax;
    z_uint64    rbx;
    z_uint64    rcx;
    z_uint64    rdx;
    z_uint64    rdi;
    z_uint64    rsi;
    z_uint64    rbp;
    z_uint64    rsp;
    z_uint64    r8;
    z_uint64    r9;
    z_uint64    r10;
    z_uint64    r11;
    z_uint64    r12;
    z_uint64    r13;
    z_uint64    r14;
    z_uint64    r15;
    z_uint64    rip;
    z_uint64    rflags;
    z_uint64    cs;
    z_uint64    fs;
    z_uint64    gs;
};

struct mmst_reg
{
    z_int8      mmst_reg[10];
    z_int8      mmst_rsrv[6];
};

struct xmm_reg
{
    z_int8      xmm_reg[16];
};

struct fp_control
{
    z_uint16    invalid     :1,
                denorm      :1,
                zdiv        :1,
                ovrfl       :1,
                undfl       :1,
                precis      :1,
                            :2,
                pc          :2,
                rc          :2,
                /*inf*/     :1,
                            :3;
};

struct fp_status
{
    z_uint16    invalid     :1,
                denorm      :1,
                zdiv        :1,
                ovrfl       :1,
                undfl       :1,
                precis      :1,
                stkflt      :1,
                errsumm     :1,
                c0          :1,
                c1          :1,
                c2          :1,
                tos         :3,
                c3          :1,
                busy        :1;
};

struct x86_float_state64
{
    z_int32             fpu_reserved[2];
    struct fp_control   fpu_fcw;       /* x87 FPU control word */
    struct fp_status    fpu_fsw;       /* x87 FPU status word */
    z_uint8             fpu_ftw;       /* x87 FPU tag word */
    z_uint8             fpu_rsrv1;      /* reserved */
    z_uint16            fpu_fop;       /* x87 FPU Opcode */

    /* x87 FPU Instruction Pointer */
    z_uint32            fpu_ip;         /* offset */
    z_uint16            fpu_cs;         /* Selector */
    z_uint16            fpu_rsrv2;      /* reserved */

    /* x87 FPU Instruction Operand(Data) Pointer */
    z_uint32            fpu_dp;         /* offset */
    z_uint16            fpu_ds;         /* Selector */

    z_uint16            fpu_rsrv3;      /* reserved */
    z_uint32            fpu_mxcsr;      /* MXCSR Register state */
    z_uint32            fpu_mxcsrmask;  /* MXCSR mask */
    struct mmst_reg     fpu_stmm0;      /* ST0/MM0   */
    struct mmst_reg     fpu_stmm1;      /* ST1/MM1  */
    struct mmst_reg     fpu_stmm2;      /* ST2/MM2  */
    struct mmst_reg     fpu_stmm3;      /* ST3/MM3  */
    struct mmst_reg     fpu_stmm4;      /* ST4/MM4  */
    struct mmst_reg     fpu_stmm5;      /* ST5/MM5  */
    struct mmst_reg     fpu_stmm6;      /* ST6/MM6  */
    struct mmst_reg     fpu_stmm7;      /* ST7/MM7  */
    struct xmm_reg      fpu_xmm0;       /* XMM 0  */
    struct xmm_reg      fpu_xmm1;       /* XMM 1  */
    struct xmm_reg      fpu_xmm2;       /* XMM 2  */
    struct xmm_reg      fpu_xmm3;       /* XMM 3  */
    struct xmm_reg      fpu_xmm4;       /* XMM 4  */
    struct xmm_reg      fpu_xmm5;       /* XMM 5  */
    struct xmm_reg      fpu_xmm6;       /* XMM 6  */
    struct xmm_reg      fpu_xmm7;       /* XMM 7  */
    struct xmm_reg      fpu_xmm8;       /* XMM 8  */
    struct xmm_reg      fpu_xmm9;       /* XMM 9  */
    struct xmm_reg      fpu_xmm10;      /* XMM 10  */
    struct xmm_reg      fpu_xmm11;      /* XMM 11 */
    struct xmm_reg      fpu_xmm12;      /* XMM 12  */
    struct xmm_reg      fpu_xmm13;      /* XMM 13  */
    struct xmm_reg      fpu_xmm14;      /* XMM 14  */
    struct xmm_reg      fpu_xmm15;      /* XMM 15  */
    z_int8              fpu_rsrv4[6 * 16];  /* reserved */
    z_int32             fpu_reserved1;
};

struct z_mcontext64
{
    struct x86_exception_state64    es;
    struct x86_thread_state64       ss;
    struct x86_float_state64        fs;
};

struct z_ucontext
{
    z_int32             uc_onstack;
    z_uint32            uc_sigmask;     /* signal mask used by this context */
    struct z_stack_t    uc_stack;       /* stack used by this context */
    struct z_ucontext   *uc_link;       /* pointer to resuming context */
    z_uint64            uc_mcsize;      /* size of the machine context passed in */
    struct z_mcontext64 *uc_mcontext;   /* pointer to machine specific context */
    struct z_mcontext64 mcontext_data;
};

z_int32 z_getcontext(struct z_ucontext *ucp);
z_int32 z_setcontext(struct z_ucontext *ucp);
z_int32 z_swapcontext(struct z_ucontext *oucp, struct z_ucontext *ucp);
z_void z_makecontext(struct z_ucontext *ucp, z_void (*start)(z_void), z_int32 argc, ...);

#endif /* z_uccontext_h */
