#include "types.h"
#include "stat.h"
#include "riscv.h"
#include "defs.h"

uint64
sys_hello(void){   


    int number;

    argint(0,&number);

    printf("Hello Userspace, Group %d!\n",number);

    return 0;

}
