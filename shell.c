#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

pid_t pid,wpid;

int split_save(char* what,char* how, char* where[])
{
   char* temp = strtok(what,how);
   int count = 0;
   while(temp != NULL)
   {
      where[count] = temp;
      count++;
      temp = strtok(NULL,how);
   }
   return count-1;
}

int lineex(char input[],int k)
{
   pid_t pid_arr[500];
   char* no_semicolon[100] = {NULL,};
   int temp = split_save(input,";\n",no_semicolon);

   if(temp<0)
   {
      printf("no command\n");
   }
   int count = 0;

   while(no_semicolon[count] != NULL)
   {   
      if(strcmp(no_semicolon[count],"quit")==0)
      {
         printf("QUIT DETECTED\n");
         return 100;
      }
      char* no_space[100] = {NULL, };
      split_save(no_semicolon[count]," ",no_space);

      pid = fork();
      pid_arr[count] = pid;
      if(pid<0)
      {
         printf("FORK ERROR\n");
      }
      else if(pid==0)
      {
         if(execvp(no_space[0],no_space) == -1)
         {
            fprintf(stderr, "error occurred: %s\n", strerror(errno));
            kill(getpid(),SIGINT); 
         }
      }
      count++;
   }

   for(int i = 0; i < count; i++) {
      waitpid(pid_arr[i], NULL, 0);
   }
   if(k)
      printf("prompt> ");
   return 1;
}

int main(int argc, char** argv, char** env)
{   
//interactive mode 
   if(argc==1)
   {
      printf("prompt> ");
      char input[1000] = {0,};
      while(fgets(input, sizeof(input),stdin))
      {
         int t = lineex(input,1);
         if(t == 100)
         {
            return 0;
         }
      }
   }
//batch mode
   else if(argc==2)
   {
      FILE *fi = fopen(argv[1],"r");
      if(fi == NULL){
         perror("Open FAIL\n");
         exit(EXIT_FAILURE);
      }
      while(!feof(fi)) 
         {   char input[1000] = {0,};
      if(fgets(input, sizeof(input), fi) == NULL) 
         break;
      input[strlen(input) - 1] = '\0';
      printf("%s\n",input);
      int q = lineex(input,0);
      if(q == 100)
      {
         return 0;
      }
   }
   fclose(fi);
}
return 0;
}
