//
//  z_select.h
//  SystemCall
//
//  Created by NULL
//

#ifndef z_select_h
#define z_select_h

#include <SystemCall/z_types.h>
#include <SystemCall/z_time.h>

struct z_fd_set
{
    z_int32 fds_bits[1024];
};

z_void z_fd_set(z_int32 fd, struct z_fd_set *p);
z_int32 z_fd_isset(z_int32 _fd, struct z_fd_set *_p);
z_int32 z_select(z_int32 maxfdp, struct z_fd_set *readset, struct z_fd_set *writeset, struct z_fd_set *exceptset, struct z_timeval *timeout);

#endif /* z_select_h */
