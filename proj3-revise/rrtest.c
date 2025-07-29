#include "types.h"
#include "user.h"

extern int set_quantum(int);

#define N 30

int fib(int i){
	if(i<1)return i;
	return fib(i-1)+fib(i-2);
}

void busy_computing(){
	fib(25);
}

void print_pid(){
	int i=0;
	while(i++<N){
		busy_computing();
		printf(1,"-%d-",getpid());
	}
}

int main(){

        int i,pid;
        int fd[4][2];
        char c;
        printf(1,"================================\n");
        pid=getpid();
        printf(1,"Parent (pid=%d)\n",pid);
        for(i=0;i<4;i++){
                pipe(fd[i]);				
                if((pid=fork())==0){
                        //child
			if(i>=2) set_quantum(5); //the first 2 child processes have default quantum, the rest have quantum 5
                        close(fd[i][1]);//child close write end
                        read(fd[i][0],&c,1);//wait signal from parent
                        print_pid();
                        exit();
                }else{
                        close(fd[i][0]);//parent close read end                        
                        printf(1,"Child (pid=%d) created!\n",pid);
                }
        }        
        fib(30);//wait for all children finishing printing prompt
	printf(1,"================================\n");        

        //parent wake up all children
        for(i=0;i<4;i++){
        c='a';
        write(fd[i][1],&c,1);
        close(fd[i][1]);
        }

		//parent wait for child
        for(i=0;i<4;i++)
                wait();        
        printf(1,"\n");
        exit();
        return 0;
}

