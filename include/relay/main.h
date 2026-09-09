#include "common.h"
#include "lcd.h"
#include "eeprom.h"
#include "gsm.h"

//GSM
#define PHONE_LENGTH 10

u8  __xdata  phone[11] ;
u8  __xdata  found_role;   // role byte of the last phone_find() hit
u8  __xdata  sdt_luu[11];  // so dt dang cho xac nhan / vua luu (de hien LCD)
u8  __xdata  vaitro_luu;   // vai tro se luu khi xac nhan (M/m/u)
u8  __xdata  man_hinh_luu; // 0=binh thuong, 1=cho xac nhan luu (SMS), 2=da luu
__bit them_sdt;            // so cho luu chua co trong bang (can them)
__bit phone_update;
__bit kiem_tra_danh_ba;
u8 __data danh_ba_cuoi;

// --- Hop 4 relay ---
u8   __xdata relay_sel;     // relay dang chon o man hinh panel (0..3)
__bit rf_dang_giu;          // remote dang duoc giu (chong toggle lien tuc)
// --- Tu lay lai gio ---
__bit gio_hop_le;           // da lay duoc gio mang hop le
__bit lay_lai_gio;          // co: vong lap chinh goi lai gsm_thietlapngaygiothuc()
u8   __xdata gio_retry;     // dem nguoc (giay) truoc lan thu lay gio ke tiep
// XOA SO / XOA REMOTE (menu): idx dang duyet.
u8   __xdata del_phone_idx;  // 1-based idx trong bang so dang duyet (0 = chua co)
u8   __xdata del_rf_idx;     // 0-based idx trong danh sach remote thuong dang duyet


u8  __xdata lenh_sms[161]; 
__bit da_gui_bao_cao;
__bit da_gui_bao_cao_thang;

__bit gsm_reset;
u8 __xdata gsm_delay_reset;
u8 __xdata relay1_delay_tat;
u8 __xdata relay2_delay_tat;
u8 __xdata relay3_delay_tat;
u8 __xdata relay4_delay_tat;
__bit relay2giu;

u8 __data so_lan_goi_dien;
u8 __data delay_cuoc_goi_ke_tiep;
u8 __xdata delay_chay_khoi_tao;
// __bit da_chay_khoi_tao;
__bit co_cuoc_goi_toi;
__bit phone_so_sanh_that_bai;
__bit phone_master,phone_super;

__bit phone_header;
__bit pin_chinh_xac;
__bit sms_dang_xu_ly;
__bit dang_ky_sms;      // SMS den khi dang o man hinh dang ky CHINH/PHU (mode 2, sub_mode<2)
__bit co_tin_nhan_moi;
__bit lenh_khong_hop_le;
__bit have_quote;
__bit have_master;
u8  __data  sms_index;
u8  __data  sms_index_goc;


//EEPROM
u8  __xdata eeprom_buf[EEPROM_BUF_LENGTH];

//RF
__bit rfstatus,rfprocess,rfstop,rflearn,rflock,pt2240;
u8  __xdata rfwait;
u8  __xdata rfindex;
u8  __xdata rfdata[26];
u8  __xdata count_low,count_hi;
u8  __xdata  rf_khancap,rf_khancap_delay;

//TIME
u8	__data  second;
u8	__data  minute;
u8	__data  hour;
u8	__data  date;
u8	__data  day;
u8	__data  month;
u8	__data  year;
__bit  new_day;
__bit  phut_out;
__bit  gio_out;

u16	__xdata	PCA_Timer0;
u16	__xdata	PCA_Timer1;
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

u8 __xdata mode, sub_mode;
__bit chop;
u8 __xdata mode_wait;
u8 __xdata pin[4],new_pin[4],so_lan_sai_pin;


//GSM MODULE
__bit   skip_gsm_cmd;
__bit   gui_lenh_thanh_cong;
__bit   have_not;
__bit   nosim;
__bit   error;
u8 __xdata   nha_mang;
u8 __xdata   signal;

u8 __xdata   connect;
u8 __xdata   connect_time_out;
u8 __xdata   total_try_time_out;
u8 __xdata   gsm_serial_cmd;

u8 __xdata   gsm_receive_pointer;
u8 __xdata  gsm_receive_buf[15];

