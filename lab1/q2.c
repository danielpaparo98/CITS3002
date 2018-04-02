#include <stdlib.h>
#include <stdbool.h>
#include <sys/param.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "checksum_ccitt.c"
#include "checksum_crc16.c"
#include "checksum_internet.c"


/*  This is a simple function designed to test checksum algorithms.
    The function randomly corrupts one or more bits of a given frame.

    As corruption is random, you'll need to call srand() from somewhere
    earlier in your program to ensure different random sequences.

    You may like to devise other forms of corruption and run tests on them.
 */

#define FRAMESIZE  200

typedef unsigned char    byte;

byte    frame[FRAMESIZE];


void corrupt_frame(byte frame[], int length)
{
//  this is a severe/unrealistic function, swapping adjacent, different chars
    while(true) {
        int byte                = (rand() % (length-1));

        if(frame[byte] != frame[byte+1]) {      // ensure chars are different
            char save  = frame[byte];

            frame[byte]         = frame[byte+1];
            frame[byte+1]       = save;
            break;
        }
    }
}

int main(void){
    //SEED THE RANDOM NUMBER GENERATOR (TO PRODUCE DIFFERENT RESULT, EACH TIME)
    srand( getpid() );

//  POPULATE THE FRAME WITH RANDOM BYTES
    for(int i=0 ; i < FRAMESIZE ; ++i) {
        frame[i] = rand() % 256;
    }

    int ccitb,ccita, interneta, internetb, crc16a, crc16b;

    // Generates a checksum for each of the different algorithms
    ccita  = checksum_ccitt(frame, FRAMESIZE);
    interneta  = checksum_internet(frame, FRAMESIZE);
    crc16a  = checksum_crc16(frame, FRAMESIZE);

    //Generates a corrupt version of the original frame
    corrupt_frame(frame, FRAMESIZE);

    //Generates the checksums again on the corrupted version 
    ccitb  = checksum_ccitt(frame, FRAMESIZE);
    internetb  = checksum_internet(frame, FRAMESIZE);
    crc16b  = checksum_crc16(frame, FRAMESIZE);

    //Prints out the different checksum values found
    printf("CCITT:      %i - %i \n", ccita, ccitb);
    printf("CRC16:      %i - %i \n", crc16a, crc16b);
    printf("INTERNET:   %i - %i \n", interneta, internetb);
}

