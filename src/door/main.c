#include "true.h"
#include "main.h"
/*
	change log:
		0.8.4: sua loi kiem tra phonemaster voi danh ba mac dinh cua sim
		0.8.5: them thu vao ngay thang nam
		A7-1.0: chuyen sang module A7680C; nhan dien so goi/nhan tin bang
		        bang so trong EEPROM (thay cho danh ba SIM); bo lich su
		A7-1.2: ATH -> AT+CHUP (cup may); fix remote khan cap nhan tin
		        lien tuc (khe rong khop nhieu, bao 1 lan khi mo khoa)
		A7-1.4: dang ky so bang tin nhan "luu"/"save" o man hinh CHINH/PHU;
		        hien so + bam M xac nhan / B huy; hien so vua luu tren LCD
		A7-1.5: mac dinh BAT bao cao (bao tin nhan mo/dong/dung cua) cho
		        may moi va sau khi khoi tao lai
		A7-1.6: them delay 1s sau COPS=0 truoc khi doc CCLK (cho module nhan
		        gio mang NITZ - giong ban gps_a7680c chay tot; sua loi mot so
		        SIM khong lay duoc gio); them lenh SMS Cg de chinh gio thu cong
		A7-1.7: sua remote RF - (1) chot frame ngay khi du 24 bit (bam nhanh
		        van an, khong phai giu); (2) khi nha nut huy frame chua kip xu
		        ly de motor khong giat them 1 nhip; (3) hien "DA HOC REMOTE!"
		        tren LCD khi hoc remote o CHINH. (Thuat toan nhan RF se lam
		        moi hoan toan o ban 2.0 - tu hieu chinh theo tung remote.)
		A7-1.8: HOTFIX khan cap - fix 1.7 "chot ngay 24 bit" da vo tinh bo mat
		        buoc loc nhieu (doi sync ke tiep), khien nhieu RF trong moi
		        truong nhieu bi hieu nham thanh remote la va TU HOC vao bang dù
		        khong bam gi. Fix: chi cho phep HOC khi 2 frame lien tiep giai
		        ma ra DUNG CUNG 1 ma (remote that phat lien tuc nen van nhanh;
		        nhieu gan nhu khong bao gio lap dung 2 lan). Duong dieu khien
		        binh thuong (mo/dong/relay) KHONG doi - da duoc bao ve boi yeu
		        cau khop bang da hoc san, khong can them xac nhan.
		CC-1.9: (doi ten dong san pham cua cuon tu "A7-" sang "CC-" tu ban nay)
		        Menu them 2 muc XOA SO va XOA REMOTE (can PIN, giong het cac
		        muc menu khac):
		          - XOA SO: duyet tung so (bo qua vai tro M/m master, khong the
		            xoa master qua duong nay), hien index + so dien thoai day
		            du, + de chon, xac nhan M:OK B:Huy roi moi xoa that.
		          - XOA REMOTE: duyet tung remote da hoc (khong gom khe khan
		            cap/module bao dong); bam thu remote that de xem "OK -
		            DUNG REMOTE" hay "X - KHAC REMOTE" (chi de xem, KHONG hoc/
		            KHONG mo cua), + de chon xoa, xac nhan M:OK B:Huy.
		        Chuc nang "giu B o CHINH/PHU de xoa tat ca" (duong khoi phuc
		        khi mat may/quen PIN) GIU NGUYEN nhu cu (van khong can PIN rieng
		        vi da nam trong menu can PIN); them: bao SMS canh bao ve so
		        master CU ngay truoc khi xoa, de chu thiet bi biet neu bi lam
		        dung (khong ngan duoc thao tac, chi giup phat hien). Lenh SMS
			Xoa,N cung duoc bao ve khong cho xoa so Master.
		CC-1.10: sua phan hoi "hoc remote kho" nguoi dung bao sau 1.8/1.9 (user
		        bao 1.7 nhay nhat, 1.6 on, 1.8/1.9 hoc kho). Nguyen nhan: fix
		        loc nhieu o 1.8 doi hoi 2 frame LIEN TIEP TUYET DOI giong nhau
		        moi cho hoc - qua khat khe, chi 1 frame bi nhieu xen giua 2
		        frame that (dang giu nut, phat lien tuc) la mat tien do phai
		        lam lai tu dau. Doi thanh "cua so khoan dung" (RF_WIN_MAX=4
		        frame la duoc phep xen giua truoc khi bo ung vien) trong
		        include/rf_frame.inc (dung chung 2 san pham) - remote that xac
		        nhan nhanh gan bang 1.7 (chi can 2 frame KHOP, khong bat buoc
		        lien tiep tuyet doi), trong khi nhieu thuan tuy van gan nhu
		        khong bao gio tu khop 2 lan (xac suat ~1e-6/cua so, van an
		        toan). KHONG revert ve 1.7 nguyen ban vi se mat het bao ve
		        chong hoc nham nhieu da xac nhan tren phan cung.
		CC-1.11: nguoi dung test 4 ban tren phan cung, xac nhan 1.7 hoc/bam
		        remote nhay+de nhat, 1.8/1.9 kho hoc. THEO Y NGUOI DUNG: bo
		        hoan toan buoc "xac nhan 2 frame" - hoc remote NGAY khi giai ma
		        1 frame, dung nhu 1.7. Danh doi: co the thi thoang tu hoc nham
		        1 muc nhieu vao bang -> don bang XOA REMOTE (co tu 1.9). Duong
		        dieu khien (mo/dong cua) van an toan vi doi hoi khop bang da hoc.
		        Giu nguyen XOA SO/XOA REMOTE va bao ve master khi xoa qua SMS.
		CC-1.12: SUA LOI "CC-1.11 khong hoc duoc remote" (user A/B tren cung
		        board: 1.7 hoc duoc, 1.11 khong - da xoa sach + remote moi).
		        Logic hoc giong het 1.7; nguyen nhan la code cac tinh nang xoa
		        lam main() phinh to khien SDCC sinh ma SAI o duong hoc remote
		        (loi codegen tren 8051 voi ham qua lon). Fix: tach toan bo khoi
		        xu ly RF (hoc/dieu khien/xoa-remote) ra ham rieng xu_ly_rf() de
		        codegen on dinh, doc lap kich thuoc main(). KHONG doi logic.
		CC-1.14: sua loi muc EXIT trong menu khong bam thoat duoc (chi cho
		        timeout 60s). Menu gop them XOA SO/XOA REMOTE da lam EXIT roi
		        vao sub_mode 6 ma khong co phim nao xu ly thoat. Fix: ca nut +
		        lan nut B khi dang o EXIT deu thoat ve man hinh chinh.
*/

