
#include "true.h"
#include "main.h"
// _IAP_CONTR = 0x60 //reset to ISP
/*
	Change Log:
		0.2.6: +doi relay 4 thanh relas4 va nguoc lai
			de khong can module mo rong van khoi dong lai thang may duoc
		0.2.7: kiem tra co phone master moi gui thong bao khoi dong

*/

u8 __code ver[] = " THANGMAY 0.3.0";

#include "motor_cam_phim.c"
#include "gsm_serial.c"
#include "xu_ly_tin_nhan.c"

void main() {
	
	/*PORT IO INIT*/
	P0M1 = 0; P0M0 = 0xff; //port LCD -- chân xuất với điện trở kéo lên nhỏ, dòng lớn -> 20mA
	P1M1 = P1M0 = 0;
	P2M1 = 0; P2M0 = 0xff; // output
	P3M1 = P3M0 = 0; //full compatip 8051 -- key and cam
	P4M1 = 2; P4M0 = 0x1c;//full compatip 8051 --motor port
	P5M1 = P5M0 = 0; // full compatip 8051
	P4 = 0;
	P3 = 0xff;
	P2 = 0;
	/****************/
	IP = 0x81; //priority PCA
	CLK_DIV = 0;
	EA = 1; //bat tat ca interupt
	/****************/
	bao_loi_bien_tan = loi_bien_tan = 0;
	bao_loi_stop = che_do_stop = 0;
	delay_tinhieuD_cao = delay_tinhieuD_thap = 80;
	delay_tinhieuA_cao = delay_tinhieuA_thap = 80;
	so_lan_goi_dien = 0;
	gsm_delay_reset=10;
	relays4_delay_tat = 6;
	phim_mode_doi = phim_back_doi = phim_cong_doi = 2;
	mode = 0;

	/*validate eeprom*/
	// u8 __xdata i;
	IAP_docxoasector1();
	if(((eeprom_buf[PIN_EEPROM]  <'0' && eeprom_buf[PIN_EEPROM]  > 9) || eeprom_buf[PIN_EEPROM]  >'9')
	|| (eeprom_buf[PIN_EEPROM+1]<'0' || eeprom_buf[PIN_EEPROM+1]>'9')
	|| (eeprom_buf[PIN_EEPROM+2]<'0' || eeprom_buf[PIN_EEPROM+2]>'9')
	|| (eeprom_buf[PIN_EEPROM+3]<'0' || eeprom_buf[PIN_EEPROM+3]>'9'))
	eeprom_buf[PIN_EEPROM] = eeprom_buf[PIN_EEPROM+1] = eeprom_buf[PIN_EEPROM+2] = eeprom_buf[PIN_EEPROM+3] = '0';
	
	IAP_ghisector1();	
	
	/*Khoi tao serial baudrate 57600 cho gsm sim900*/
	gsm_init();
	
	/*PCA TIMER 0 INIT 50us*/
	PCA_Timer_init();	
	
  	// /*Khoi tao man hinh LCD*/
	LCD_Init();

	gsm_thietlapgoidien();
	gsm_thietlapnhantin();

	mode_wait = 60;

	/******** Initial watdog ****WDT**/	
	WDT_CONTR = EN_WDT | CLR_WDT | WDT_SCALE_64; // Enable watchdog, clear watchdog, pre scale = 64, watchdog idle mode = NO
	
	phone[0] = '0';
	phone[10] = 0;
	if(kiemtraphonemaster()) baocaosms("\rKhoi dong");

	while(1){
		if(!mode_wait && mode) mode = 0;

		if(!bao_loi_bien_tan && loi_bien_tan){
			bao_loi_bien_tan = 1;
			if(kiemtraphonemaster())baocaosms("\rLoi bien tan");
		}
		if(bao_loi_bien_tan && !loi_bien_tan){
			bao_loi_bien_tan = 0;
			if(kiemtraphonemaster())baocaosms("\rHet loi bien tan");
		}

		if(!bao_loi_stop && che_do_stop){
			bao_loi_stop = 1;
			if(kiemtraphonemaster())baocaosms("\rThang may vao che do STOP");
		}
		if(bao_loi_stop && !che_do_stop){
			bao_loi_stop = 0;
			if(kiemtraphonemaster())baocaosms("\rThang may thoat khoi che do STOP");
		}

		if(co_tin_nhan_moi){
			co_tin_nhan_moi = 0;
			gsm_sendandcheck("AT\r", 15, 1,ver);
			send_gsm_cmd("AT+CMGL=\"ALL\"\r");
		}
		if(sms_dang_xu_ly){
			xu_ly_tin_nhan();
			gsm_sendandcheck("AT+CMGDA=\"DEL ALL\"\r",15,1," DELETING SMS ");
			sms_dang_xu_ly = 0;
		}
		if(phim_mode_nhan){
			phim_mode_nhan = 0;
			mode = (mode+1)%4;
			mode_wait = 60;
			LCD_xoa(TREN);
		}
		if(phim_back_nhan){
			// u8 thong_so[4];
			phim_back_nhan = 0;
			xoadanhba(0);
			IAP_docxoasector1();
			eeprom_buf[PIN_EEPROM] = eeprom_buf[PIN_EEPROM+1] = eeprom_buf[PIN_EEPROM+2] = eeprom_buf[PIN_EEPROM+3] = '0';
			IAP_ghisector1();
		}
		if(gsm_reset){
			gsm_reset = 0;
			gsm_thietlapgoidien();
			gsm_thietlapnhantin();
		}
		if(phim_cong_nhan){
			phim_cong_nhan = 0;
			gsm_serial_cmd = CUS2;
			gsm_sendandcheck("AT+CUSD=1,\"*110#\",\r",3,30,"  KIEM TRA SDT  ");
			LCD_guilenh(0x80);
			LCD_guichuoi("SDT:");
			LCD_guilenh(0x84);
			phone[10] = 0; 
			LCD_guichuoi(phone);
			while(!phim_cong_nhan) WATCHDOG;
			phim_cong_nhan = 0;
		}
		if(lcd_update_chop){
			lcd_update_chop = 0;	
			LCD_guilenh(0x80);
			if(mode==1) LCD_guichuoi("CHI:");
			else if(mode==2) LCD_guichuoi("PHU:");
			else if(mode==3) LCD_guichuoi("TAM:");
			else LCD_guichuoi(Relay1? "ON :":"OFF:");
			LCD_guigio(0xc0,"  TMA  ",hour,minute,second,flip_pulse);
		}
		if(phone_update){
			phone_update = 0;
			if(co_cuoc_goi_toi){
				co_cuoc_goi_toi = 0;
				if(mode){
					gsm_sendandcheck("AT\r",15,1,ver);
					phone[10] = 0;
					if(phone_so_sanh_that_bai) gsm_themdanhba(phone,mode==1?'m':(mode==2?'u':'t'));
					gsm_sendandcheck("AT+CPBR=1,99\r", 15, 1,"  SENDING CPBR  ");
					baocaosms("\rLuu danh ba thanh cong");
				}else{
					if(!phone_so_sanh_that_bai){
						RelayS4 = 1;
						baocaosms("\rTAT BAT = CUOC GOI");
					} 
				}
			}	
			LCD_guilenh(0x84);
			phone[10] = 0; 
			LCD_guichuoi(phone);
		}
		WATCHDOG;
	}
}



