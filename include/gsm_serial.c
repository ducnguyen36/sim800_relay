#include "help.h"

/* ---- Bang so dien thoai trong EEPROM (thay cho danh ba SIM) ----
   Moi so: 9 chu so (so VN bo so 0 dau) + 1 byte vai tro ('M'/'m'/'u', 0 = trong). */

/* Tim so 9 chu so num9 trong bang. Tra ve chi so+1 neu thay (0 neu khong),
   va luu vai tro vao found_role. */
u8 phone_find(u8 *num9){
    u8 i,j,ok;
    for(i=0;i<eep_phone_count;i++){
        ok = 1;
        for(j=0;j<9;j++) if(eep_phone[i*PHONE_ENTRY+j]!=num9[j]){ok=0;break;}
        if(ok && eep_phone[i*PHONE_ENTRY+9]){
            found_role = eep_phone[i*PHONE_ENTRY+9];
            return i+1;
        }
    }
    return 0;
}

/* Them so 9 chu so + vai tro vao khe trong dau tien (hoac cuoi bang).
   Tra ve 1 neu thanh cong, 0 neu bang da day. */
__bit phone_add(u8 *num9, u8 role){
    u8 j,slot,cnt = eep_phone_count;
    for(slot=0;slot<cnt;slot++) if(!eep_phone[slot*PHONE_ENTRY+9]) break;
    if(slot==cnt && cnt>=PHONE_MAX) return 0;   // day: khong dong toi flash
    IAP_docxoasector1();
    for(j=0;j<9;j++) eeprom_buf[PHONE_TABLE_EEPROM+slot*PHONE_ENTRY+j] = num9[j];
    eeprom_buf[PHONE_TABLE_EEPROM+slot*PHONE_ENTRY+9] = role;
    if(slot==cnt) eeprom_buf[PHONE_COUNT_EEPROM] = cnt+1;
    IAP_ghisector1();
    return 1;
}

/* Xoa mot so (idx 1-based) hoac tat ca (idx==0). */
void phone_del(u8 idx){
    u16 j;
    IAP_docxoasector1();
    if(!idx){
        for(j=0;j<PHONE_MAX*PHONE_ENTRY;j++) eeprom_buf[PHONE_TABLE_EEPROM+j] = 0;
        eeprom_buf[PHONE_COUNT_EEPROM] = 0;
    }else if(idx<=eeprom_buf[PHONE_COUNT_EEPROM]){
        for(j=0;j<PHONE_ENTRY;j++) eeprom_buf[PHONE_TABLE_EEPROM+(idx-1)*PHONE_ENTRY+j] = 0;
    }
    IAP_ghisector1();
}

/* Xoa 1 remote THUONG (p = 0-based, 0..eep_rfindex-1; khe 0=khan cap, khe 1=
   module bao dong KHONG xoa duoc bang ham nay). Don bang cach doi cac khe sau
   xuong 1 vi tri (khong de lai khe trong) roi giam dem. Dung chung ca 2 san
   pham (dung boi man hinh XOA REMOTE). */
void rf_del(u8 p){
    u8 k,j;
    IAP_docxoasector2();
    for(k=p; k+1<eeprom_buf[RFINDEX_EEPROM-SECTOR2]; k++){
        for(j=0;j<3;j++)
            eeprom_buf[RFDATA_EEPROM+(k+2)*3+j-SECTOR2] = eeprom_buf[RFDATA_EEPROM+(k+3)*3+j-SECTOR2];
    }
    if(eeprom_buf[RFINDEX_EEPROM-SECTOR2]){
        for(j=0;j<3;j++)
            eeprom_buf[RFDATA_EEPROM+(eeprom_buf[RFINDEX_EEPROM-SECTOR2]+1)*3+j-SECTOR2] = 0;
        eeprom_buf[RFINDEX_EEPROM-SECTOR2]--;
    }
    IAP_ghisector2();
}

