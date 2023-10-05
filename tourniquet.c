#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "tourniquet.h"

///b////////////////////////////////////////////////////////////////////////////////////m///
///                                                                                      ///
///                                                                                      ///
///                                                                                      ///
///                                 			                                 ///
///                                                                                      ///
///                                                                                      ///
///                                                                                      ///
///b////////////////////////////////////////////////////////////////////////////////////m///

#define tUnit 20

const char* status[4] = {"ELU","PRET","BLOQUE","TERMINE"};


Process* dispatcher(Process *process, int status_index)
{
    // Change process status from elect (ELU) to another one.
    strcpy(process->status,status[status_index]);
    if(status_index == 3)
    {
        time_t now;
        time(&now);
        process->end_time = localtime(&now);
	return process;
    }
    // Enqueueing the last Elect (ELU) process
    if(process->next!=NULL)
        process = enqueue_process(process->next,process);
    return process;
}

Process *enqueue_process(Process *head_process,Process *new_process)
{
    if(head_process == NULL)
    { 
        if(new_process)
        {
            new_process->next = NULL;
	    new_process->prev = NULL;
        }
        return new_process;
    }
    if(new_process == NULL)
        return head_process;
    Process *process_iter=NULL;
    for(process_iter=head_process;process_iter->next!=NULL;process_iter=process_iter->next)
        ;
    process_iter->next = new_process;
    new_process->prev = process_iter;
    new_process->next = NULL;
    head_process->prev = NULL;
    return head_process;
}
//return a random value from the range [a,b]
int frand(int a, int b)
{
    return ((rand()%(b-a+1))+a);
}
void free_process(Process *processes)
{
    int p = 0;
    Process* current_process = processes;
    while(current_process)
    {
        current_process = current_process->next;
        free(processes);
        processes = current_process;
	p++;
    }
    printf("\n\nLiberation de %d processus.\n", p);
}
int index_of_status(char s[10])
{
    for(int i=0;i<4;i++)
        if(strcmp(s,status[i])==0) return i;
    return -1;
}
Queue *insert_queue_by_order(Queue *head_queue,Process *new_process)
{
    Queue *current_queue=NULL, *tmp_queue;
    Process *current_process = NULL;
    if(new_process==NULL)return head_queue;
    if(head_queue==NULL) return make_queue(new_process);
    if(new_process->next != NULL)
    {
        printf("\nWarning : file de precessus ignoree !\n");
        printf("\nWarning : vous avez fourni une file de precessus \n"
        "Warning : au lieu d'un processus unique !\n");
        return head_queue;
    }
    current_queue = head_queue;
    while(current_queue!=NULL)
    {
        current_process = current_queue->head_process;
        // Protecting access to priority field
        if(!current_process) return head_queue;
        //
        if(new_process->priority == current_process->priority)
        {
            current_queue->head_process = enqueue_process(current_process, new_process);
            return head_queue;
        }
        else if(new_process->priority < current_process->priority)
        {
            Queue* new_queue = make_queue(new_process);
            if(new_queue)
            {
                new_queue->next = current_queue;
                new_queue->prev = current_queue->prev;
                tmp_queue = current_queue->prev;
                current_queue->prev = new_queue;//was forgotten
                if(tmp_queue) tmp_queue->next = new_queue;
                if(current_queue == head_queue) head_queue = new_queue;
            }
            else printf("\nWarning : couldn't make queue !\n");
            return head_queue;
        }
        tmp_queue = current_queue;
        current_queue = current_queue->next;
    }
    Queue* new_queue = make_queue(new_process);
    // Enqueuing new_queue
    if(new_queue)
    {
        new_queue->prev = tmp_queue;
        tmp_queue->next = new_queue;
    }
    else printf("\nWarning : couldn't make queue !\n");
    return head_queue;
}
Queue* make_queue(Process*head_process)
{
    if(head_process == NULL) return NULL;
    Queue* new_queue = NULL;
    new_queue = (Queue *)malloc(sizeof(Queue));
    if(new_queue == NULL) return NULL;
    new_queue->head_process = head_process;
    new_queue->prev = NULL;
    new_queue->next = NULL;
    return new_queue;
}
Queue *manage_into_queues(Process *processes)
{
    Queue *queues = NULL;
    Process *current_process, *process;
    current_process = processes;
    while(current_process != NULL)
    {
        process = current_process;
        current_process = current_process->next;
        process->next = NULL;
        process->prev = NULL;
        queues = insert_queue_by_order(queues,process);
    }
    return queues;
}
Process *new_process_with_random_data(const int process_id,const int processes_number)
{
    Process *new_process;
    time_t now;
    new_process = (Process *)malloc(sizeof(Process));
    if(!new_process) return NULL;
    sprintf(new_process->name,"Proc%03d",process_id);
    // By default all processes are ready [PRET]
    strcpy(new_process->status,status[1]);
    new_process->time_required = frand(20,200);
    new_process->priority = frand(1,(int)sqrt(processes_number));
    new_process->time_remained = new_process->time_required;
    time(&now);
    new_process->begin_time = localtime(&now);
    new_process->end_time = NULL;
    new_process->prev = NULL;
    new_process->next = NULL;
    return new_process;
}

