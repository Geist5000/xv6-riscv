#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void
main(int argc, char* argv[]){   
    
    if(argc <= 1){
        fprintf(2,"usage: hello $number");
        exit(1);
    }
    int number = *argv[1] - '0';
    hello(number);
}