/* Nap so master ('m'/'M') dau tien vao phone[] (de gui bao cao).
   Tra ve 1 neu co master, 0 neu chua. Thay cho kiemtraphonemaster(). */
__bit get_master_phone(){
    u8 i,j,r;
    for(i=0;i<eep_phone_count;i++){
        r = eep_phone[i*PHONE_ENTRY+9];
        if(r=='m' || r=='M'){
            phone[0] = '0';
            for(j=0;j<9;j++) phone[j+1] = eep_phone[i*PHONE_ENTRY+j];
            phone[10] = 0;
            return 1;
        }
    }
    return 0;
}

__bit gsm_sendandcheck(u8 *cmd, u8 retry, u8 delay, u8 *display){
    connect_time_out = delay;
    total_try_time_out = retry*delay + 10;
    gui_lenh_thanh_cong = 0;
    send_gsm_cmd(cmd);
    while(!gui_lenh_thanh_cong && total_try_time_out && !nosim){ 
            WATCHDOG;
            lcd_update_chop = 0;
            LCD_guilenh(0x80);
            LCD_guichuoi(display);
            LCD_guigio(0xc0,"  CCA  ",hour,minute,second,flip_pulse);
            if(skip_gsm_cmd){skip_gsm_cmd = 0;return 0;}
            if(!connect || error){
                error = 0;
                if(!retry--) break;
                connect_time_out = connect = delay;
                // A7680C: lenh A/ (lap lai lenh truoc) da bo -> gui lai cmd
                send_gsm_cmd(cmd);
            } 
    }
    LCD_xoa(TREN);
    gsm_serial_cmd = NORMAL;
    return gui_lenh_thanh_cong;
}



__bit send_sms(){
    if(lenh_sms[0] && !lenh_sms[3]){gsm_sendandcheck("\032",3,1,"TK<1000 K BAOCAO"); return 0;}
    phone[10] = 0;
    send_gsm_cmd("AT+CMGS=\"");
    send_gsm_cmd(phone);
    gsm_serial_cmd = CMGS;
    return gsm_sendandcheck("\"\r",5,61,"   SENDING   ");
   
}

__bit gsm_quay_so(u8 *phone){
    send_gsm_cmd("ATD");
    send_gsm_cmd(phone);
    return gsm_sendandcheck(";\r",1,60,"    CALLING     ");
}

void send_pb_index(u8 n){
    if(n<10) send_gsm_byte(n+'0');
    else if(n<100){
        send_gsm_byte(n/10+'0');
        send_gsm_byte(n%10+'0');
    }else{
        send_gsm_byte(n/100+'0');
        send_gsm_byte(n/10%10+'0');
        send_gsm_byte(n%10+'0');
    }
}

/* Gui ve tin nhan danh sach cac so trong bang EEPROM. */
void baocao_bang_sdt(){
    u8 i,j;
    lenh_sms[0] = 0;
    if(!send_sms()) return;
    for(i=0;i<eep_phone_count;i++){
        if(!eep_phone[i*PHONE_ENTRY+9]) continue;
        send_pb_index(i+1);
        send_gsm_byte('.');
        send_gsm_byte('0');
        for(j=0;j<9;j++) send_gsm_byte(eep_phone[i*PHONE_ENTRY+j]);
        send_gsm_byte(',');
        send_gsm_byte(eep_phone[i*PHONE_ENTRY+9]);
        send_gsm_byte('\r');
    }
    gsm_sendandcheck("\032",50,1,"DANG GUI BAO CAO");
}

void baocaosms(u8  *noidung){
    gsm_sendandcheck("AT\r", 15, 1,ver);
    lenh_sms[0] = 0;
    if(!send_sms()) return;
    send_gsm_cmd(" KHOA=");
    send_gsm_cmd(eep_khoa?"ON":"OFF");
    send_gsm_cmd(noidung);
    if(*(noidung+1)=='*') send_gsm_cmd("\032");
    else gsm_sendandcheck("\032",120,1,"DANG GUI BAO CAO");
}

