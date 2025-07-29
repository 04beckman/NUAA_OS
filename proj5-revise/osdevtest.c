#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int osdevfd=-1;


void tc1(){
  //test open
  osdevfd=open("osdev",O_RDWR);
  if(osdevfd<0){
    printf(1,"TC1 fail: open osdev failed with exit code %d\n",osdevfd);
    exit();
  }
  printf(1,"TC1 success: open osdev with fd=%d\n",osdevfd);
}

void tc2(){
  //test basic read and write
  //write and then read
  char c[20]="0123456789\0",d[20];
  int nwrites=write(osdevfd,c,10);
  int nreads=read(osdevfd,d,10);
  printf(1,"TC2: write: %s (%d) read: %s (%d)\n",c,nwrites,d,nreads);
  if(nreads!=nwrites||strcmp(c,d)!=0){
    printf(1,"TC2 fail: write string %s, but read string %s\n",c,d);
    exit();
  }
  printf(1,"TC2 success\n");
}

void tc3(){
  //test whether multiple processes share the same device
  if(fork()==0){
    //child write something
    if(fork()==0){
      write(osdevfd,"one-",4);
      exit();
    }else{
      wait();
      write(osdevfd,"two\n",4);
      exit();
    }   
  }else{
    //parent read
    wait();
    char d[100];
    int n=read(osdevfd,d,100);
    d[n]='\0';//otherwise printf shows weired symbol
    printf(1,"TC3: %s",d);
  }
}

void tc4(){
  //test block reading
  if(fork()==0){
    //child, block until parent write something
    char d[100]="TC4 fail: you should not see this string\n";
    int n=read(osdevfd,d,100);
    if(n>0) d[n]='\0';
    printf(1,"%s\n",d);
    exit();    
  }else{
    char ss[100];
    printf(1,"TC4: Please type a string from keyboard: \n");
    int n=read(1,ss,100);
    write(osdevfd,ss,n);
    wait();
  }
}

int
main(int argc, char *argv[])
{
  tc1();
  tc2();
  tc3();
  tc4();
  exit();
}
