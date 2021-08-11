#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main(int argc, char *argv[]){
    getprocesstimedetails();
    sleep(2000);
    int i = fork();
    if(i>1){
        int j = fork();
        if(j>1){
            printf(1,"Hello parent");
            sleep(2000);
            getprocesstimedetails();
            wait(0);
            getprocesstimedetails();

        }
        printf(1,"\nparent task");
        getprocesstimedetails();
        wait(0);
        printf(1,"\nparent task-->2 after waiting");
        getprocesstimedetails();
    }
    printf(1,"\nchild task");
    sleep(1000);
    getprocesstimedetails();
    exit(0);
}