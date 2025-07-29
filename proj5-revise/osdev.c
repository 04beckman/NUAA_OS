// osdev.c
#include "types.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "fs.h"

#define OSDEV_BUF_SIZE 1024

struct {
  char buffer[OSDEV_BUF_SIZE];
  uint readIndex;
  uint writeIndex;
  struct spinlock lock;
} osdev;

void osdevinit(void) {
  initlock(&osdev.lock, "osdev");
  osdev.readIndex = 0;
  osdev.writeIndex = 0;
}