Process* ordonnanceur(Queue *queues, const int processes_number)
{
    Process *finished_processes = NULL;
    Queue *current_queue=NULL, *tmp_queue = NULL;
    Process *current_process;
    int status_index = -1;
    int queue_remained_time = 0;
    time_t now;
    char str_time[26];
    current_queue = queues;
    while(current_queue)
    {
        current_process = current_queue->head_process;
        if(current_process)
        {
            queue_remained_time = 4 * ((int)sqrt(processes_number)) * tUnit / current_process->priority;
            time(&now);
            asctime_r(gmtime(&now),str_time); 
            str_time[24] = '\0';
            printf("\n[%s] File de priorite %d [Debut d'execution]\n",str_time,current_process->priority);
        }
        while(current_queue->head_process != NULL && queue_remained_time >= tUnit)
        {
            current_process = current_queue->head_process;
            status_index = index_of_status(current_process->status);
            switch(status_index)
            {
                case 1:
                    // the head process of the current queue is ready (PRET) and must be
                    // processed, we must mark it as elect (ELU) and send it to the processor
                    strcpy(current_process->status,status[0]);
                    time(&now);
                    current_process->begin_time = localtime(&now);
                    current_queue->head_process = processeur(current_process, &queue_remained_time);
                    break;
                case 2:
                    // The process is block we give it 75% of chance to get ready (PRET)
                    // and to be elect (ELU) and processed, the next time it is reached
                    if (frand(1,100)>25)
                        strcpy(current_process->status,status[1]);
                    // Enqueue the current process
                    if(current_process->next != NULL)
                        current_queue->head_process = enqueue_process(current_process->next, current_process);
		    // queue_remained_time -= tUnit;
                    break;
                case 3:
                    // The current processus is finished
                    current_queue->head_process = current_process->next;
                    if (current_queue->head_process) current_queue->head_process->prev = NULL;
                    finished_processes = enqueue_process(finished_processes, current_process);
                    break;
            }
        }
        time(&now);
        asctime_r(gmtime(&now),str_time); 
        str_time[24] = '\0';
        if(current_queue->head_process == NULL)
        {
            if(current_process)
	        printf("[%s] File de priorite %d [Fin d'execution]\n",str_time,current_process->priority);
            if(queues == current_queue)
	        queues = current_queue->next;
            tmp_queue = current_queue->next;
            if(tmp_queue)
            {
                tmp_queue->prev = current_queue->prev;
                if (tmp_queue->prev) tmp_queue->prev->next = tmp_queue;
            }
	    else if(current_queue->prev) current_queue->prev->next = NULL ;
            free(current_queue);
            current_queue = tmp_queue;
        }
        else if(current_process) // if the previous loop is executed then current_process is different
				 // from current_queue->head_process at this point. 
        {
            printf("[%s] File de priorite %d [Fin du temps imparti]\n",str_time,current_process->priority);
            current_queue = current_queue->next;
        }
        // Begin the process if you reach the queue end.
	if(!current_queue) current_queue = queues;
    }
    return finished_processes;
}
Process* processeur(Process* process, int * queue_remained_time)
{
    if(process == NULL) return NULL;
    const int Quantum = 2*tUnit;
    Process* head_process = process;
    int time_remained = process->time_remained/tUnit, quantum_secs = Quantum/tUnit,i, Blocked = 0;
    time_t now;
    time(&now);
    char str_time[26];
    asctime_r(gmtime(&now),str_time);
    str_time[24] = '\0';
    printf("\t[%s] Processus %s de priorite %d [Execution] ", str_time,process->name,process->priority);
    for(i=0; i < time_remained && i < quantum_secs; i++, process->time_remained -= tUnit)
    {
        printf(".");
        traitement(tUnit);
        *queue_remained_time -= tUnit;
        if(*queue_remained_time < tUnit && (i + 1) < time_remained && (i + 1) < quantum_secs)
        {
            // If the current queue processing time is over but the current process still have
            // some processing time. The current process is marked ready (PRET).
            process->time_remained -= tUnit;
            strcpy(process->status,status[1]);
            time(&now);
            asctime_r(process->begin_time,str_time);
            str_time[24] = '\0';
            printf("\n\t[%s] ELU",str_time);
            asctime_r(gmtime(&now),str_time);
            str_time[24] = '\0';
            printf("\n\t[%s] PRET\n",str_time);
            return head_process;
        }
        if(frand(0,99) < 5)
        {
            // Any Elect (ELU) has 5% of chance to be blocked.
            process->time_remained -= tUnit;
            head_process = dispatcher(process,2);
            Blocked = 1;
            time(&now);
            asctime_r(process->begin_time,str_time);
            str_time[24] = '\0';
            printf("\n\t[%s] ELU", str_time);
            asctime_r(gmtime(&now),str_time);
            str_time[24] = '\0';
            printf("\n\t[%s] BLOQUE\n",str_time);
            break;
        }
    }
    if (time_remained == i)
    {
        head_process = dispatcher(process,3);
        asctime_r(process->begin_time,str_time);
        str_time[24] = '\0';
        printf("\n\t[%s] ELU", str_time);
        asctime_r(process->end_time,str_time);
        str_time[24] = '\0';
        printf("\n\t[%s] TERMINE\n",str_time);
    }
    else if(!Blocked)
    {
        head_process = dispatcher(process,1);
        asctime_r(process->begin_time,str_time);
        str_time[24] = '\0';
        printf("\n\t[%s] ELU", str_time);
        time(&now);
        asctime_r(gmtime(&now),str_time);
        str_time[24] = '\0';
        printf("\n\t[%s] PRET\n",str_time);
    }
    return head_process;
}
void traitement(const int execution_time)
{
    int secs = execution_time/tUnit;
    clock_t attente;
    attente = (secs * CLOCKS_PER_SEC) + clock();
    while(attente > clock());
}