u8 __code ver[] = " CC-1.14A       ";

#include "motor_cam_phim.c"
#include "gsm_serial.c"

#include "xu_ly_tin_nhan.c"

/* Tra ve 1 neu 4 chu so pin nhap trung voi ma pin luu trong eeprom. */
__bit pin_dung(){
	return pin[0] == eep_pin[0]-'0' && pin[1] == eep_pin[1]-'0' &&
	       pin[2] == eep_pin[2]-'0' && pin[3] == eep_pin[3]-'0';
}

/* Man hinh XOA SO: tim so KE TIEP (1-based idx) sau "cur" khong phai vai tro
   M/m (master) va khong trong (role!=0). Tra ve 0 neu khong co so nao du dieu
   kien. Chi goi khi vao man hinh / khi bam B - KHONG goi moi vong lap. */
u8 xoa_so_ke_tiep(u8 cur){
	u8 i,idx,role;
	if(!eep_phone_count) return 0;
	for(i=0;i<eep_phone_count;i++){
		idx = (cur+i)%eep_phone_count;   // cur la 1-based; +i roi mod la du (cur<=count)
		role = eep_phone[idx*PHONE_ENTRY+9];
		if(role && role!='M' && role!='m') return idx+1;
	}
	return 0;
}


/* Xu ly 1 frame RF (hoc / dieu khien / xoa-remote OK-X). Tach ra ham rieng
   de codegen on dinh, khong bi anh huong boi kich thuoc main() (tranh loi hoc
   remote o ban gop nhieu tinh nang). */
