#include "true.h"
#include "main.h"
// _IAP_CONTR = 0x60 //reset to ISP


u8 __code ver[] = " CUACUON 0.3.0";

#include "motor_cam_phim.c"
#include "gsm_serial.c"

void luu_lich_su(u8 *phone,u8 cmd){
	u8 temp=0,i=0;
	gsm_sendandcheck("AT\r",15,1,ver);
	send_gsm_cmd("AT+CPBF=\"");
	send_gsm_byte(phone_master?'m':'u');
	phone[10] = 0;
	send_gsm_cmd(phone);
	send_gsm_byte(phone_master?'m':'u');
	lenh_sms[159] = 9;
	sms_index = 0;
	gsm_sendandcheck("\"\r",15,2," SAVING HISTORY ");
	// signal = lenh_sms[0];
	while(lenh_sms[i]<58)temp = temp*10 + lenh_sms[i++]-48;
	temp = temp*10 + lenh_sms[i] - 65;
	IAP_docxoasector1();
	i = eeprom_buf[INDEX_HISTORY_EEPROM];
	// signal = i;
	eeprom_buf[i*4+8] = temp;
	// signal = cmd;
	temp = (cmd<<2) + ((year-20)<3?year-20:3);
	// signal = temp;
	eeprom_buf[i*4+ 9] = (temp<<4) + month;
	eeprom_buf[i*4+10] = (day<<3) + (hour>>2);
	eeprom_buf[i*4+11] = ((hour&3)<<6) + minute;
	if(eeprom_buf[INDEX_HISTORY_EEPROM]==99)eeprom_buf[INDEX_HISTORY_EEPROM]=0;
	else eeprom_buf[INDEX_HISTORY_EEPROM]++;
	IAP_ghisector1();
}

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
	
	so_lan_goi_dien = 0;
	gsm_delay_reset=10;
	relay1_delay_tat = 6;
	relay2_delay_tat = 2;
	relay3_delay_tat = 2;
	relay4_delay_tat = 2;
	phim_mode_doi = phim_cong_doi = 2;
	phim_back_doi = 6;
	mode = 0;
	nosim=0;

	/*validate eeprom*/
	// u8 __xdata i;
	IAP_docxoasector1();
	if(eeprom_buf[INDEX_HISTORY_EEPROM]>99)eeprom_buf[INDEX_HISTORY_EEPROM]=0;
	if(eeprom_buf[BAOCAO_EEPROM]>1) eeprom_buf[BAOCAO_EEPROM] = 0;
	if(eeprom_buf[KHOA_EEPROM]>1) eeprom_buf[KHOA_EEPROM] = 0;
	if(eeprom_buf[UPS_EEPROM]>10) eeprom_buf[UPS_EEPROM] = 0;
	if((eeprom_buf[PIN_EEPROM]  <'0' || eeprom_buf[PIN_EEPROM]  >'9')
	|| (eeprom_buf[PIN_EEPROM+1]<'0' || eeprom_buf[PIN_EEPROM+1]>'9')
	|| (eeprom_buf[PIN_EEPROM+2]<'0' || eeprom_buf[PIN_EEPROM+2]>'9')
	|| (eeprom_buf[PIN_EEPROM+3]<'0' || eeprom_buf[PIN_EEPROM+3]>'9'))
	eeprom_buf[PIN_EEPROM] = eeprom_buf[PIN_EEPROM+1] = eeprom_buf[PIN_EEPROM+2] = eeprom_buf[PIN_EEPROM+3] = '0';
	
	IAP_ghisector1();

	IAP_docxoasector2();	
	if(eeprom_buf[RFINDEX_EEPROM-SECTOR2]>99)eeprom_buf[RFINDEX_EEPROM-SECTOR2]=0;
	IAP_ghisector2();
	relay2giu = 0;
	Relay2 = eep_khoa;
	Relay4 = eep_ups?1:0;

	/*Khoi tao serial baudrate 57600 cho gsm sim900*/
	// delay_ms(5000);
	gsm_init();
	
	/*PCA TIMER 0 INIT 50us*/
	PCA_Timer_init();	
	
  	// /*Khoi tao man hinh LCD*/
	LCD_Init();

	// gsm_thietlapsim800();
	// if(!nosim && gsm_thietlapsim800()){
	// 	gsm_thietlapngaygiothuc();
	// 	gsm_thietlapgoidien();
	// 	gsm_thietlapnhantin();
	// }

	mode_wait = 60;

	/******** Initial watdog ****WDT**/	
	WDT_CONTR = EN_WDT | CLR_WDT | WDT_SCALE_64; // Enable watchdog, clear watchdog, pre scale = 64, watchdog idle mode = NO
	
	phone[0] = '0';
	phone[10] = 0;

	rfstop = 1;

	while(1){
		if(phut_out && eep_ups){
			phut_out=0;
			if(eep_ups==1) Relay4 = 0;
			IAP_docxoasector1();
			eeprom_buf[UPS_EEPROM]--;
			IAP_ghisector1();
		}
		if(!mode_wait && mode) mode = 0;
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
		if(!mode && !phim_mode_doi){
			phim_mode_nhan = 0;
			mode = 1;
			mode_wait = 60;
			LCD_xoa(TREN);
		}
		if(mode && phim_mode_nhan){
			phim_mode_nhan = 0;
			mode = (mode+1)%4;
			mode_wait = 60;
			LCD_xoa(TREN);
		}
		if(!phim_back_doi){
			// u8 thong_so[4];
			phim_back_nhan = 0;
			// xoadanhba(0);
			IAP_xoasector(SECTOR2);
			IAP_docxoasector1();
			eeprom_buf[PIN_EEPROM] = eeprom_buf[PIN_EEPROM+1] = eeprom_buf[PIN_EEPROM+2] = eeprom_buf[PIN_EEPROM+3] = '0';
			IAP_ghisector1();			
		}
		if(gsm_reset){
			gsm_reset = 0;
			gsm_thietlapgoidien();
			gsm_thietlapnhantin();
		}

		if(rfprocess){
			u8 i,j,data[3];
			__bit match=0;
			data[0]=data[1]=data[2]=0;
			for(i=0;i<20;i++){
				j = i/8;
				data[j] = data[j]*2 + rfdata[i];
				send_gsm_byte(rfdata[i]+'0');
			}

			send_gsm_hex(data[0]);
			send_gsm_hex(data[1]);
			send_gsm_hex(data[2]);
			send_gsm_byte(eep_rfindex+'0');
			for(i=0;!match && i<eep_rfindex;i++){
				match = data[0] == eep_rfdata[i*3] && data[1] == eep_rfdata[i*3+1] && data[2] == eep_rfdata[i*3+2];
				if(match){
					send_gsm_byte(i/10+'0');
					send_gsm_byte(i%10+'0');
				}
			}

			if(rflearn){
				if(!match){
					IAP_docxoasector2();
					eeprom_buf[RFDATA_EEPROM+eeprom_buf[RFINDEX_EEPROM-SECTOR2]*3-SECTOR2] = data[0];
					eeprom_buf[RFDATA_EEPROM+eeprom_buf[RFINDEX_EEPROM-SECTOR2]*3+1-SECTOR2] = data[1];
					eeprom_buf[RFDATA_EEPROM+eeprom_buf[RFINDEX_EEPROM-SECTOR2]*3+2-SECTOR2] = data[2];
					eeprom_buf[RFINDEX_EEPROM-SECTOR2]++;
					IAP_ghisector2();
					send_gsm_byte(' ');
					send_gsm_hex(eep_rfdata[eep_rfindex*3-3]);
					send_gsm_hex(eep_rfdata[eep_rfindex*3-2]);
					send_gsm_hex(eep_rfdata[eep_rfindex*3-1]);
				}
				rflearn = rfstop = 0;
			}else{
				if(match){
					send_gsm_byte('$');
					send_gsm_byte(rfdata[20]+'0');
					send_gsm_byte(rfdata[21]+'0');
					send_gsm_byte(rfdata[22]+'0');
					send_gsm_byte(rfdata[23]+'0');
					if(relay2giu){
						relay2giu = Relay2 = rfdata[22];
					}else{
						Relay2 = !rfdata[22] || !rfdata[20];
						relay2giu = !rfdata[20];
						Relay1 = !rfdata[21] && !Relay2;
						Relay3 = !rfdata[23] && !Relay1 && !Relay2;
					}					
					
				}
				
			}
			rfprocess = rfstatus = 0;
		}
		if(phim_cong_nhan){
			phim_cong_nhan = 0;
			LCD_xoa(TREN);
			LCD_guilenh(0x80);
			if(eep_rfindex>99) {LCD_guichuoi(" HET BO NHO HOC "); delay_ms(2000);}
			else rflearn = !rflearn;
			// kiemtrataikhoan();
			// LCD_guilenh(0x80);
			// LCD_guichuoi(lenh_sms);
			// while(!phim_cong_nhan)WATCHDOG;
			// phim_cong_nhan = 0;
			// kiemtrasodienthoai();
			// LCD_guilenh(0x84);
			// phone[10] = 0;
			// LCD_guichuoi(phone);
			// while(!phim_cong_nhan)WATCHDOG;
			// phim_cong_nhan=0;
		}

		if(lcd_update_chop){
			lcd_update_chop = 0;
			LCD_xoa(TREN);	
			LCD_guilenh(0x8e);
			LCD_guidulieu(signal/10+'0');
			LCD_guidulieu(signal%10+'0');
			LCD_guilenh(0x80);
			if(rflearn) {LCD_guichuoi("BAM HOC REMOTE");LCD_guidulieu(eep_rfindex/10+'0');LCD_guidulieu(eep_rfindex%10+'0');}
			else if(mode==1) LCD_guichuoi("CHINH:");
			else if(mode==2) LCD_guichuoi("PHU  :");
			else if(mode==3) LCD_guichuoi("TAM  :");
			else LCD_guichuoi(Relay1? "ON :":"OFF:");
			LCD_guigio(0xc7,"",hour,minute,second,flip_pulse);
			// LCD_guilenh(0xcf);
			// LCD_guidulieu(nha_mang);
			LCD_guingay(0xc0,year,month,day);
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
						Relay1 = 1;
						delay_ms(100);
						Relay1 = 0;
						phone[10] = 0;
						luu_lich_su(phone,0);
						baocaosms("\rMo cua len");
					} 
				}
			}
			phone[10] = 0;	
			LCD_guilenh(0x84); 
			LCD_guichuoi(phone);
		}
		WATCHDOG;
	}
}



