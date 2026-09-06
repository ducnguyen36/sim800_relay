#include "true.h"
#include "main.h"
/*
	RF-UART-TEST — firmware CHAN DOAN remote RF.

	Muc dich: remote truoc chay duoc, gio khong. Firmware nay BO HET GSM/SMS/
	menu/LCD/thoi-gian, CHI giu:
	  - Khoi tao port + PCA timer y HET production (de dieu kien thu tin RF
	    giong het firmware that).
	  - Bo giai ma RF trong PCA_Handler (motor_cam_phim.c) — dung nguyen xi.
	  - UART TX 38400 (polled) de in ket qua ra COM cho may tinh.

	Cach dung: nap firmware, mo terminal COM (38400 8N1) tren may tinh, bam
	nut remote. Moi frame giai ma duoc se in 1 dong:

	    P<pt>-<data0><data1><data2>-<c0><c1><c2><c3>-<idx>-<match>

	  pt      : 1 neu la ma PT2240, 0 neu khac
	  dataN   : 3 byte ma remote (hex)
	  cN      : 4 bit nut (0 = nut do dang bam, active-low)
	  idx     : so bit thu duoc (binh thuong 24)
	  match   : 0 = khong khop remote da hoc; >0 = co khop (in kem chi so khe)

	Chan doan:
	  - KHONG thay dong nao khi bam remote  -> RF khong thu duoc (an-ten/nguon/
	    tan so/module thu) hoac nut khong phat.
	  - Thay dong nhung idx != 24 / data nhay lung tung -> nhieu, thu sai.
	  - data on dinh nhung match=0 -> remote chua hoc / du lieu hoc bi hong.
	  - Dau '.' in ra moi ~2 giay = firmware + timer dang chay (dau hieu song).
*/

u8 __code ver[] = "RF-UART-TEST v1";

#include "motor_cam_phim.c"   /* PCA_Timer_init + xunggiay + PCA_Handler (giai ma RF) */

void main() {

	/*PORT IO INIT — GIU Y HET production de chan thu RF cau hinh giong nhau*/
	P0M1 = 0; P0M0 = 0xff;
	P1M1 = P1M0 = 0;
	P2M1 = 0; P2M0 = 0xff;
	P3M1 = P3M0 = 0;      /* P36 = chan thu RF (cam_che) */
	P4M1 = 2; P4M0 = 0x1c;
	P5M1 = P5M0 = 0;
	P4 = 0;
	P3 = 0xff;
	P2 = 0;

	IP = 0x81;   /* uu tien PCA */
	CLK_DIV = 0;
	EA = 1;

	/* Khoi tao cac bo dem xunggiay() dung toi, tranh no tu dat co reset gsm */
	delay_chay_khoi_tao = 30;
	gsm_delay_reset = 10;
	phim_mode_doi = phim_cong_doi = 2;
	phim_back_doi = 6;
	gsm_pw = 1;    /* != 0 -> xunggiay() khong dat gsm_reset */
	nosim = 1;
	mode = 0;

	/*UART TX 38400 (polled) — KHONG bat ngat serial (khong can ISR RX)*/
	SCON = 0x50;
	AUXR |= 0x40;      /* Timer1 clock = Fosc (1T) */
	AUXR &= 0xFE;      /* Timer1 lam baudrate generator */
	TL1 = (65536 - (FOSC/4/gsm_BAUD)) & 0xFF;
	TH1 = (65536 - (FOSC/4/gsm_BAUD)) >> 8;
	ET1 = 0;
	TR1 = 1;
	ES  = 0;

	PCA_Timer_init();   /* chay PCA -> bat dau giai ma RF trong PCA_Handler */

	send_gsm_cmd("\r\n=== ");
	send_gsm_cmd(ver);
	send_gsm_cmd(" ===\r\n");
	send_gsm_cmd("Bam nut remote. Dinh dang: P<pt>-<data>-<cmd>-<idx>-<match>\r\n");

	{
		u8 last_second = 0xff;
		while(1){
			WATCHDOG;

			/* Nhip song: in '.' moi ~2 giay de biet firmware + timer con chay */
			if(second != last_second){
				last_second = second;
				if(!(second & 1)) send_gsm_byte('.');
			}

			if(rfprocess){
				u8 i,data[3],cmd[4];
				u8 match=0;
				u8 nrf = (eep_rfindex>99) ? 0 : eep_rfindex;  /* clamp flash rong */

				/* ===== giai ma GIONG HET production (main.c) ===== */
				data[0]  = rfdata[0]*128 +rfdata[1]*64+rfdata[2]*32+rfdata[3]*16;
				data[0] += rfdata[4]*8 + rfdata[5]*4 + rfdata[6]*2 +rfdata[7];
				data[1]  = rfdata[8]*128 +rfdata[9]*64+rfdata[10]*32+rfdata[11]*16;
				data[1] += rfdata[12]*8 + rfdata[13]*4 + rfdata[14]*2 +rfdata[15];
				if(pt2240){
					data[2] = rfdata[16]*8 + rfdata[17]*4 + rfdata[18]*2 +rfdata[19];
					cmd[1] = rfdata[21]; cmd[2] = rfdata[22]; cmd[3] = rfdata[23];
				}
				else{
					data[2] = 0;
					cmd[1] = rfdata[22]; cmd[2] = rfdata[18]; cmd[3] = rfdata[16];
				}
				cmd[0] = rfdata[20];

				/* ===== in ra UART ===== */
				send_gsm_byte('P');
				send_gsm_byte(pt2240+'0');
				send_gsm_byte('-');
				send_gsm_hex(data[0]);
				send_gsm_hex(data[1]);
				send_gsm_hex(data[2]);
				send_gsm_byte('-');
				send_gsm_byte(cmd[0]+'0');
				send_gsm_byte(cmd[1]+'0');
				send_gsm_byte(cmd[2]+'0');
				send_gsm_byte(cmd[3]+'0');
				send_gsm_byte('-');
				send_gsm_byte(rfindex/10+'0');
				send_gsm_byte(rfindex%10+'0');
				send_gsm_byte('-');

				/* doi chieu remote da hoc (chi doc flash, KHONG ghi) */
				for(i=0;!match && i<nrf+2;i++){
					if(i<2 && ((eep_rfdata[i*3]==0 && eep_rfdata[i*3+1]==0 && eep_rfdata[i*3+2]==0)
					        || (eep_rfdata[i*3]==0xff && eep_rfdata[i*3+1]==0xff && eep_rfdata[i*3+2]==0xff))) continue;
					match = data[0] == eep_rfdata[i*3] && data[1] == eep_rfdata[i*3+1] && data[2] == eep_rfdata[i*3+2];
					if(match){
						if(i<2)match = i+2;
						send_gsm_byte(i/10+'0');
						send_gsm_byte(i%10+'0');
					}
				}
				send_gsm_byte('-');
				send_gsm_byte(match+'0');
				send_gsm_byte('\r');
				send_gsm_byte('\n');

				rfstatus = 0;
				rfprocess = 0;
			}
		}
	}
}
