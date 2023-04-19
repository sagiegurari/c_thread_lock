#include "test.h"
#include "threadlock.h"

static bool _global_invoked = false;


static void *_test_run(void *args)
{
  assert_string_equal((char *)args, "test");
  _global_invoked = true;
  return("output");
}


static void test_impl()
{
  struct ThreadLock *lock = threadlock_new();

  void              *output = threadlock_lock_and_run(lock, _test_run, "test");

  assert_string_equal((char *)output, "output");

  threadlock_release(lock);
}


int main()
{
  test_run(test_impl);
}

