#include <cnet.h>
#include <stdlib.h>
#include <string.h>

/*  an adaptation of the previous stop and wait protocol which implements a negative acknoledgment
 */

typedef enum    { DL_DATA, DL_ACK, DL_NACK }   FRAMEKIND;

typedef struct {
    char        data[MAX_MESSAGE_SIZE];
} MSG;

typedef struct {
    FRAMEKIND    kind;      	// only ever DL_DATA or DL_ACK
    size_t	 len;       	// the length of the msg field only
    int          checksum;  	// checksum of the whole frame
    int          seq;       	// only ever 0 or 1

    MSG          msg;
} FRAME;

#define FRAME_HEADER_SIZE  (sizeof(FRAME) - sizeof(MSG))
#define FRAME_SIZE(f)      (FRAME_HEADER_SIZE + f.len)


MSG       	*lastmsg;
size_t		lastlength		= 0;
CnetTimerID	lasttimer		= NULLTIMER;

int       	ackexpected		= 0;
int		nextframetosend		= 0;
int		frameexpected		= 0;


void transmit_frame(MSG *msg, FRAMEKIND kind, size_t length, int seqno)
{
    FRAME       f;
    int		link = 1;

    f.kind      = kind;
    f.seq       = seqno;
    f.checksum  = 0;
    f.len       = length;

    switch (kind) {
    case DL_ACK :
        printf("ACK transmitted, seq=%d\n", seqno);
	break;

    case DL_NACK:
        printf("NACK transmitted, seq=%d\n", seqno);
    break;

    case DL_DATA: {
	CnetTime	timeout;

        printf(" DATA transmitted, seq=%d\n", seqno);
        memcpy(&f.msg, msg, (int)length);

	timeout = FRAME_SIZE(f)*((CnetTime)8000000 / linkinfo[link].bandwidth) +
				linkinfo[link].propagationdelay;

        lasttimer = CNET_start_timer(EV_TIMER1, 3 * timeout, 0);
	break;
      }
    }
    length      = FRAME_SIZE(f);
    f.checksum  = CNET_ccitt((unsigned char *)&f, (int)length);
    CHECK(CNET_write_physical(link, &f, &length));
}

EVENT_HANDLER(application_ready)
{
    CnetAddr destaddr;

    lastlength  = sizeof(MSG);
    CHECK(CNET_read_application(&destaddr, lastmsg, &lastlength));
    CNET_disable_application(ALLNODES);

    printf("down from application, seq=%d\n", nextframetosend);
    transmit_frame(lastmsg, DL_DATA, lastlength, nextframetosend);
    nextframetosend = 1-nextframetosend;
}

EVENT_HANDLER(physical_ready)
{
    FRAME        f;
    size_t	 len;
    int          link, checksum;

    len         = sizeof(FRAME);
    CHECK(CNET_read_physical(&link, &f, &len));

    checksum    = f.checksum;
    f.checksum  = 0;
    if(CNET_ccitt((unsigned char *)&f, (int)len) != checksum) {
        printf("\t\t\t\tBAD checksum - Negative acknoledgment sent!\n");
        transmit_frame(NULL, DL_NACK, 0, 0);
        return;      
    }

    switch (f.kind) {
    case DL_ACK :
        if(f.seq == ackexpected) {
            printf("\t\t\t\tACK received, seq=%d\n", f.seq);
            CNET_stop_timer(lasttimer);
            ackexpected = 1-ackexpected;
            CNET_enable_application(ALLNODES);
        }
	break;

    case DL_NACK:
        if(f.seq != ackexpected){
            printf("\t\t\t\tNACK received => retransmit seq=%d\n", ackexpected);
            CNET_stop_timer(lasttimer);
            transmit_frame(lastmsg, DL_DATA, lastlength, ackexpected);

        }
    break;

    case DL_DATA :
        printf("\t\t\t\tDATA received, seq=%d, ", f.seq);
        if(f.seq == frameexpected) {
            printf("up to application\n");
            len = f.len;
            CHECK(CNET_write_application(&f.msg, &len));
            frameexpected = 1-frameexpected;
        }
        else
            printf("ignored\n");
        transmit_frame(NULL, DL_ACK, 0, f.seq);
	break;
    }
}

EVENT_HANDLER(timeouts)
{
    printf("timeout, seq=%d\n", ackexpected);
    transmit_frame(lastmsg, DL_DATA, lastlength, ackexpected);
}

EVENT_HANDLER(showstate)
{
    printf(
    "\n\tackexpected\t= %d\n\tnextframetosend\t= %d\n\tframeexpected\t= %d\n",
		    ackexpected, nextframetosend, frameexpected);
}

EVENT_HANDLER(reboot_node)
{
    if(nodeinfo.nodenumber > 1) {
	fprintf(stderr,"This is not a 2-node network!\n");
	exit(1);
    }

    lastmsg	= calloc(1, sizeof(MSG));

    CHECK(CNET_set_handler( EV_APPLICATIONREADY, application_ready, 0));
    CHECK(CNET_set_handler( EV_PHYSICALREADY,    physical_ready, 0));
    CHECK(CNET_set_handler( EV_TIMER1,           timeouts, 0));
    CHECK(CNET_set_handler( EV_DEBUG0,           showstate, 0));

    CHECK(CNET_set_debug_string( EV_DEBUG0, "State"));

    if(nodeinfo.nodenumber == 0) {
	CNET_enable_application(ALLNODES);
    }
}
