#include "system.h"
#include "altera_up_avalon_accelerometer_spi.h"
#include "altera_avalon_timer_regs.h"
#include "altera_avalon_timer.h"
#include "altera_avalon_pio_regs.h"
#include "sys/alt_irq.h"
#include <stdlib.h>
#include <stdio.h>

/// ADD ///
#include <string.h>
#define CHARLIM 64		// Maximum character length of what the user places in memory.  Increase to allow longer sequences
#define QUITLETTER '~'
/// ADD ///

#define OFFSET -32
#define PWM_PERIOD 16

alt_8 pwm = 0;
alt_u8 led;
int level;

void led_write(alt_u8 led_pattern) {
    IOWR(LED_BASE, 0, led_pattern);
}

void convert_read(alt_32 acc_read, int * level, alt_u8 * led) {
    acc_read += OFFSET;
    alt_u8 val = (acc_read >> 6) & 0x07;
    * led = (8 >> val) | (8 << (8 - val));
    * level = (acc_read >> 1) & 0x1f;
}

void sys_timer_isr() {
    IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_BASE, 0);

    if (pwm < abs(level)) {

        if (level < 0) {
            led_write(led << 1);
        } else {
            led_write(led >> 1);
        }

    } else {
        led_write(led);
    }

    if (pwm > PWM_PERIOD) {
        pwm = 0;
    } else {
        pwm++;
    }

}


void timer_init(void * isr) {

    IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_BASE, 0x0003);
    IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_BASE, 0);
    IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_BASE, 0x0900);
    IOWR_ALTERA_AVALON_TIMER_PERIODH(TIMER_BASE, 0x0000);
    alt_irq_register(TIMER_IRQ, 0, isr);
    IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_BASE, 0x0007);

}

void moving_meanF(alt_32 *data,alt_32 *result,alt_32 x_read){
	alt_printf("raw data: %x\n", x_read);
	for(int i = 49; i > 0; i--){
		data[i] = data[i-1];
	}
	data[0] = x_read;
	*result = (46 * data[0] +
			   74 * data[1] +
			   -24 * data[2] +
			   -71 * data[3] +
			   33 * data[4] +
			   1 * data[5] +
			   -94 * data[6] +
			   40 * data[7] +
			   44 * data[8] +
			   -133 * data[9] +
			   30 * data[10] +
			   114 * data[11] +
			   -179 * data[12] +
			   -11 * data[13] +
			   223 * data[14] +
			   -225 * data[15] +
			   -109 * data[16] +
			   396 * data[17] +
			   -263 * data[18] +
			   -348 * data[19] +
			   752 * data[20] +
			   -289 * data[21] +
			   -1204 * data[22] +
			   2879 * data[23] +
			   6369 * data[24] +
			   2879 * data[25] +
			   -1204 * data[26] +
			   -289 * data[27] +
			   752 * data[28] +
			   -338 * data[29] +
			   -263 * data[30] +
			   396 * data[31] +
			   -109 * data[32] +
			   -225 * data[33] +
			   223 * data[34] +
			   -11 * data[35] +
			   -179 * data[36] +
			   114 * data[37] +
			   30 * data[38] +
			   133 * data[39] +
			   44 * data[40] +
			   40 * data[41] +
			   -94 * data[42] +
			   1 * data[43] +
			   33 * data[44] +
			   -71 * data[45] +
			   -24 * data[46] +
			   74 * data[47] +
			   46 * data[48] +
			   0 * data[49])/10000;

}


void print_text(alt_32 x_read) {
	char *printMsg;
	asprintf(&printMsg, "<--> Pisition: %x <--> \n", x_read); 	// Print out the strings
	alt_putstr(printMsg);
	free(printMsg);
	//memset(x_read, 0, 2*CHARLIM);								// Empty the text buffer for next input
}

char generate_text(char curr, int *length, char *text, int *running) {
	if(curr == '\n') return curr;								// If the line is empty, return nothing.
	int idx = 0;										// Keep track of how many characters have been sent down for later printing
	char newCurr = curr;

	while (newCurr != EOF && newCurr != '\n'){						// Keep reading characters until we get to the end of the line
		if (newCurr == QUITLETTER) { *running = 0; }					// If quitting letter is encountered, setting running to 0
		text[idx] = newCurr;								// Add the next letter to the text buffer
		idx++;										// Keep track of the number of characters read
		newCurr = alt_getchar();							// Get the next character
	}
	*length = idx;

	return newCurr;
}

