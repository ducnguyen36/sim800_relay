


void PCA_Timer_init(){
	CCAP0L = CCAP0H = 0;
	PCA_Timer0 = 25000;
	CCAPM0 = 0x49;
	CR=1;
}


void xunggiay(){
	WATCHDOG;
	flip_pulse^=1;
	over_cur_led = flip_pulse;
	// if(Relay1 && !--relay1_delay_tat){
	// 	Relay1 = 0;
	// 	relay1_delay_tat = 5;
	// } 
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
		second=0;
		if(so_lan_goi_dien && !--delay_cuoc_goi_ke_tiep) so_lan_goi_dien = 0;
		if(++minute>59){
			minute=0;
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

}