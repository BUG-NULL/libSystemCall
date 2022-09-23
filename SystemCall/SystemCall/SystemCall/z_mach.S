//
//  z_unistd.S
//  Example
//
//  Created by NULL
//

#define _COMM_PAGE64_BASE_ADDRESS       (0x00007fffffe00000)
#define _COMM_PAGE_CPU_CAPABILITIES64   (_COMM_PAGE64_BASE_ADDRESS + 0x010)

    .text
    .align 2, 0x90
    .globl _z_get_cpu_capabilities
_z_get_cpu_capabilities:
    movq    $(_COMM_PAGE_CPU_CAPABILITIES64), %rax
    movq    (%rax), %rax
    ret

