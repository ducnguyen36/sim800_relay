


void PCA_Timer_init(){
	CCAP0L = CCAP0H = 0;
	PCA_Timer0 = 25000;
	PCA_Timer1 = 250;
	CCAPM0 = 0x49;
	CCAPM1 = 0x49;
	CR=1;
}

/* Ap bitmask 4 relay (bit0=R1 .. bit3=R4). */
void dat_relay(u8 m){
	Relay1 = m & 1;
	Relay2 = (m>>1) & 1;
	Relay3 = (m>>2) & 1;
	Relay4 = (m>>3) & 1;
}

/* Luu trang thai 4 relay hien tai vao EEPROM (khoi phuc sau mat dien). */
void luu_relay(){
	IAP_docxoasector1();
	eeprom_buf[RELAY_STATE_EEPROM] =
		(Relay1?1:0) | (Relay2?2:0) | (Relay3?4:0) | (Relay4?8:0);
	IAP_ghisector1();
}


void xunggiay(){
	WATCHDOG;
	flip_pulse^=1;
	over_cur_led = flip_pulse;
	// Tu lay lai gio: neu chua co gio hop le, dem nguoc roi bao vong lap chinh thu lai.
	if(!gio_hop_le && gio_retry && !--gio_retry){
		lay_lai_gio = 1;
		gio_retry = 180;   // ~3 phut/lan cho toi khi lay duoc gio
	}
	if(rf_khancap_delay && !--rf_khancap_delay) rf_khancap = rf_khancap_delay = 0;
	if(phim_mode_doi && phim_mode_giu)phim_mode_doi--;
	if(phim_back_doi && phim_back_giu)phim_back_doi--;
	if(phim_cong_doi && phim_cong_giu)phim_cong_doi--;
	if(!phim_cong_doi)skip_gsm_cmd = 1;
	if(connect) connect--;
	if(total_try_time_out) total_try_time_out--;
	if(mode && mode_wait) mode_wait--;
	if(!gsm_pw && !--gsm_delay_reset){
		gsm_pw = 1;
		gsm_delay_reset = 10;
		gsm_reset=1;
	}
	if(++second>59){
		phut_out = 1;
		second=0;
		if(so_lan_goi_dien && !--delay_cuoc_goi_ke_tiep) so_lan_goi_dien = 0;
		if(++minute>59){
			minute=0;
			gio_out = 1;
			if(++hour>23){
				hour=0;
				if(ngay_reset_con_lai)
				ngay_reset_con_lai--;
			}
		}
			
	}
}


u8 __xdata cnt = 20;
u8 __xdata counter_xung_giay=40;
__bit lcd_update_chop = 0;
void	PCA_Handler (void) __interrupt PCA_VECTOR __using MEM_DONG_HO{
	WATCHDOG;
	if(CCF0){
		CCF0=0;//tat co PCA timer 0
		CCAP0L = PCA_Timer0; //nap vao vi tri timer tiep theo
		CCAP0H = PCA_Timer0 >> 8;
		PCA_Timer0 += 25000; //tang bien nap vao len 25ms
		
		if(phim_mode_xuong && phim_mode_vao) phim_mode_doi = 2;
		phim_mode_giu = phim_mode_xuong && !phim_mode_vao;
		phim_mode_xuong = !phim_mode_cu && !phim_mode_vao;
		phim_mode_nhan = phim_mode_nhan || (!phim_mode_giu && phim_mode_xuong);
		phim_mode_cu = phim_mode_vao;
		
		if(phim_back_xuong && key_in2) phim_back_doi = 6;
		phim_back_giu = phim_back_xuong && !key_in2;
		phim_back_xuong = !phim_back_cu && !key_in2;
		phim_back_nhan = phim_back_nhan || (!phim_back_giu && phim_back_xuong);
		phim_back_cu = key_in2;

		if(phim_cong_xuong && key_in3) phim_cong_doi = 2;
		phim_cong_giu = phim_cong_xuong && !key_in3;
		phim_cong_xuong = !phim_cong_cu && !key_in3;
		phim_cong_nhan = phim_cong_nhan || (!phim_cong_giu && phim_cong_xuong);
		phim_cong_cu = key_in3;

		if(!--cnt){
			lcd_update_chop = 1;
			cnt=10;
			chop=!chop;
		
		}

		if(!--counter_xung_giay){
			counter_xung_giay=40;
			xunggiay();
		}
		
	}
	if(CCF1){
		CCF1 = 0;
		CCAP1L = PCA_Timer1;
		CCAP1H = PCA_Timer1>>8;
		PCA_Timer1 +=250;
		if(rfprocess)return;
		if(!rfwait++){
			// Het tin hieu RF (nha nut): KHONG tat relay (relay giu trang thai);
			// chi danh dau da nha de lan bam ke tiep toggle duoc.
			rfstop = 1;
			rf_dang_giu = 0;
		}
		if(cam_che){
			if(!count_low){
				if(rfstatus && count_hi>2 && count_hi<7) {
					rfdata[rfindex++] = 0;
				}
			}
			count_low++;count_hi=0;
		}else{
			if(!count_hi){
				if(rfstatus && count_low>2 && count_low<7) {
					if(!pt2240 && rfindex%2 && !rfdata[rfindex-1]) pt2240 = 1;
					rfdata[rfindex++] = 1;
				}
				else if(count_low>28){
					rfwait = 1;
					if(rfstatus && rfindex==24) {
						rfprocess = 1;
					}
					else if(rfstop) {
						rfstatus = 1;
						pt2240 = rfindex = 0;
					}
					
				}
			}
			count_hi++;count_low=0;
		}
		if(rfindex>24){
			rfindex = 0;
			rfstatus = 0;
		}
	}
}