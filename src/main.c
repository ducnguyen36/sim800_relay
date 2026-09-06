#include "true.h"
#include "main.h"
/*
	RF TIMING LOGGER — cong cu do timing that cua remote de thiet ke decoder 2.0.

	Lay mau chan RF (cam_che = P36) moi 125us. Moi khi thay 1 khoang lang dai
	(het frame), in ra UART day do dai cac xung (tinh bang so mau 125us), xen ke
	CAO (H) / THAP (L):

	    FR:L02 H07 L07 H02 L02 H07 ...

	=> Tu day biet base-time Te that cua remote + mau bit, de viet decoder tu
	hieu chinh (2.0) dung so, khong doan.

	Cach dung: nap, mo COM 38400 8N1, bam giu 1 nut remote ~1 giay, copy vai dong
	FR: cho lai. Lam voi CA remote "nhay" va remote "kem" de so.
*/

u8 __code ver[] = "RF TIMING LOG";

u8   __xdata rf_run = 0;     // do dai run hien tai (so mau)
__bit         rf_lvl = 0;    // muc hien tai (1=cao)
u8   __xdata rflog[64];      // buffer run: bit7=muc, bit0..6=do dai (bao hoa 127)
u8   __xdata rflog_n = 0;
__bit         rflog_ready = 0;

void PCA_Timer_init(){
	CCAP1L = CCAP1H = 0;
	PCA_Timer1 = 125;
	CCAPM0 = 0;        // tat module 0 (khong can key/second cho logger)
	CCAPM1 = 0x49;     // module 1: compare + ngat
	CR = 1;
}

void PCA_Handler(void) __interrupt PCA_VECTOR {
	WATCHDOG;
	if(CCF1){
		CCF1 = 0;
		CCAP1L = PCA_Timer1;
		CCAP1H = PCA_Timer1 >> 8;
		PCA_Timer1 += 125;         // lay mau 125us
		if(rflog_ready) return;    // dang cho main in xong
		{
			__bit lv = cam_che ? 1 : 0;
			if(lv == rf_lvl){
				if(rf_run < 250) rf_run++;
				// LOW dai = khoang lang giua frame -> ket thuc 1 frame
				if(!rf_lvl && rf_run > 60 && rflog_n > 8) rflog_ready = 1;
			}else{
				if(rflog_n < 64)
					rflog[rflog_n++] = (rf_lvl ? 0x80 : 0) | (rf_run > 127 ? 127 : rf_run);
				rf_lvl = lv;
				rf_run = 1;
			}
		}
	}
}

void main(){
	/*PORT IO INIT — giong production de chan RF cau hinh giong nhau*/
	P0M1 = 0; P0M0 = 0xff;
	P1M1 = P1M0 = 0;
	P2M1 = 0; P2M0 = 0xff;
	P3M1 = P3M0 = 0;       /* P36 = chan thu RF */
	P4M1 = 2; P4M0 = 0x1c;
	P5M1 = P5M0 = 0;
	P4 = 0; P3 = 0xff; P2 = 0;

	IP = 0x81;
	CLK_DIV = 0;
	EA = 1;

	gsm_init();   /* init UART 38400 (cung bat ES=1) */
	ES = 0;       /* tat ngat serial: logger chi TX, khong can ISR RX */
	PCA_Timer_init();

	send_gsm_cmd("\r\n=== RF TIMING LOG (125us/mau) ===\r\n");
	send_gsm_cmd("Bam remote. Moi frame: FR:<L/H><so mau> ...\r\n");

	while(1){
		WATCHDOG;
		if(rflog_ready){
			u8 i, v;
			send_gsm_cmd("FR:");
			for(i=0;i<rflog_n;i++){
				send_gsm_byte(rflog[i] & 0x80 ? 'H' : 'L');
				v = rflog[i] & 0x7f;
				send_gsm_byte(v/10 + '0');
				send_gsm_byte(v%10 + '0');
				send_gsm_byte(' ');
			}
			send_gsm_byte('\r');
			send_gsm_byte('\n');
			rflog_n = 0;
			rf_run = 0;
			rf_lvl = cam_che ? 1 : 0;
			rflog_ready = 0;
		}
	}
}
