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
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
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


extern int display_enabled;
int sys_enable_sched_display(void){
	int i;
	if(argint(0,&i)<0)
		return -1;
	display_enabled=i;
	return 0;
}

int
sys_set_quantum(void) {
    int quantum;
    if (argint(0, &quantum) < 0) return -1;
    myproc()->quantum = quantum;
    return 0;
}

int
sys_set_priority(void) {
    int pid, priority;
    if (argint(0, &pid) < 0 || argint(1, &priority) < 0) return -1;
    return set_priority(pid, priority);
}

int 
sys_dealloc_sem(void) {
  int sem_id;
  if (argint(0, &sem_id) < 0) return -1;
  return dealloc_sem(sem_id);
}

int 
sys_signal_sem(void) {
  int sem_id;
  if (argint(0, &sem_id) < 0) return -1;
  return signal_sem(sem_id);
}

int 
sys_wait_sem(void) {
  int sem_id;
  if (argint(0, &sem_id) < 0) return -1;
  return wait_sem(sem_id);
}

int 
sys_alloc_sem(void) {
  int v;
  if (argint(0, &v) < 0 || v < 0) return -1; // 参数错误或值为负
  return alloc_sem(v);
}