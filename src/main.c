#include "true.h"
#include "main.h"
/*
	RF TIMING LOGGER v2 — do timing that cua remote (60us/mau, cat frame dung).

	Cai tien so voi v1:
	  - Lay mau 60us (min gap doi v1) de khoi under-sample xung ngan.
	  - Cat frame DUNG: mot "frame" = doan giua 2 khoang lang lon (idle gap).
	  - In kem so xung cua frame: "N=48:" -> frame sach 24 bit thuong ~48 xung.
	  - In do dai 3 chu so (khong con loi '<' nhu v1).

	Doc ket qua:
	  - Neu giu 1 nut ma cac dong "N=..:" co so xung ON DINH (~40-52) va mau
	    xung LAP LAI giong nhau -> tin hieu sach -> em viet decoder tu-hieu-chinh.
	  - Neu so xung nhay lung tung, mau khong lap -> tin hieu nhieu/yeu -> nut
	    that o KHAU THU (module/antenna/remote), khong phai thuat toan.

	Cach dung: nap, COM 38400 8N1, GIU 1 nut ~1-2 giay, copy 4-5 dong "N=..:".
*/

u8 __code ver[] = "RF TIMING LOG2";

#define IDLE_GAP   100      // LOW > 100 mau (~6ms @60us) = khoang lang giua frame

u8   __xdata rf_run = 0;
__bit         rf_lvl = 0;
u8   __xdata rflog[80];
u8   __xdata rflog_n = 0;
__bit         seen_gap = 0;   // da gap it nhat 1 idle gap de canh frame
__bit         rflog_ready = 0;

void PCA_Timer_init(){
	CCAP1L = CCAP1H = 0;
	PCA_Timer1 = 60;
	CCAPM0 = 0;
	CCAPM1 = 0x49;
	CR = 1;
}

void PCA_Handler(void) __interrupt PCA_VECTOR {
	WATCHDOG;
	if(CCF1){
		CCF1 = 0;
		CCAP1L = PCA_Timer1;
		CCAP1H = PCA_Timer1 >> 8;
		PCA_Timer1 += 60;              // 60us
		if(rflog_ready) return;
		{
			__bit lv = cam_che ? 1 : 0;
			if(lv == rf_lvl){
				if(rf_run < 250) rf_run++;
				// LOW dai = idle gap -> ket thuc frame dang thu
				if(!rf_lvl && rf_run == IDLE_GAP){
					if(seen_gap && rflog_n >= 8){
						rflog_ready = 1;   // co 1 frame day du giua 2 gap
					}else{
						seen_gap = 1;      // gap dau tien: bat dau canh
						rflog_n = 0;
					}
				}
			}else{
				// canh: luu run vua ket thuc (chi luu khi da qua idle gap dau)
				if(seen_gap && !rflog_ready && rflog_n < 80)
					rflog[rflog_n++] = (rf_lvl ? 0x80 : 0) | (rf_run > 127 ? 127 : rf_run);
				rf_lvl = lv;
				rf_run = 1;
			}
		}
	}
}

void main(){
	P0M1 = 0; P0M0 = 0xff;
	P1M1 = P1M0 = 0;
	P2M1 = 0; P2M0 = 0xff;
	P3M1 = P3M0 = 0;
	P4M1 = 2; P4M0 = 0x1c;
	P5M1 = P5M0 = 0;
	P4 = 0; P3 = 0xff; P2 = 0;

	IP = 0x81;
	CLK_DIV = 0;
	EA = 1;

	gsm_init();
	ES = 0;
	PCA_Timer_init();

	send_gsm_cmd("\r\n=== RF TIMING LOG2 (60us/mau) ===\r\n");
	send_gsm_cmd("Giu 1 nut ~1-2s. Moi frame: N=<so xung>: <L/H><so mau> ...\r\n");

	while(1){
		WATCHDOG;
		if(rflog_ready){
			u8 i, v;
			send_gsm_cmd("N=");
			send_gsm_byte(rflog_n/10 + '0');
			send_gsm_byte(rflog_n%10 + '0');
			send_gsm_byte(':');
			send_gsm_byte(' ');
			for(i=0;i<rflog_n;i++){
				send_gsm_byte(rflog[i] & 0x80 ? 'H' : 'L');
				v = rflog[i] & 0x7f;
				send_gsm_byte(v/100 + '0');
				send_gsm_byte((v/10)%10 + '0');
				send_gsm_byte(v%10 + '0');
				send_gsm_byte(' ');
			}
			send_gsm_byte('\r');
			send_gsm_byte('\n');
			// bat dau frame moi (gap vua roi la canh)
			rflog_n = 0;
			rf_run = 0;
			rf_lvl = cam_che ? 1 : 0;
			seen_gap = 1;
			rflog_ready = 0;
		}
	}
}
