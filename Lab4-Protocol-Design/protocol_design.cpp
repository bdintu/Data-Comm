#define COM3BASE	0x3E8
#define COM4BASE	0x2E8
#define TXDATA		COM3BASE
#define LCR			(COM3BASE + 3)	// 0x3F8 line control
#define LSR			(COM3BASE + 5)	// 0x3FD line status

/**
										Type 0 : Data
		0			1			2		3			4			5				11		12
	[ START_BIT | START_BIT | TYPE | frame_id | DATA[0] | DATA[1] | ... | DATA[7] | STOPBIT ]
		0			0			0		1			H			e		...		1		1
									(send frame 1 : Hell ... \n)
									( frame size : 11 byte )


							Type 1 : ACK
		0		1				2		3			4
	[ START_BIT | START_BIT | TYPE | frame_id | STOPBIT ]
		0			0			6		0			1
							( ack0)
					( frame sisze = 4 byte )
*/

#define FRAME_SIZE				13

#define FRAME_STARTBIT_DATA		0
#define FRAME_STARTBIT_BEGIN	0
#define FRAME_STARTBIT_SIZE		2
#define FRAME_STARTBIT_END		(FRAME_STARTBIT_BEGIN + FRAME_STARTBIT_SIZE -1)

#define	FRAME_TYPE_INDEX		2
#define FRAME_TYPE_DATA			1
#define FRAME_TYPE_END			4
#define FRAME_TYPE_ACK			6

#define FRAME_ID_INDEX			3

#define	FRAME_DATA_BEGIN		4
#define FRAME_DATA_SIZE			8
#define FRAME_DATA_END			(FRAME_DATA_BEGIN + FRAME_DATA_SIZE -1)

#define FRAME_STOPBIT_INDEX		12
#define FRAME_STOPBIT_DATA		1

#include <conio.h>
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	char type;
	char frame_id;
	char data[FRAME_DATA_SIZE];
} frame;

void setup_serial(void);
void send_character(char ch);
int get_character(void);
void send_frame(frame* buffer);
void get_frame(frame* buffer);
int readFile(FILE* fp, frame* buffer);
void fill_data(frame* buffer, int i);
void print_data(frame* buffer);

int main(void) {
	FILE* fp;
	char fdir[256];

	frame inword, outword;
	int mode = 1;						// 0:send in buffer, 1:read to buffer for send
	char ch;

    clrscr();
    setup_serial();

    printf("Send or Receive ('s' or 'r') : ");
	ch = getch();

    if (ch == 's') {
        printf("s\nSend file : ");
        gets(fdir);
        fp = fopen(fdir, "r");
        if (!fp) {
            printf("can't open file dir %s\n", fdir);
            return -1;
        }

		outword.type = FRAME_TYPE_DATA;
		outword.frame_id = 0;
        while (1) {
			// mode = 1, read next word to buffer
            if (mode == 1) {
				if (readFile(fp, &outword) == -1) {
					send_frame(&outword);
					printf("EOF\n");
					break;
				}
            }

            printf("\nSend frame : %d\n", outword.frame_id);
            printf("Data		: [");
			print_data(&outword);
            printf("]\n");
            printf("Timeout		: ");

            send_frame(&outword);
			get_frame(&inword);

			// timeout
			ch = getch();
			if (ch == 't') {
				mode = 0;
				printf("t\nRetransmit frame %d\n", outword.frame_id);
				continue;
			}

			// check ack
            if (inword.type == FRAME_TYPE_ACK) {
				mode = 1;
				outword.frame_id ^= 1;
				printf("\nReceive ACK%d\n", inword.frame_id);
			}
        }

		// dicconnect
		outword.type = FRAME_TYPE_END;
		outword.frame_id ^= 1;
		fill_data(&outword, 0);
		send_frame(&outword);

        fclose(fp);
    }

    if (ch == 'r') {
        printf("r\nSender Send : ");
        gets(fdir);
        fp = fopen(fdir, "w+");
        if (!fp) {
            printf("can't write file dir %s\n", fdir);
            return -1;
        }

		outword.type = FRAME_TYPE_ACK;
		outword.frame_id = 0;
		fill_data(&outword, 0);
        while (1) {
			get_frame(&inword);
			if (inword.type == FRAME_TYPE_END)
				break;

			printf("\nReceive frame : %d\n", inword.frame_id);
			printf("Data          : [");
			print_data(&inword);
			printf("]\n");
			printf("Action frame  : ");

			do {
				ch = getch();
			} while(ch == '\n');

			switch (ch) {
				case 'a' :
					if (outword.frame_id == inword.frame_id) {
						outword.frame_id ^= 1;						
						fputs(inword.data, fp);
		                printf("a\nReceived & Send ACK%d\n", outword.frame_id);
					} else {
						printf("a\nReject & Send ACK%d\n", outword.frame_id);
					}
				break;

				case 'n' :
					if (outword.frame_id == inword.frame_id) {
						outword.frame_id ^= 1;
						fputs(inword.data, fp);
						printf("n\nReceived & Sleep\n");
					} else {
						printf("n\nReject & Sleep\n");
					}
				break;

				case 'r' :
					printf("r\nReject & Sleep\n");
				break;
			}
		}

		fclose(fp);
	}

	getch();
	return 0;
}