/* Hien ket qua hoc remote (LCD + SMS neu vua hoc moi), kem MA remote (hex 3
   byte) va vi tri (index) de nguoi dung/ho tro doi chieu dung nguoi bam.
   da_co=0: vua hoc moi thanh cong (co gui SMS). da_co=1: remote nay DA CO
   san trong bang (khong hoc lai - chi bao LCD, khong gui SMS de tranh spam
   khi lo bam trung remote cu luc dang hoc).
   idx_hien: 0 = khe khan cap, 0xFF = khe module bao dong, khac = so thu tu
   1-based trong danh sach remote THUONG (giong so hien o man hinh XOA
   REMOTE). Dung chung ca 2 san pham. */
void bao_ket_qua_hoc(u8 *data, u8 idx_hien, __bit da_co){
    LCD_xoa(TREN);
    LCD_guilenh(0x80);
    LCD_guichuoi(da_co ? " REMOTE DA CO!  " : " DA HOC REMOTE! ");
    LCD_guilenh(0xc0);
    if(idx_hien==0) LCD_guichuoi("KC ");
    else if(idx_hien==0xff) LCD_guichuoi("BD ");
    else{
        LCD_guidulieu('#');
        LCD_guidulieu(idx_hien/10+'0');
        LCD_guidulieu(idx_hien%10+'0');
    }
    LCD_guidulieu(' ');
    LCD_guichuoi("MA:");
    LCD_guihex(data[0]); LCD_guihex(data[1]); LCD_guihex(data[2]);
    LCD_guichuoi("   ");
    delay_ms(1800);
    LCD_xoa(TREN); LCD_xoa(DUOI);
    if(!da_co && get_master_phone() && eep_baocao){
        gsm_sendandcheck("AT\r", 15, 1, ver);
        lenh_sms[0] = 0;
        if(send_sms()){
            send_gsm_cmd(" KHOA=");
            send_gsm_cmd(eep_khoa?"ON":"OFF");
            send_gsm_cmd("\rremote dc hoc ");
            if(idx_hien==0) send_gsm_cmd("KHAN CAP");
            else if(idx_hien==0xff) send_gsm_cmd("BAO DONG");
            else{
                send_gsm_byte('#');
                send_gsm_byte(idx_hien/10+'0');
                send_gsm_byte(idx_hien%10+'0');
            }
            send_gsm_cmd(" MA:");
            send_gsm_hex(data[0]); send_gsm_hex(data[1]); send_gsm_hex(data[2]);
            gsm_sendandcheck("\032",120,1,"DANG GUI BAO CAO");
        }
    }
}

void clear_sms_buffer(u8 index_dau){
    sms_index = 0;
    while(index_dau<161)lenh_sms[index_dau++] = 0;
}

void gui_huong_dan(){
    lenh_sms[0]=0;
    if(!send_sms()) return;
    send_gsm_cmd(huongdan);
    gsm_sendandcheck("\032",50,1," GUI HUONG DAN  ");
}

__bit gsm_thietlapsim800(){
    if(gsm_sendandcheck("AT\r", 15, 1,ver)){
        clear_sms_buffer(0);
        sms_index = 0;
        gsm_serial_cmd = CSPN;
        if(gsm_sendandcheck("AT+CSPN?\r",15,1," TEN MANG ")){
            nha_mang = lenh_sms[4];
        }
        return 1;
    }
    return 0;
}

