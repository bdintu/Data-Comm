#define COM3BASE 0x3E8
#define COM4BASE 0x2E8
#define TXDATA COM3BASE
#define LCR (COM3BASE + 3) /* 0x3F8 line control*/
#define LSR (COM3BASE + 5) /* 0x3FD line status*/
#include <conio.h>
#include <dos.h>
#include <stdio.h>
#include <string.h>

int inchar, outchar;
char ans;

FILE* fp;
char *inword, *outword;
char f_dir[255];
int status = 0;
int mode = 1;			/* 0:unread, 1:read */
int n = 0;
int isEOF = 0;
int frame_i = 0;

void setup_serial(void);
void send_character(char ch);
int get_character(void);
void send_word(char* word);
char* get_word(void);

int main(void) {
    clrscr();
    setup_serial();
    printf("Send or Receive ('s' or 'r') : ");
    ans = getch();

    if (ans == 's') {
        printf("Send file : ");
        gets(f_dir);
        fp = fopen(f_dir, "r");
        if (!fp) {
            printf("can't open file dir %s\n", f_dir);
            return -1;
        }

        while (!isEOF) {
            if (mode == 1) {
                for (i = 0; i < 7; ++i) {
                    outword[i] = fgetc(fp);
                    if (outword[i] == EOF) {
                        for (j = i; j < 7; ++j) {
                            outword[j] = '\0';
                        }
                        isEOF = 1;
                        break;
                    }
                }
                outword[i] = '\0';
            }

            send_character(frame_i);
            send_word(outword);

            printf("\n\nSend frame : %d\n", frame_i);
            printf("Data       : [");
            for (i = 0; i < 7; ++i) {
                if (outword[i] == '\0') {
                    break;
                }
                printf("%c", outword[i]);
            }
            printf("]\n");
            printf("Timeout    : ");

            status = get_character();
            if (status == 'a') {
                printf("\nReceive ACK%d", get_character());
                mode = 1;
                frame_i ^= 1;
            } else if (status == 't') {
                printf("t\nRetransmit frame %d", i);
                mode = 0;
            } else {  // DEBUG
                printf("Error! [%d - %c]", status, status);
            }
        }

        outword[0] = '\0';
        outword[1] = '\0';

        send_character(frame_i^1);
        send_word(outword);

        fclose(fp);
    }

    if (ans == 'r') {
        i = 0;
        printf("Sender Send : ");
        gets(f_dir);
        fp = fopen(f_dir, "w+");
        if (!fp) {
            printf("can't write file dir %s\n", f_dir);
            return -1;
        }
        do {
            frame_i = get_character();
            inword = get_word();
            if (inword[0] == '\0') {
                break;
            }
            printf("\n\nReceive frame : %d\n", i);
            printf("Data          : [%s]\n", inword);
            printf("Action frame  : ");
            do {
                status = getchar();
            } while (status == '\n');
            if (status == 'a') {
                if (frame_i == i) {
                    printf("Received & Send ACK%d", i + 1);
                    fputs(inword, fp);
                    mode = 1;
                    send_character('a');
                    send_character(i + 1);
                } else {
                    printf("Reject & Send ACK%d", i);
                    mode = 0;
                    send_character('a');
                    send_character(i);
                }

            } else if (status == 'n') {
                if (frame_i == i) {
                    printf("Received & Sleep");
                    fputs(inword, fp);
                    mode = 1;
                } else {
                    printf("Reject & Sleep");
                    mode = 0;
                }
                send_character('t');
            } else if (status == 'r') {
                printf("Reject & Sleep");
				send_character('t');
				mode = 0;
			} else {
				printf("Error! [%d - %c]", status, status);
			}

			if (mode == 1) {
				i ^= 1;
			}

		} while (1);

		fclose(fp);
	}

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
	/* 1  - parity on, 0 – 1 stop bit */
	/* 10 – 7 data bits */
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

void send_word(char* word) {
	int i = 0;
	for (i = 0; i < 7; ++i) {
		send_character(word[i]);
	}
	send_character('\0');
}

char* get_word(void) {
	int i = 0;
	char* word;
	do {
		word[i++] = get_character();
	} while (word[i - 1] != '\0');
	return word;
}
