


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
	if(RelayS4)relays4_delay_tat--;
	if(!relays4_delay_tat){
		RelayS4 = 0;
		relays4_delay_tat = 6;
	}
	if(connect) connect--;
	if(total_try_time_out) total_try_time_out--;
	if(mode && mode_wait) mode_wait--;
	if(!gsm_pw && !--gsm_delay_reset){
		gsm_pw = 1;
		gsm_delay_reset = 10;
		gsm_reset=1;
	}

	if(tinhieuD){
		delay_tinhieuD_thap = 15;
		if(delay_tinhieuD_cao)delay_tinhieuD_cao--;
	}else{
		delay_tinhieuD_cao = 15;
		if(delay_tinhieuD_thap)delay_tinhieuD_thap--;
	}

	if(!loi_bien_tan && !delay_tinhieuD_cao) loi_bien_tan = 1;
	else if(loi_bien_tan && !delay_tinhieuD_thap) loi_bien_tan = 0;

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
		
		if(phim_back_xuong && phim_back_vao) phim_back_doi = 2;
		phim_back_giu = phim_back_xuong && !phim_back_vao;
		phim_back_xuong = !phim_back_cu && !phim_back_vao;
		phim_back_nhan = phim_back_nhan || (!phim_back_giu && phim_back_xuong);
		phim_back_cu = phim_back_vao;

		if(phim_cong_xuong && phim_cong_vao) phim_cong_doi = 2;
		phim_cong_giu = phim_cong_xuong && !phim_cong_vao;
		phim_cong_xuong = !phim_cong_cu && !phim_cong_vao;
		phim_cong_nhan = phim_cong_nhan || (!phim_cong_giu && phim_cong_xuong);
		phim_cong_cu = phim_cong_vao;

		

		// if(tinhieuD){
		// 	delay_tinhieuD_thap = 80;
		// 	if(delay_tinhieuD_cao)delay_tinhieuD_cao--;
		// }else{
		// 	delay_tinhieuD_cao = 80;
		// 	if(delay_tinhieuD_thap)delay_tinhieuD_thap--;
		// }

		// if(tinhieuA){
		// 	delay_tinhieuA_thap = 40;
		// 	if(delay_tinhieuA_cao)delay_tinhieuA_cao--;
		// }else{
		// 	delay_tinhieuA_cao = 40;
		// 	if(delay_tinhieuA_thap)delay_tinhieuA_thap--;
		// }

		// if(!loi_bien_tan && !delay_tinhieuD_thap && !delay_tinhieuA_thap)loi_bien_tan = 1;

		// if(!che_do_stop && !delay_tinhieuD_thap && !delay_tinhieuA_cao) che_do_stop = 1;

		// if(!delay_tinhieuD_cao)loi_bien_tan = che_do_stop = 0;
		

		
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