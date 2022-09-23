
#define MCONTEXT_SS_RAX     16
#define MCONTEXT_SS_RBX     24
#define MCONTEXT_SS_RCX     32
#define MCONTEXT_SS_RDX     40
#define MCONTEXT_SS_RDI     48
#define MCONTEXT_SS_RSI     56
#define MCONTEXT_SS_RBP     64
#define MCONTEXT_SS_RSP     72
#define MCONTEXT_SS_R8      80
#define MCONTEXT_SS_RIP     144
#define MCONTEXT_SS_RFLAGS  152

    .text
    .globl    _z_getcontext
_z_getcontext:
    /* struct ucontext_t * $rdi */
    push  %rbp
    movq  %rsp, %rbp
    movq  %rsp, %rsi
    call  _z_getmcontext  /* z_getmcontext(uctx, sp) */
    pop   %rbp

    movq  %rbp, MCONTEXT_SS_RBP(%rax)
    movq  %rbx, MCONTEXT_SS_RBX(%rax)
    movq  %r12, MCONTEXT_SS_R8 + 32(%rax)
    movq  %r13, MCONTEXT_SS_R8 + 40(%rax)
    movq  %r14, MCONTEXT_SS_R8 + 48(%rax)
    movq  %r15, MCONTEXT_SS_R8 + 56(%rax)
    movq  (%rsp), %rcx        /* return address */
    movq  %rcx, MCONTEXT_SS_RIP(%rax)
    leaq  8(%rsp), %rcx
    movq  %rcx, MCONTEXT_SS_RSP(%rax)
    xorl  %eax, %eax
    ret

    .text
    .globl    _zz_setcontext
_zz_setcontext:
    /* struct mcontext_t * %rdi */
    movq  MCONTEXT_SS_RBX(%rdi),   %rbx
    movq  MCONTEXT_SS_R8 + 32(%rdi), %r12
    movq  MCONTEXT_SS_R8 + 40(%rdi), %r13
    movq  MCONTEXT_SS_R8 + 48(%rdi), %r14
    movq  MCONTEXT_SS_R8 + 56(%rdi), %r15

    movq  MCONTEXT_SS_RSP(%rdi), %rsp
    movq  MCONTEXT_SS_RBP(%rdi), %rbp

    xorl  %eax, %eax     /* force x=getcontext(); ... setcontext(); to keep x==0 */
    jmp  *MCONTEXT_SS_RIP(%rdi)

    .text
    .globl    _z_ctx_start
_z_ctx_start:
    popq    %rax        /* accounted for in makecontext() */
    /* makecontext will simulate 6 parameters at least */
    /* Or it could just set these in the mcontext... */
    popq    %rdi
    popq    %rsi
    popq    %rdx
    popq    %rcx
    popq    %r8
    popq    %r9

    callq   *%rax        /* call start function */
    movq    %r12, %rsp  /* setup stack for completion routine ucp is now at top of stack*/
    movq    (%rsp), %rdi
    call    _z_ctx_done    /* should never return */
    int $5                /* trap */

