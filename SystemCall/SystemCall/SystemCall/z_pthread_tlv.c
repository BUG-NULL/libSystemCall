//
//  z_pthread_tlv.c
//  SystemCall
//
//  Created by NULL
//

#include "z_pthread_tlv.h"
#include "z_pthread_tsd.h"
#include "z_mman.h"
#include "z_malloc.h"
#include "z_string.h"

#define offsetof(t, d)      __builtin_offsetof(t, d)

typedef z_void (*term_func)(z_void *);

struct z_tlv_terminator_list_entry
{
    term_func   termFunc;
    z_void      *objAddr;
};

struct z_tlv_terminator_list
{
    z_uint32                            allocCount;
    z_uint32                            useCount;
    struct z_tlv_terminator_list_entry  entries[1];  // variable length
};

// MachO Section64(__DATA, __thread_vars)
struct z_tlv_descriptor
{
    z_void      *(*thunk)(struct z_tlv_descriptor *);
    z_uint64    key;
    z_uint64    offset;
};

struct z_tlv_image_info
{
    z_uint64                key;
    struct z_mach_header_64 *mh;
};

struct z_tlv_image_info *tlv_live_images = Z_NULL;
z_uint32 tlv_live_image_alloc_count = 0;
z_uint32 tlv_live_image_used_count = 0;
z_uint64 z_pthread_tlv_terminators_key = 0;

// linked images with TLV have references to this symbol, but it is never used at runtime
z_void _tlv_bootstrap(z_void)
{

}

z_void z_pthread_tlv_finalize_list(struct z_tlv_terminator_list *list)
{
    // destroy in reverse order of construction
    for (z_uint32 i = list->useCount; i > 0 ; --i)
    {
        struct z_tlv_terminator_list_entry *entry = &list->entries[i - 1];
        if (entry->termFunc != Z_NULL)
        {
            (*entry->termFunc)(entry->objAddr);
        }

        // If a new tlv was added via tlv_atexit, then we need to immediately
        // destroy it
        struct z_tlv_terminator_list *newlist = (struct z_tlv_terminator_list *)z_pthread_getspecific(z_pthread_tlv_terminators_key);
        if (newlist != Z_NULL)
        {
            // Set the list to NULL so that if yet another tlv is registered, we put it in a new list
            z_pthread_setspecific(z_pthread_tlv_terminators_key, Z_NULL);
            z_pthread_tlv_finalize_list(newlist);
        }
    }
    z_free(list, 0);
}

z_void z_pthread_tlv_finalize(z_void *storage)
{
    // Note, on entry to this function, _tlv_exit set the list to NULL.  libpthread
    // also set it to NULL before calling us.  If new thread locals are added to our
    // tlv_terminators_key, then they will be on a new list, but the list we have here
    // is one we own and need to destroy it
    z_pthread_tlv_finalize_list((struct z_tlv_terminator_list *)storage);
}

z_void z_pthread_tlv_free(z_void *storage)
{
    z_free(storage, 0);
}

struct z_mach_header_64 *z_pthread_tlv_get_image_for_key(z_uint64 key)
{
    struct z_mach_header_64 *result = Z_NULL;
    for (z_uint32 i = 0; i < tlv_live_image_used_count; ++i)
    {
        if (tlv_live_images[i].key == key)
        {
            result = tlv_live_images[i].mh;
            break;
        }
    }
    return result;
}

