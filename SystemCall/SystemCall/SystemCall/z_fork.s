//
//  z_fork.s
//  Example
//
//  Created by NULL
//

    .globl  _zz_fork
    .align  2, 0x90
_zz_fork:
    subq    $24, %rsp           // Align the stack, plus room for local storage
    movq    $0x2000002, %rax;   // code for fork -> rax
    syscall                     // do the system call
    jnc     L1                  // jump if CF == 0

    movq    %rax, %rdi
    call    _z_cerror
    movq    $-1, %rax
    addq    $24, %rsp           // restore the stack
    ret
    
L1:
    orl     %edx, %edx      // CF = OF = 0,  ZF set if zero result
    jz      L2              // parent, since r1 == 0 in parent, 1 in child

    // child here...
    xorq    %rax, %rax
    // movl    %eax,(%r11)
L2:
    // parent ends up here skipping child portion
    addq    $24, %rsp       // restore the stack
    ret







