#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/param.h>
#include <time.h>
#include <sys/time.h>
#include <sys/param.h>

//Includes the checksum algorithms
#include "./checksum_ccitt.c"
#include "./checksum_crc16.c"
#include "./checksum_internet.c"

#define FRAMESIZE       100
#define FRAMES         1000000

//  corrupt a frame with a burst error
#define MIN_BURSTLENGTH         10
#define MAX_BURSTLENGTH         100

//  CORRUPT A FRAME WITH A BURST ERROR
void corrupt_frame(unsigned char frame[], int length)
{
#define MIN_BURSTLENGTH         10
#define MAX_BURSTLENGTH         100
    int nbits           = (length * NBBY);
    while(true) {
        int     b0      = rand() % nbits;
        int     b1      = rand() % nbits;
	int	burst	= b1 - b0;

        if(burst >= MIN_BURSTLENGTH && burst <= MAX_BURSTLENGTH) {
            for(int b=b0 ; b<b1 ; ++b) {
                int     byte    = b / NBBY;
                int     bit     = b % NBBY;

                frame[byte]     = (frame[byte] | (1UL << bit));
            }
            break;
        }
    }
}

int main(void){
    time_t t;
    int frames, fails;

    srand((unsigned) time(&t));


}