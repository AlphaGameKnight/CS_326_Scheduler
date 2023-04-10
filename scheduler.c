/**********************************************************************/
/*                                                                    */
/* Program Name: scheduler - Simulate an operating system scheduler   */
/* Author:       William S. Wu                                        */
/* Installation: Pensacola Christian College, Pensacola, Florida      */
/* Course:       CS326, Operating Systems                             */
/* Date Written: April 9, 2023                                        */
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
#define AFTER             "AFTER"
                              /* Process table after scheduling       */
#define BEFORE            "BEFORE"
                              /* Process table before scheduling      */
#define BLOCKED           'B' /* Process state is Blocked             */
#define FALSE             0   /* Constant for a false value           */
#define HEADER_ALLOC_ERR  1   /* Header memory alloc error            */
#define INITIAL_CPU       0   /* Initial process CPU time used        */
#define INITIAL_PID       5   /* End process PID after initialization */
#define INITIAL_PRI       0   /* Initial process priority             */
#define INITIAL_QUANTUM   0   /* Initial process quantum used         */
#define INITIAL_STATE     'R' /* Initial process state (Ready)        */
#define INITIAL_TICKS     0   /* Initial process wait ticks for CPU   */
#define LIST_HEADER       MIN_PID - 1 
                              /* Lowest possible PID minus one        */
#define LIST_TRAILER      MAX_PID + 1 
                              /* Highest possible PID plus one        */
#define MAX_PID           99  /* Highest possbile PID                 */
#define MIN_PID           1   /* Lowest possible PID                  */
#define PROCESS_ALLOC_ERR 3   /* Process memory alloc error           */
#define READY             'R' /* Process state is Ready               */
#define RUNNING           'N' /* Process state is ruNning             */
#define TRAILER_ALLOC_ERR 2   /* Trailer memory alloc error           */
#define TRUE              1   /* Constant for a true value            */

/**********************************************************************/
/*                         Program Structures                         */
/**********************************************************************/
/* A process table record                                             */
struct process_record
{
   int block_time,   /* Clock ticks used until process blocks         */
       cpu_used,     /* Total clock ticks used by the process so far  */
       max_time,     /* Max CPU time needed by process                */
       pid,          /* Process ID assigned when new process arrives  */
       priority,     /* Priority of the process                       */
       quantum_used, /* Amount of quantum used by the process         */
       wait_ticks;   /* Total clock ticks the process waited for CPU  */
   char state;       /* State of the process                          */
   struct process_record *p_next_process;
                     /* Points to the next process in the table       */
};
typedef struct process_record PROCESS;

/**********************************************************************/
/*                         Function Prototypes                        */
/**********************************************************************/
PROCESS* create_process_table();
   /* Create an empty process table                                   */
void insert_process(PROCESS *p_process_table, int pid);
   /* Insert a new process at the end of the process queue            */
int  count_processes(PROCESS *p_process_table);
   /* Count the number of processes in the table                      */
void sort_by_pid(PROCESS *p_process_table);
   /* Sort processes by their PID                                     */
int  get_next_pid(PROCESS *p_process_table, int *p_pid);
   /* Get the PID of the next process to enter the table              */
void print_process_table(PROCESS *p_process_table, char *p_table_time,
                                                               int pid);
   /* Print the process table                                         */
void initialize_program(PROCESS *p_process_table);
   /* Initialize the process table with 5 Ready processes             */
void add_process(PROCESS *p_process_table, int *p_next_pid);
   /* Add a new process to the process table                          */
void unblock_process(PROCESS *p_process_table);
   /* Check if a blocked process needs to be unblocked                */
void sort_by_priority(PROCESS* p_process_table);
   /* Sort the processes by their priority                            */
int check_if_running(PROCESS *p_process_table);
   /* Check if a process is running                                   */
int check_all_blocked(PROCESS *p_process_table);
   /* Check if all processes are blocked                              */
void schedule_process(PROCESS *p_process_table, int pid);
   /* Schedule the next Ready process to run                          */
void run_process(PROCESS *p_process_table);
   /* Give the CPU to another process                                 */
void terminate_process(PROCESS *p_process_table, int pid);
   /* Terminate a process from the process table                      */
int recalculate_priority(int old_priority, int quantum_used);
   /* Recalculate the priority of the runNing process                 */
void preempt_process(PROCESS *p_process_table, int pid);
   /* Preempt a ruNning process                                       */