void gsm_thietlapngaygiothuc(){
    if(gsm_sendandcheck("AT+CTZU=1\r",25,2," BAT LAY GIO ")){   // A7680C auto timezone
        if(gsm_sendandcheck("AT+COPS=2\r",15,1,"   NGAT MANG    ")){
            gsm_serial_cmd = COPS;
            if(gsm_sendandcheck("AT+COPS=0\r",10,60,"    TIM MANG    ")){
                delay_ms(1000);   // cho module nhan gio mang (NITZ) sau khi dang ky roi moi doc CCLK
                clear_sms_buffer(0);
                sms_index = 0;
                gsm_serial_cmd = CLK;
                if(gsm_sendandcheck("AT+CCLK?\r",15,1," LAY GIO GPS ")){
                    year   = (lenh_sms[3] -48)*10 + lenh_sms[4]  - 48;
                    month  = (lenh_sms[6] -48)*10 + lenh_sms[7]  - 48;
                    day    = (lenh_sms[9] -48)*10 + lenh_sms[10] - 48;
                    hour   = (lenh_sms[12]-48)*10 + lenh_sms[13] - 48;
                    minute = (lenh_sms[15]-48)*10 + lenh_sms[16] - 48;
                    second = (lenh_sms[18]-48)*10 + lenh_sms[19] - 48;
                    u16 check = (23*month/9 + day + (month>2?!(year%4):2) + year + (year+3)/4 + 1) ;
                    date = check%7+1;
                    // Danh dau gio hop le (module da nhan gio mang) de ngung thu lai nhanh.
                    gio_hop_le = (year>=24 && year<=99 && month>=1 && month<=12 && day>=1 && day<=31);
                }
            }
        }
    }
}

__bit gsm_thietlapgoidien(){
    // A7680C: CLIP la du de nhan dien cuoc goi den (bo AT+CCALR?)
    return gsm_sendandcheck("AT+CLIP=1\r", 15, 1,"  SENDING CLIP  ");
}


__bit gsm_thietlapnhantin(){
    
    if(gsm_sendandcheck("AT+CMGF=1\r", 15, 1,"  SENDING CMGF  ")){
        if(gsm_sendandcheck("AT+CNMI=1,1,0,0,1\r", 15, 1,"  SENDING CNMI  ")){
            if(gsm_sendandcheck("AT+CMGD=1,4\r", 15, 1,"  THIET LAP TN  ")){
                gsm_serial_cmd = CSQ;
                clear_sms_buffer(0);
                sms_index  = 0;
                if(gsm_sendandcheck("AT+CSQ\r",15,1," SONG ")){
                    u8 i = 0;
                    signal = 0;
                    while(lenh_sms[i]!=',')signal = lenh_sms[i++]-48 + signal*10; 
                    
                }
                return 1;
            }
        }
    }	
    return 0;
}


