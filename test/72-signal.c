#include "thread.h"
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

void handle_sigint(int i)
{
    printf("Caught signal %d \n", i);
}

static void* thfunc(void *id __attribute__((unused)))
{
  thread_yield();
  thread_yield();
  thread_signal(1, handle_sigint);
  thread_signal(3, handle_sigint);
  sleep(1);
  thread_yield();
  thread_yield();
  return NULL;
}

int main()
{
  thread_t th1;
  int err;

  err = thread_create(&th1, thfunc, NULL);
  assert(!err);
  sleep(1);

  thread_kill(th1, 1);
  thread_kill(th1, 3);
  thread_yield();
  thread_yield();
  
  err = thread_join(th1, NULL);
  assert(!err);


  return 0;
}
