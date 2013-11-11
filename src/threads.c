/* An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * simple_threads.c
 *
 * Simple multi-threaded example.
 */
#include <stdlib.h>
#include <stdio.h> 
#include <time.h>
#include <string.h>

#include <unistd.h>

#include <pthread.h>
 
void do_one_thing(int *);
void do_another_thing(int *);
void do_wrap_up(int, int);
 
int r1 = 0, r2 = 0;
 
 
int main(int argc, const char *argv[])
{
  pthread_t       thread1, thread2;
 
  pthread_create(&thread1, 
		 NULL,
		 (void *) do_one_thing,
		 (void *) &r1); 
 
  pthread_create(&thread2, 
		 NULL, 
		 (void *) do_another_thing,
		 (void *) &r2); 

  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
 
  do_wrap_up(r1, r2);
 
}
 
void do_one_thing(int *pnum_times)
{
  int i, j, x;

   while(1) {
    printf("doing one thing\n"); 
    usleep(100);
  }  
}
 
void do_another_thing(int *pnum_times)
{
  int i, j, x;

  while(1) {
    printf("doing another \n"); 
    usleep(100);
  } 
}
 
void do_wrap_up(int one_times, int another_times)
{
  int total;
 
  total = one_times + another_times;
  printf("All done, one thing %d, another %d for a total of %d\n",
	one_times, another_times, total);
}
