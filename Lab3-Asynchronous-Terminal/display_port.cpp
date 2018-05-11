#include <stdio.h>
#include <dos.h>

void main(void) {
	unsigned int far *ptraddr;			/* Pointer to location of Port Addresses */
	unsigned int address;				/* Address of Port */
	ptraddr = (unsigned int far *)0x00000400;

	int a;
	for (a=0; a<4; ++a) {

		address = *ptraddr;
		if (address == 0) {
			printf("No port found for COM%d \n",a+1);
		} else {
			printf("Address assigned to COM%d is %Xh\n", a+1, address);
		}

		*ptraddr++;
	}
}