void setup_serial(void) {
	outportb(LCR, 0x80);
	/*set up bit 7 to a 1 to set Register address bit*/
	outportb(TXDATA, 0x0C);
	outportb(TXDATA + 1, 0X00);
	outportb(LCR, 0xA);
	/*load TxRegister with 12, crystal frequency is 1.8432 MHz*/ outportb(LCR,
			0x0A);
	/* Bit pattern loads is 00001010b, from MSB to LSB these are: */
	/* 0  - access TD/RD buffer, 0 - normal output */
	/* 0  - no stick bit, 0 - even parity */
	/* 1  - parity on, 0 � 1 stop bit */
	/* 10 � 7 data bits */
}

void send_character(char ch) {
	char status;
	do {
		status = inportb(LSR) & 0x40;
	} while (status != 0x40);
	/*repeat until Tx buffer empty ie bit 6 set */
	outportb(TXDATA, (char)ch);
}

int get_character(void) {
	int status;
	do {
		status = inportb(LSR) & 0x01;
	} while (status != 0x01);
	/*Repeat until bit 1 in LSR is set */
	return ((int)inportb(TXDATA));
}

void send_frame(frame* buffer) {
	send_character(FRAME_STARTBIT_DATA);
	send_character(FRAME_STARTBIT_DATA);
	send_character(buffer->type);
	send_character(buffer->frame_id);
	int i;
	for (i = 0; i < FRAME_DATA_SIZE; ++i) {
		send_character(buffer->data[i]);
	}
	send_character(FRAME_STOPBIT_DATA);
}

void get_frame(frame* buffer) {
	int i;
	char tmp;
	tmp = get_character();
	tmp = get_character();
	buffer->type = get_character();
	buffer->frame_id = get_character();
	for (i = 0; i < FRAME_DATA_SIZE; ++i) {
		buffer->data[i] = get_character();
	}
	tmp = get_character();
}

int readFile(FILE* fp, frame* buffer) {
	int i;
	for (i = 0; i < FRAME_DATA_SIZE; ++i) {
		buffer->data[i] = fgetc(fp);
		// is End of File
		if (buffer->data[i] == EOF) {
			fill_data(buffer, i);
			return -1;
		}
	}
	printf(">>readFile: {i:%d}",i);
	return 0;
}

void fill_data(frame* buffer, int i) {
	for(; i <= FRAME_DATA_SIZE; ++i) {
		buffer->data[i] = '\0';
	}
}

void print_data(frame* buffer) {
	int i;
	for (i = 0; i < FRAME_DATA_SIZE; ++i) {
		if (buffer->data[i] == '\0') {
			break;
		}
		printf("%c", buffer->data[i]);
	}
}
