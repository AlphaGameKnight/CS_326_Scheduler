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
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

/**********************************************************************/
/*                         Symbolic Constants                         */
/**********************************************************************/
#define HEADER_ALLOC_ERR  1   /* Header memory alloc error            */
#define INITIAL_CPU       0   /* No CPU time used yet                 */
#define INITIAL_PID       1   /* PID of first process                 */
#define INITIAL_PRI       0   /* Initial process priority             */
#define INITIAL_QUANTUM   0   /* No quantum used yet                  */
#define INITIAL_STATE     'R' /* Initial process state (Ready)        */
#define INITIAL_TICKS     0   /* Process not waiting for CPU          */
#define LIST_HEADER       MIN_PID - 1 
                              /* Lowest possible pid minus one        */
#define LIST_TRAILER      MAX_PID + 1 
                              /* Highest possible pid plus one        */
#define MAX_PID           99  /* Highest possbile pid                 */
#define MIN_PID           1   /* Lowest possible pid                  */
#define PROCESS_ALLOC_ERR 3   /* Process memory alloc error           */
#define TRAILER_ALLOC_ERR 2   /* Trailer memory alloc error           */
#define BEFORE            "BEFORE"
#define AFTER             "AFTER"

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
void printTables(PROCESS *p_process_list, char *p_table);
   /* Print the BEFORE and AFTER process tables                       */
PROCESS* create_process_list();
   /* Create an empty process list with a valid header and trailer    */
void initialize_scheduler(PROCESS *p_process_list);
   /* Initialize and populate a process table                         */
void insert_process(PROCESS *p_process_list, int pid);
   /* Insert a new process at the beginning of the process list       */
int calculate_priority(int old_priority, int quantum_used);
   /* Recalculate the priority of the runNing process                 */
int count_processes(PROCESS *p_process_list);
   /* Count all account records                                       */
void check_max_time(PROCESS *p_process_list);
   /* Remove process that has reached its max CPU time                */
void check_blocked(PROCESS *p_process_list);
   /* Check if a blocked process needs to be unblocked                */
void check_time(PROCESS *p_process_list, int clock_ticks);
   /* Check if a process needs to block                               */
void sort_processes(PROCESS* p_process_list, int number_of_processes);
   /* Sorts the processes by their priority                           */
void schedule_process(PROCESS *p_process_list);
   /* Schedule a process to run                                       */
void check_preemption(PROCESS *p_process_list);
   /* Check if a process needs to be preempted                        */
void add_process(PROCESS *p_process_list);
   /* Add a new process to the process table                          */
int get_next_pid(PROCESS *p_process_list);
   /* Get the biggest PID in the process table                        */

/**********************************************************************/
/*                            Main Function                           */
/**********************************************************************/
int main()
{
   PROCESS *p_next_process,     /* Points to one process              */
           *p_process_list,     /* Points to an process list          */
           *p_process,          /* Points to an process in the list   */
           *p_previous_account; /* Points to previous process         */
   int     clock_ticks = 1,     /* Number of clock ticks by scheduler */
           pid = 1;

   p_process_list = create_process_list();
   p_process = p_process_list;

   printf("\n\n\n\n\n\n");
   initialize_scheduler(p_process_list);
   printTables(p_process_list, BEFORE);
   
   while (get_next_pid(p_process_list) < 10)
   {
      if (count_processes(p_process_list) < 10)
      {
         add_process(p_process_list);
         schedule_process(p_process_list, pid);
      }

      check_blocked(p_process_list);
      clock_ticks++;
      printf("\n%d\n", clock_ticks);
   }
   printf("\n%d\n", clock_ticks);
   
   printTables(p_process_list, AFTER);
   printf("\n\n\n\n\n\n");
   return 0;
}

