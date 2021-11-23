#include "true.h"
#include "main.h"
// _IAP_CONTR = 0x60 //reset to ISP
/*
	change log:
		0.8.4: sua loi kiem tra phonemaster voi danh ba mac dinh cua sim
		0.8.5: them thu vao ngay thang nam
*/

u8 __code ver[] = " CUACUON 0.8.5";

#include "motor_cam_phim.c"
#include "gsm_serial.c"

void luu_lich_su(u8 *phone,u8 cmd){
	u8 temp=0,i=0;
	gsm_sendandcheck("AT\r",15,1,ver);
	send_gsm_cmd("AT+CPBF=\"");
	send_gsm_byte(phone_super?'M':(phone_master?'m':'u'));
	phone[10] = 0;
	send_gsm_cmd(phone);
	send_gsm_byte(phone_super?'M':(phone_master?'m':'u'));
	lenh_sms[159] = 9;
	sms_index = 0;
	gsm_sendandcheck("\"\r",15,2," SAVING HISTORY ");
	// signal = lenh_sms[0];
	while(lenh_sms[i]<58)temp = temp*10 + lenh_sms[i++]-48;
	temp = temp*10 + lenh_sms[i] - 65;
	IAP_docxoasector1();
	i = eeprom_buf[INDEX_HISTORY_EEPROM];
	// signal = i;
	eeprom_buf[i*4+HISTORY_EEPROM] = temp;
	// signal = cmd;
	temp = (cmd<<2) + ((year-20)<3?year-20:3);
	// signal = temp;
	eeprom_buf[i*4 + HISTORY_EEPROM + 1] = (temp<<4) + month;
	eeprom_buf[i*4 + HISTORY_EEPROM + 2] = (day<<3) + (hour>>2);
	eeprom_buf[i*4 + HISTORY_EEPROM + 3] = ((hour&3)<<6) + minute;
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
	delay_chay_khoi_tao = 30;
	so_lan_goi_dien = 0;
	gsm_delay_reset=10;
	relay1_delay_tat = 2;
	relay2_delay_tat = 2;
	relay3_delay_tat = 2;
	relay4_delay_tat = 2;
	phim_mode_doi = phim_cong_doi = 2;
	phim_back_doi = 6;
	mode = 0;
	nosim=0;
	rf_khancap = rf_khancap_delay = 0;
	ngay_reset_con_lai = 1;

	__bit nhan_remote_lan_dau = 1;
	/*validate eeprom*/
	// u8 __xdata i;
	IAP_docxoasector1();
	if(eeprom_buf[INDEX_HISTORY_EEPROM]>97)eeprom_buf[INDEX_HISTORY_EEPROM]=0;
	if(eeprom_buf[BAOCAO_EEPROM]>1) eeprom_buf[BAOCAO_EEPROM] = 0;
	if(eeprom_buf[KHOA_EEPROM]>3) eeprom_buf[KHOA_EEPROM] = 0;
	if(eeprom_buf[HUONG_MOTOR]>1) eeprom_buf[HUONG_MOTOR] = 0;
	if(eeprom_buf[UPS_EEPROM]>10) eeprom_buf[UPS_EEPROM] = 0;
	if((eeprom_buf[PIN_EEPROM]  <'0' || eeprom_buf[PIN_EEPROM]  >'9')
	|| (eeprom_buf[PIN_EEPROM+1]<'0' || eeprom_buf[PIN_EEPROM+1]>'9')
	|| (eeprom_buf[PIN_EEPROM+2]<'0' || eeprom_buf[PIN_EEPROM+2]>'9')
	|| (eeprom_buf[PIN_EEPROM+3]<'0' || eeprom_buf[PIN_EEPROM+3]>'9'))
	eeprom_buf[PIN_EEPROM] = eeprom_buf[PIN_EEPROM+1] = eeprom_buf[PIN_EEPROM+2] = eeprom_buf[PIN_EEPROM+3] = '0';
	
	IAP_ghisector1();
	Relay4 = eep_ups?1:0;
	IAP_docxoasector2();	
	if(eeprom_buf[RFINDEX_EEPROM-SECTOR2]>99)eeprom_buf[RFINDEX_EEPROM-SECTOR2]=0;
	if(eeprom_buf[RFLOCK_EEPROM-SECTOR2]>1) eeprom_buf[RFLOCK_EEPROM-SECTOR2] = 0;
	IAP_ghisector2();

	/*Khoi tao serial baudrate 57600 cho gsm sim900*/
	delay_ms(5000);
	
	gsm_init();
	
	/*PCA TIMER 0 INIT 50us*/
	PCA_Timer_init();	
	
  	// /*Khoi tao man hinh LCD*/
	LCD_Init();

	// gsm_thietlapsim800();
	if(!nosim && gsm_thietlapsim800()){
		gsm_thietlapngaygiothuc();
		gsm_thietlapgoidien();
		gsm_thietlapnhantin();
	}

	mode_wait = 60;

	/******** Initial watdog ****WDT**/	
	// WDT_CONTR = EN_WDT | CLR_WDT | WDT_SCALE_64; // Enable watchdog, clear watchdog, pre scale = 64, watchdog idle mode = NO
	
	phone[0] = '0';
	phone[10] = 0;
	have_master = kiemtraphonemaster();
	__bit run_button = 0;
	if(have_master){
		baocaosms("\rBDK Khoi Dong");
	}
	while(1){
		if(gio_out){
			gio_out = 0;
			gsm_thietlapngaygiothuc();
		}
		if(phut_out && eep_ups){
			phut_out=0;
			if(eep_ups==1) Relay4 = 0;
			IAP_docxoasector1();
			eeprom_buf[UPS_EEPROM]--;
			IAP_ghisector1();
		}
		if(!mode_wait && mode){
			mode = sub_mode = 0;
			pin[0] = pin[1] = pin[2] = pin[3] = 0;
			new_pin[0] = new_pin[1] = new_pin[2] = new_pin[3] = 0;

		} 
		if(co_tin_nhan_moi){
			co_tin_nhan_moi = 0;
			gsm_sendandcheck("AT\r", 15, 1,ver);
			send_gsm_cmd("AT+CMGL=\"ALL\"\r");
			// send_gsm_cmd("ATAT\r");
		}
		if(sms_dang_xu_ly && !mode){
			// CCAPM1 = 0x49;
			xu_ly_tin_nhan();
			gsm_sendandcheck("AT+CMGDA=\"DEL ALL\"\r",15,1,"  DELETING SMS  ");
			sms_dang_xu_ly = 0;
			send_gsm_byte('S');
		}
		if(!ngay_reset_con_lai && !hour && minute>5){
			IAP_CONTR = 0x60;
		}
		switch(mode){
			default:
			case 0:
				//display
				if(lcd_update_chop){
					lcd_update_chop = 0;
					LCD_guilenh(0x80);
					LCD_guichuoi(ver);
					LCD_guigio(0xc7,"",hour,minute,second,flip_pulse);
					LCD_guingay(0xc0,year,month,day,date);
				}
				//button
				//M
				if(!(eep_khoa&2) && !phim_mode_doi){
					phim_mode_nhan = 0;
					mode_wait = 60;
					sub_mode = 0;
					if(have_master){
						mode = 1;
						LCD_xoa(TREN);
						LCD_guilenh(0x80);
						LCD_guichuoi("PIN:");
						LCD_guidulieu(pin[0]+'0');
						LCD_guidulieu(pin[1]+'0');
						LCD_guidulieu(pin[2]+'0');
						LCD_guidulieu(pin[3]+'0');
						
						
					}
					else mode = 2;
				}
				//+
				if(phim_cong_nhan){
					phim_cong_nhan = 0;
					if(run_button){
						rfprocess = 1;
						Relay2 = 1;
						delay_ms(100);
						rfprocess = Relay2 = 0;
					}else{
						rfprocess = 1;
						if(eep_huong){
							Relay3 = 1;
							delay_ms(100);
							rfprocess = Relay3 = 0;
						}else{
							Relay1 = 1;
							delay_ms(100);
							rfprocess = Relay1 = 0;
						}
					}
					run_button = !run_button;
				}
				//B
				if(phim_back_nhan){
					phim_back_nhan = 0;
					if(run_button){
						rfprocess = 1;
						Relay2 = 1;
						delay_ms(100);
						rfprocess = Relay2 = 0;
					}else{
						rfprocess = 1;
						if(eep_huong){
							Relay1 = 1;
							delay_ms(100);
							rfprocess = Relay1 = 0;
						}else{
							Relay3 = 1;
							delay_ms(100);
							rfprocess = Relay3 = 0;
						}
					}
					run_button = !run_button;
				}
				break;
			case 1:
				LCD_blinkXY(TREN,4+sub_mode);
				//button
				//M
				if(phim_mode_nhan){
					phim_mode_nhan = 0;
					sub_mode++;

					if(sub_mode==4){
						sub_mode = 0;
						if(pin[0] == eep_pin[0]-'0' && pin[1] == eep_pin[1]-'0' &&
				   		pin[2] == eep_pin[2]-'0' && pin[3] == eep_pin[3]-'0'){
							mode = 2;
							so_lan_sai_pin = 0;
							pin[0] = pin[1] = pin[2] = pin[3] = 0;
							LCD_noblink();
							LCD_xoa(TREN);
						}else{

							if(++so_lan_sai_pin>4){
								mode = 0;
								IAP_docxoasector1();
								eeprom_buf[KHOA_EEPROM] |= 2;
								IAP_ghisector1();
								if(kiemtraphonemaster())baocaosms("\rSai pin 5 lan"); 
							
							}
							LCD_guilenh(0x88);
							LCD_guidulieu('X');
							LCD_guidulieu(so_lan_sai_pin+'0');
						}
					}
				}
				//+
				if(phim_cong_nhan){
					phim_cong_nhan = 0;
					pin[sub_mode] = (pin[sub_mode]+1)%10;
					LCD_guilenh(0x84+sub_mode);
					LCD_guidulieu(pin[sub_mode]+'0');
				}
				//B
				if(phim_back_nhan){
					phim_back_nhan = 0;
					if(sub_mode)sub_mode--;
					else mode = 0;
				}
				//display
				
				
				
				if(lcd_update_chop){
					lcd_update_chop =  0;
					// LCD_blinkXY(TREN,4+sub_mode);
					LCD_guigio(0xc7,"",hour,minute,second,flip_pulse);
					LCD_guingay(0xc0,year,month,day,date);
				}
				
				break;
			case 2:
				LCD_guigio(0xc7,"",hour,minute,second,flip_pulse);
				LCD_guingay(0xc0,year,month,day,date);
				LCD_guilenh(0x80);
				switch(sub_mode){
					case 0: LCD_guichuoi(have_master?"CHINH:          ":"MASTER:          "); break;
					case 1: LCD_guichuoi("PHU:            "); break;
					case 2: LCD_guichuoi("    DOI  PIN    "); break;
					case 3: LCD_guichuoi("  HUONG  MOTOR  "); break;
					case 4: LCD_guichuoi("      EXIT      "); break;
				}
				if(phim_mode_nhan){
					phim_mode_nhan = 0;
					phim_back_nhan = 0;
					sub_mode = (sub_mode+1)%5;
				}
				if(phim_cong_nhan){
					phim_cong_nhan = 0;
					if(sub_mode<2){
						u16 doims = 200;
						kiemtrataikhoan();
						LCD_guilenh(0x80);
						LCD_guichuoi(lenh_sms);
						while(!phim_cong_nhan && doims--)delay_ms(300);
						phim_cong_nhan = 0;
						kiemtrasodienthoai();
						LCD_guilenh(0x84);
						phone[10] = 0;
						LCD_guichuoi(phone);
						doims=200;
						while(!phim_cong_nhan && doims--)delay_ms(300);
						// while(!phim_cong_nhan)WATCHDOG;
						mode_wait = 60;
						phim_cong_nhan=0;
					}else sub_mode = 4;
					
				}
				if(phim_back_nhan && sub_mode>1){
					phim_back_nhan = 0;
					mode = (sub_mode+1)%5;
					LCD_xoa(TREN);
					LCD_guilenh(0x80);
					switch(sub_mode){
						case 2:
							LCD_guichuoi("PIN:");
							LCD_guidulieu(pin[0]+'0');
							LCD_guidulieu(pin[1]+'0');
							LCD_guidulieu(pin[2]+'0');
							LCD_guidulieu(pin[3]+'0');
							LCD_guidulieu(' ');
							LCD_guidulieu(' ');
							LCD_guidulieu(' ');
							LCD_guidulieu(new_pin[0]+'0');
							LCD_guidulieu(new_pin[1]+'0');
							LCD_guidulieu(new_pin[2]+'0');
							LCD_guidulieu(new_pin[3]+'0');
							LCD_guidulieu(' ');
							sub_mode = 0;
							break;
						case 3:
							sub_mode = eep_huong;
							LCD_guichuoi("HUONG: ");
							LCD_guichuoi(sub_mode?"PH":"TR");
							LCD_guichuoi("AI");
							break;
					}
				}
				
				if(!phim_back_doi && sub_mode <2){
					phim_back_nhan = 0;
					xoadanhba(0);
					IAP_xoasector(SECTOR2);
					IAP_ghibyte(RFINDEX_EEPROM,0);
					IAP_docxoasector1();
					eeprom_buf[PIN_EEPROM] = eeprom_buf[PIN_EEPROM+1] = eeprom_buf[PIN_EEPROM+2] = eeprom_buf[PIN_EEPROM+3] = '0';
					IAP_ghisector1();
					have_master = 0;
					mode = sub_mode = 0;

				}
				
				
				break;
			case 3:
				if(phim_mode_nhan){
					phim_mode_nhan = 0;
					sub_mode++;

					if(sub_mode==4){
						if(pin[0] != eep_pin[0]-'0' || pin[1] != eep_pin[1]-'0' ||
				   		pin[2] != eep_pin[2]-'0' || pin[3] != eep_pin[3]-'0'){
							sub_mode = 0;
							LCD_guilenh(0x88);
							LCD_guidulieu('X');
							
						}else{
							LCD_guilenh(0x88);
							LCD_guidulieu(' ');
							
						}
					}else if(sub_mode == 8){
						mode = sub_mode = 2;
						IAP_docxoasector1();
						eeprom_buf[PIN_EEPROM  ] = new_pin[0]+'0';//1+0
						eeprom_buf[PIN_EEPROM+1] = new_pin[1]+'0';//2+1
						eeprom_buf[PIN_EEPROM+2] = new_pin[2]+'0';//3+2
						eeprom_buf[PIN_EEPROM+3] = new_pin[3]+'0';//4+3
						IAP_ghisector1();
						pin[0] = pin[1] = pin[2] = pin[3] = 0;
						new_pin[0] = new_pin[1] = new_pin[2] = new_pin[3] = 0;
						LCD_noblink();
					}
				}
				//+
				if(phim_cong_nhan){
					phim_cong_nhan = 0;
					LCD_guilenh(0x84+sub_mode+sub_mode/4*3);
					if(sub_mode<4){
						pin[sub_mode] = (pin[sub_mode]+1)%10;
						LCD_guidulieu(pin[sub_mode]+'0');
					}else{
						new_pin[sub_mode-4] = (new_pin[sub_mode-4]+1)%10;
						LCD_guidulieu(new_pin[sub_mode-4]+'0');
					}
				}
				//B
				if(phim_back_nhan){
					phim_back_nhan = 0;
					mode = sub_mode = 2;
					pin[0] = pin[1] = pin[2] = pin[3] = 0;
					new_pin[0] = new_pin[1] = new_pin[2] = new_pin[3] = 0;
					LCD_noblink();
				}
				//display
				
				
				LCD_blinkXY(TREN,4+sub_mode+sub_mode/4*3);
				if(lcd_update_chop){
					lcd_update_chop =  0;
					// LCD_blinkXY(TREN,4+sub_mode+sub_mode/4*3);
					LCD_guigio(0xc7,"",hour,minute,second,flip_pulse);
					LCD_guingay(0xc0,year,month,day,date);
				}
				break;
			case 4:
				if(phim_cong_nhan){
					phim_cong_nhan = 0;
					sub_mode = 1 - sub_mode;
					LCD_guilenh(0x87);
					LCD_guichuoi(sub_mode?"PH":"TR");
				}
				if(phim_mode_nhan){
					phim_mode_nhan = 0;
					IAP_docxoasector1();
					eeprom_buf[HUONG_MOTOR] = sub_mode;//1+0
					IAP_ghisector1();
					mode = 2;
					sub_mode = 3;
				}
				if(phim_back_nhan){
					phim_back_nhan = 0;
					sub_mode = 3;
					mode = 2;
				}
				LCD_guigio(0xc7,"",hour,minute,second,flip_pulse);
				LCD_guingay(0xc0,year,month,day,date);
				break;
		}
		


		if(rfprocess){
			u8 i,data[3],cmd[4];
			u8 match=0;
			// data[0]=data[1]=data[2]=0;
			// for(i=0;i<rfindex-4;i++){
			// 	j = i/8;
			// 	data[j] = data[j]*2 + rfdata[i];
			// 	// send_gsm_byte(rfdata[i]+'0');
			// }
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
			
			for(i=0;!match && i<eep_rfindex+2;i++){
				match = data[0] == eep_rfdata[i*3] && data[1] == eep_rfdata[i*3+1] && data[2] == eep_rfdata[i*3+2];
				if(match){
					if(i<2)match = i+2;
					send_gsm_byte(i/10+'0');
					send_gsm_byte(i%10+'0');
				}
			}
			send_gsm_byte('-');
			send_gsm_byte(match+'0');
			send_gsm_byte('-');

			if(mode==2){
				if(!match){
					if(!have_master){
						//remote khan cap
						IAP_docxoasector2();
						eeprom_buf[0] = data[0];
						eeprom_buf[1] = data[1];
						eeprom_buf[2] = data[2];
						IAP_ghisector2();
					}else{
						if(!sub_mode){	
							if(eep_rfindex>97) {LCD_guichuoi(" HET BO NHO HOC "); delay_ms(2000);}
							else{
								IAP_docxoasector2();
								eeprom_buf[RFDATA_EEPROM+eeprom_buf[RFINDEX_EEPROM-SECTOR2]*3+6-SECTOR2] = data[0];
								eeprom_buf[RFDATA_EEPROM+eeprom_buf[RFINDEX_EEPROM-SECTOR2]*3+7-SECTOR2] = data[1];
								eeprom_buf[RFDATA_EEPROM+eeprom_buf[RFINDEX_EEPROM-SECTOR2]*3+8-SECTOR2] = data[2];
								eeprom_buf[RFINDEX_EEPROM-SECTOR2]++;
								IAP_ghisector2();
								// send_gsm_byte(' ');
								// send_gsm_hex(eep_rfdata[eep_rfindex*3-3]);
								// send_gsm_hex(eep_rfdata[eep_rfindex*3-2]);
								// send_gsm_hex(eep_rfdata[eep_rfindex*3-1]);
								if(kiemtraphonemaster() && eep_baocao) baocaosms("\rremote dc hoc");
							}
						}else if(sub_mode == 1){
							IAP_docxoasector2();
							eeprom_buf[3] = data[0];
							eeprom_buf[4] = data[1];
							eeprom_buf[5] = data[2];
							IAP_ghisector2();
							if(kiemtraphonemaster() && eep_baocao) baocaosms("\rmodule bao dong duoc hoc");
						}
					}
				}
				rfstop = 0;
			}else{
				if(match){
					// send_gsm_byte('$');
					// send_gsm_byte(rfdata[20]+'0');
					// send_gsm_byte(rfdata[21]+'0');
					// send_gsm_byte(rfdata[22]+'0');
					// send_gsm_byte(rfdata[23]+'0');
					if(match==2){
						if(phim_back_nhan){
							phim_back_nhan = 0;
							xoadanhba(0);
							IAP_xoasector(SECTOR2);
							IAP_ghibyte(RFINDEX_EEPROM,0);
							IAP_docxoasector1();
							eeprom_buf[PIN_EEPROM] = eeprom_buf[PIN_EEPROM+1] = eeprom_buf[PIN_EEPROM+2] = eeprom_buf[PIN_EEPROM+3] = '0';
							IAP_ghisector1();			
						}
						relay2giu = 0;
						if(kiemtraphonemaster()) baocaosms("\rremote khan cap duoc su dung");
					}
					if(rflock){
						if(!cmd[2]){
							rflock = 0;
							IAP_docxoasector2();
							eeprom_buf[RFLOCK_EEPROM-SECTOR2] = 0;
							IAP_ghisector2();
						}
					}else{
						if(!cmd[0]){
							rflock = 1;
							IAP_docxoasector2();
							eeprom_buf[RFLOCK_EEPROM-SECTOR2] = 1;
							IAP_ghisector2();
						}else if(!relay2giu){
							Relay2 = !cmd[2];
							if(nhan_remote_lan_dau){
								nhan_remote_lan_dau = 0;
								delay_chay_khoi_tao += 60;
							}
							if(eep_huong){
								Relay3 = !cmd[1] && !Relay2;
								Relay1 = !cmd[3] && !Relay3 && !Relay2;
							}else{
								Relay1 = !cmd[1] && !Relay2;
								Relay3 = !cmd[3] && !Relay1 && !Relay2;
							}
						}
					}
					if(!cmd[0]) {rf_khancap++;rf_khancap_delay = 10;}
					if(relay2giu && (rf_khancap>29 || (match==2 &&  !cmd[1] ))){
						IAP_docxoasector1();
                		eeprom_buf[KHOA_EEPROM] = 0;
               			IAP_ghisector1();
						relay2giu = 0;
						Relay2 = 0;
						if(eep_huong){
							Relay3 = 1;
							delay_ms(100);
							Relay3 = 0;
						}else{
							Relay1 = 1;
							delay_ms(100);
							Relay1 = 0;
						}
						rf_khancap = rf_khancap_delay = 0;
						
					}				
					
				}
				
			}
			rfstatus = 0; 
			rfprocess = 0;
		}


		// if(phim_cong_nhan){
		// 	phim_cong_nhan = 0;
			
		// 	if(mode == 1 || mode ==4){
		// 		if(sub_mode<4){
		// 			pin[sub_mode] = (pin[sub_mode]+1)%10;
		// 		}else{
		// 			new_pin[sub_mode-4] = (new_pin[sub_mode-4]+1)%10;
		// 		}
		// 	}else if(mode == 2){
		// 		u16 doims = 200;

				
		// 		kiemtrataikhoan();
		// 		LCD_guilenh(0x80);
		// 		LCD_guichuoi(lenh_sms);
		// 		while(!phim_cong_nhan && doims--)delay_ms(300);
		// 		phim_cong_nhan = 0;
		// 		kiemtrasodienthoai();
		// 		LCD_guilenh(0x84);
		// 		phone[10] = 0;
		// 		LCD_guichuoi(phone);
		// 		doims=200;
		// 		while(!phim_cong_nhan && doims--)delay_ms(300);
		// 		// while(!phim_cong_nhan)WATCHDOG;
		// 		mode_wait = 60;
		// 		phim_cong_nhan=0;
		// 	}
			
		// }

		// if(lcd_update_chop){
		// 	LCD_xoa(TREN);
		// 	lcd_update_chop = 0;
		// 	if(mode == 1){
		// 		LCD_guilenh(0x80);
		// 		LCD_guichuoi("PIN:");
		// 		LCD_guidulieu(pin[0]+'0');
		// 		LCD_guidulieu(pin[1]+'0');
		// 		LCD_guidulieu(pin[2]+'0');
		// 		LCD_guidulieu(pin[3]+'0');
		// 		LCD_guidulieu(so_lan_sai_pin?'X':' ');
		// 		LCD_guidulieu(so_lan_sai_pin?so_lan_sai_pin+'0':' ');
		// 	}else{
		// 		LCD_guilenh(0x8e);
		// 		LCD_guidulieu(signal/10+'0');
		// 		LCD_guidulieu(signal%10+'0');
		// 		LCD_guilenh(0x80);
		// 		if(mode==2) LCD_guichuoi("CHINH:");
		// 		else if(mode==3) LCD_guichuoi("PHU  :");
		// 		// else if(mode==3) LCD_guichuoi("TAM  :");
		// 		else if(mode == 4){
		// 			LCD_guichuoi("PIN:");
		// 			LCD_guidulieu(pin[0]+'0');
		// 			LCD_guidulieu(pin[1]+'0');
		// 			LCD_guidulieu(pin[2]+'0');
		// 			LCD_guidulieu(pin[3]+'0');
		// 			LCD_guidulieu(so_lan_sai_pin?'X':' ');
		// 			LCD_guidulieu(so_lan_sai_pin?so_lan_sai_pin+'0':' ');
		// 			LCD_guidulieu(' ');
		// 			LCD_guidulieu(new_pin[0]+'0');
		// 			LCD_guidulieu(new_pin[1]+'0');
		// 			LCD_guidulieu(new_pin[2]+'0');
		// 			LCD_guidulieu(new_pin[3]+'0');
		// 			LCD_guidulieu(' ');
		// 		}
		// 		else LCD_guichuoi(Relay1? "ON :":"OFF:");
		// 	}
			
				
			
		// 	LCD_guigio(0xc7,"",hour,minute,second,flip_pulse);
		// 	// LCD_guilenh(0xcf);
		// 	// LCD_guidulieu(nha_mang);
		// 	LCD_guingay(0xc0,year,month,day,date);
		// 	if(mode == 1 || mode == 4){
		// 		LCD_blinkXY(TREN,4+sub_mode+sub_mode/4*3);
		// 	}else{
		// 		LCD_noblink();
		// 	}
		// }
		if(phone_update){
			phone_update = 0;
			if(co_cuoc_goi_toi){
				co_cuoc_goi_toi = 0;
				if(mode == 2 || mode == 3){
					gsm_sendandcheck("AT\r",15,1,ver);
					phone[10] = 0;
					if(phone_so_sanh_that_bai) gsm_themdanhba(phone,have_master?(sub_mode?'u':'m'):'M');
					// gsm_sendandcheck("AT+CPBR=1,99\r", 15, 1,"  SENDING CPBR  ");
					if(have_master)baocaosms("\rLuu thanh cong");
					else baocaosms("\rLuu Master");
					if(have_master && kiemtraphonemaster() && eep_baocao)baocaosms("\rDT moi duoc luu");
					have_master = 1;
					
				}else{
					if(!phone_so_sanh_that_bai && !mode){
						if(relay2giu)baocaosms("\rCua dang khoa");
                    	else{
							rfprocess = 1;
							if(eep_huong){
								Relay3 = 1;
								delay_ms(100);
								rfprocess = Relay3 = 0;
							}else{			
								Relay1 = 1;
								delay_ms(100);
								rfprocess = Relay1 = 0;
							}
							phone[10] = 0;
							luu_lich_su(phone,0);
							baocaosms("\rMo cua len");
						}
						
					} 
				}
			}
			// phone[10] = 0;	
			// LCD_guilenh(0x84); 
			// LCD_guichuoi(phone);
			CCAPM1 = 0x49;
		}
		// if(chay_khoi_tao){
		// 	chay_khoi_tao = 0;
		// 	rflock = eep_rflock;	
		// 	Relay2 = relay2giu = eep_khoa;
		// 	Relay4 = eep_ups?1:0;
		// }
		WATCHDOG;
	}
}