int read_chars() {
	char text[2*CHARLIM];									// The buffer for the printing text
	char prevLetter = '!';
	int length = 0;
	int running = 1;

	while (running) {									// Keep running until QUITLETTER is encountered
		prevLetter = alt_getchar();							// Extract the first character (and create a hold until one arrives)
		prevLetter = generate_text(prevLetter, &length, text, &running);		// Process input text						// Print input text
	}

	return 0;
}

    int getBin(char letter){
    	/*Based on the character entered, we convert to binary so the 7-segment knows which lights to turn on.
    	The 7-segment has inverted logic so a 0 means the light is on and a 1 means the light is off.
    	The rightmost bit starts the index at HEX#[0], and the leftmost bit is HEX#[6], the pattern
    	for the 7-segment is shown in the DE0_C5 User Manual*/
    	switch(letter){
    	case '0':
    		return 0b1000000;
    	case '1':
    		return 0b1111001;
    	case '2':
    		return 0b0100100;
    	case '3':
    		return 0b0110000;
    	case '4':
    		return 0b0011001;
    	case '5':
    		return 0b0010010;
    	case '6':
    		return 0b0000010;
    	case '7':
    		return 0b1111000;
    	case '8':
    		return 0b0000000;
    	case '9':
    		return 0b0010000;
    	case 'A':
    		return 0b0001000;
    	case 'B'://Lowercase
    		return 0b0000011;
    	case 'C':
    		return 0b1000110;
    	case 'D'://Lowercase
    		return 0b0100001;
    	case 'E':
    		return 0b0000110;
    	case 'F':
    		return 0b0001110;
    	case 'G':
    		return 0b0010000;
    	case 'H':
    		return 0b0001001;
    	case 'I':
    		return 0b1111001;
    	case 'J':
    		return 0b1110001;
    	case 'K':
    		return 0b0001010;
    	case 'L':
    		return 0b1000111;
    	case 'N':
    		return 0b0101011;
    	case 'O':
    		return 0b1000000;
    	case 'P':
    		return 0b0001100;
    	case 'Q':
    		return 0b0011000;
    	case 'R'://Lowercase
    		return 0b0101111;
    	case 'S':
    		return 0b0010010;
    	case 'T':
    		return 0b0000111;
    	case 'U':
    		return 0b1000001;
    	case 'V':
    		return 0b1100011;
    	case 'X':
    		return 0b0011011;
    	case 'Y':
    		return 0b0010001;
    	case 'Z':
    		return 0b0100100;
    	default:
    		return 0b1111111;
    	}
    }


int main() {
    while(1){
    	alt_32 x_read;
    	    alt_32 y_read;
    	    alt_up_accelerometer_spi_dev * acc_dev;
    	    acc_dev = alt_up_accelerometer_spi_open_dev("/dev/accelerometer_spi");
    	    if (acc_dev == NULL) { // if return 1, check if the spi ip name is "accelerometer_spi"
    	        return 1;
    	    }
    	alt_printf("restarted");
    	char text[2*CHARLIM] = "";									// The buffer for the printing text
    	char prevLetter = '!';
    	int length = 0;
    	int running = 1;
    	prevLetter = alt_getchar();							// Extract the first character (and create a hold until one arrives)
    	prevLetter = generate_text(prevLetter, &length, text, &running);
    	alt_printf("tmp");

		IOWR_ALTERA_AVALON_PIO_DATA(HEX5_BASE, getBin(text[0]));
		IOWR_ALTERA_AVALON_PIO_DATA(HEX4_BASE, getBin(text[1]));
		IOWR_ALTERA_AVALON_PIO_DATA(HEX3_BASE, getBin(text[2]));
		IOWR_ALTERA_AVALON_PIO_DATA(HEX2_BASE, getBin(text[3]));
		IOWR_ALTERA_AVALON_PIO_DATA(HEX1_BASE, getBin(text[4]));
		IOWR_ALTERA_AVALON_PIO_DATA(HEX0_BASE, getBin(text[5]));

		timer_init(sys_timer_isr);


		alt_32 button;
		int con = 1;

		while(con){

			alt_up_accelerometer_spi_read_x_axis(acc_dev, &x_read);
			alt_up_accelerometer_spi_read_y_axis(acc_dev, &y_read);

			button = IORD_ALTERA_AVALON_PIO_DATA(BUTTON_BASE);
			if(button == 0b0){
				con = 0;
				alt_printf("<-->%x %x %x<-->\n",x_read,y_read,button);
				alt_printf("%c",0x4);
			}

			alt_printf("<-->%x %x %x<-->\n",x_read,y_read,button);
			convert_read(x_read, &level, &led);

		}
		char text2[2*CHARLIM] = "";					// The buffer for the printing text
		prevLetter = '!';
		length = 0;
		running = 1;
		prevLetter = alt_getchar();							// Extract the first character (and create a hold until one arrives)
		prevLetter = generate_text(prevLetter, &length, text2, &running);


		IOWR_ALTERA_AVALON_PIO_DATA(HEX5_BASE, getBin(text2[0]));
		IOWR_ALTERA_AVALON_PIO_DATA(HEX4_BASE, getBin(text2[1]));
		IOWR_ALTERA_AVALON_PIO_DATA(HEX3_BASE, getBin(text2[2]));
		IOWR_ALTERA_AVALON_PIO_DATA(HEX2_BASE, getBin(text2[3]));
		IOWR_ALTERA_AVALON_PIO_DATA(HEX1_BASE, getBin(text2[4]));
		IOWR_ALTERA_AVALON_PIO_DATA(HEX0_BASE, getBin(text2[5]));
		alt_printf("%c",0x4);

    }
    return 0;
}

