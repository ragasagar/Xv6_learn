#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main(int argc, char *argv[]){
    if(argc <=1){
        printf(1, "Missing Arguement: <pid>\n");
        exit(-1);
    }
    getprocesstimedetails(atoi(argv[0]));
    exit(0);
}