z_void *z_pthread_tlv_allocate_and_initialize_for_key(z_uint64 key)
{
    struct z_mach_header_64 *mh = z_pthread_tlv_get_image_for_key(key);
    if (mh == Z_NULL)
        return Z_NULL;    // if data structures are screwed up, don't crash
    
    // first pass, find size and template
    z_uint8 *start = Z_NULL;
    z_uint64 size = 0;
    z_uint64 slide = 0;
    z_int8 slideComputed = 0;
    z_int8 hasInitializers = 0;
    z_uint32 cmd_count = mh->ncmds;
    struct z_load_command *cmds = (struct z_load_command *)(((z_uint8 *)mh) + sizeof(struct z_mach_header_64));
    struct z_load_command *cmd = cmds;
    for (z_uint32 i = 0; i < cmd_count; ++i)
    {
        if (cmd->cmd == LC_SEGMENT_64)
        {
            struct segment_command_64 *seg = (struct segment_command_64 *)cmd;
            if (!slideComputed && (seg->filesize != 0))
            {
                slide = (z_uint64)mh - seg->vmaddr;
                slideComputed = 1;
            }
            struct section_64 *sectionsStart = (struct section_64 *)((z_int8 *)seg + sizeof(struct segment_command_64));
            struct section_64 *sectionsEnd = &sectionsStart[seg->nsects];
            for (struct section_64 *sect = sectionsStart; sect < sectionsEnd; ++sect)
            {
                switch (sect->flags & SECTION_TYPE)
                {
                    case S_THREAD_LOCAL_INIT_FUNCTION_POINTERS:
                        hasInitializers = 1;
                        break;
                    case S_THREAD_LOCAL_ZEROFILL:
                    case S_THREAD_LOCAL_REGULAR:
                        if (start == Z_NULL)
                        {
                            // first of N contiguous TLV template sections, record as if this was only section
                            start = (z_uint8 *)(sect->addr + slide);
                            size = sect->size;
                        }
                        else
                        {
                            // non-first of N contiguous TLV template sections, accumlate values
                            const z_uint8 *newEnd = (z_uint8 *)(sect->addr + slide + sect->size);
                            size = newEnd - start;
                        }
                        break;
                }
            }
        }
        cmd = (struct z_load_command *)(((z_int8 *)cmd) + cmd->cmdsize);
    }
    // no thread local storage in image: should never happen
    if (size == 0)
        return Z_NULL;
    
    // allocate buffer and fill with template
    z_void *buffer = z_malloc(size);
    z_memcpy(buffer, start, size);
    
    // set this thread's value for key to be the new buffer.
    z_pthread_setspecific(key, (z_uint64)buffer);
    
    // second pass, run initializers
    if (hasInitializers)
    {
        cmd = cmds;
        for (z_uint32 i = 0; i < cmd_count; ++i)
        {
            if (cmd->cmd == LC_SEGMENT_64)
            {
                struct segment_command_64 *seg = (struct segment_command_64 *)cmd;
                struct section_64 *sectionsStart = (struct section_64 *)((z_int8 *)seg + sizeof(struct segment_command_64));
                struct section_64 *sectionsEnd = &sectionsStart[seg->nsects];
                for (struct section_64 *sect = sectionsStart; sect < sectionsEnd; ++sect)
                {
                    if ((sect->flags & SECTION_TYPE) == S_THREAD_LOCAL_INIT_FUNCTION_POINTERS)
                    {
                        typedef z_void (*InitFunc)(z_void);
                        InitFunc *funcs = (InitFunc *)(sect->addr + slide);
                        z_uint64 count = sect->size / sizeof(z_uint64);
                        for (z_uint64 j = count; j > 0; --j)
                        {
                            InitFunc func = funcs[j - 1];
                            func();
                        }
                    }
                }
            }
            cmd = (struct z_load_command *)(((z_int8 *)cmd) + cmd->cmdsize);
        }
    }
    return buffer;
}

z_void *z_pthread_tlv_get_addr(struct z_tlv_descriptor *d)
{
    z_void *val = (z_void *)z_pthread_getspecific_direct(d->key);
    if (val != Z_NULL)
    {
        return val + d->offset;
    }
    else
    {
        val = z_pthread_tlv_allocate_and_initialize_for_key(d->key);
    }
    return val + d->offset;
}

z_void z_pthread_tlv_set_key_for_image(struct z_mach_header_64 *mh, z_uint64 key)
{
    if (tlv_live_image_used_count == tlv_live_image_alloc_count)
    {
        z_uint32 newCount = (tlv_live_images == Z_NULL) ? 8 : 2 * tlv_live_image_alloc_count;
        struct z_tlv_image_info *newBuffer = z_malloc(sizeof(struct z_tlv_image_info) * newCount);
        if (tlv_live_images != Z_NULL)
        {
            z_memcpy(newBuffer, tlv_live_images, sizeof(struct z_tlv_image_info) * tlv_live_image_used_count);
            z_free(tlv_live_images, 0);
        }
        tlv_live_images = newBuffer;
        tlv_live_image_alloc_count = newCount;
    }
    tlv_live_images[tlv_live_image_used_count].key = key;
    tlv_live_images[tlv_live_image_used_count].mh = mh;
    ++tlv_live_image_used_count;
}

