#define COM3BASE	0x3E8
#define COM4BASE	0x2E8
#define TXDATA		COM3BASE
#define LCR			(COM3BASE + 3)	// 0x3F8 line control
#define LSR			(COM3BASE + 5)	// 0x3FD line status

/**
	Type 1 : Data
		0		1			2		3				9
	[ TYPE | FRAME_ID | DATA[0] | DATA[1] | ... | DATA[7] ]
		1		0			H		e		...		\n
					(send frame 1 : Hell ... \n)
					( frame size : 10 byte )

	Type 4 : END
		0		1
	[ TYPE | FRAME_ID ]
		4		0
		(disconnect)
	( frame size = 2 byte )

	Type 6 : ACK
		0		1
	[ TYPE | FRAME_ID ]
		6		0
		(ack0)
	( frame size = 2 byte )
*/

#define FRAME_SIZE				10

#define	FRAME_TYPE_INDEX		0
#define FRAME_TYPE_DATA			1
#define FRAME_TYPE_END			4
#define FRAME_TYPE_ACK			6

#define FRAME_ID_INDEX			1

#define	FRAME_DATA_BEGIN		2
#define FRAME_DATA_SIZE			8
#define FRAME_DATA_END			(FRAME_DATA_BEGIN + FRAME_DATA_SIZE -1)

#include <conio.h>
#include <dos.h>
#include <stdio.h>
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
	scanf("%c", ch);

    if (ch == 's') {
        printf("\nSend file : ");
        gets(fdir);
        fp = fopen(fdir, "r");
        if (!fp) {
            printf("can't open file dir %s\n", fdir);
			getch();
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
			
			// timeout
			scanf("%c", ch);

			if (ch == 't') {
				mode = 0;
				printf("\nRetransmit frame %d\n", outword.frame_id);
				continue;
			}

			get_frame(&inword);
			// check ack
            if (inword.type == FRAME_TYPE_ACK) {
				mode = 1;
				outword.frame_id ^= 1;
				printf("\nReceive ACK%d\n", inword.frame_id);
			}
        }

		// disconnect
		outword.type = FRAME_TYPE_END;
		outword.frame_id ^= 1;
		fill_data(&outword, 0);
		send_frame(&outword);

        fclose(fp);
    }

    if (ch == 'r') {
        printf("\nSender Send : ");
        gets(fdir);
		printf("\nSave as : ");
		gets(fdir);
        fp = fopen(fdir, "w+");
        if (!fp) {
            printf("can't write file dir %s\n", fdir);
			getch();
            return -1;
        }

		outword.type = FRAME_TYPE_ACK;
		outword.frame_id = 0;
		fill_data(&outword, 0);
        while (1) {
			get_frame(&inword);

			// disconnect
			if (inword.type == FRAME_TYPE_END)
				break;

			printf("\nReceive frame : %d\n", inword.frame_id);
			printf("Data          : [");
			print_data(&inword);
			printf("]\n");
			printf("Action frame  : ");

			do {
				scanf("%c", ch);
			} while(ch == '\n');

			switch (ch) {
				case 'a' :
					if (outword.frame_id == inword.frame_id) {
						outword.frame_id ^= 1;						
						fputs(inword.data, fp);
		                printf("\nReceived & Send ACK%d\n", outword.frame_id);
					} else {
						printf("\nReject & Send ACK%d\n", outword.frame_id);
					}
					send_frame(&outword);
				break;

				case 'n' :
					if (outword.frame_id == inword.frame_id) {
						outword.frame_id ^= 1;
						fputs(inword.data, fp);
						printf("\nReceived & Sleep\n");
					} else {
						printf("\nReject & Sleep\n");
					}
				break;

				case 'r' :
					printf("\nReject & Sleep\n");
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
	outportb(LCR, 0x0A);
	/*load TxRegister with 12, crystal frequency is 1.8432 MHz*/
	/* Bit pattern loads is 00001010b, from MSB to LSB these are: */
	/* 0  - access TD/RD buffer, 0 - normal output */
	/* 0  - no stick bit, 0 - even parity */
	/* 1  - parity on, 0 ? 1 stop bit */
	/* 10 ? 7 data bits */
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
	send_character(buffer->type);
	send_character(buffer->frame_id);
	int i;
	for (i = 0; i < FRAME_DATA_SIZE; ++i) {
		send_character(buffer->data[i]);
	}
}

void get_frame(frame* buffer) {
	buffer->type = get_character();
	buffer->frame_id = get_character();
	int i;
	for (i = 0; i < FRAME_DATA_SIZE; ++i) {
		buffer->data[i] = get_character();
	}
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