/**********************************************************************/
/*             Print the BEFORE and AFTER process tables              */
/**********************************************************************/
void printTables(PROCESS *p_process_list, char *p_table)
{
   PROCESS *p_process; /* Points to an process in the process list    */

   printf("%s SCHEDULING CPU: Next PID = %d, Number of Processes = %d\n",
          p_table, get_next_pid(p_process_list),
          count_processes(p_process_list));
   printf("PID   CPU Used   MAX Time   STATE   PRI   QUANTUM USED    BLK TIME   WAIT TKS\n");

   while (p_process_list = p_process_list->p_next_process,
          p_process_list->pid != LIST_TRAILER)
   {
      printf(" %2d      %2d         %2d        %c       %d         %d             %d           %d\n",
             p_process_list->pid, p_process_list->cpu_used, p_process_list->max_time,
             p_process_list->state, p_process_list->priority, p_process_list->quantum_used,
             p_process_list->block_time, p_process_list->wait_ticks);
   }
   printf("\n");

   return;
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
   p_process_list->p_next_process->p_next_process = NULL;

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
   p_process_list->p_next_process = p_new_process;

   return;
}

/**********************************************************************/
/*               Initialize and populate a process table              */
/**********************************************************************/
void initialize_scheduler(PROCESS *p_process_list)
{
   int process_counter;

   for(process_counter = 5; process_counter >= 1; process_counter--)
   {
      insert_process(p_process_list, process_counter);
   }

   return;
}

/**********************************************************************/
/*           Recalculate the priority of the runNing process          */
/**********************************************************************/
int calculate_priority(int old_priority, int quantum_used)
{
   return (abs(old_priority) + quantum_used)/2;
}

/**********************************************************************/
/*                   Counts all account records                       */
/**********************************************************************/
int count_processes(PROCESS *p_process_list)
{
   int total_processes = 0; /* Number of processes in the table       */

   while (p_process_list = p_process_list->p_next_process,
          p_process_list->pid != LIST_TRAILER)
   {
      total_processes += 1;
   }

   return total_processes;
}

/**********************************************************************/
/*          Remove process that has reached its max CPU time          */
/**********************************************************************/
void check_max_time(PROCESS *p_process_list)
{
   PROCESS *p_process, /* Points to an process in the process list    */
           *p_previous_process;
                       /* Points to the process before p_process      */

   p_previous_process = p_process_list;
   p_process = p_previous_process->p_next_process;

   while (p_process->p_next_process->pid != LIST_TRAILER)
   {
      if (p_process->quantum_used >= p_process->max_time)
      {
         p_previous_process->p_next_process = p_process->p_next_process;

         free(p_process);
         printTables(p_process_list, BEFORE);
         printTables(p_process_list, AFTER);
      }
      else
      {
         p_previous_process = p_process;
      }
      p_process = p_process->p_next_process;
   }
      

   return;
}

/**********************************************************************/
/*         Check if a blocked process needs to be unblocked           */
/**********************************************************************/
void check_blocked(PROCESS *p_process_list)
{
   PROCESS *p_process; /* Points to one process                       */

   p_process = p_process_list;
   while (p_process->p_next_process->pid != LIST_TRAILER)
   {
      if (p_process->state == 'B' && ((rand() % 20 + 1) == 1))
      {
         p_process->state = 'R';
         printTables(p_process_list, BEFORE);
         printTables(p_process_list, AFTER);
      }

      p_process = p_process->p_next_process;
   }
   
   return;
}

/**********************************************************************/
/*                 Check if a process needs to block                  */
/**********************************************************************/
void check_time(PROCESS *p_process_list, int clock_ticks)
{
   PROCESS *p_process; /* Points to a process                         */

   p_process = p_process_list;

   while (p_process->p_next_process->pid != LIST_TRAILER)
   {
      if (p_process->cpu_used == p_process->block_time)
      {
         p_process->state = 'B';
         p_process->priority -= 1;
         printTables(p_process_list, BEFORE);
         printTables(p_process_list, AFTER);
      }

      p_process = p_process->p_next_process;
   }

   return;
}

