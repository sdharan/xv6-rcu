#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"


int
main(int argc, char *argv[])
{

  if(shmget(12,(char *)0x7FFF0000,4096)<0){
    printf(1,"error\n");
  }

  char *temp = (char*) 0x7FFF0000;
  char *test = (char*) 0x7FFF0000;
  int i = 0;
  for(;i<4096; i++){
    *test = 'A';
    test++;
  }
  
  if(fork()==0){
    // child
    // enter critical  section and stay for 100 ticks
    char *test1;
    if(shmget(12,(char *)0x20000000,0) < 0){
      printf(1,"Child error");
      exit();
    }
    test1 =(char*) 0x20000000;

    printf(1,"Child: Entering RCU read lock\n");
    rcureadlock();
    int i = 0;
    int j = 0;
    for (; j < 1000; j++) {
      i= 0;
      for (;i < 4096; i++) {
        if (test1[i] != 'A') {
          printf(1, "Child: Wrong input!\n");
        }
      }
    }
    printf (1,"Child : Value of J is %d\n", j);
    printf(1,"Child: Leaving read lock!\n");
    rcureadunlock();
  } else {
    // In parent, make it
    sleep(20);
    printf(1,"Parent: Calling rcuwait to wait for safe update\n");
    rcuwait();
    printf(1,"Parent: Rcu wait returns, good to update shared memory!\n");
    i  = 0;
    for(;i<4096; i++){
      *temp = 'B';
      temp++;
    }
  }
  wait ();
  exit();
}


