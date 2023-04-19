#include "threadlock.h"
#include <pthread.h>
#include <stdlib.h>

struct ThreadLock
{
  int             wait_timeout_in_milliseconds;
  pthread_mutex_t lock;
  pthread_cond_t  signal;
};

struct ThreadLockOptions threadlock_new_default_options()
{
  struct ThreadLockOptions options =
  {
    .wait_timeout_in_milliseconds = 0,
  };

  return(options);
}

struct ThreadLock *threadlock_new()
{
  struct ThreadLockOptions options = threadlock_new_default_options();

  return(threadlock_new_with_options(options));
}

struct ThreadLock *threadlock_new_with_options(struct ThreadLockOptions options)
{
  struct ThreadLock *lock = malloc(sizeof(struct ThreadLock));

  lock->wait_timeout_in_milliseconds = options.wait_timeout_in_milliseconds;

  if (pthread_mutex_init(&lock->lock, NULL))
  {
    free(lock);
    return(NULL);
  }
  if (pthread_cond_init(&lock->signal, NULL))
  {
    pthread_mutex_destroy(&lock->lock);
    free(lock);
    return(NULL);
  }

  return(lock);
}


void threadlock_release(struct ThreadLock *lock)
{
  if (lock == NULL)
  {
    return;
  }

  pthread_mutex_destroy(&lock->lock);
  pthread_cond_destroy(&lock->signal);
  free(lock);
}


bool threadlock_lock(struct ThreadLock *lock)
{
  if (lock == NULL)
  {
    return(false);
  }

  pthread_mutex_lock(&lock->lock);
  return(true);
}


bool threadlock_unlock(struct ThreadLock *lock)
{
  if (lock == NULL)
  {
    return(false);
  }

  pthread_mutex_unlock(&lock->lock);
  return(true);
}


bool threadlock_signal(struct ThreadLock *lock, bool wrap_with_lock)
{
  if (lock == NULL)
  {
    return(false);
  }

  if (wrap_with_lock)
  {
    if (!threadlock_lock(lock))
    {
      return(false);
    }
  }

  pthread_cond_signal(&lock->signal);

  if (wrap_with_lock)
  {
    threadlock_unlock(lock);
  }

  return(true);
}


bool threadlock_wait(struct ThreadLock *lock, bool wrap_with_lock)
{
  if (lock == NULL)
  {
    return(false);
  }

  return(threadlock_wait_with_timeout(lock, wrap_with_lock, lock->wait_timeout_in_milliseconds));
}


bool threadlock_wait_with_timeout(struct ThreadLock *lock, bool wrap_with_lock, int wait_timeout_in_milliseconds)
{
  if (lock == NULL)
  {
    return(false);
  }

  if (wrap_with_lock)
  {
    if (!threadlock_lock(lock))
    {
      return(false);
    }
  }

  if (wait_timeout_in_milliseconds <= 0)
  {
    pthread_cond_wait(&lock->signal, &lock->lock);
  }
  else
  {
    int             seconds      = wait_timeout_in_milliseconds / 1000;
    int             nano_seconds = (wait_timeout_in_milliseconds - seconds * 1000) * 1000000;

    struct timespec ts;
    ts.tv_sec  = time(NULL) + seconds;
    ts.tv_nsec = nano_seconds;
    pthread_cond_timedwait(&lock->signal, &lock->lock, &ts);
  }

  if (wrap_with_lock)
  {
    threadlock_unlock(lock);
  }

  return(true);
}


void *threadlock_lock_and_run(struct ThreadLock *lock, void * (*fn)(void *), void *args)
{
  if (lock == NULL)
  {
    return(NULL);
  }

  if (!threadlock_lock(lock))
  {
    return(NULL);
  }

  void *output = fn(args);

  threadlock_unlock(lock);

  return(output);
}

