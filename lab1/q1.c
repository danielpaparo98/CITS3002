#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int  fd = open("Q1.datafile", O_RDONLY, 0600);
    int number;

    if(fd >= 0){
        for(int i = -50; i < 50; i++){
            read(fd, &number, sizeof(number));
            printf("%i \n", number);
        }
    }
}
