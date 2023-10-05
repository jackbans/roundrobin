#ifndef TOURNIQUET_H
#define TOURNIQUET_H

typedef struct Proc
{
    char name[10];
    char status[10];
    int time_required;
    int priority;
    int time_remained;
    struct tm *begin_time;
    struct tm *end_time;
    struct Proc *prev;
    struct Proc *next;
} Process;

typedef struct Q
{
    Process * head_process;
    struct Q *prev;
    struct Q *next;
} Queue;

Process* dispatcher(Process*process, int status_index);
Process *enqueue_process(Process *head_process,Process *new_process);
int frand(int , int);
void free_process(Process *processes);
int index_of_status(char s[10]);
Queue *insert_queue_by_order(Queue *head_queue,Process *new_process);
Queue* make_queue(Process*head_process);
Queue *manage_into_queues(Process *processes);
Process *new_process_with_random_data(const int process_id,const int process_number);
Process* ordonnanceur(Queue *queues, const int processes_number);
Process* processeur(Process* process, int * queue_remained_time);
void traitement(const int execution_time);
#endif
