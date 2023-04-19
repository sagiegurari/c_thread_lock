#include "test.h"
#include "threadlock.h"
#include <time.h>


static void test_impl()
{
  struct ThreadLockOptions options = threadlock_new_default_options();

  options.wait_timeout_in_milliseconds = 3000;
  struct ThreadLock *lock = threadlock_new_with_options(options);

  time_t            start     = time(NULL);
  bool              done      = threadlock_wait(lock, true);
  time_t            wait_time = time(NULL) - start;
  assert_true_with_description(done, "failed to wait");
  assert_true_with_description(wait_time > 1, "timeout was too short");

  threadlock_release(lock);
}


int main()
{
  test_run(test_impl);
}