z_void z_pthread_tlv_initialize_descriptors(struct z_mach_header_64 *mh)
{
    z_uint64 key = 0;
    z_uint64 slide = 0;
    z_int8 slideComputed = 0;
    z_uint32 cmd_count = mh->ncmds;
    struct z_load_command *cmds = (struct z_load_command *)(((z_uint8 *)mh) + sizeof(struct z_mach_header_64));
    struct z_load_command *cmd = cmds;
    
    for (z_uint32 i = 0; i < cmd_count; ++i)
    {
        if (cmd->cmd == LC_SEGMENT_64)
        {
            struct segment_command_64 *seg = (struct segment_command_64 *)cmd;
            if (!slideComputed && (seg->filesize != 0))
            {
                slide = (z_uint64)mh - seg->vmaddr;
                slideComputed = 1;
            }
            
            struct section_64 *sectionsStart = (struct section_64 *)((z_int8 *)seg + sizeof(struct segment_command_64));
            struct section_64 *sectionsEnd = &sectionsStart[seg->nsects];
            for (struct section_64 *sect = sectionsStart; sect < sectionsEnd; ++sect)
            {
                if ((sect->flags & SECTION_TYPE) == S_THREAD_LOCAL_VARIABLES)
                {
                    if (sect->size != 0)
                    {
                        // allocate pthread key when we first discover this image has TLVs
                        if (key == 0)
                        {
                            z_int32 result = z_pthread_key_create(&key, &z_pthread_tlv_free);
                            if (result != 0)
                                ;//abort();
                            z_pthread_tlv_set_key_for_image(mh, key);
                        }
                        // initialize each descriptor
                        struct z_tlv_descriptor *start = (struct z_tlv_descriptor *)(sect->addr + slide);
                        struct z_tlv_descriptor *end = (struct z_tlv_descriptor *)(sect->addr + sect->size + slide);
                        for (struct z_tlv_descriptor *d = start; d < end; ++d)
                        {
                            // d->thunk old value is addr of func _tlv_bootstrap
                            d->thunk = z_pthread_tlv_get_addr;
                            d->key = key;
                            //d->offset = d->offset;  // offset unchanged
                        }
                    }
                }
            }
        }
        cmd = (struct z_load_command *)(((z_int8 *)cmd) + cmd->cmdsize);
    }
}

z_void z_pthread_tlv_initializer(struct z_mach_header_64 *mh)
{
    // create pthread key to handle thread_local destructors
    // NOTE: this key must be allocated before any keys for TLV
    // so that _pthread_tsd_cleanup will run destructors before deallocation
    z_pthread_key_create(&z_pthread_tlv_terminators_key, &z_pthread_tlv_finalize);
    z_pthread_tlv_initialize_descriptors(mh);
}

z_void z_pthread_tlv_atexit(z_void *func, z_void *objAddr)
{
    // NOTE: this does not need locks because it only operates on current thread data
    struct z_tlv_terminator_list *list = (struct z_tlv_terminator_list *)z_pthread_getspecific(z_pthread_tlv_terminators_key);
    if (list == Z_NULL)
    {
        // handle first allocation
        list = (struct z_tlv_terminator_list *)z_malloc(offsetof(struct z_tlv_terminator_list, entries[1]));
        list->allocCount = 1;
        list->useCount = 1;
        list->entries[0].termFunc = (term_func)func;
        list->entries[0].objAddr = objAddr;
        z_pthread_setspecific(z_pthread_tlv_terminators_key, (z_uint64)list);
    }
    else
    {
        if (list->useCount == list->allocCount)
        {
            // handle resizing allocation
            z_uint32 newAllocCount = list->allocCount * 2;
            z_uint64 newAllocSize = offsetof(struct z_tlv_terminator_list, entries[newAllocCount]);
            struct z_tlv_terminator_list *newlist = (struct z_tlv_terminator_list *)z_malloc(newAllocSize);
            newlist->allocCount = newAllocCount;
            newlist->useCount = list->useCount;
            for (z_uint32 i = 0; i < list->useCount; ++i)
                newlist->entries[i] = list->entries[i];
            z_pthread_setspecific(z_pthread_tlv_terminators_key, (z_uint64)newlist);
            z_free(list, 0);
            list = newlist;
        }
        // handle appending new entry
        list->entries[list->useCount].termFunc = func;
        list->entries[list->useCount].objAddr = objAddr;
        list->useCount += 1;
    }
}

z_void z_pthread_tlv_exit(z_void)
{
    z_void *termFuncs = (z_void *)z_pthread_getspecific(z_pthread_tlv_terminators_key);
    if (termFuncs != Z_NULL)
    {
        // Clear the value so that calls to tlv_atexit during tlv_finalize
        // will go on to a new list to destroy.
        z_pthread_setspecific(z_pthread_tlv_terminators_key, Z_NULL);
        z_pthread_tlv_finalize(termFuncs);
    }
}

