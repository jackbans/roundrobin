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

int main(void)
{

    srand(time(NULL));
    Process *processes = NULL, *new_process=NULL;
    const int processes_number = frand(16,50);
    time_t now;
    printf("\n\n   ==============================\n");
    printf("   || TOURNIQUET AVEC PRIORITE ||\n");
    printf("   ==============================\n");
    char str_time[26];
    time(&now);
    asctime_r(gmtime(&now),str_time); 
    str_time[24] = '\0';
    printf("\n[%s] Generation aleatoire de %d processus...", str_time, processes_number);
    for (int pid= 0; pid < processes_number; pid++)
    {
        new_process = new_process_with_random_data(pid + 1, processes_number);
        processes = enqueue_process(processes, new_process);
    }
    time(&now);
    asctime_r(gmtime(&now),str_time); 
    str_time[24] = '\0';
    printf("\n[%s] Generation terminee !",str_time);
    Queue *queues = NULL;
    time(&now);
    asctime_r(gmtime(&now),str_time); 
    str_time[24] = '\0';
    printf("\n\n[%s] Organisation des processus en files par priorite...",str_time);
    queues = manage_into_queues(processes);
    time(&now);
    asctime_r(gmtime(&now),str_time); 
    str_time[24] = '\0';
    printf("\n[%s] Organisation terminee !",str_time);
    Process* finished_processes = NULL;
    time(&now);
    asctime_r(gmtime(&now),str_time); 
    str_time[24] = '\0';
    printf("\n\n[%s] Execution des files d'attente...\n",str_time);
    finished_processes = ordonnanceur(queues, processes_number);
    time(&now);
    asctime_r(gmtime(&now),str_time); 
    str_time[24] = '\0';
    printf("\n\n[%s] Execution terminee AVEC SUCCES !!\n",str_time);
    time(&now);
    asctime_r(gmtime(&now),str_time); 
    str_time[24] = '\0';
    printf("\n[%s] Liberation de la memoire...",str_time);
    free_process(finished_processes);
    time(&now);
    asctime_r(gmtime(&now),str_time); 
    str_time[24] = '\0';
    printf("\n[%s] Memoire liberee !",str_time);
    printf("\n\n[%s] FIN AVEC SUCCES !\n\n",str_time);
    return 0;
}
