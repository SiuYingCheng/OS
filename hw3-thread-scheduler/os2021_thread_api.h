#ifndef OS2021_API_H
#define OS2021_API_H
#define STACK_SIZE 8192

#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include "function_libary.h"

typedef struct thread
{
    int TID;
    char*thread_name;
    int thread_state;
    int priority;
    int cur_priority;//current priority
    int cancel_mode;
    int cancel_status;//0=nothing, 1=waiting cancel
    int thread_wait;//event I want wait(0~7), -1 = no waiting
    long queue_time;
    long time_quantum;  // time need to wait
    long thread_wtime;//waiting time
    long th_already_wait;//use when ThreadWaitTime
    void (*threadFunction)(void);
    ucontext_t uctx; // context
} thread;

typedef struct node
{
    thread *Thread;
    struct node *next;
} node;

typedef struct queue
{
    int count;
    node *front;
    node *rear;
} queue;

int OS2021_ThreadCreate(char *job_name, char *p_function, char*priority, int cancel_mode);
void OS2021_ThreadCancel(char *job_name);
void OS2021_ThreadWaitEvent(int event_id);
void OS2021_ThreadSetEvent(int event_id);
void OS2021_ThreadWaitTime(int msec);
void OS2021_DeallocateThreadResource();
void OS2021_TestCancel();


void CreateContext(ucontext_t *, ucontext_t *, void *);
void ResetTimer();
void Dispatcher();
void StartSchedulingSimulation();

#endif

