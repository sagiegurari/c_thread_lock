#include "test.h"
#include "threadlock.h"


static void test_impl()
{
  struct ThreadLock *lock = threadlock_new();

  bool              done = threadlock_lock(lock);

  assert_true_with_description(done, "failed to lock");
  done = threadlock_unlock(lock);
  assert_true_with_description(done, "failed to unlock");

  threadlock_release(lock);
}


int main()
{
  test_run(test_impl);
}

