#include <stdlib.h>
#include <stdio.h>

int main()
{
   //int x;
   //for(int i=0; i < 10; i++)
   //{
   //   x = (rand() % 6 + 1) < (2/3);
   //   printf("\n%d\n", x);
   //}

   //int x; 
   //int y;

   //for (int i = 0; i < 20; i++)
   //{
   //   x = rand() % 100 + 1;
   //   if (((x)/ 100.0f) < (1.0f / 3.0f))
   //   {
         
   //      y = 6;
   //      printf("\nx: %d\ny: %d\n", x, y);
   //   }
   //   else
   //   {
   //      y = (rand() % 5 + 1);
   //      printf("\nx: %d\ny: %d\n", x, y);
   //   }
   //}

   int x = 1;
   for(int i = 0; i < 5; i++)
   {
      if ((rand() % 5 + 1) == 1)
      {
         printf("\n\nooga booga %d\n", x);
      }
      else
      {
         printf("\ndang it! %d\n\n", x);
      }
      x++;
   }


   return 0;
}