/**********************************************************************/
/*               Sorts the processes by their priority                */
/**********************************************************************/
void sort_processes(PROCESS* p_process_list, int number_of_processes)
{
   int  counter;        /* Number of times through the list           */
   PROCESS *p_process,  /* Points to a process in the account list    */
           *p_temp,     /* Temp process pointer for a swap            */
           *p_negative, /* Points to negative priority process list   */
           *p_temp_negative,
           *p_negative_process;
   
   p_negative = create_process_list();

   for (counter = 1; counter <= number_of_processes - 1; counter++)
   {
      p_process = p_process_list;
      
      while (p_process->p_next_process->p_next_process->pid 
                                                        != LIST_TRAILER)
      {
        if (p_process->p_next_process->priority >
            p_process->p_next_process->p_next_process->priority)
         {
            p_temp                    = p_process->p_next_process->p_next_process;
            p_process->p_next_process->p_next_process 
                                      = 
               p_process->p_next_process->p_next_process->p_next_process;
            p_temp->p_next_process    = p_process->p_next_process;
            p_process->p_next_process = p_temp;
         }
         
         p_process = p_process->p_next_process;
      }
   }

   // Starts hooking up negative priority processes to negative list
   if(p_process_list->p_next_process->priority < INITIAL_PRI)
   {
      p_process = p_process_list;
      p_negative_process = p_negative;
      p_temp    = p_negative->p_next_process; // storing trailer location
      p_negative_process->p_next_process = p_process->p_next_process;
      while (p_negative_process->p_next_process->priority < INITIAL_PRI)
      {
         p_negative_process = p_negative_process->p_next_process;
      }
      p_temp_negative = p_negative_process->p_next_process; // storing location of priority 0
      p_negative_process->p_next_process = p_temp; // connects to the negative trailer

      // sorting negative priorities
      for (counter = 1; counter <= count_processes(p_negative) - 1; counter++)
      {
         p_negative_process = p_negative;
         
         while (p_negative_process->p_next_process->p_next_process->pid 
                                                         != LIST_TRAILER)
         {
            if (abs(p_negative_process->p_next_process->priority) >
                abs(p_negative_process->p_next_process->p_next_process->priority))
            {
               p_temp                     = p_negative_process->p_next_process->p_next_process;
               p_negative_process->p_next_process->p_next_process 
                                          = 
                  p_negative_process->p_next_process->p_next_process->p_next_process;
               p_temp->p_next_process     = p_negative_process->p_next_process;
               p_negative_process->p_next_process = p_temp;
            }   
            p_negative_process = p_negative->p_next_process;
         }
      }
   }
   
   return;
}

/**********************************************************************/
/*                     Schedule a process to run                      */
/**********************************************************************/
void schedule_process(PROCESS *p_process_list)
{
   PROCESS *p_process;

   p_process = p_process_list;
   
   while (p_process-> p_next_process->pid != LIST_TRAILER)
   {
      if(p_process->p_next_process->state == 'R')
      {
         p_process->p_next_process->state = 'N';
         printTables(p_process_list, BEFORE);
         printTables(p_process_list, AFTER);
      }

      p_process = p_process->p_next_process;
   }
   

   return;
}

/**********************************************************************/
/*              Check if a process needs to be preempted              */
/**********************************************************************/
void check_preemption(PROCESS *p_process_list)
{
   PROCESS *p_process; /* Points to one process                       */

   p_process =  p_process_list;

   while (p_process->p_next_process->pid != LIST_TRAILER)
   {
      if (p_process->state == 'B' || p_process->quantum_used == p_process->block_time)
      {
         p_process->priority = calculate_priority(p_process->block_time, p_process->quantum_used);
         sort_processes(p_process_list, count_processes(p_process_list));
         printTables(p_process_list, BEFORE);
         printTables(p_process_list, AFTER);
      }
      p_process = p_process->p_next_process;
   }

      return;
}

/**********************************************************************/
/*              Add a new process to the process table                */
/**********************************************************************/
void add_process(PROCESS *p_process_list)
{
   int new_pid;        /* PID of the new process                      */
   PROCESS *p_process; /* Points to one process                       */

   if (rand() % 5 + 1 == 1)
   {
      insert_process(p_process_list, get_next_pid(p_process_list));
      printTables(p_process_list, BEFORE);
      printTables(p_process_list, AFTER);
   }

   return;
}

/**********************************************************************/
/*              Get the biggest PID in the process table              */
/**********************************************************************/
int get_next_pid(PROCESS *p_process_list)
{
   PROCESS *p_process;
   int max_pid;
   
   max_pid = 0;

   p_process = p_process_list;

   while (p_process->p_next_process->pid != LIST_TRAILER)
   {
      if (max_pid < p_process->p_next_process->pid)
      {
         max_pid = p_process->p_next_process->pid;
      }
      p_process = p_process->p_next_process;
   }

   return max_pid + 1;
}