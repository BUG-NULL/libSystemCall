//
//  z_thread.s
//  SystemCall
//
//  Created by NULL
//

#define _PTHREAD_STRUCT_DIRECT_STACKADDR_OFFSET   -48
#define _PTHREAD_STRUCT_DIRECT_STACKBOTTOM_OFFSET -40

    .align 2, 0x90
    .globl _z_thread_set_tsd_base
_z_thread_set_tsd_base:
    movl    $0, %esi            // 0 as the second argument
    movl    $0x3000003, %eax    // Machine-dependent syscall number 3
    syscall
    ret

    .align 2, 0x90
    .globl _z_start_wqthread
_z_start_wqthread:
    // This routine is never called directly by user code, jumped from kernel
    // Push a sentinel frame, so backtracers know when to stop.
    push   $0
    push   %rbp
    mov    %rsp, %rbp
    sub    $16, %rsp      // align the stack
    // call   __pthread_wqthread
    ud2 // never returns

    .align 2, 0x90
    .globl _z_thread_start
_z_thread_start:
    // This routine is never called directly by user code, jumped from kernel
    // Push a sentinel frame, so backtracers know when to stop.
    push   $0
    push   %rbp
    mov    %rsp, %rbp
    sub    $16, %rsp        // align the stack
    call   _z_pthread_start
    leave
    ret

