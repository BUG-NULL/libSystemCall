//
//  z_mach_trap.h
//  SystemCall
//
//  Created by NULL
//

#ifndef z_mach_trap_h
#define z_mach_trap_h

#include <SystemCall/z_types.h>

typedef struct
{
    z_uint32    msgh_bits;
    z_uint32    msgh_size;
    z_uint32    msgh_remote_port;
    z_uint32    msgh_local_port;
    z_uint32    msgh_voucher_port;
    z_int32     msgh_id;
} z_mach_msg_header_t;

z_int32 z_mach_vm_allocate_trap(z_uint32 target, z_uint64 *addr, z_uint64 size, z_int32 flags);
z_int32 z_mach_vm_deallocate_trap(z_uint32 target, z_uint64 *addr, z_uint64 size);
z_int32 z_mach_vm_map_trap(z_uint32 target, z_uint64 *addr, z_uint64 size, z_uint64 mask, z_int32 flags, z_int32 protection);
z_int32 z_mach_port_allocate_trap(z_uint32 target, z_uint32 right, z_uint32 *name);
z_int32 z_mach_port_deallocate_trap(z_uint32 target, z_uint32 name);
z_uint32 z_task_self_trap(z_void);
z_uint32 z_mach_msg(z_mach_msg_header_t *msg, z_int32 option, z_uint32 send_size, z_uint32 rcv_size, z_uint32 rcv_name, z_uint32 timeout, z_uint32 notify);
z_int32 z_task_for_pid(z_uint32 target, z_int32 pid, z_uint32 *dst_target);
z_int32 z_task_name_for_pid(z_uint32 target, z_int32 pid, z_uint32 *dst_target);
z_int32 z_pid_for_task(z_uint32 target, z_int32 *pid);
z_int32 z_mach_reply_port(z_void);
z_int32 z_pthread_self_trap(z_void);
z_int32 z_swtch_pri(z_int32 pri);
z_int32 z_swtch(z_void);
z_int32 z_thread_switch(z_uint32 thread_name, z_int32 option, z_uint32 option_time);

#endif /* z_mach_trap_h */
