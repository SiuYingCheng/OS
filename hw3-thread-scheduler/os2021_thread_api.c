#include "os2021_thread_api.h"
#include <json-c/json.h>
struct itimerval Signaltimer;
ucontext_t dispatch_context;
ucontext_t timer_context;
ucontext_t finish_context;
queue*ready=NULL;
queue*running=NULL;
queue*terminate=NULL;
queue*waiting=NULL;
int user=1; // calculate for thread inside system
long time_past = 0;
char priority_char[3] = {'L', 'M', 'H'};
int tq[3] = {300, 200, 100};
enum THREAD_STATE
{
    RUNNING,
    READY,
    WAITING,
    TERMINATED
};
const char*stateName[] = {"RUNNING","READY","WAITING","TERMINATED"};
queue *init_queue()
{
    queue *q = (queue *)malloc(sizeof(queue));
    if (!q)
    {
        return q;
    }
    q->count = 0;
    q->front = NULL;
    q->rear = NULL;
    return q;
}
int isEmpty(queue *q)
{
    return (q->front == NULL);
}
void enqueue(queue *q, thread*t)
{
    node *in;
    in = malloc(sizeof(node));
    in->Thread = t;
    in->next=NULL;
    if (!isEmpty(q))
    {
        q->rear->next = in;	// let node rear next pointer point to input node
        q->rear = in; // let node rear equal to input node
    }
    else
    {
        q->front = q->rear = in;
    }
    q->count++;
}

thread *dequeue(queue *q)
{
    if (!isEmpty(q))
    {
        thread*t = q->front->Thread;
        if (q->front->next != NULL)
            q->front = q->front->next;
        else
            q->front = q->rear = NULL;
        q->count--;
        return t;
    }
    else
    {
        return NULL;
    }
}
thread *search_name(queue *q, char*thread_name)
{
    node *curr = q->front;
    node *prev = 0;
    while (curr != 0 && curr->Thread->thread_name != thread_name)
    {
        prev = curr;
        curr = curr->next;
    }

    if (curr == 0)
    {
        return NULL;
    }
    else
    {
        return curr->Thread;
    }
}
thread *search_and_remove(queue *q, char*thread_name)
{
    node *curr = q->front;
    node *prev = 0;
    while (curr != 0 && curr->Thread->thread_name != thread_name)
    {
        prev = curr;
        curr = curr->next;
    }

    if (curr == 0)
    {
        return NULL;
    }

    else if (curr == q->front)
    {
        q->front = curr->next;
        q->count--;
        return curr->Thread;
    }
    else
    {
        prev->next = curr->next;
        q->count--;
        return curr->Thread;
    }
}
thread *search_and_remove_ID(queue *q, int ID)
{
    node *curr = q->front;
    node *prev = 0;
    while (curr != 0 && curr->Thread->thread_wait != ID)
    {
        prev = curr;
        curr = curr->next;
    }

    if (curr == 0)
    {
        return NULL;
    }
    if (curr == q->front)
    {
        q->front = curr->next;
        q->count--;
        return curr->Thread;
    }
    else
    {
        prev->next = curr->next;
        q->count--;
        return curr->Thread;
    }
}

