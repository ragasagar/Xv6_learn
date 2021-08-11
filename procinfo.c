#include "types.h"
#include "user.h"
#include "fcntl.h"

int 
main(int argc, char *argv[]){
    
    if(argc <=1){
        printf(1, "Missing Arguement: <pid>\n");
        exit(-1);
    }

    procinfo(atoi(argv[1]));
    exit(0);
}