/**********************************************************************/
/*                            Main Function                           */
/**********************************************************************/
int main()
{
   PROCESS *p_process_table; /* Points to the process table           */
   int     pid;              /* Last process PID after initialization */
   
   p_process_table = create_process_table();
   pid = INITIAL_PID;

   printf("\n\n\n\n\n\n");
   initialize_program(p_process_table);

   while (get_next_pid(p_process_table, &pid) <= LIST_TRAILER)
   {
      if (count_processes(p_process_table) < 10)
      {
         add_process(p_process_table, &pid);
      }

      unblock_process(p_process_table);
      schedule_process(p_process_table, pid);
      terminate_process(p_process_table, pid);
   }
   
   printf("\n\n\n\n\n\n");
   return 0;
}

/**********************************************************************/
/*                    Create an empty process table                   */
/**********************************************************************/
PROCESS* create_process_table()
{
   PROCESS *p_process_table; /* Points to a new process table         */

   if ((p_process_table = (PROCESS *)malloc(sizeof(PROCESS))) == NULL)
   {
      printf("\nError number %d occurred in create_process_table function.",
         HEADER_ALLOC_ERR);
      printf("\nUnable to allocate memory for header node.");
      printf("\nProgram is aborting.");
      exit(HEADER_ALLOC_ERR);
   }
   p_process_table->pid = LIST_HEADER;

   if ((p_process_table->p_next_process = (PROCESS *)
            malloc(sizeof(PROCESS))) == NULL)
   {
      printf("\nError number %d occurred in create_process_table function.",
         TRAILER_ALLOC_ERR);
      printf("\nUnable to allocate memory for trailer node.");
      printf("\nProgram is aborting.");
      exit(TRAILER_ALLOC_ERR);
   }
   p_process_table->p_next_process->pid = LIST_TRAILER;
   p_process_table->p_next_process->p_next_process = NULL;

   return p_process_table;
}

/**********************************************************************/
/*        Insert a new process at the end of the process queue        */
/**********************************************************************/
void insert_process(PROCESS *p_process_table, int pid)
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
   
   p_new_process->p_next_process = p_process_table->p_next_process;
   p_new_process->pid = pid;

   if (((rand() % 100 + 1) / 100.0f) <= (1.0f / 3.0f))
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
   p_process_table->p_next_process = p_new_process;

   sort_by_pid(p_process_table);

   return;
}

/**********************************************************************/
/*             Count the number of processes in the table             */
/**********************************************************************/
int count_processes(PROCESS *p_process_table)
{
   PROCESS *p_process;      /* Points to a process in the table       */
   int     total_processes; /* Total number of processes in the table */

   total_processes = 0;
   p_process = p_process_table;
   while (p_process = p_process->p_next_process,
          p_process->pid != LIST_TRAILER)
   {
      total_processes += 1;
   }

   return total_processes;
}

