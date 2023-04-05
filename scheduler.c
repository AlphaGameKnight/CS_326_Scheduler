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
#define BLOCKED           'B' /* Process state is Blocked             */
#define FALSE             0   /* Constant for a false value           */
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
#define READY             'R' /* Process state is Ready               */
#define RUNNING           'N' /* Process state is ruNning             */
#define TRAILER_ALLOC_ERR 2   /* Trailer memory alloc error           */
#define TRUE              1   /* Constant for a true value            */
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
   /* Print the process table                                         */
PROCESS* create_process_list();
   /* Create an empty process list with a valid header and trailer    */
void initialize_scheduler(PROCESS *p_process_list);
   /* Initialize and populate a process table                         */
int check_if_running(PROCESS *p_process_list);
   /* Check if a process is running                                   */
void insert_process(PROCESS *p_process_list, int pid);
   /* Insert a new process at the beginning of the process list       */
int recalculate_priority(int old_priority, int quantum_used);
   /* Recalculate the priority of the runNing process                 */
int count_processes(PROCESS *p_process_list);
   /* Count all processes                                             */
void check_max_time(PROCESS *p_process_list);
   /* Remove process that has reached its max CPU time                */
void unblock_process(PROCESS *p_process_list);
   /* Check if a blocked process needs to be unblocked                */
void check_time(PROCESS *p_process_list, int clock_ticks);
   /* Check if a process needs to block                               */
void sort_processes(PROCESS* p_process_list);
   /* Sorts the processes by their priority                           */
void schedule_process(PROCESS *p_process_list);
   /* Schedule a process to run                                       */
void run_process(PROCESS *p_process_list);
   /* Give the CPU to another process                                 */
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
   PROCESS *p_process_list,     /* Points to an process list          */
           *p_process;          /* Points to an process in the list   */
   int     clock_ticks = 1,     /* Number of clock ticks by scheduler */
           next_pid;

   p_process_list = create_process_list();
   p_process = p_process_list;

   printf("\n\n\n\n\n\n");
   initialize_scheduler(p_process_list);
   
   //while (get_next_pid(p_process_list) < LIST_TRAILER)
   //{
      schedule_process(p_process_list);
   //}
      
   
   
   
   
   printf("\n\n\n\n\n\n");
   return 0;
}

