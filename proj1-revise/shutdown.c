#include "types.h"
#include "stat.h"
#include "user.h"

extern int shutdown();

int
main(int argc, char *argv[])
{
  int code=-1;
  if (argc>=2){
  	code = atoi(argv[1]);
  }
  shutdown(code);
  exit();
}
