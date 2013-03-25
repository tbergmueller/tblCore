/*
 * dcs_hardwareFunctions.c
 *
 * Created: 05.12.2012 14:27:02
 *  Author: SchR
 */ 

#include "dcm_hardwareFunctions.h"
#include "dcm_internal.h"
#include "dcm_basicFunctions.h"



// ##########################################################################

#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <stdlib.h>
#include <pthread.h>

// FIXME remove later on when sure that the line in set_interface_attribs works as well
 void make_raw(int fd)
 {
     struct termios ios;

     //Not a TTY: nothing to do
     if (!isatty(fd))
         return;

     tcgetattr(fd, &ios);
     cfmakeraw(&ios);
     tcsetattr(fd, TCSANOW, &ios);
 }



int
set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                printf ("error %d from tcgetattr", errno);
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);
        cfmakeraw(&tty);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // ignore break signal
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                printf ("error %d from tcsetattr", errno);
                return -1;
        }

      //  make_raw(fd);
        return 0;
}

void
set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                printf ("error %d from tggetattr", errno);
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
                printf ("error %d setting term attributes", errno);
}



// ####################################################################


















static int fd;
static  pthread_t readingThread;
static unsigned char stopReading = 0;




void dcm_processTxcInterrupt()
{

}


unsigned char stopped = 0;

void *readSerial(void* ptr)
{
	while(!stopReading)
	{
		unsigned char buf [300];
		int n = read (fd, buf, sizeof(buf));  // read up to 100 characters if ready to read

		if(n)
		{
			int i;
			//printf("Read byte: ");
			for(i = 0; i<n; i++)
			{
				//printf("%02x ", (buf[i]));
				dcm_processReceived(buf[i]);
			}

			//printf("\n");
		}
	}
	stopped = 1;
	return NULL;
}

void dcm_send(unsigned char* ch, unsigned short dataLength)
{
	write (fd, ch, dataLength);
}


void dcm_stop(DevComMaster_t* rMaster)
{
	stopped = 0;
	stopReading = 1;

	while(!stopped)
	{
		// wait
	}
}




void dcm_start(DevComMaster_t* rMaster, uint32_t vFcpu, uint32_t vBaudrate)
{

	setCurrentMaster(rMaster);
	DevComMaster_t* currentMaster = getCurrentMaster();

		currentMaster->Ping = dcm_ping;
		currentMaster->SendResetSequence = dcm_sendResetSequence;
		currentMaster->RequestSlaveInformation = dcm_readSlaveInfo;
		currentMaster->Request = dcm_request;
		currentMaster->SendCommand = dcm_command;
		currentMaster->SendData = dcm_data;
		currentMaster->SendCommandBroadcast = dcm_commandBC;
		currentMaster->SendDataBroadcast = dcm_dataBC;


		char *portname = currentMaster->UART;

		fd = open (portname, O_RDWR | O_NOCTTY | O_SYNC);
		if (fd < 0)
		{
				printf ("error %d opening %s: %s", errno, portname, strerror (errno));
				exit(-1);
		}

		set_interface_attribs (fd, B500000, 0);  // set speed to 115,200 bps, 8n1 (no parity)
		set_blocking (fd, 0);                // set no blocking

		pthread_create( &readingThread, NULL, readSerial, (void*) currentMaster);
}

// set the uart baud rate
void dcm_setBaudrate()
{

}