/**********************************************************************/
/*                     Sort processes by their PID                    */
/**********************************************************************/
void sort_by_pid(PROCESS *p_process_table)
{
   PROCESS *p_process, /* Points to a process in the table            */
           *p_temp;    /* Temporary process for the swap              */  
   int     counter;    /* Counts through every process in the table   */

   for (counter = 1; counter <= count_processes(p_process_table);
                                                              counter++)
   {
      p_process = p_process_table;
      while (p_process->p_next_process->pid != LIST_TRAILER)
      {
         if (p_process->p_next_process->pid >
            p_process->p_next_process->p_next_process->pid)
         {
            p_temp                    =
               p_process->p_next_process->p_next_process;
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
/*         Get the PID of the next process to enter the table         */
/**********************************************************************/
int get_next_pid(PROCESS *p_process_table, int *p_pid)
{
   PROCESS *p_process; /* Points to a process in the table            */
   int largest_pid;    /* Largest PID value in the process table      */
   
   largest_pid = *p_pid;

   p_process = p_process_table;
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

/**********************************************************************/
/*                       Print the process table                      */
/**********************************************************************/
void print_process_table(PROCESS *p_process_table, char *p_table_time,
                         int pid)
{
   PROCESS *p_process; /* Points to a process in the table            */

   p_process = p_process_table;

   printf(" %s SCHEDULING CPU: Next PID = %d, Number of Processes = %d\n",
          p_table_time, pid,
          count_processes(p_process));
   printf(" PID   CPU Used   MAX Time   STATE   PRI   QUANTUM USED    ");
   printf("BLK TIME   WAIT TKS\n");

   while (p_process = p_process->p_next_process,
          p_process->pid != LIST_TRAILER)
   {
      printf("  %2d      %2d         %2d        %c      %2d",
             p_process->pid, p_process->cpu_used, p_process->max_time,
             p_process->state, p_process->priority);
      printf("         %d             %d         %3d\n",
             p_process->quantum_used, p_process->block_time,
             p_process->wait_ticks);
   }
   printf("\n");

   return;
}

/**********************************************************************/
/*         Initialize the process table with 5 Ready processes        */
/**********************************************************************/
void initialize_program(PROCESS *p_process_table)
{
   int counter; /* Counts through the processes in the table          */
   
   for(counter = 1; counter <= 5; counter++)
   {
      insert_process(p_process_table, counter);
   }

   return;
}

/**********************************************************************/
/*              Add a new process to the process table                */
/**********************************************************************/
void add_process(PROCESS *p_process_table, int *p_next_pid)
{
   int next_pid; /* PID value of the next process added to table      */
   
   if (rand() % 5 + 1 == 1)
   {
      next_pid = *p_next_pid;
      if (next_pid < get_next_pid(p_process_table, p_next_pid))
      {
         *p_next_pid = get_next_pid(p_process_table, p_next_pid);
         next_pid = *p_next_pid;
      }
      
      insert_process(p_process_table, next_pid);
      sort_by_priority(p_process_table);
   }

   return;
}

/**********************************************************************/
/*          Check if a blocked process needs to be unblocked          */
/**********************************************************************/
void unblock_process(PROCESS *p_process_table)
{
   PROCESS *p_process; /* Points to a process in the table            */

   p_process = p_process_table;
   while (p_process->p_next_process->pid != LIST_TRAILER)
   {
      if (p_process->p_next_process->state == BLOCKED &&
         ((rand() % 20 + 1) == 1))
      {
         p_process->p_next_process->state = READY;
      }
      p_process = p_process->p_next_process;
   }

   return;
}

/**********************************************************************/
/*                Sort the processes by their priority                */
/**********************************************************************/
void sort_by_priority(PROCESS* p_process_table)
{
   int  counter;       /* Counts through the processes in the table   */
   PROCESS *p_process, /* Points to a process in the table            */
           *p_temp;    /* Temporary process for the swap              */

   for(counter = 0; counter <= count_processes(p_process_table) - 1;
                                                              counter++)
   {
      p_process = p_process_table;
      while (p_process->p_next_process->p_next_process->pid != 
                                                           LIST_TRAILER)
      {
         if (p_process->p_next_process->priority > 
             p_process->p_next_process->p_next_process->priority &&
             p_process->p_next_process->priority > INITIAL_PRI)
         {
            p_temp                    =
               p_process->p_next_process->p_next_process;
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
                p_process->p_next_process->p_next_process->priority < 
                                                            INITIAL_PRI)
            {
               p_temp                    = 
                  p_process->p_next_process->p_next_process;
               p_process->p_next_process->p_next_process 
                                         = 
                  p_process->p_next_process->p_next_process->
                                                         p_next_process;
               p_temp->p_next_process    = p_process->p_next_process;
               p_process->p_next_process = p_temp;
            }
            else
            {
               if (p_process->p_next_process->priority == INITIAL_PRI  && 
                   p_process->p_next_process->p_next_process->priority < 
                                                            INITIAL_PRI)
               {
                  p_temp                    =
                     p_process->p_next_process->p_next_process;
                  p_process->p_next_process->p_next_process 
                                            = 
                     p_process->p_next_process->p_next_process->
                                                         p_next_process;
                  p_temp->p_next_process    = p_process->p_next_process;
                  p_process->p_next_process = p_temp;
               }
               else
               {
                  if(p_process->p_next_process->state    ==     READY &&
                    (p_process->p_next_process->priority ==
                        p_process->p_next_process->p_next_process->
                                                            priority) &&
                    (p_process->p_next_process->pid >
                        p_process->p_next_process->p_next_process->
                                                                 pid) &&
                     p_process->p_next_process->p_next_process->priority >
                                                            INITIAL_PRI)
                  {
                     p_temp                    =
                        p_process->p_next_process->p_next_process;
                     p_process->p_next_process->p_next_process 
                                               = 
                        p_process->p_next_process->p_next_process->
                                                         p_next_process;
                     p_temp->p_next_process    = p_process->
                                                         p_next_process;
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
/*                    Check if a process is running                   */
/**********************************************************************/
int check_if_running(PROCESS *p_process_table)
{
   PROCESS *p_process;   /* Points to a process in the list           */
   int     running_flag; /* Flag determining if process is running    */

   p_process = p_process_table;
   while (p_process->p_next_process->pid != LIST_TRAILER)
   {
      if(p_process->p_next_process->state == RUNNING)
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
/*                 Check if all processes are blocked                 */
/**********************************************************************/
int check_all_blocked(PROCESS *p_process_table)
{
   PROCESS *p_process;   /* Points to a process in the list           */
   int     blocked_flag; /* Flag determining if table is blocked      */

   p_process = p_process_table;
   while (p_process->p_next_process->pid != LIST_TRAILER)
   {
      if(p_process->p_next_process->state == BLOCKED)
      {
         blocked_flag = TRUE;
      }
      else
      {
         blocked_flag = FALSE;
         break;
      }
      p_process = p_process->p_next_process;
   }
   
   return blocked_flag;
}

/**********************************************************************/
/*               Schedule the next Ready process to run               */
/**********************************************************************/
void schedule_process(PROCESS *p_process_table, int pid)
{
   PROCESS *p_process; /* Points to a process in the list             */

   p_process = p_process_table;
   while (p_process->p_next_process->pid != LIST_TRAILER &&
          check_if_running(p_process_table) == FALSE)
   {  
      if (p_process->p_next_process->state == READY)
      {
         print_process_table(p_process_table, BEFORE,
            get_next_pid(p_process_table, &pid));
         p_process->p_next_process->state = RUNNING;
         print_process_table(p_process_table, AFTER,
            get_next_pid(p_process_table, &pid));
      }
      p_process = p_process->p_next_process;
   }

   preempt_process(p_process_table, pid);

   return;
}

/**********************************************************************/
/*                   Give the CPU to another process                  */
/**********************************************************************/
void run_process(PROCESS *p_process_table)
{
   PROCESS *p_process; /* Points to a process in the list             */
   
   p_process = p_process_table;
   while (p_process->p_next_process->pid != LIST_TRAILER)
   {  
      if (p_process->p_next_process->state == READY &&
          check_if_running(p_process_table) == FALSE)
      {
         p_process->p_next_process->state = RUNNING;
      }
      p_process = p_process->p_next_process;
   }

   return;
}

/**********************************************************************/
/*             Terminate a process from the process table             */
/**********************************************************************/
void terminate_process(PROCESS *p_process_table, int pid)
{
   PROCESS *p_process,          /* Points to a process in the table   */
           *p_previous_process; /* Points to the previous process     */

   p_previous_process = p_process_table;
   p_process = p_previous_process->p_next_process;
   
   while(p_process = p_previous_process->p_next_process,
         p_process->pid != LIST_TRAILER)
   {
      if(p_process->cpu_used == p_process->max_time)
      {
         print_process_table(p_process_table, BEFORE,
            get_next_pid(p_process_table, &pid));
         p_previous_process->p_next_process = p_process->p_next_process;
         free(p_process);
         run_process(p_process_table);
         print_process_table(p_process_table, AFTER,
            get_next_pid(p_process_table, &pid));
      }
      p_previous_process = p_process;
   }
   
   return;
}

/**********************************************************************/
/*          Recalculate the priority of the runNing process           */
/**********************************************************************/
int recalculate_priority(int old_priority, int quantum_used)
{
   float new_priority; /* New priority for a process                  */
   
   new_priority = ((abs(old_priority) + quantum_used)/2.0f) + 0.5f;

   return (int) new_priority;
}

/**********************************************************************/
/*                       a ruNning process                     */
/**********************************************************************/
void preempt_process(PROCESS *p_process_table, int pid)
{
   PROCESS *p_process,          /* Points to a process in the table   */
           *p_previous_process; /* Points to the previous process     */

   p_previous_process = p_process_table;
   while (p_process = p_previous_process->p_next_process,
          p_process->pid != LIST_TRAILER)
   {
      if (p_process->state == RUNNING)
      {
         if (p_process->quantum_used == p_process->block_time)
         {
            if (p_process->block_time == 6)
            {
               print_process_table(p_process_table, BEFORE,
                  get_next_pid(p_process_table, &pid));
               p_process->state = READY;
               p_process->priority = 
                  recalculate_priority(p_process->priority, 
                                       p_process->quantum_used);
               sort_by_priority(p_process_table);
               p_process->quantum_used = INITIAL_QUANTUM;
               run_process(p_process_table);
               print_process_table(p_process_table, AFTER, 
                  get_next_pid(p_process_table, &pid));
            }
            else
            {
               print_process_table(p_process_table, BEFORE,
                  get_next_pid(p_process_table, &pid));
               p_process->state = BLOCKED;
               p_process->priority =
                  -(recalculate_priority(p_process->priority,
                                         p_process->quantum_used));
               p_process->quantum_used = INITIAL_QUANTUM;
               run_process(p_process_table);
               sort_by_priority(p_process_table);
               print_process_table(p_process_table, AFTER,
                  get_next_pid(p_process_table, &pid));
            }
         }
         else
         {
            p_process->cpu_used += 1;
            p_process->quantum_used += 1;
         }
      }
      else if (p_process->state == READY)
      {
         p_process->wait_ticks += 1;
      }
      p_previous_process = p_process;
   }
   return;
}