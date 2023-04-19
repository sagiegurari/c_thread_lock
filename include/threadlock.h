#ifndef THREADLOCK_H
#define THREADLOCK_H

#include <stdbool.h>
#include <stddef.h>

struct ThreadLock;

struct ThreadLockOptions
{
  int wait_timeout_in_milliseconds;
};

struct ThreadLockOptions threadlock_new_default_options();
struct ThreadLock        *threadlock_new();
struct ThreadLock        *threadlock_new_with_options(struct ThreadLockOptions);
void threadlock_release(struct ThreadLock *);
bool threadlock_lock(struct ThreadLock *);
bool threadlock_unlock(struct ThreadLock *);
bool threadlock_signal(struct ThreadLock *, bool wrap_with_lock);
bool threadlock_wait(struct ThreadLock *, bool wrap_with_lock);
bool threadlock_wait_with_timeout(struct ThreadLock *, bool wrap_with_lock, int wait_timeout_in_milliseconds);
void *threadlock_lock_and_run(struct ThreadLock *, void * (*fn)(void *), void *);

#endif

