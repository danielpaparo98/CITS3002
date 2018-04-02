#include <stdlib.h>
#include <stdbool.h>
#include <sys/param.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <sys/time.h>


#include "checksum_ccitt.c"
#include "checksum_crc16.c"
#include "checksum_internet.c"

/*  This is a simple function designed to test checksum algorithms.
    The function randomly corrupts one or more bits of a given frame.

    As corruption is random, you'll need to call srand() from somewhere
    earlier in your program to ensure different random sequences.

    You may like to devise other forms of corruption and run tests on them.
 */

typedef unsigned char byte;

void corrupt_frame(char frame[], int length)
{
    //  this is a severe/unrealistic function, swapping adjacent, different chars
    while (true)
    {
        int byte = (rand() % (length - 1));

        if (frame[byte] != frame[byte + 1])
        { // ensure chars are different
            char save = frame[byte];

            frame[byte] = frame[byte + 1];
            frame[byte + 1] = save;
            break;
        }
    }
}

int64_t timing(bool start)
{
    static      struct timeval startw, endw;
    int64_t     usecs   = 0;

    if(start) {
        gettimeofday(&startw, NULL);
    }
    else {
        gettimeofday(&endw, NULL);
        usecs   =
               (endw.tv_sec - startw.tv_sec)*1000000 +
               (endw.tv_usec - startw.tv_usec);
    }
    return usecs;
}


int main(void)
{
    FILE *file = fopen("mobydick.txt", "r"); /* should check the result */
    char line[BUFSIZ];

    int ccittime, internettime, crc16time;
    int count = 0;

    while (fgets(line, sizeof(line), file))
    {

        count++;
        count++;
        int ccitb, ccita, interneta, internetb, crc16a, crc16b;
        int ccittime_a, internettime_a, crc16time_a, ccittime_b, internettime_b, crc16time_b;

        // Generates a checksum for each of the different algorithms
        timing(true);
        ccita = checksum_ccitt(line, BUFSIZ);
        ccittime_a = timing(false);
        timing(true);
        interneta = checksum_internet(line, BUFSIZ);
        internettime_a = timing(false);
        timing(true);
        crc16a = checksum_crc16(line, BUFSIZ);
        crc16time_a = timing(false);

        //Generates a corrupt version of the original frame
        corrupt_frame(line, BUFSIZ);

        //Generates the checksums again on the corrupted version
        timing(true);
        ccitb = checksum_ccitt(line, BUFSIZ);
        ccittime_b = timing(false);
        timing(true);
        internetb = checksum_internet(line, BUFSIZ);
        internettime_b = timing(false);
        timing(true);
        crc16b = checksum_crc16(line, BUFSIZ);
        crc16time_b = timing(false);


        //Prints out the different checksum values found
        printf("CCITT:      %i - %i \n", ccita, ccitb);
        printf("CRC16:      %i - %i \n", crc16a, crc16b);
        printf("INTERNET:   %i - %i \n", interneta, internetb);
        
        ccittime = ccittime + ccittime_a + ccittime_b;
        internettime = internettime + internettime_a +internettime_b;
        crc16time = crc16time + crc16time_a + crc16time_b;
    }

    printf("AVERAGE LENGTH OF TIME\n");
    printf("CCITT:      %i\n", ccittime / count);
    printf("CRC16:      %i\n", crc16time/count);
    printf("INTERNET:   %i\n", internettime/count);

    fclose(file);

    return 0;
}
