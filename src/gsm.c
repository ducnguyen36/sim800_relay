
#include "gsm.h"
void gsm_init(){
    


/******* Initial SIM URAR *****/

	SCON = 0x50;  // 8 bits, variable baudrate 
	AUXR |= 0x40;		//Timer1's clock is Fosc (1T)
	AUXR &= 0xFE;		//Use Timer1 as baudrate generator
	TL1= (65536 - (FOSC/4/gsm_BAUD));	
	TH1= (65536 - (FOSC/4/gsm_BAUD))>>8;
	ET1 = 0;
	TR1 = 1;
	ES = 1;
    gsm_pw = 1;
}
void send_gsm_byte(u8 dulieu){
	if(!dulieu) return;
	gsm_TI = 0;
	gsm_SBUF = dulieu;
	while(!gsm_TI)WATCHDOG;
	gsm_TI = 0;
}

void send_gsm_cmd(u8 *cmd){
	gsm_TI = 0;
    while(*cmd){
		gsm_SBUF = *cmd;
		while(!gsm_TI)WATCHDOG;
        gsm_TI=0;
		if(*cmd==26) return;
		cmd++;
	}
}

