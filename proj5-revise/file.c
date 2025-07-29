//
// File descriptors
//

#include "types.h"
#include "defs.h"
#include "param.h"
#include "fs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "file.h"

#define OSDEV_BUF_SIZE 1024

struct devsw devsw[NDEV];
struct {
  struct spinlock lock;
  struct file file[NFILE];

} ftable;

extern struct {
  char buffer[1024];
  uint readIndex;
  uint writeIndex;
  struct spinlock lock;
} osdev;


void
fileinit(void)
{
  initlock(&ftable.lock, "ftable");
}

// Allocate a file structure.
struct file*
filealloc(void)
{
  struct file *f;

  acquire(&ftable.lock);
  for(f = ftable.file; f < ftable.file + NFILE; f++){
    if(f->ref == 0){
      f->ref = 1;
      release(&ftable.lock);
      return f;
    }
  }
  release(&ftable.lock);
  return 0;
}

// Increment ref count for file f.
struct file*
filedup(struct file *f)
{
  acquire(&ftable.lock);
  if(f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.lock);
  return f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
void
fileclose(struct file *f)
{
  struct file ff;

  acquire(&ftable.lock);
  if(f->ref < 1)
    panic("fileclose");
  if(--f->ref > 0){
    release(&ftable.lock);
    return;
  }
  ff = *f;
  f->ref = 0;
  f->type = FD_NONE;
  release(&ftable.lock);

  if(ff.type == FD_PIPE)
    pipeclose(ff.pipe, ff.writable);
  else if(ff.type == FD_INODE){
    begin_op();
    iput(ff.ip);
    end_op();
  }
}

// Get metadata about file f.
int
filestat(struct file *f, struct stat *st)
{
  if(f->type == FD_INODE){
    ilock(f->ip);
    stati(f->ip, st);
    iunlock(f->ip);
    return 0;
  }
  return -1;
}

// Read from file f.
int
fileread(struct file *f, char *addr, int n)
{
  int r;

  if(f->readable == 0)
    return -1;
  if(f->type == FD_PIPE)
    return piperead(f->pipe, addr, n);
  if(f->type == FD_INODE){
    ilock(f->ip);
    if((r = readi(f->ip, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
    return r;
  }
  if(f->type == FD_OSDEV) {
    int i;
    acquire(&osdev.lock);                      // 获取锁
    while(osdev.readIndex == osdev.writeIndex) // 缓冲区为空则等待
        sleep(&osdev.readIndex, &osdev.lock);  // 睡眠等待数据
    
    // 读取数据直到满足请求或缓冲区为空
    for(i = 0; i < n && osdev.readIndex != osdev.writeIndex; i++) {
        addr[i] = osdev.buffer[osdev.readIndex++ % OSDEV_BUF_SIZE];
    }
    release(&osdev.lock);                      // 释放锁
    return i;                                  // 返回读取字节数
  }
  panic("fileread");
}

//PAGEBREAK!
// Write to file f.
int
filewrite(struct file *f, char *addr, int n)
{
  int r;

  if(f->writable == 0)
    return -1;
  if(f->type == FD_PIPE)
    return pipewrite(f->pipe, addr, n);
  if(f->type == FD_INODE){
    // write a few blocks at a time to avoid exceeding
    // the maximum log transaction size, including
    // i-node, indirect block, allocation blocks,
    // and 2 blocks of slop for non-aligned writes.
    // this really belongs lower down, since writei()
    // might be writing a device like the console.
    int max = ((MAXOPBLOCKS-1-1-2) / 2) * 512;
    int i = 0;
    while(i < n){
      int n1 = n - i;
      if(n1 > max)
        n1 = max;

      begin_op();
      ilock(f->ip);
      if ((r = writei(f->ip, addr + i, f->off, n1)) > 0)
        f->off += r;
      iunlock(f->ip);
      end_op();

      if(r < 0)
        break;
      if(r != n1)
        panic("short filewrite");
      i += r;
    }
    return i == n ? n : -1;
  }
  if(f->type == FD_OSDEV) {
    int i;
    acquire(&osdev.lock); // 获取锁
    
    // 写入数据直到满足请求或缓冲区满
    for(i = 0; i < n && (osdev.writeIndex - osdev.readIndex) < OSDEV_BUF_SIZE; i++) {
        osdev.buffer[osdev.writeIndex++ % OSDEV_BUF_SIZE] = addr[i];
    }
    
    release(&osdev.lock);      // 释放锁
    wakeup(&osdev.readIndex);  // 唤醒可能等待的读进程
    return i;                  // 返回写入字节数
  }
  panic("filewrite");
}

