/*
 64-bit Arguments are passed through the rdi, rsi, rdx, rcx, r8, r9 and rsp, respectively.
 The write function takes three arguments, which are described in the following assembly:
 You need to add 0x2000000 to the call number using a syscalls.master file.
 
 mov rax, 0x2000004     ; sys_write call identifier
 mov rdi, 1             ; STDOUT file descriptor
 mov rsi, myMessage     ; buffer to print
 mov rdx, myMessageLen  ; length of buffer
 syscall                ; make the system call
 */

    .text
	.globl	_z_syscall_mach
_z_syscall_mach:
    movq    %rdi, %rax      /* Syscall number -> rax.  */
    movq    %rsi, %rdi      /* shift arg1 - arg5.  */
    movq    %rdx, %rsi
    movq    %rcx, %rdx
    movq    %r8, %rcx
    movq    %r9, %r8
    movq    8(%rsp), %r9    /* arg6 is on the stack.  */
    movq    %rcx, %r10
    syscall                 /* Do the system call.  */
	retq

    .text
    .globl  _z_syscall_unix
_z_syscall_unix:
    movl    $0x2000000, %eax
    movq    %rcx, %r10
    syscall                 /* Do the system call.  */
/*
 The JNB instruction (jump when not below) is also used to jump,
 but the JNB instruction only jumps when the CF flag bit is 0
 The JAE instruction (jump when above or equal) is exactly the same as the JNB
 */
    jnb     2f
    movq    %rax, %rdi
    jmp     _z_cerror
2:
    retq