void priority_change(thread**target, int time_past, int time_quantum)
{
    char priority_char[3] = {'L', 'M', 'H'};
    if(time_past < time_quantum)  // tq is 100 or 200 or 300, for increase priority b/c the use of time less
    {
        if((*target)->cur_priority != 2) // 2 is the highest, change to the highest
        {
            (*target)->cur_priority++;
            printf("The priority of thread %s is changed from %c to %c\n", (*target)->thread_name, priority_char[(*target)->cur_priority-1], priority_char[(*target)->cur_priority]);
        }
    }
    else
    {
        if((*target)->cur_priority != 0)
        {
            (*target)->cur_priority--;
            printf("The priority of thread %s is changed from %c to %c\n", (*target)->thread_name, priority_char[(*target)->cur_priority+1], priority_char[(*target)->cur_priority]);
        }
    }
    return;
}
void display_queue(queue *q)
{
    node *tmp = q->front;
    while (tmp != NULL)
    {
        printf("*\t%d\t%-10s\t%s\t\t%c\t\t%c\t\t%ld\t%ld\t *\n", tmp->Thread->TID, tmp->Thread->thread_name, stateName[tmp->Thread->thread_state],priority_char[tmp->Thread->priority], priority_char[tmp->Thread->cur_priority], tmp->Thread->queue_time, tmp->Thread->thread_wtime);
        tmp = tmp->next;
    }
}
void button_control()
{
    printf("**************************************************************************************************\n");
    printf("*\tTID\tName\t\tState\t\tB_Priority\tC_Priority\tQ_Time\tW_time\t *\n");
    display_queue(running);
    display_queue(ready);
    display_queue(waiting);
    printf("**************************************************************************************************\n");
}
int OS2021_ThreadCreate(char*job_name, char *p_function, char*priority, int cancel_mode)
{
    int p=0;
    switch(priority[0])
    {
    case 'H':
        p = 2;
        break;
    case 'M':
        p = 1;
        break;
    default:
        p = 0;
        break;
    }
    if (strcmp(p_function,"ResourceReclaim")==0||strcmp(p_function,"Function1")==0||strcmp(p_function,"Function2")==0||strcmp(p_function,"Function3")==0||strcmp(p_function,"Function4")==0||strcmp(p_function,"Function5")==0)
    {
        thread*t = (thread*)malloc(sizeof(thread));
        t->TID = user++;
        t->thread_name = job_name;
        t->queue_time = 0;
        t->priority = t->cur_priority=p;
        t->cancel_mode = cancel_mode;
        t->cancel_status=0;
        t->thread_wait=-1;  //event I want wait(0~7), -1 = no waiting
        t->time_quantum=0;
        t->thread_wtime=0;
        t->th_already_wait=0;
        t->thread_state = READY;
        if (strcmp(p_function,"Function1")==0)
        {
            t->threadFunction = Function1;
        }
        else if (strcmp(p_function,"Function2")==0)
        {
            t->threadFunction = Function2;
        }
        else if (strcmp(p_function,"Function3")==0)
        {
            t->threadFunction = Function3;
        }
        else if (strcmp(p_function,"Function4")==0)
        {
            t->threadFunction = Function4;
        }
        else if (strcmp(p_function,"Function5")==0)
        {
            t->threadFunction = Function5;
        }
        else
        {
            t->threadFunction = ResourceReclaim;
        }
        if (strcmp(p_function,"ResourceReclaim")!=0)
            CreateContext(&(t->uctx), &finish_context, t->threadFunction);
        else
            CreateContext(&(t->uctx), NULL, t->threadFunction);
        enqueue(ready, t);
        return t->TID;
    }
    else
    {
        return -1;
    }
}
void OS2021_ThreadWaitTime(int msec)
{
    thread*t = dequeue(running);
    priority_change(&t, time_past, tq[t->cur_priority]);
    t->time_quantum = msec;
    enqueue(waiting, t);
    t->thread_state = WAITING;
    swapcontext(&(t->uctx), &dispatch_context);
    return;
}
void OS2021_ThreadWaitEvent(int event_id)   // change the state in waiting and enter event waiting queue
{
    thread*target = dequeue(running);
    target->thread_wait = event_id;  // th_wait the id of thread target
    printf("%s wants to waiting for event %d\n", target->thread_name, event_id);
    priority_change(&target, time_past, tq[target->cur_priority]);//change priority
    enqueue(waiting,target);
    target->thread_state=WAITING;
    swapcontext(&(target->uctx), &dispatch_context);//save current status and reschedule
    return;
}
void OS2021_TestCancel()    // allow thread cancel in safe point
{
    if(running->front->Thread->cancel_status == 1)//change when it was cancel by somebody
    {
        thread*target = dequeue(running);
        enqueue(terminate,target);
        target->thread_state=TERMINATED;
        setcontext(&dispatch_context);//reschedule
    }
    else
    {
        return;
    }
}
void OS2021_DeallocateThreadResource()  // deallocate rsc in terminate
{
    thread*target=dequeue(terminate);
    while(target != NULL)
    {
        printf("The memory space by %s has been released.\n", target->thread_name);
        target = dequeue(terminate);
    }
    return;
}
void OS2021_ThreadCancel(char *job_name)
{
    int count=0;
    thread*target=NULL;
    if(strcmp("reclaimer", job_name)==0)
        return; //reclaimer can't enter terminate state
    target= search_name(ready,job_name);
    if (target==NULL)
    {
        target = search_name(waiting,job_name);
        count=1;
    }
    if (target!=NULL)
    {
        if (target->cancel_mode==0)
        {
            if (count==0)
            {
                target = search_and_remove(ready,job_name);
            }
            else
            {
                target =search_and_remove(waiting,job_name);
            }
            enqueue (terminate,target);
            target->thread_state=TERMINATED;
        }
        else
        {
            target->cancel_status=1;
            printf("%s wants to cancel thread %s\n", running->front->Thread->thread_name, target->thread_name);
        }
    }
    return;
}
void OS2021_ThreadSetEvent(int event_id)
{
    thread*hit_th = search_and_remove_ID(waiting,event_id);
    if (hit_th!=NULL)
    {
        hit_th->thread_wait=-1;
        printf("%s changes the status of %s to READY.\n", running->front->Thread->thread_name, hit_th->thread_name);
        enqueue(ready, hit_th);
        hit_th->thread_state = READY;
        return;

    }
    return;
}
void CreateContext(ucontext_t *context, ucontext_t *next_context, void *func)
{
    getcontext(context);
    context->uc_stack.ss_sp = malloc(STACK_SIZE);
    context->uc_stack.ss_size = STACK_SIZE;
    context->uc_stack.ss_flags = 0;
    context->uc_link = next_context;
    makecontext(context,(void (*)(void))func,0);
}
void ResetTimer()
{
    Signaltimer.it_value.tv_sec = 0;
    Signaltimer.it_value.tv_usec = 10000;  // first time wait 10ms run timer
    if(setitimer(ITIMER_REAL, &Signaltimer, NULL) < 0)
    {
        printf("ERROR SETTING TIME SIGALRM!\n");
    }
}
void TimerHandler()   // change something here
{
    time_past += 10;
    node*temp_th = ready->front;
    node*ex_th = NULL;
    while(temp_th != NULL)      // increase time for each thread in ready queue
    {
        temp_th->Thread->queue_time += 10;
        temp_th = temp_th->next;
    }
    temp_th = waiting->front;
    while(temp_th != NULL)      // increase time for each thread in waiting queue
    {
        temp_th->Thread->thread_wtime += 10;    // wait time +=10
        if(temp_th->Thread->time_quantum != 0)   // the time temp need to wait
        {
            node*target = temp_th;
            node*target_ex = ex_th;
            target->Thread->th_already_wait ++; //use when ThreadWaitTime
            if(target->Thread->th_already_wait >= target->Thread->time_quantum)  // if the time it really waiting access the time it already wait, then change to ready queue
            {
                target->Thread->time_quantum = 0;
                target->Thread->th_already_wait = 0;
                if (target == waiting->front)
                {
                    target->Thread = dequeue(waiting);
                }
                else
                {
                    target_ex->next=target->next;
                }
                enqueue(ready,target->Thread);
                target->Thread->thread_state=READY;
            }
        }
        ex_th = temp_th;
        temp_th = temp_th->next;
    }
    if (running->front!=NULL)
    {
        if(time_past >= tq[running->front->Thread->cur_priority])
        {
            if(running->front->Thread->cur_priority !=0)
            {
                running->front->Thread->cur_priority--;
                printf("The priority of thread %s is changed from %c to %c\n",running->front->Thread->thread_name, priority_char[running->front->Thread->cur_priority+1], priority_char[running->front->Thread->cur_priority]);
            }
            thread*t=dequeue(running);
            enqueue(ready,t);
            t->thread_state=READY;
            swapcontext(&(t->uctx), &dispatch_context);//reschedule
        }
    }
    ResetTimer();
    return;
}
void Dispatcher()
{
    thread*t= dequeue(ready);
    enqueue(running,t);
    t->thread_state=RUNNING;
    time_past = 0;
    ResetTimer();
    setcontext(&(t->uctx));
}
void FinishThread()
{
    thread*target = dequeue(running);
    enqueue(terminate,target);
    target->thread_state=TERMINATED;
    setcontext(&dispatch_context); //reschedule
}
void Parser()
{
    FILE *fp;
    char buffer[10000];
    struct json_object *parsed_json;
    struct json_object*threads;
    struct json_object*single_thread;
    struct json_object *name;
    struct json_object *entry_function;
    struct json_object *priority;
    struct json_object *cancel_mode;
    char*str_name;
    char*str_entry_function;
    char*cp_priority=0;
    int cp_cancel_mode=0;
    size_t n_threads;
    size_t i;

    fp = fopen("init_threads.json","r");
    fread(buffer, 1024, 1, fp);
    fclose(fp);

    parsed_json = json_tokener_parse(buffer);
    json_object_object_get_ex(parsed_json, "Threads", &threads);
    n_threads = json_object_array_length(threads);
    for(i=0; i<n_threads; i++)
    {
        single_thread = json_object_array_get_idx(threads, i);
        json_object_object_get_ex(single_thread, "name", &name);
        json_object_object_get_ex(single_thread, "entry function", &entry_function);
        json_object_object_get_ex(single_thread, "priority", &priority);
        json_object_object_get_ex(single_thread, "cancel mode", &cancel_mode);
        str_name = (char*)(json_object_get_string(name));
        str_entry_function = (char*)(json_object_get_string(entry_function));
        cp_priority = (char*)(json_object_get_string(priority));
        cp_cancel_mode = (int)(json_object_get_int(cancel_mode));
        //printf("%s %s %s %s %d\n",str_name,str_entry_function,cp_priority,cp_priority,cp_cancel_mode);
        OS2021_ThreadCreate(str_name,str_entry_function,cp_priority,cp_cancel_mode);
    }
}
void StartSchedulingSimulation()
{
    ready = init_queue();
    running = init_queue();
    waiting = init_queue();
    terminate = init_queue();
    Signaltimer.it_interval.tv_usec = 100;
    Signaltimer.it_interval.tv_sec = 0;
    signal(SIGALRM, TimerHandler);      // have to change
    signal(SIGTSTP, button_control);
    CreateContext(&dispatch_context, NULL, &Dispatcher);    // run dispatcher, end dispatch context will out the program
    CreateContext(&finish_context, &dispatch_context, &FinishThread);
    Parser();
    OS2021_ThreadCreate("reclaimer", "ResourceReclaim", "L", 1);
    setcontext(&dispatch_context);  // change to the dispatch_context
}