void xu_ly_rf(){
	if(!rfprocess) return;
	{
			u8 i,data[3],cmd[4];
			u8 match=0;
			#include "rf_frame.inc"
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
				// Khe khan cap/bao dong (i<2) chua hoc (trong = 0x00 hoac 0xFF) -> bo qua
				// de nhieu (noise) khong khop nham voi khe rong.
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
			send_gsm_byte('-');

			if(mode==2){
				// CC-1.11: hoc ngay khi giai ma duoc 1 frame (hanh vi 1.7 - de/nhay
				// nhat theo phan hoi nguoi dung). Bo buoc "xac nhan 2 frame". Rac
				// nhieu neu co thi dung XOA REMOTE de don.
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
								// LOI 3: bao "da hoc" tren LCD (ngoai SMS) de thay ngay
								LCD_xoa(TREN);
								LCD_guilenh(0x80);
								LCD_guichuoi(" DA HOC REMOTE! ");
								delay_ms(1500);
								LCD_xoa(TREN);
								if(get_master_phone() && eep_baocao) baocaosms("\rremote dc hoc");
							}
						}else if(sub_mode == 1){
							IAP_docxoasector2();
							eeprom_buf[3] = data[0];
							eeprom_buf[4] = data[1];
							eeprom_buf[5] = data[2];
							IAP_ghisector2();
							if(get_master_phone() && eep_baocao) baocaosms("\rmodule bao dong duoc hoc");
						}
					}
				}
				rfstop = 0;
			}else if(mode==6){
				// XOA REMOTE: CHI xem remote vua bam co phai khe dang duyet
				// khong (hien OK/X) - KHONG hoc, KHONG mo/dong cua.
				if(!man_hinh_luu && eep_rfindex && del_rf_idx<eep_rfindex){
					__bit giong = data[0]==eep_rfdata[(del_rf_idx+2)*3]
					           && data[1]==eep_rfdata[(del_rf_idx+2)*3+1]
					           && data[2]==eep_rfdata[(del_rf_idx+2)*3+2];
					LCD_guilenh(0xc0);
					LCD_guichuoi(giong?"OK - DUNG REMOTE":"X - KHAC REMOTE ");
					delay_ms(1000);
					lcd_update_chop = 1;
				}
			}else{
				if(match){
					if(match==2){
						if(phim_back_nhan){
							phim_back_nhan = 0;
							phone_del(0);
							IAP_xoasector(SECTOR2);
							IAP_ghibyte(RFINDEX_EEPROM,0);
							IAP_docxoasector1();
							eeprom_buf[PIN_EEPROM] = eeprom_buf[PIN_EEPROM+1] = eeprom_buf[PIN_EEPROM+2] = eeprom_buf[PIN_EEPROM+3] = '0';
							IAP_ghisector1();			
						}
						// Chi bao 1 lan khi remote khan cap thuc su mo khoa (relay2giu 1->0),
						// tranh nhan tin lien tuc khi remote phat lien tiep.
						if(relay2giu){
							relay2giu = 0;
							if(get_master_phone()) baocaosms("\rremote khan cap duoc su dung");
						}
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
}

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

	nhan_remote_lan_dau = 1;
	/*validate eeprom*/
	IAP_docxoasector1();
	if(eeprom_buf[BAOCAO_EEPROM]>1) eeprom_buf[BAOCAO_EEPROM] = 1; // mac dinh BAT bao cao
	if(eeprom_buf[KHOA_EEPROM]>3) eeprom_buf[KHOA_EEPROM] = 0;
	if(eeprom_buf[HUONG_MOTOR]>1) eeprom_buf[HUONG_MOTOR] = 0;
	if(eeprom_buf[UPS_EEPROM]>10) eeprom_buf[UPS_EEPROM] = 0;
	if(eeprom_buf[PHONE_COUNT_EEPROM]>PHONE_MAX) eeprom_buf[PHONE_COUNT_EEPROM] = 0;
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

	if(!nosim && gsm_thietlapsim800()){
		gsm_thietlapngaygiothuc();
		gsm_thietlapgoidien();
		gsm_thietlapnhantin();
	}

	mode_wait = 60;

	
	phone[0] = '0';
	phone[10] = 0;
	have_master = get_master_phone();
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
			man_hinh_luu = 0;
			pin[0] = pin[1] = pin[2] = pin[3] = 0;
			new_pin[0] = new_pin[1] = new_pin[2] = new_pin[3] = 0;

		} 
		if(co_tin_nhan_moi){
			co_tin_nhan_moi = 0;
			gsm_sendandcheck("AT\r", 15, 1,ver);
			send_gsm_cmd("AT+CMGL=\"ALL\"\r");
		}
		// Dang ky so bang tin nhan: khi o man hinh CHINH/PHU, nhan "luu"/"save"
		// tu so nao thi luu so do (giong nhu goi den de dang ky). Luon tieu thu
		// tin nhan nay (khong cho lot xuong xu_ly de tranh so la chay lenh).
		if(sms_dang_xu_ly && dang_ky_sms){
			sms_dang_xu_ly = 0;
			dang_ky_sms = 0;
			if(mode==2 && sub_mode<2 &&
			   (((lenh_sms[0]=='l'||lenh_sms[0]=='L') && (lenh_sms[1]=='u'||lenh_sms[1]=='U'))    // luu
			 || ((lenh_sms[0]=='s'||lenh_sms[0]=='S') && (lenh_sms[1]=='a'||lenh_sms[1]=='A')))){  // save
				// Chua luu voi: hien so ra LCD, cho bam M xac nhan / B huy (xu ly o case 2)
				u8 j; phone[10] = 0;
				for(j=0;j<11;j++) sdt_luu[j] = phone[j];
				vaitro_luu = have_master?(sub_mode?'u':'m'):'M';
				them_sdt   = phone_so_sanh_that_bai;
				man_hinh_luu = 1;
				mode_wait = 60;
			}
			gsm_sendandcheck("AT+CMGD=1,4\r",15,1,"  DELETING SMS  ");
		}
		if(sms_dang_xu_ly && !mode){
			// CCAPM1 = 0x49;
			xu_ly_tin_nhan();
			gsm_sendandcheck("AT+CMGD=1,4\r",15,1,"  DELETING SMS  ");
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
					LCD_guingay(0xc0,year,month,day);
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
						if(pin_dung()){
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
								if(get_master_phone())baocaosms("\rSai pin 5 lan"); 
							
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
					LCD_guigio(0xc7,"",hour,minute,second,flip_pulse);
					LCD_guingay(0xc0,year,month,day);
				}
				
				break;
			case 2:
				if(man_hinh_luu){
					// Man hinh luu so: 1 = cho xac nhan (SMS), 2 = da luu (hien so)
					LCD_guilenh(0x80);
					LCD_guichuoi(man_hinh_luu==1?"LUU SO? M:OK B:X":"DA LUU SO:      ");
					LCD_guilenh(0xc0);
					LCD_guichuoi(sdt_luu);
					LCD_guichuoi("      ");
					if(man_hinh_luu==1){
						if(phim_mode_nhan){          // M = xac nhan luu
							phim_mode_nhan = phim_back_nhan = 0;
							mode_wait = 60;
							if(them_sdt) phone_add(sdt_luu+1,vaitro_luu);
							{u8 j; for(j=0;j<11;j++) phone[j] = sdt_luu[j];}
							if(have_master)baocaosms("\rLuu thanh cong");
							else baocaosms("\rLuu Master");
							have_master = 1;
							man_hinh_luu = 2;
							LCD_xoa(DUOI);
						}else if(phim_back_nhan){    // B = huy
							phim_back_nhan = 0;
							mode_wait = 60;
							man_hinh_luu = 0;
							LCD_xoa(TREN); LCD_xoa(DUOI);
						}
					}else if(phim_mode_nhan || phim_back_nhan || phim_cong_nhan){
						phim_mode_nhan = phim_back_nhan = phim_cong_nhan = 0;
						mode_wait = 60;
						man_hinh_luu = 0;
						LCD_xoa(TREN); LCD_xoa(DUOI);
					}
					break;
				}
				LCD_guigio(0xc7,"",hour,minute,second,flip_pulse);
				LCD_guingay(0xc0,year,month,day);
				LCD_guilenh(0x80);
				switch(sub_mode){
					case 0: LCD_guichuoi(have_master?"CHINH:          ":"MASTER:          "); break;
					case 1: LCD_guichuoi("PHU:            "); break;
					case 2: LCD_guichuoi("    XOA SO      "); break;
					case 3: LCD_guichuoi("   XOA REMOTE   "); break;
					case 4: LCD_guichuoi("    DOI  PIN    "); break;
					case 5: LCD_guichuoi("  HUONG  MOTOR  "); break;
					case 6: LCD_guichuoi("      EXIT      "); break;
				}
				if(phim_mode_nhan){
					phim_mode_nhan = 0;
					phim_back_nhan = 0;
					sub_mode = (sub_mode+1)%7;
				}
				if(phim_cong_nhan){
					phim_cong_nhan = 0;
					if(sub_mode<2){
						// CHINH/PHU: luu so bang cach goi den. Bo tra cuu TK/SDT (khong dung CUSD tren A7680C)
						mode_wait = 60;
					}else if(sub_mode==6){   // EXIT: + de thoat ve man hinh chinh
						mode = sub_mode = 0;
						LCD_xoa(TREN);
					}else sub_mode = 6;

				}
				if(phim_back_nhan && sub_mode>1){
					phim_back_nhan = 0;
					LCD_xoa(TREN);
					LCD_guilenh(0x80);
					switch(sub_mode){
						case 6:   // EXIT: B cung thoat ve man hinh chinh
							mode = sub_mode = 0;
							break;
						case 2:   // XOA SO
							mode = 5;
							del_phone_idx = xoa_so_ke_tiep(0);
							lcd_update_chop = 1;
							break;
						case 3:   // XOA REMOTE
							mode = 6;
							del_rf_idx = 0;
							lcd_update_chop = 1;
							break;
						case 4:   // DOI PIN
							mode = 3;
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
						case 5:   // HUONG MOTOR
							mode = 4;
							sub_mode = eep_huong;
							LCD_guichuoi("HUONG: ");
							LCD_guichuoi(sub_mode?"PH":"TR");
							LCD_guichuoi("AI");
							break;
					}
				}

				if(!phim_back_doi && sub_mode <2){
					phim_back_nhan = 0;
					// Bao cho so chinh CU (truoc khi xoa) biet may vua bi RESET,
					// de phat hien neu bi ke xau loi dung - khong ngan duoc thao
					// tac (day la duong khoi phuc khi mat may/quen PIN) nhung it
					// nhat chu nha se duoc bao ngay.
					if(get_master_phone()) baocaosms("\rCANH BAO: thiet bi vua RESET (xoa so+remote) bang nut bam");
					phone_del(0);
					IAP_xoasector(SECTOR2);
					IAP_ghibyte(RFINDEX_EEPROM,0);
					IAP_docxoasector1();
					eeprom_buf[PIN_EEPROM] = eeprom_buf[PIN_EEPROM+1] = eeprom_buf[PIN_EEPROM+2] = eeprom_buf[PIN_EEPROM+3] = '0';
					IAP_ghisector1();
					have_master = 0;
					mode = sub_mode = 0;

				}


				break;
			case 5:
				// XOA SO: duyet cac so (tru vai tro M/m - master), + chon xoa,
				// man hinh xac nhan M:OK B:Huy (giong pattern man_hinh_luu).
				if(man_hinh_luu){
					LCD_guilenh(0x80);
					LCD_guichuoi(man_hinh_luu==1?"XOA SO NAY? M:OK":"DA XOA SO       ");
					LCD_guilenh(0xc0);
					LCD_guichuoi(sdt_luu);
					LCD_guichuoi("        ");
					if(man_hinh_luu==1){
						if(phim_mode_nhan){        // M = xac nhan xoa
							phim_mode_nhan = phim_back_nhan = 0;
							mode_wait = 60;
							phone_del(del_phone_idx);
							man_hinh_luu = 0;
							del_phone_idx = xoa_so_ke_tiep(0);   // duyet lai tu dau
							LCD_xoa(TREN); LCD_xoa(DUOI);
						}else if(phim_back_nhan){  // B = huy
							phim_back_nhan = 0;
							mode_wait = 60;
							man_hinh_luu = 0;
							LCD_xoa(TREN); LCD_xoa(DUOI);
						}
					}
					break;
				}
				if(lcd_update_chop){
					lcd_update_chop = 0;
					LCD_guilenh(0x80);
					if(!del_phone_idx){
						LCD_guichuoi("(KHONG CO SO)   ");
						LCD_guilenh(0xc0);
						LCD_guichuoi("M:thoat         ");
					}else{
						LCD_guichuoi("SO #");
						LCD_guidulieu(del_phone_idx/10+'0');
						LCD_guidulieu(del_phone_idx%10+'0');
						LCD_guidulieu(' ');
						LCD_guidulieu(eep_phone[(del_phone_idx-1)*PHONE_ENTRY+9]);
						LCD_guichuoi("  B:tiep +:xoa");
						LCD_guilenh(0xc0);
						LCD_guidulieu('0');
						{u8 j; for(j=0;j<9;j++) LCD_guidulieu(eep_phone[(del_phone_idx-1)*PHONE_ENTRY+j]);}
						LCD_guichuoi("      ");
					}
				}
				if(phim_back_nhan){
					phim_back_nhan = 0;
					mode_wait = 60;
					del_phone_idx = xoa_so_ke_tiep(del_phone_idx);
					lcd_update_chop = 1;
				}
				if(phim_cong_nhan && del_phone_idx){
					phim_cong_nhan = 0;
					mode_wait = 60;
					{u8 j; sdt_luu[0]='0'; for(j=0;j<9;j++) sdt_luu[j+1]=eep_phone[(del_phone_idx-1)*PHONE_ENTRY+j]; sdt_luu[10]=0;}
					man_hinh_luu = 1;
					LCD_xoa(TREN); LCD_xoa(DUOI);
				}
				if(phim_mode_nhan){
					phim_mode_nhan = 0;
					mode = 2;
					sub_mode = 2;
					LCD_xoa(TREN);
				}
				break;
			case 6:
				// XOA REMOTE: duyet cac remote thuong (khong gom khe khan cap/
				// module bao dong). Bam remote that de xem OK/X (khop hay khong
				// khop khe dang duyet). + chon xoa, man hinh xac nhan M:OK B:Huy.
				if(man_hinh_luu){
					LCD_guilenh(0x80);
					LCD_guichuoi(man_hinh_luu==1?"XOA REMOTE NAY? ":"DA XOA REMOTE   ");
					LCD_guilenh(0xc0);
					LCD_guichuoi("M:OK B:Huy      ");
					if(man_hinh_luu==1){
						if(phim_mode_nhan){
							phim_mode_nhan = phim_back_nhan = 0;
							mode_wait = 60;
							rf_del(del_rf_idx);
							man_hinh_luu = 0;
							del_rf_idx = 0;
							LCD_xoa(TREN); LCD_xoa(DUOI);
						}else if(phim_back_nhan){
							phim_back_nhan = 0;
							mode_wait = 60;
							man_hinh_luu = 0;
							LCD_xoa(TREN); LCD_xoa(DUOI);
						}
					}
					break;
				}
				if(lcd_update_chop){
					lcd_update_chop = 0;
					LCD_guilenh(0x80);
					if(!eep_rfindex){
						LCD_guichuoi("(KHONG CO REMOTE)");
						LCD_guilenh(0xc0);
						LCD_guichuoi("M:thoat         ");
					}else{
						LCD_guichuoi("REMOTE ");
						LCD_guidulieu((del_rf_idx+1)/10+'0');
						LCD_guidulieu((del_rf_idx+1)%10+'0');
						LCD_guidulieu('/');
						LCD_guidulieu(eep_rfindex/10+'0');
						LCD_guidulieu(eep_rfindex%10+'0');
						LCD_guichuoi("        ");
						LCD_guilenh(0xc0);
						LCD_guichuoi("B:tiep +:xoa    ");
					}
				}
				if(phim_back_nhan && eep_rfindex){
					phim_back_nhan = 0;
					mode_wait = 60;
					del_rf_idx = (del_rf_idx+1)%eep_rfindex;
					lcd_update_chop = 1;
				}
				if(phim_cong_nhan && eep_rfindex){
					phim_cong_nhan = 0;
					mode_wait = 60;
					man_hinh_luu = 1;
					LCD_xoa(TREN); LCD_xoa(DUOI);
				}
				if(phim_mode_nhan){
					phim_mode_nhan = 0;
					mode = 2;
					sub_mode = 3;
					LCD_xoa(TREN);
				}
				break;
			case 3:
				if(phim_mode_nhan){
					phim_mode_nhan = 0;
					sub_mode++;

					if(sub_mode==4){
						if(!pin_dung()){
							sub_mode = 0;
							LCD_guilenh(0x88);
							LCD_guidulieu('X');
							
						}else{
							LCD_guilenh(0x88);
							LCD_guidulieu(' ');
							
						}
					}else if(sub_mode == 8){
						mode = 2; sub_mode = 4;
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
					mode = 2; sub_mode = 4;
					pin[0] = pin[1] = pin[2] = pin[3] = 0;
					new_pin[0] = new_pin[1] = new_pin[2] = new_pin[3] = 0;
					LCD_noblink();
				}
				//display
				
				
				LCD_blinkXY(TREN,4+sub_mode+sub_mode/4*3);
				if(lcd_update_chop){
					lcd_update_chop =  0;
					LCD_guigio(0xc7,"",hour,minute,second,flip_pulse);
					LCD_guingay(0xc0,year,month,day);
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
					sub_mode = 5;
				}
				if(phim_back_nhan){
					phim_back_nhan = 0;
					sub_mode = 5;
					mode = 2;
				}
				LCD_guigio(0xc7,"",hour,minute,second,flip_pulse);
				LCD_guingay(0xc0,year,month,day);
				break;
		}
		


		xu_ly_rf();


		if(phone_update){
			phone_update = 0;
			if(co_cuoc_goi_toi){
				co_cuoc_goi_toi = 0;
				if(mode == 2 || mode == 3){
					gsm_sendandcheck("AT\r",15,1,ver);
					phone[10] = 0;
					if(phone_so_sanh_that_bai) phone_add(phone+1,have_master?(sub_mode?'u':'m'):'M');
					{u8 j; for(j=0;j<11;j++) sdt_luu[j] = phone[j];}  // luu so de hien LCD
					man_hinh_luu = 2;
					if(have_master)baocaosms("\rLuu thanh cong");
					else baocaosms("\rLuu Master");
					if(have_master && get_master_phone() && eep_baocao)baocaosms("\rDT moi duoc luu");
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
							baocaosms("\rMo cua len");
						}
						
					} 
				}
			}
			CCAPM1 = 0x49;
		}
		WATCHDOG;
	}
}



