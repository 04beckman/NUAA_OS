#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int address;
  int n;
  
  if(argint(0, &n) < 0)//获取请求的内存大小参数 n
    return -1;
  address = myproc()->sz;
  
  // 检查是否超过内核空间
  if(myproc()->sz + n > KERNBASE)
    return -1;
  
  // 若请求负值内存，立即调用 deallocuvm 释放内存
  if(n < 0){
    if(deallocuvm(myproc()->pgdir, myproc()->sz, myproc()->sz + n) == 0)
      return -1;
  }
  
  // 仅调整虚拟内存大小，不分配物理内存
  myproc()->sz += n;
  return address;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int
sys_shutdown(void){
  outw(0x604, 0x2000);
  return 0;
}

extern int free_frame_cnt;
int
sys_get_free_frame_cnt(void){
  return free_frame_cnt;
}