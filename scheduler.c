/**********************************************************************/
/*                                                                    */
/* Program Name: scheduler - Simulate an operating system scheduler   */
/* Author:       William S. Wu                                        */
/* Installation: Pensacola Christian College, Pensacola, Florida      */
/* Course:       CS326, Operating Systems                             */
/* Date Written: March 26, 2023                                       */
/*                                                                    */
/**********************************************************************/

/**********************************************************************/
/*                                                                    */
/* I pledge this assignment is my own first time work.                */
/* I pledge I did not copy or try to copy work from the Internet.     */
/* I pledge I did not copy or try to copy work from any student.      */
/* I pledge I did not copy or try to copy work from any where else.   */
/* I pledge the only person I asked for help from was my teacher.     */
/* I pledge I did not attempt to help any student on this assignment. */
/* I understand if I violate this pledge I will receive a 0 grade.    */
/*                                                                    */
/*                                                                    */
/*                      Signed: _____________________________________ */
/*                                           (signature)              */
/*                                                                    */
/*                                                                    */
/**********************************************************************/

/**********************************************************************/
/*                                                                    */
/* This program simulates the environement in UNIX where new          */
/* processes are continually arriving, existing processes are vying   */
/* for the CPU, processes are using their given quantum (CPU bound)   */
/* or blocking because of I/O operations, and processees are          */
/* terminating when their work is finished.                           */
/*                                                                    */
/**********************************************************************/

#include <stdio.h>
#include <ctype.h>
#include <math.h>

/**********************************************************************/
/*                         Symbolic Constants                         */
/**********************************************************************/
#define HEADER_ALLOC_ERR  1   /* Header memory alloc error        */
#define INITIAL_CPU       0   /* No CPU time used yet             */
#define INITIAL_PID       1   /* PID of first process             */
#define INITIAL_PRI       0   /* Initial process priority         */
#define INITIAL_QUANTUM   0   /* No quantum used yet              */
#define INITIAL_STATE     'R' /* Initial process state (Ready)    */
#define INITIAL_TICKS     0   /* Process not waiting for CPU      */
#define LIST_HEADER       MIN_PID - 1 
                              /* Lowest possible pid minus one    */
#define LIST_TRAILER      MAX_PID + 1 
                              /* Highest possible pid plus one    */
#define MAX_PID           100 /* Highest possbile pid             */
#define MIN_PID           1   /* Lowest possible pid              */
#define PROCESS_ALLOC_ERR 3   /* Process memory alloc error       */
#define TRAILER_ALLOC_ERR 2   /* Trailer memory alloc error       */


/**********************************************************************/
/*                         Program Structures                         */
/**********************************************************************/
/* A process table record                                             */
struct process_record
{
   int block_time,   /* Clock ticks used until process blocks         */
       cpu_used,     /* Total clock ticks used by the process so far  */
       pid,          /* Process ID assigned when new process arrives  */
       max_time,     /* Max CPU time needed by process                */
       priority,     /* Priority of the process                       */
       quantum_used, /* Amount of quantum used by the process         */
       wait_ticks;   /* Total clock ticks the process has waited      */
   char state;       /* State of the process                          */
   struct process_record *p_next_process;
                     /* Points to the next process in the table       */
};
typedef struct process_record PROCESS;

/**********************************************************************/
/*                         Function Prototypes                        */
/**********************************************************************/
PROCESS* create_process_list();
   /* Create an empty process list with a valid header and trailer    */
void initialize_program(PROCESS process);
   /* Initialize and populate a process table                         */
int calculate_priority(int old_priority, int quantum_used);
   /* Recalculate the priority of the runNing process                 */

/**********************************************************************/
/*                            Main Function                           */
/**********************************************************************/
int main()
{

   return 0;
}

/**********************************************************************/
/*                    Create an empty process table                   */
/**********************************************************************/
PROCESS* create_process_list()
{
   PROCESS *p_process_list; /* Points to a newly created account list */

   if ((p_process_list = (PROCESS *)malloc(sizeof(PROCESS))) == NULL)
   {
      printf("\nError number %d occurred in create_process_list function.",
             HEADER_ALLOC_ERR);
      printf("\nUnable to allocate memory for header node.");
      printf("\nProgram is aborting.");
      exit(HEADER_ALLOC_ERR);
   }
   p_process_list->pid = LIST_HEADER;

   if ((p_process_list->p_next_process = (PROCESS *)
            malloc(sizeof(PROCESS))) == NULL)
   {
      printf("\nError number %d occurred in create_process_list function.",
             TRAILER_ALLOC_ERR);
      printf("\nUnable to allocate memory for trailer node.");
      printf("\nProgram is aborting.");
      exit(TRAILER_ALLOC_ERR);
   }
   p_process_list->p_next_process->pid = LIST_TRAILER;
   p_process_list->p_next_process->pid = NULL;

   return p_process_list;
}

/**********************************************************************/
/*      Insert a new process at the beginning of the process list     */
/**********************************************************************/
void insert_process(PROCESS *p_process_list, int pid)
{
   PROCESS *p_new_process; /* Points to a newly created process       */

   if ((p_new_process = (PROCESS *)malloc(sizeof(PROCESS))) == NULL)
   {
      printf("\nError number %d occurred in insert_process function.",
             PROCESS_ALLOC_ERR);
      printf("\nUnable to allocate memory for an process node.");
      printf("\nProgram is aborting.");
      exit(PROCESS_ALLOC_ERR);
   }
   p_new_process->p_next_process = p_process_list->p_next_process;
   p_new_process->pid = pid;

   if (((rand() % 100 + 1) / 100.0f) < (1.0f / 3.0f))
   {
      p_new_process->block_time = 6;
   }
   else
   {
      p_new_process->block_time = (rand() % 5 + 1);
   }

   p_new_process->cpu_used     = INITIAL_CPU;
   p_new_process->max_time     = (rand() % 18 + 1);
   p_new_process->priority     = INITIAL_PRI;
   p_new_process->quantum_used = INITIAL_QUANTUM;
   p_new_process->wait_ticks   = INITIAL_TICKS;
   p_new_process->state        = INITIAL_STATE;
   p_new_process->p_next_process = p_new_process;

   return;
}

/**********************************************************************/
/*               Initialize and populate a process table              */
/**********************************************************************/
void initialize_program(PROCESS process)
{
   
   return;
}

/**********************************************************************/
/*           Recalculate the priority of the runNing process          */
/**********************************************************************/
int calculate_priority(int old_priority, int quantum_used)
{
   return (abs(old_priority) + quantum_used)/2;
}