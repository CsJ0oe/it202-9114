#include "thread.h"
#include <stdio.h>
#include <assert.h>

void handle_sigint(int i __attribute__((unused)))
{
  printf("Caught signal \n");
}

static void* thfunc(void *id __attribute__((unused)))
{
  thread_yield();
  thread_yield();
  thread_signal(0, handle_sigint);
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

  thread_kill(th1, 0);
  thread_yield();
  thread_yield();
  
  err = thread_join(th1, NULL);
  assert(!err);


  return 0;
}
