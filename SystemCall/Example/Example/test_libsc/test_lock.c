//
//  test_lock.c
//  Example
//
//  Created by NULL
//

#include "test_lock.h"

// Add value to *ptr, update the result to *ptr, return old value of *ptr
z_void test__sync_fetch_and_add(z_void)
{
    z_uint32 current_val = 15;
    z_uint32 old_val = __sync_fetch_and_add(&current_val, 5);
    z_assert((current_val == 20) && (old_val == 15));
}

// Add value to *ptr, update the result to *ptr, return new value of *ptr
z_void test__sync_add_and_fetch(z_void)
{
    z_uint32 current_val = 15;
    z_uint32 old_val = __sync_add_and_fetch(&current_val, 5);
    z_assert((current_val == 20) && (old_val == 20));
}

// Subtract value from *ptr, update the result to *ptr, return old value of *ptr
z_void test__sync_fetch_and_sub(z_void)
{
    z_uint32 current_val = 15;
    z_uint32 old_val = __sync_fetch_and_sub(&current_val, 5);
    z_assert((current_val == 10) && (old_val == 15));
}

// Subtract value from *ptr, update the result to *ptr, return new value of *ptr
z_void test__sync_sub_and_fetch(z_void)
{
    z_uint32 current_val = 15;
    z_uint32 old_val = __sync_sub_and_fetch(&current_val, 5);
    z_assert((current_val == 10) && (old_val == 10));
}

// OR *ptr with value, update the result to *ptr, return old value of *ptr
z_void test__sync_fetch_and_or(z_void)
{
    z_uint32 current_val = 10;
    z_uint32 old_val = __sync_fetch_and_or(&current_val, 5);
    z_assert((current_val == 15) && (old_val == 10));
}

// OR *ptr with value, update the result to *ptr, return new value of *ptr
z_void test__sync_or_and_fetch(z_void)
{
    z_uint32 current_val = 10;
    z_uint32 old_val = __sync_or_and_fetch(&current_val, 5);
    z_assert((current_val == 15) && (old_val == 15));
}

// And *ptr with value, update the result to *ptr, return old value of *ptr
z_void test__sync_fetch_and_and(z_void)
{
    z_uint32 current_val = 10;
    z_uint32 old_val = __sync_fetch_and_and(&current_val, 5);
    z_assert((current_val == 0) && (old_val == 10));
}

// And *ptr with value, update the result to *ptr, return new value of *ptr
z_void test__sync_and_and_fetch(z_void)
{
    z_uint32 current_val = 10;
    z_uint32 old_val = __sync_and_and_fetch(&current_val, 5);
    z_assert((current_val == 0) && (old_val == 0));
}

// XOR *ptr with value, update the result to *ptr, return old value of *ptr
z_void test__sync_fetch_and_xor(z_void)
{
    z_uint32 current_val = 10;
    z_uint32 old_val = __sync_fetch_and_xor(&current_val, 5);
    z_assert((current_val == 15) && (old_val == 10));
}

// XOR *ptr with value, update the result to *ptr, return new value of *ptr
z_void test__sync_xor_and_fetch(z_void)
{
    z_uint32 current_val = 10;
    z_uint32 old_val = __sync_xor_and_fetch(&current_val, 5);
    z_assert((current_val == 15) && (old_val == 15));
}

// Compare *ptr and old value, if they are equal, update new value to *ptr and return true
z_int8 test__sync_bool_compare_and_swap(z_void)
{
    z_uint32 current_val = 10;
    z_uint32 ret = __sync_bool_compare_and_swap(&current_val, 10, 15);
    z_assert((ret == 1) && (current_val == 15));
    return ret;
}

// Compare *ptr and old value, if they are equal, update new value to *ptr and return old value
z_void test__sync_val_compare_and_swap(z_void)
{
    z_uint32 current_val = 10;
    z_uint32 old_val = __sync_val_compare_and_swap(&current_val, 10, 15);
    z_assert((current_val == 15) && (old_val == 10));
}

z_void test__sync_lock(z_void)
{
    z_uint32 current_val = 10;
    z_uint32 old_val = 0;
    
    // Write value to *ptr, lock *ptr, return old value of *ptr. That is, try spinlock semantics
    old_val = __sync_lock_test_and_set(&current_val, 1);
    z_assert((current_val == 1) && (old_val == 10));
    
    // mfence
    __sync_synchronize();
    
    // Write 0 to *ptr, and unlock *ptr. That is, unlock spinlock semantics
    __sync_lock_release(&current_val);
    z_assert(current_val == 0);
}

z_void test_cas(z_void)
{
    test__sync_fetch_and_add();
    test__sync_add_and_fetch();
    
    test__sync_fetch_and_sub();
    test__sync_sub_and_fetch();
    
    test__sync_fetch_and_or();
    test__sync_or_and_fetch();
    
    test__sync_fetch_and_and();
    test__sync_and_and_fetch();
    
    test__sync_fetch_and_xor();
    test__sync_xor_and_fetch();

    test__sync_bool_compare_and_swap();
    test__sync_val_compare_and_swap();

    test__sync_lock();
}

z_void test_lock(z_void)
{
    z_int8 ret = 0;
    struct z_lock_s lock;
    lock.z_lock_opaque = 0;

    z_lock_lock(&lock);
    ret = z_lock_trylock(&lock);
    if (ret == 0)
    {
        z_printf("try lock failed\n");
        z_lock_unlock(&lock);
        ret = z_lock_trylock(&lock);
        if (ret)
        {
            z_printf("try lock success\n");
        }
    }
}
