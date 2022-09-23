//
//  z_mach_trap.c
//  SystemCall
//
//  Created by NULL
//

#include "z_mach_trap.h"
#include "z_syscall.h"
#include "z_error.h"

z_int32 z_mach_vm_allocate_trap(z_uint32 target, z_uint64 *addr, z_uint64 size, z_int32 flags)
{
    return (z_uint32)(z_syscall_mach(SYSCALL_MACH(SYS__kernelrpc_mach_vm_allocate_trap), target, addr, size, flags));
}

z_int32 z_mach_vm_deallocate_trap(z_uint32 target, z_uint64 *addr, z_uint64 size)
{
    return (z_uint32)(z_syscall_mach(SYSCALL_MACH(SYS__kernelrpc_mach_vm_deallocate_trap), target, addr, size));
}

z_int32 z_mach_vm_map_trap(z_uint32 target, z_uint64 *addr, z_uint64 size, z_uint64 mask, z_int32 flags, z_int32 protection)
{
    return (z_uint32)(z_syscall_mach(SYSCALL_MACH(SYS__kernelrpc_mach_vm_map_trap), target, addr, size, mask, flags, protection));
}

z_int32 z_mach_port_allocate_trap(z_uint32 target, z_uint32 right, z_uint32 *name)
{
    return (z_uint32)(z_syscall_mach(SYSCALL_MACH(SYS__kernelrpc_mach_port_allocate_trap), target, right, name));
}

z_int32 z_mach_port_deallocate_trap(z_uint32 target, z_uint32 name)
{
    return (z_uint32)(z_syscall_mach(SYSCALL_MACH(SYS__kernelrpc_mach_port_deallocate_trap), target, name));
}

z_uint32 z_task_self_trap(z_void)
{
    return (z_uint32)(z_syscall_mach(SYSCALL_MACH(SYS_task_self_trap)));
}

z_uint32 z_mach_msg(z_mach_msg_header_t *msg, z_int32 option, z_uint32 send_size, z_uint32 rcv_size, z_uint32 rcv_name, z_uint32 timeout, z_uint32 notify)
{
    return (z_uint32)(z_syscall_mach(SYSCALL_MACH(SYS_mach_msg_trap), msg, option, send_size, rcv_size, rcv_name, timeout, notify));
}

z_int32 z_task_for_pid(z_uint32 target, z_int32 pid, z_uint32 *dst_target)
{
    return (z_uint32)(z_syscall_mach(SYSCALL_MACH(SYS_task_for_pid), target, pid, dst_target));
}

z_int32 z_task_name_for_pid(z_uint32 target, z_int32 pid, z_uint32 *dst_target)
{
    return (z_uint32)(z_syscall_mach(SYSCALL_MACH(SYS_task_name_for_pid), target, pid, dst_target));
}

z_int32 z_pid_for_task(z_uint32 target, z_int32 *pid)
{
    return (z_uint32)(z_syscall_mach(SYSCALL_MACH(SYS_pid_for_task), target, pid));
}

z_int32 z_mach_reply_port(z_void)
{
    return (z_int32)(z_syscall_mach(SYSCALL_MACH(SYS_mach_reply_port)));
}

z_int32 z_pthread_self_trap(z_void)
{
    return (z_int32)(z_syscall_mach(SYSCALL_MACH(SYS_thread_self_trap)));
}

z_int32 z_swtch_pri(z_int32 pri)
{
    return (z_int32)(z_syscall_mach(SYSCALL_MACH(SYS_swtch_pri), pri));
}

z_int32 z_swtch(z_void)
{
    return (z_int32)(z_syscall_mach(SYSCALL_MACH(SYS_swtch)));
}

z_int32 z_thread_switch(z_uint32 thread_name, z_int32 option, z_uint32 option_time)
{
    return (z_int32)(z_syscall_mach(SYSCALL_MACH(SYS_thread_switch), thread_name, option, option_time));
}