void gsm_serial_interrupt() __interrupt gsm_SERIAL_INT __using SERIAL_MEM{
	if(gsm_RI){
        WATCHDOG;
	 	connect = connect_time_out;
        gsm_receive_buf[gsm_receive_pointer] = SBUF;
        
        if((gsm_receive_buf[gsm_receive_pointer]=='N' && gsm_receive_buf[(gsm_receive_pointer+12)%13] =='I' &&
        gsm_receive_buf[(gsm_receive_pointer+11)%13] ==' ' && gsm_receive_buf[(gsm_receive_pointer+10)%13] =='T' &&
        gsm_receive_buf[(gsm_receive_pointer+9)%13] =='O' && gsm_receive_buf[(gsm_receive_pointer+8)%13] =='N')){
                                
            nosim = 1;

        }

        switch(gsm_serial_cmd){
            case CSQ:
                lenh_sms[sms_index++] = SBUF;
                if(SBUF==' ' &&  gsm_receive_buf[(gsm_receive_pointer+12)%13] ==':')sms_index = 0;
                if(SBUF==',')sms_index = gsm_serial_cmd = NORMAL;
                break;
            case COPS:
                // A7680C tra ve COPS khac SIM800L: ky tu kiem tra la G E V (SIM800L la D S T)
                if(SBUF=='V' &&  gsm_receive_buf[(gsm_receive_pointer+12)%13] =='E' &&  gsm_receive_buf[(gsm_receive_pointer+11)%13] =='G')
                    gui_lenh_thanh_cong = 1;
                else if(SBUF=='R' &&  gsm_receive_buf[(gsm_receive_pointer+12)%13] =='O' &&  gsm_receive_buf[(gsm_receive_pointer+11)%13] =='R')
                    connect = 0;
                break;
            case CSPN:
                lenh_sms[sms_index++] = SBUF;
                if(SBUF=='"' &&  gsm_receive_buf[(gsm_receive_pointer+12)%13] ==' ')sms_index = 0;
                if(SBUF==',' &&  gsm_receive_buf[(gsm_receive_pointer+12)%13] =='"')sms_index = gsm_serial_cmd = 0;
                break;
            case CALR:
                if(SBUF=='1')gui_lenh_thanh_cong = 1;
                break;
            case CLK:
                lenh_sms[sms_index++] = SBUF;
                if(SBUF=='K' &&  gsm_receive_buf[(gsm_receive_pointer+12)%13] =='L' &&  gsm_receive_buf[(gsm_receive_pointer+11)%13] =='C')sms_index = 0;
                if(SBUF=='\r' &&  gsm_receive_buf[(gsm_receive_pointer+12)%13] =='"')sms_index = gsm_serial_cmd = 0;
                break;
            case NORMAL:

                if(SBUF=='>'){
                    send_gsm_cmd("\032");
                }else if((gsm_receive_buf[gsm_receive_pointer]=='G' && gsm_receive_buf[(gsm_receive_pointer+12)%13] =='N' &&
                gsm_receive_buf[(gsm_receive_pointer+11)%13] =='I' && gsm_receive_buf[(gsm_receive_pointer+10)%13] =='R')){
                                        
                    CCAPM1 = 0;
                    // send_gsm_cmd("ATH\r");
                    send_gsm_cmd("AT+CHUP\r");

                }else if((gsm_receive_buf[gsm_receive_pointer]==':' && (gsm_receive_buf[(gsm_receive_pointer+12)%13] =='R' || gsm_receive_buf[(gsm_receive_pointer+12)%13] =='F') &&
                gsm_receive_buf[(gsm_receive_pointer+11)%13] =='B' && gsm_receive_buf[(gsm_receive_pointer+10)%13] =='P' &&
                gsm_receive_buf[(gsm_receive_pointer+9)%13] =='C' && gsm_receive_buf[(gsm_receive_pointer+8)%13] =='+')){
                    if(lenh_sms[159]>9){lenh_sms[159]=11; break;}
                    kiem_tra_danh_ba = 1;              
                    gsm_serial_cmd = CPBR;
                    danh_ba_cuoi = have_quote = 0;
                    
                /*SMS buoc 2: sau khi kiem duoc CMGL thi chuyen qua tim kiem so dien thoai phu hop
                                neu nhu da nhan duoc tin nhan can xu ly thi khong doc tin nhan khac tiep tuc*/
                }else if((gsm_receive_buf[gsm_receive_pointer]==':' && gsm_receive_buf[(gsm_receive_pointer+12)%13] =='I' &&
                gsm_receive_buf[(gsm_receive_pointer+11)%13] =='T' && gsm_receive_buf[(gsm_receive_pointer+10)%13] =='M' &&
                gsm_receive_buf[(gsm_receive_pointer+9)%13] =='C' && gsm_receive_buf[(gsm_receive_pointer+8)%13] =='+')){
                                        
                    co_tin_nhan_moi = 1;
                    
                }else if((gsm_receive_buf[gsm_receive_pointer]==' ' && gsm_receive_buf[(gsm_receive_pointer+12)%13] ==':' &&
                gsm_receive_buf[(gsm_receive_pointer+11)%13] =='L' && gsm_receive_buf[(gsm_receive_pointer+10)%13] =='G' &&
                gsm_receive_buf[(gsm_receive_pointer+9)%13] =='M'  && gsm_receive_buf[(gsm_receive_pointer+8)%13] =='C' &&
                gsm_receive_buf[(gsm_receive_pointer+7)%13] =='+')||(gsm_receive_buf[gsm_receive_pointer]==' ' && gsm_receive_buf[(gsm_receive_pointer+12)%13] ==':' &&
                gsm_receive_buf[(gsm_receive_pointer+11)%13] =='P' && gsm_receive_buf[(gsm_receive_pointer+10)%13] =='I' &&
                gsm_receive_buf[(gsm_receive_pointer+9)%13] =='L'  && gsm_receive_buf[(gsm_receive_pointer+8)%13] =='C' &&
                gsm_receive_buf[(gsm_receive_pointer+7)%13] =='+')){
                    co_cuoc_goi_toi = gsm_receive_buf[(gsm_receive_pointer+9)%13] =='L';
                    if(!co_cuoc_goi_toi && sms_dang_xu_ly) break;
                    
                    
                    phone_header = 0;
                    phone_so_sanh_that_bai = 0;
                    phone_master = 0;
                    phone_super = 0;
                    gsm_serial_cmd = PHONE;

                }else if(gsm_receive_buf[gsm_receive_pointer]=='K' && gsm_receive_buf[(gsm_receive_pointer+12)%13] =='O'){
                    if(gsm_serial_cmd==CLK) sms_index = 0;
                    gui_lenh_thanh_cong = !have_not;
                    have_not = 0;
                    if(kiem_tra_danh_ba){
                        lenh_sms[sms_index-1]+=10;
                        sms_index = have_quote = 0;
                    }
                    if(sms_index){
                        sms_index = 0;
                        send_gsm_cmd("AT+CMGD=1,4\r");
                    }


                }else if(gsm_receive_buf[gsm_receive_pointer]=='T' && gsm_receive_buf[(gsm_receive_pointer+12)%13] =='O' &&
                gsm_receive_buf[(gsm_receive_pointer+11)%13] =='N'){

                    have_not = 1;

                }
                break;
            case PHONE:
                /* A7680C: khong dung danh ba SIM. Doc so dien thoai tu
                   +CLIP/+CMGL vao phone[1..9] roi tra cuu trong bang EEPROM. */
                if(phone_header){
                    if(!sms_index){
                        if(gsm_SBUF=='\r'){
                            if(co_cuoc_goi_toi){
                                phone_update = 1;
                                delay_cuoc_goi_ke_tiep = 2;
                                so_lan_goi_dien++;
                            }
                            // SMS den khi dang o man hinh dang ky (CHINH/PHU) -> cho phep
                            // xu ly de dang ky bang tin nhan "luu"/"save" (ke ca so la).
                            dang_ky_sms = !co_cuoc_goi_toi && mode==2 && sub_mode<2;
                            if(co_cuoc_goi_toi || (phone_so_sanh_that_bai && !dang_ky_sms)) gsm_serial_cmd = NORMAL;
                            else gsm_serial_cmd = CMD;
                        }
                    }
                    else if(sms_index==PHONE_LENGTH && gsm_SBUF=='"'){
                        /* Du 9 chu so -> tra cuu bang, xac dinh vai tro */
                        u8 idx;
                        phone[0] = '0';
                        phone[10] = 0;
                        idx = phone_find(phone+1);
                        phone_so_sanh_that_bai = !idx;
                        phone_super  = idx && found_role=='M';
                        phone_master = idx && (found_role=='m' || found_role=='M');
                        sms_index = 0;
                    }
                    else{
                        if(sms_index<PHONE_LENGTH) phone[sms_index] = gsm_SBUF;
                        if(sms_index>PHONE_LENGTH) {sms_index = 0; phone_so_sanh_that_bai = 1;}
                        else sms_index++;
                    }
                }
                else{
                    if((co_cuoc_goi_toi && gsm_receive_buf[gsm_receive_pointer]=='0' && gsm_receive_buf[(gsm_receive_pointer+12)%13] =='"')
                       || (gsm_receive_buf[gsm_receive_pointer]=='4' && gsm_receive_buf[(gsm_receive_pointer+12)%13] =='8'
                           && gsm_receive_buf[(gsm_receive_pointer+11)%13] =='+' && gsm_receive_buf[(gsm_receive_pointer+10)%13] =='"')){
                        phone_header = 1;
                        sms_index = 1;
                    }
                    if(SBUF=='\r') {gsm_serial_cmd = NORMAL; CCAPM1 = 0x49;}
                }
                break;
            case CMD:/*SMS buoc 7: tim duoc pin chinh xac tien hanh lay lenh trong tin nhan
                                    khi gap ; hoac xuong dong hoac het 160 ky tu tin nhan
                                    thi chuyen qua xu ly lenh thong bao gui lenh thanh cong*/
                if(pin_chinh_xac){
                     
                    if(SBUF!=';' && SBUF!='\r' && sms_index<160){
                       
                        lenh_sms[sms_index++] = SBUF;    
                    }
                    else{
                        lenh_sms[sms_index] = 0;
                        sms_index = 0;
                        pin_chinh_xac = 0;
                        gsm_serial_cmd = NORMAL;
                        sms_dang_xu_ly = 1;
                    }
                }
                else {/*SMS buoc 6: tim xem co ma pin trung khop khong neu khong co truoc khi gap ky tu xuong dong thi quay ve NORMAL*/
                    pin_chinh_xac = phone_super || dang_ky_sms || (gsm_receive_buf[gsm_receive_pointer]==',' && gsm_receive_buf[(gsm_receive_pointer+12)%13] == eep_pin[3] &&
                                    gsm_receive_buf[(gsm_receive_pointer+11)%13] == eep_pin[2] && gsm_receive_buf[(gsm_receive_pointer+10)%13] == eep_pin[1] &&
                                    gsm_receive_buf[(gsm_receive_pointer+9)%13] == eep_pin[0]);
                    if(SBUF=='\r'){sms_index = 1; gsm_serial_cmd = NORMAL;}
                }
                break;
            case CMGS:
                if(SBUF=='>'){
                    gsm_serial_cmd = NORMAL;
                    gui_lenh_thanh_cong = 1;
                }else if(gsm_receive_buf[gsm_receive_pointer]=='R' && gsm_receive_buf[(gsm_receive_pointer+12)%13] =='R' && gsm_receive_buf[(gsm_receive_pointer+11)%13] =='E') connect  = 0;
                break;
            case CPBR:
                if(SBUF == '"'){
                    have_quote = 1;
                    if(gsm_receive_buf[(gsm_receive_pointer+12)%13] == ',')lenh_sms[sms_index-1]+=17;
                    else{
                        lenh_sms[sms_index-1] += 27;
                        ++lenh_sms[159];
                        gsm_serial_cmd = NORMAL;
                    }
                }else if(SBUF>47){
                    if(!have_quote)danh_ba_cuoi = danh_ba_cuoi*10 + SBUF - '0'; 
                    lenh_sms[sms_index++] = SBUF;
                }
                break;
            case PBR2:
                
                lenh_sms[sms_index++] = SBUF;
                if(SBUF=='"' && gsm_receive_buf[(gsm_receive_pointer+12)%13] ==',')sms_index = sms_index_goc;
                else if(SBUF=='"'){
                    lenh_sms[sms_index-1] = 0;
                    sms_index = 0;
                    gsm_serial_cmd = NORMAL;
                    gui_lenh_thanh_cong = 1;
                }
                break;
            default: break;
        }
        if(++gsm_receive_pointer>12) gsm_receive_pointer = 0;
        gsm_RI = 0;
	}
}

