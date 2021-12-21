#include "common.h"
#include "lcd.h"
#include "eeprom.h"
#include "gsm.h"

//GSM
#define PHONE_LENGTH 10
#define MAX_TANG 4

u8  __xdata  phone[11] ;
__bit phone_update;
__bit kiem_tra_danh_ba;
u8 __data danh_ba_cuoi;


u8  __xdata lenh_sms[161]; 
__bit da_gui_bao_cao;
__bit da_gui_bao_cao_thang;

__bit gsm_reset;
u8 __xdata gsm_delay_reset;
u8 __xdata relay1_delay_tat;
u8 __xdata relay2_delay_tat;
u8 __xdata relay3_delay_tat;
u8 __xdata relay4_delay_tat;
u8 __xdata relays1_delay_tat;
u8 __xdata relays2_delay_tat;
u8 __xdata relays3_delay_tat;
u8 __xdata relays4_delay_tat;
u8 __xdata delay_tinhieuD_cao;
u8 __xdata delay_tinhieuD_thap;
u8 __xdata delay_tinhieuA_cao;
u8 __xdata delay_tinhieuA_thap;


u8 __data so_lan_goi_dien;
u8 __data delay_cuoc_goi_ke_tiep;
__bit co_cuoc_goi_toi;
__bit phone_so_sanh_that_bai;
__bit phone_master;

__bit phone_header;
__bit pin_chinh_xac;
__bit sms_dang_xu_ly;
__bit co_tin_nhan_moi;
__bit have_cusd;
__bit lenh_khong_hop_le;
__bit have_quote;
u8  __data  sms_index;


//EEPROM
u8  __xdata eeprom_buf[SECTOR1_LENGTH];

//TIME
u8	__data  second;
u8	__data  minute;
u8	__data  hour;

u16	__xdata	PCA_Timer0;
u8  __xdata  ngay_reset_con_lai;

//XUNG GIAY
// __bit		giay_out;
// __bit		phut_out;
__bit		flip_pulse;


//PHIM
__bit      phim_mode_xuong;
__bit      phim_mode_nhan;
__bit      phim_mode_cu;
__bit      phim_mode_giu;
u8 __xdata phim_mode_doi;
__bit      phim_back_xuong;
__bit      phim_back_nhan;
__bit      phim_back_cu;
__bit      phim_back_giu;
u8 __xdata phim_back_doi;
__bit      phim_cong_xuong;
__bit      phim_cong_nhan;
__bit      phim_cong_cu;
__bit      phim_cong_giu;
u8 __xdata phim_cong_doi;

u8 __xdata mode;
__bit chop;
u8 __xdata mode_wait;


//GSM MODULE
__bit   gui_lenh_thanh_cong;
__bit   have_not;

u8 __xdata   connect;
u8 __xdata   connect_time_out;
u8 __xdata   total_try_time_out;
u8 __xdata   gsm_serial_cmd;

u8 __xdata   gsm_receive_pointer;
u8 __xdata  gsm_receive_buf[15];

//LOI va BAO LOI
u8 __xdata delay_next_reset_loi_bien_tan;

__bit loi_bien_tan;
__bit bao_loi_bien_tan;
__bit che_do_stop;
__bit bao_loi_stop;

