#include "copyright.h"
#include "system.h"
#include "synch.h"
#include "utility.h"
#include "list.h"
#include "thread.h"
#include "system.h"

int testnum = 1;

//Send receive test

// these functions are made because t->Fork only take a function and one input argument and 
// your send takes 2 arguments similarly for receiver
void sendHelper (int msg){
    //call your send function
}
void rHelper (int k){
    //your receive function
}
void Send_Receive_test(){
    printf("Sender thread created\n");
    Thread *t = new Thread("Sender thread");
    t->Fork(sendHelper,3);
    printf("Receiver thread created\n");
    Thread *t1 = new Thread("Receiver thread");
    t1->Fork(rHelper,1);
}


//Join test only if your join does not take any input arguments
void
Joiner(Thread *joinee)
{
  currentThread->Yield();
  currentThread->Yield();

  printf("Waiting for the Joinee to finish executing.\n");

  currentThread->Yield();
  currentThread->Yield();

  joinee->Join();

  currentThread->Yield();
  currentThread->Yield();

  printf("Joinee has finished executing, we can continue.\n");

  currentThread->Yield();
  currentThread->Yield();
}

void
Joinee()
{
  int i;

  for (i = 0; i < 5; i++) {
    printf("Smell the roses.\n");
    currentThread->Yield();
  }

  currentThread->Yield();
  printf("Done smelling the roses!\n");
  currentThread->Yield();
}

void
ForkerThread()
{
  Thread *joiner = new Thread("joiner", 0);  // will not be joined
  Thread *joinee = new Thread("joinee", 1);  // WILL be joined

  // fork off the two threads and let them do their business
  joiner->Fork((VoidFunctionPtr) Joiner, (int) joinee);
  joinee->Fork((VoidFunctionPtr) Joinee, 0);

  // this thread is done and can go on its merry way
  printf("Forked off the joiner and joiner threads.\n");
}


// alarm test
void print(int k){
  printf("Hello\n");
  printf("How are you\n");
  printf("Ach ok fir\n");
  printf("Me going to sleep\n");
  GoToSleepFor(k);
  printf("Hello agian\n");
  printf("How are you\n");
  printf("Ach ok fir\n");
  printf("ab me chala\n");
}
void testAlarm(){
    t = new Timer(CallBack,1,0); // starts a timer which calls Callback function at each interupt 
                  // you can have any other function here (P.S. timer.h is in machine folder not in threads folder)
    printf("Alarm thread created\n");
    Thread *t = new Thread("Alarm thread");
    t->Fork(print,5000);
    Thread *t1 = new Thread("Alarm threaasd");
    t1->Fork(print,200);
    Thread *t2 = new Thread("Alarm threadsad");
    t2->Fork(print,50);
}
//----------------------------------------------------------------------
// ThreadTest
//  Invoke a test routine.
//----------------------------------------------------------------------

void
ThreadTest()
{
    switch (testnum) {
    case 1:
  testAlarm(); //replace it with ForkerThread() for join test ,Send_Receive_test() for send receive
  break;
    default:
  printf("No test specified.\n");
  break;
    }
}