/**********************************************************************/
/*             Print the BEFORE and AFTER process tables              */
/**********************************************************************/
void printTables(PROCESS *p_process_list, char *p_table)
{
   PROCESS *p_process; /* Points to an process in the process list    */

   p_process = p_process_list;

   printf("%s SCHEDULING CPU: Next PID = %d, Number of Processes = %d\n",
          p_table, get_next_pid(p_process),
          count_processes(p_process));
   printf("PID   CPU Used   MAX Time   STATE   PRI   QUANTUM USED    BLK TIME   WAIT TKS\n");

   while (p_process = p_process->p_next_process,
          p_process->pid != LIST_TRAILER)
   {
      printf(" %2d      %2d         %2d        %c      %2d         %d             %d          %2d\n",
             p_process->pid, p_process->cpu_used, p_process->max_time,
             p_process->state, p_process->priority, p_process->quantum_used,
             p_process->block_time, p_process->wait_ticks);
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
/*               Initialize and populate a process table              */
/**********************************************************************/
void initialize_scheduler(PROCESS *p_process_list)
{
   int counter; /* Counts the processes in the list                   */

   for(counter = 5; counter >= 1; counter--)
   {
      insert_process(p_process_list, counter);
   }

   return;
}

/**********************************************************************/
/*                    Check if a process is running                   */
/**********************************************************************/
int check_if_running(PROCESS *p_process_list)
{
   PROCESS *p_process;   /* Points to a process in the list           */
   int     running_flag; /* Flag determining if process is running    */

   p_process = p_process_list;
   while (p_process->p_next_process->pid != LIST_TRAILER)
   {
      if(p_process->p_next_process->state == 'N')
      {
         running_flag = TRUE;
         break;
      }
      else
      {
         running_flag = FALSE;
      }
      p_process = p_process->p_next_process;
   }
   
   return running_flag;
}

/**********************************************************************/
/*         Insert a new process at the end of the process list        */
/**********************************************************************/
void insert_process(PROCESS *p_process_list, int pid)
{
   PROCESS *p_new_process, /* Points to a newly created process       */
           *p_process,     /* Points to a process in the list         */
           *p_temp;        /* Temp process pointer for a swap         */
   int     counter;        /* Counts processes in the list            */

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

   for (counter = 1; counter <= count_processes(p_process_list);
                                                         counter++)
   {
      p_process = p_process_list;
      while (p_process->p_next_process->pid != LIST_TRAILER)
      {
         if (p_process->p_next_process->pid >
            p_process->p_next_process->p_next_process->pid)
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
   
   return;
}


/**********************************************************************/
/*           Recalculate the priority of the runNing process          */
/**********************************************************************/
int recalculate_priority(int old_priority, int quantum_used)
{
   float new_priority; /* New priority for a process                  */
   
   new_priority = ((abs(old_priority) + quantum_used)/2.0f) + 0.5f;

   return (int) new_priority;
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
/*         Check if a blocked process needs to be unblocked           */
/**********************************************************************/
void unblock_process(PROCESS *p_process_list)
{
   PROCESS *p_process; /* Points to one process                       */

   p_process = p_process_list;
   while (p_process->p_next_process->pid != LIST_TRAILER)
   {
      if (p_process->p_next_process->state == 'B' &&
         ((rand() % 20 + 1) == 1))
      {
         p_process->p_next_process->state = 'R';
         p_process->p_next_process->cpu_used += 1;
         p_process = p_process->p_next_process; 
      }
      p_process = p_process->p_next_process;
   }
   
   return;
}

/**********************************************************************/
/*               Sorts the processes by their priority                */
/**********************************************************************/
void sort_processes(PROCESS* p_process_list)
{
   int  counter;        /* Number of times through the list           */
   PROCESS *p_process,  /* Points to a process in the account list    */
           *p_temp;     /* Temp process pointer for a swap            */

   for(counter = 0; counter < count_processes(p_process_list); counter++)
   {
      p_process = p_process_list;
      while (p_process->p_next_process->p_next_process->pid != LIST_TRAILER)
      {
         if (p_process->p_next_process->priority > 
             p_process->p_next_process->p_next_process->priority &&
             p_process->p_next_process->priority > INITIAL_PRI)
         {
            p_temp                    = p_process->p_next_process->p_next_process;
            p_process->p_next_process->p_next_process 
                                    = 
               p_process->p_next_process->p_next_process->p_next_process;
            p_temp->p_next_process    = p_process->p_next_process;
            p_process->p_next_process = p_temp;
         }
         else
         {
            if (p_process->p_next_process->priority < 
                p_process->p_next_process->p_next_process->priority &&
                p_process->p_next_process->priority < INITIAL_PRI   &&
                p_process->p_next_process->p_next_process->priority < INITIAL_PRI)
            {
               p_temp                    = p_process->p_next_process->p_next_process;
               p_process->p_next_process->p_next_process 
                                         = 
                  p_process->p_next_process->p_next_process->p_next_process;
               p_temp->p_next_process    = p_process->p_next_process;
               p_process->p_next_process = p_temp;
            }
            else
            {
               if (p_process->p_next_process->priority == INITIAL_PRI  && 
                   p_process->p_next_process->p_next_process->priority < INITIAL_PRI)
               {
                  p_temp                    = p_process->p_next_process->p_next_process;
                  p_process->p_next_process->p_next_process 
                                          = 
                     p_process->p_next_process->p_next_process->p_next_process;
                  p_temp->p_next_process    = p_process->p_next_process;
                  p_process->p_next_process = p_temp;
               }
               else
               {
                  if((p_process->p_next_process->priority ==
                        p_process->p_next_process->p_next_process->priority) &&
                     (p_process->p_next_process->pid >
                        p_process->p_next_process->p_next_process->pid) &&
                      p_process->p_next_process->p_next_process->priority > INITIAL_PRI)
                  {
                     p_temp                    = p_process->p_next_process->p_next_process;
                     p_process->p_next_process->p_next_process 
                                             = 
                        p_process->p_next_process->p_next_process->p_next_process;
                     p_temp->p_next_process    = p_process->p_next_process;
                     p_process->p_next_process = p_temp;
                  }
               }
            }
         }
         p_process = p_process->p_next_process;
      }
   }
   
   return;
}

/**********************************************************************/
/*                     Schedule a process to run                      */
/**********************************************************************/
void schedule_process(PROCESS *p_process_list)
{
   PROCESS *p_process, /* Points to a process in the list             */
           *p_previous_process;
                       /* Points to the process before p_process      */
   int     counter;    /* Counts the processes in the list            */

   for (counter = 0; counter < 270; counter++)
   {
      p_process = p_process_list;

      if (count_processes(p_process_list) < 10)
      {
         add_process(p_process_list);
         unblock_process(p_process_list);
         sort_processes(p_process_list);
      }

      while (p_process->p_next_process->pid != LIST_TRAILER &&
             check_if_running(p_process_list) == FALSE)
      {  
         if (p_process->p_next_process->state == READY)
         {
            sort_processes(p_process_list);
            printTables(p_process_list, BEFORE);
            p_process->p_next_process->state = RUNNING;
            printTables(p_process_list, AFTER);
            break;
         }
         else
         {
            p_process = p_process->p_next_process;
         }
         sort_processes(p_process_list);
      }

      p_previous_process = p_process_list;
      while (p_process = p_previous_process->p_next_process,
             p_process->pid != LIST_TRAILER)
      {
         if (p_process->state == RUNNING)
         {
            if(p_process->cpu_used == p_process->max_time)
            {
               printTables(p_process_list, BEFORE);
               p_previous_process->p_next_process = p_process->p_next_process;
               free(p_process);
               run_process(p_process_list);
               printTables(p_process_list, AFTER);
            }

            if (p_process->quantum_used == p_process->block_time)
            {
               if (p_process->quantum_used < 6 &&
                   p_process->quantum_used > INITIAL_QUANTUM)
               {
                  printTables(p_process_list, BEFORE);
                  p_process->state = BLOCKED;
                  p_process->priority = -(recalculate_priority(p_process->priority, p_process->quantum_used));
                  p_process->quantum_used = INITIAL_QUANTUM;
                  run_process(p_process_list);
                  sort_processes(p_process_list);
                  printTables(p_process_list, AFTER);
               }
               else if (p_process->quantum_used == 6)
               {
                  printTables(p_process_list, BEFORE);
                  p_process->state = READY;
                  p_process->priority = recalculate_priority(p_process->priority, p_process->quantum_used);
                  sort_processes(p_process_list);
                  p_process->quantum_used = 0;
                  run_process(p_process_list);
                  printTables(p_process_list, AFTER);
               }
            }
            else
            {
               p_process->cpu_used += 1;
               printf("\n\n CPU: %d", p_process->cpu_used);
               p_process->quantum_used += 1;
               printf("\n\n QUA: %d", p_process->quantum_used);
            }
         }
         else if (p_process->state == READY)
         {
            p_process->wait_ticks += 1;
         }
         p_previous_process = p_process;
      }
   }
   
   return;
}

/**********************************************************************/
/*                   Give the CPU to another process                  */
/**********************************************************************/
void run_process(PROCESS *p_process_list)
{
   PROCESS *p_process; /* Points to a process in the list             */
   
   p_process = p_process_list;

   while (p_process->p_next_process->pid != LIST_TRAILER)
   {  
      if (p_process->p_next_process->state == READY &&
          check_if_running(p_process_list) == FALSE)
      {
         p_process->p_next_process->state = RUNNING;
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
   PROCESS *p_process; /* Points to a process in the list             */

   if (rand() % 5 + 1 == 1)
   {
      insert_process(p_process_list, get_next_pid(p_process_list));
   }

   return;
}

/**********************************************************************/
/*              Get the biggest PID in the process table              */
/**********************************************************************/
int get_next_pid(PROCESS *p_process_list)
{
   PROCESS *p_process; /* Points to a process in the list             */
   int largest_pid;    /* Largest PID value in the list               */
   
   largest_pid = 0;

   p_process = p_process_list;

   while (p_process->p_next_process->pid != LIST_TRAILER)
   {
      if (largest_pid < p_process->p_next_process->pid)
      {
         largest_pid = p_process->p_next_process->pid;
      }
      p_process = p_process->p_next_process;
   }

   return largest_pid + 1;
}