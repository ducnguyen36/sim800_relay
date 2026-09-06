/*
	Bo giai ma RF DUNG CHUNG cho ca 2 san pham (door + relay).
	Duoc #include BEN TRONG khoi CCF1 cua PCA_Handler, ngay SAU phan xu ly
	timeout (rieng tung san pham). Nho vao day, sua thuat toan giai ma chi can
	sua 1 cho -> ca cua cuon lan hop relay deu duoc cap nhat.

	Dieu kien: cac bien rfstatus/rfprocess/rfdata/rfindex/count_low/count_hi/
	pt2240/rfwait/rfstop + chan cam_che phai da khai bao (qua main.h) o ca 2.

	1.7/1.1: chot frame NGAY khi du 24 bit (khong doi sync ke tiep) -> bam nhanh
	van an, khong phai giu.
*/
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
					if(rfstop) {
						rfstatus = 1;
						pt2240 = rfindex = 0;
					}
				}
			}
			count_hi++;count_low=0;
		}
		if(rfstatus && rfindex==24){
			rfprocess = 1;
		}else if(rfindex>24){
			rfindex = 0;
			rfstatus = 0;
		}
