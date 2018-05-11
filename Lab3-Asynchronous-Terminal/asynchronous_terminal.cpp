#define COM3BASE 0x3E8
#define COM4BASE 0x2E8
#define TXDATA COM3BASE
#define LCR (COM3BASE+3)	/* 0x3F8 line control*/
#define LSR (COM3BASE+5)	/* 0x3FD line status*/

#include <conio.h>
#include <dos.h>
#include <stdio.h>
#include <string.h>

void setup_serial(void);
void send_character(char);
int get_character(void);
void send_string(void);
void get_string(void);

int inchar, outchar;	/* serial buffer */
char mode;				/* 0:Sender, 1:Receive */
char str[128];					/* keyboad buffer */
int i=0;

int main(void) {
	setup_serial();
	printf("Send(s) or Receive(r) : ");
	mode = getch();

	if (mode == 's') {		/* Mode Sender */
		while(1) {
			send_string();
			get_string();
		}
	}

	if (mode == 'r') {		/* Mode Receive */
		while(1) {
			get_string();
			send_string();
		}
	}

	return 0;
}

void setup_serial(void) {
	outportb(LCR, 0x80);
	/*set up bit 7 to a 1 to set Register address bit*/
	outportb(TXDATA,0x0C);
	outportb(TXDATA+1,0X00);
	outportb(LCR, 0xA);
	/*load TxRegister with 12, crystal frequency is 1.8432 MHz*/  		outportb(LCR, 0x0A);
	/* Bit pattern loads is 00001010b, from MSB to LSB these are: */
	/* 0  - access TD/RD buffer, 0 - normal output */
	/* 0  - no stick bit, 0 - even parity */
	/* 1  - parity on, 0 – 1 stop bit */
	/* 10 – 7 data bits */
}
void send_character(char ch) {
	char status;
	do{
		status = inportb(LSR) & 0x40;
	} while ( status!=0x40);
	/*repeat until Tx buffer empty ie bit 6 set */
	outportb(TXDATA,(char) ch);
}
int get_character(void) {
	int status;
	do{
		status = inportb(LSR) & 0x01;
	}while (status!=0x01);
	/*Repeat until bit 1 in LSR is set */
	return( (int) inportb(TXDATA));
}

void send_string(void) {
	printf("Enter char to be transmitted (Ctrl-D to end)");
	printf("PC1 : ");
	gets(str);						/* get string until enter */
	for (i=0; i<strlen(str); i++) {
		send_character(str[i]);		/* send string to serial */
	}
	send_character('\0');			/* send null */
}

void get_string(void) {
	puts("PC2 : ");
	while(1) {						/* read char until 17 or null */
		inchar = get_character();
		if (inchar == 17) {			/* Ctrl-D */
			send_character(17);		/* ack for end connection */
			return 0;
		}
		printf("%c", inchar);

		if(inchar == '\0') {		/* found null */
			printf("\n");
			break;
		}
	}
}
