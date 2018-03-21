#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define FILE "Q1.datafile"

int main(void){
    int fd = open(FILE, O_RDONLY, 0600);
    int current;
    
    // Checks that the file will open
    if(fd < 0){
        fprintf(stderr, "There was a problem with opening the file!");
        exit(EXIT_FAILURE);
    }

    //Finds each integer and prints them
    else{
        for(int i=-50; i < 50; i++){
            read(fd, &current, sizeof(current));
            printf("%i \n", current);
        }
    }
    close(fd);
}