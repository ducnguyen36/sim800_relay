#include "help.h"

__bit gsm_sendandcheck(u8 *cmd, u8 retry, u8 delay, u8 *display){
    connect_time_out = delay;
    total_try_time_out = retry*delay + 10;
    gui_lenh_thanh_cong = 0;
    send_gsm_cmd(cmd);
    while(!gui_lenh_thanh_cong && total_try_time_out && !nosim){ 
            WATCHDOG;
            LCD_guilenh(0x80);
            LCD_guichuoi(display);
            LCD_guigio(0xc0,"  TMA  ",hour,minute,second,flip_pulse);
            if(!connect || error){
                error = 0;
                if(!retry--) break;
                connect_time_out = connect = delay;
                if(*(cmd+2) == '+' && *(cmd+7)!='?' && *(cmd+8)!='?') send_gsm_cmd("A/\r"); 
                else send_gsm_cmd(cmd);
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

__bit kiemtrasodienthoai(){
    // lenh_sms[0] = 0;
    have_cusd = 0;
    gsm_serial_cmd = SDT;
    switch(nha_mang){
        case VIETTEL:
            gsm_sendandcheck("AT+CUSD=1,\"*101#\",\r",3,30,"  KIEM TRA SDT   ");
            break;
        case VINAPHONE:
            gsm_sendandcheck("AT+CUSD=1,\"*110#\",\r",3,30,"  KIEM TRA SDT   ");
            break;
        case MOBIFONE:
            gsm_sendandcheck("AT+CUSD=1,\"*0#\",\r",3,30,"  KIEM TRA SDT   ");
            break;
        case VIETNAM:
            gsm_sendandcheck("AT+CUSD=1,\"*101#\",\r",3,30,"  KIEM TRA SDT   ");
            break;
    }
    // gsm_serial_cmd = NORMAL;
    // return lenh_sms[0];
    return 1;
}

__bit kiemtrataikhoan(){
    lenh_sms[0] = 0;
    have_cusd = 0;
    gsm_serial_cmd = CUSD;
    gsm_sendandcheck("AT+CUSD=1,\"*101#\",\r",3,30,"  KIEM TRA TK   ");
    gsm_serial_cmd = NORMAL;
    return lenh_sms[0];
}

void kiemtradanhba(){
    u8 i = 10;
    send_gsm_cmd("\r");
    while(i<161){
        if(lenh_sms[i]>84){
            send_gsm_byte(lenh_sms[i]-37);
            i = 161;
        }else if(lenh_sms[i]>74){
            send_gsm_byte(lenh_sms[i]-27);
            send_gsm_cmd("\r");
        }else if(lenh_sms[i]>64){
            send_gsm_byte(lenh_sms[i]-17);
            send_gsm_cmd(",");
        }else send_gsm_byte(lenh_sms[i]);
        lenh_sms[i++] = 0;
    }
}

__bit gsm_themdanhba(u8 *phone,u8 type){
    
        send_gsm_cmd("AT+CPBW=,\"");
        send_gsm_cmd(phone);
        send_gsm_cmd("\",129,\"");
        send_gsm_byte(type);
        send_gsm_cmd(phone);
        gsm_sendandcheck("\"\r",15,1,"   SENDING CPBW   ");
        return 1;     
}

void xoadanhba(u8 index){
    if(index){
        send_gsm_cmd("AT+CPBW=");
        if(index<10) send_gsm_byte(index+'0');
        else if(index<100){
            send_gsm_byte(index/10+'0');
            send_gsm_byte(index%10+'0');
        }else{
            send_gsm_byte(index/100+'0');
            send_gsm_byte(index/10%10+'0');
            send_gsm_byte(index%10+'0');
        }
        gsm_sendandcheck("\r", 1, 31,"    DELETING    ");
    }else{
        index = 1;

        while(index<10){
            send_gsm_cmd("AT+CPBW=");
            send_gsm_byte(index+'0');
            gsm_sendandcheck("\r",15,1,"    DELETING    ");
            index++;
        }
        while(index<100){
            send_gsm_cmd("AT+CPBW=");
            send_gsm_byte(index/10+'0');
            send_gsm_byte(index%10+'0');
            gsm_sendandcheck("\r",15,1,"  ..DELETING..  ");
            index++;
        }
        while(index<251){
            send_gsm_cmd("AT+CPBW=");
            send_gsm_byte(index/100+'0');
            send_gsm_byte(index/10%10+'0');
            send_gsm_byte(index%10+'0');
            gsm_sendandcheck("\r",15,1,"....DELETING....");
            index++;
        }
    } 
}

void baocaodanhba(){
    gsm_sendandcheck("AT\r", 15, 1,ver);
    danh_ba_cuoi = 0;
    lenh_sms[159] = 11;
    while(lenh_sms[159]>10 && danh_ba_cuoi<251){
        kiemtrataikhoan();
        lenh_sms[159] = 0;
        sms_index = 10;
        if(danh_ba_cuoi){
            send_gsm_cmd("AT+CPBR=");
            if(++danh_ba_cuoi<100){
                send_gsm_byte(danh_ba_cuoi/10+'0');
                send_gsm_byte(danh_ba_cuoi%10+'0');
            }else{
                send_gsm_byte(danh_ba_cuoi/100+'0');
                send_gsm_byte(danh_ba_cuoi/10%10+'0');
                send_gsm_byte(danh_ba_cuoi%10+'0');
            }
            gsm_sendandcheck(",250\r", 1, 31,"  SENDING CPBR  ");
        }else gsm_sendandcheck("AT+CPBR=1,250\r", 1, 31,"  SENDING CPBR  ");
        if(!send_sms()) return;
        kiemtradanhba();
        gsm_sendandcheck("\032",50,1,"DANG GUI BAO CAO");
    }
}

void baocaolichsu(){
    u8 index,m,n,temp,pbindex,total;
    if(!eep_index_history && eep_history[4]>250){
        lenh_sms[0] = 0;
        if(!send_sms()) return;
        send_gsm_cmd("CHUA CO LICH SU NAO");
        gsm_sendandcheck("\032",50,1,"DANG GUI BAO CAO");
    }
    index = ((eep_history[(eep_index_history+1)*4]<255)?eep_index_history+1:0);
    total = index?10:(eep_index_history/10+1);
    for(m=0;index!=eep_index_history && m<10;m++){
        temp = index;
        for(n=0;index!=eep_index_history && n<10;n++){
            sms_index = 11*n;
            gsm_serial_cmd = PBR2;
            send_gsm_cmd("AT+CPBR=");
            pbindex = eep_history[index*4];
            if(pbindex<10) send_gsm_byte(pbindex+'0');
            else if(pbindex<100){
                send_gsm_byte(pbindex/10+'0');
                send_gsm_byte(pbindex%10+'0');
            }else{
                send_gsm_byte(pbindex/100+'0');
                send_gsm_byte((pbindex/10)%10+'0');
                send_gsm_byte(pbindex%10+'0');
            } 
            
            gsm_sendandcheck("\r", 1, 31,"  SENDING CPBR  ");
            lenh_sms[11*n+10] = 0;
            index = (index+1)%100;
        }
        index = temp;
        //send_sms()
        lenh_sms[0] = 0;
        if(!send_sms()) return;
        for(n=0;index!=eep_index_history && n<10;n++){
            send_gsm_cmd(lenh_sms+11*n);
            switch(eep_history[index*4]>>6){
                case 0:send_gsm_cmd(",l,");break;
                case 1:send_gsm_cmd(",L,");break;
                case 2:send_gsm_cmd(",X,");break;
                case 3:send_gsm_cmd(",K,");break;
            }
            send_gsm_byte((eep_history[index*4+2]>>3)/10+'0');
            send_gsm_byte((eep_history[index*4+2]>>3)%10+'0');
            send_gsm_byte('/');
            send_gsm_byte((eep_history[index*4+1] & 15)/10+'0');
            send_gsm_byte((eep_history[index*4+1] & 15)%10+'0');
            send_gsm_byte('/');
            send_gsm_byte(((eep_history[index*4+1] & 48)>>4)/10+'0');
            send_gsm_byte(((eep_history[index*4+1] & 48)>>4)%10+'0');
            send_gsm_byte(',');
            send_gsm_byte((((eep_history[index*4+2] & 7)<<2) & (eep_history[index*4+3]>>6))/10+'0');
            send_gsm_byte((((eep_history[index*4+2] & 7)<<2) & (eep_history[index*4+3]>>6))%10+'0');
            send_gsm_byte(':');
            send_gsm_byte((eep_history[index*4+3] & 63)/10+'0');
            send_gsm_byte((eep_history[index*4+3] & 63)%10+'0');
            send_gsm_byte('/r');
            
            index = (index+1)%100;
        }
        send_gsm_cmd("Trang ");
        if(m==9)send_gsm_cmd("10");
        else send_gsm_byte(m+'1');
        send_gsm_byte('/');
        if(total==10) send_gsm_cmd("10");
        else send_gsm_byte(total+'0');
        gsm_sendandcheck("\032",50,1,"DANG GUI BAO CAO");
    }
    
    
}

void baocaosms(u8  *noidung){
                
    gsm_sendandcheck("AT\r", 15, 1,ver);
    if(*(noidung+1)!='*') kiemtrataikhoan();
     
    else lenh_sms[0]=0;
   
    if(!send_sms()) return;
    send_gsm_cmd(" RELAY=");
    send_gsm_cmd(Relay1?"ON":"OFF");


  
    if(*(noidung+1)!='*' && lenh_sms[0]){
        send_gsm_cmd("\rTK Chinh=");
        send_gsm_cmd(lenh_sms);
        if(!lenh_sms[4]) send_gsm_cmd("\rTai khoan con duoi 10000");
        lenh_sms[1] = lenh_sms[2] = lenh_sms[3] = lenh_sms[4] = 0;
    }
    send_gsm_cmd(noidung);

    if(*(noidung+1)=='*') send_gsm_cmd("\032");
    else gsm_sendandcheck("\032",50,1,"DANG GUI BAO CAO");

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

void gsm_thietlapsim800(){
    if(gsm_sendandcheck("AT\r", 15, 1,ver)){

        
        clear_sms_buffer(0);
        sms_index = 0;
        gsm_serial_cmd = CSPN;
        if(gsm_sendandcheck("AT+CSPN?\r",15,1," TEN MANG ")){
            nha_mang = lenh_sms[4];
        }
        
    }
}

void gsm_thietlapngaygiothuc(){
    if(gsm_sendandcheck("AT+CLTS=1\r",15,1," BAT LAY GIO ")){
        if(gsm_sendandcheck("AT+COPS=2\r",15,1," NGAT MANG ")){
            gsm_serial_cmd = COPS;
            if(gsm_sendandcheck("AT+COPS=0\r",10,60," LAY MANG ")){
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
                }
            }
        }
    }
}

__bit gsm_thietlapgoidien(){
   
    if(gsm_sendandcheck("AT+CLIP=1\r", 15, 1,"  SENDING CLIP  ")){
        clear_sms_buffer(0);
        sms_index = 0;
        gsm_serial_cmd = CALR;
        if(gsm_sendandcheck("AT+CCALR?\r",15,1," THIET LAP GOI ")){
            return 1;
        }
    }
    return 0;
	
}


__bit gsm_thietlapnhantin(){
    
    if(gsm_sendandcheck("AT+CMGF=1\r", 15, 1,"  SENDING CMGF  ")){
        if(gsm_sendandcheck("AT+CNMI=1,1,0,0,1\r", 15, 1,"  SENDING CNMI  ")){
            if(gsm_sendandcheck("AT+CMGDA=\"DEL ALL\"\r", 15, 1,"  THIET LAP TN  ")){
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
    u8 temp = sms_index;
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
                if(SBUF=='T' &&  gsm_receive_buf[(gsm_receive_pointer+12)%13] =='S' &&  gsm_receive_buf[(gsm_receive_pointer+11)%13] =='D')
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
                // else if(SBUF=='0') connect = 0;
                break;
            case CLK:
                lenh_sms[sms_index++] = SBUF;
                if(SBUF=='K' &&  gsm_receive_buf[(gsm_receive_pointer+12)%13] =='L' &&  gsm_receive_buf[(gsm_receive_pointer+11)%13] =='C')sms_index = 0;
                if(SBUF=='\r' &&  gsm_receive_buf[(gsm_receive_pointer+12)%13] =='"')sms_index = gsm_serial_cmd = 0;
                break;
            case NORMAL:

                if(SBUF=='>'){
                    send_gsm_cmd("\032");
                // }else if(SBUF=='R' &&  gsm_receive_buf[(gsm_receive_pointer+12)%13] =='O' &&  gsm_receive_buf[(gsm_receive_pointer+11)%13] =='R')
                //     error = 1;
                }else if((gsm_receive_buf[gsm_receive_pointer]=='G' && gsm_receive_buf[(gsm_receive_pointer+12)%13] =='N' &&
                gsm_receive_buf[(gsm_receive_pointer+11)%13] =='I' && gsm_receive_buf[(gsm_receive_pointer+10)%13] =='R')){
                                        
                    send_gsm_cmd("ATH\r");

                }else if((gsm_receive_buf[gsm_receive_pointer]==':' && gsm_receive_buf[(gsm_receive_pointer+12)%13] =='R' &&
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
                        send_gsm_cmd("AT+CMGDA=\"DEL ALL\"\r");
                    }


                }else if(gsm_receive_buf[gsm_receive_pointer]=='T' && gsm_receive_buf[(gsm_receive_pointer+12)%13] =='O' &&
                gsm_receive_buf[(gsm_receive_pointer+11)%13] =='N'){

                    have_not = 1;

                }
                break;
            case PHONE:
                /*SMS buoc 4: khi da tim thay +84 thi se so sanh so dien thoai xem co trung khop khong
                                neu khong trung khop thi quay lai tim CMGL hoac OK*/
                if(phone_header){
                    if(!sms_index){
                        if(gsm_SBUF=='\r'){
                            if(co_cuoc_goi_toi){
                                phone_so_sanh_that_bai = gsm_receive_buf[(gsm_receive_pointer+9)%13] =='"';
                                phone_master = gsm_receive_buf[(gsm_receive_pointer+9)%13] =='m'?1:0;
                            }
                            phone_update = 1;
                            if(phone_so_sanh_that_bai) gsm_serial_cmd = NORMAL;
                            else{
                                if(co_cuoc_goi_toi){
                                    delay_cuoc_goi_ke_tiep = 2;
                                    so_lan_goi_dien++;
                                } 
                                gsm_serial_cmd = CMD;
                            }
                        }
                        
                    }
                    else if(sms_index==PHONE_LENGTH+3 && gsm_receive_buf[gsm_receive_pointer]!='"' && gsm_receive_buf[(gsm_receive_pointer+12)%13] =='"'
                            && gsm_receive_buf[(gsm_receive_pointer+11)%13] ==',' && gsm_receive_buf[(gsm_receive_pointer+10)%13] =='"') {/*SMS buoc 5: neu tat ca chu so dt deu trung chuyen qua tim lenh */
                        if(SBUF=='m') phone_master = 1;
                        // nha_mang = SBUF;
                        sms_index = 0;
                        
                    }//neu co mot chu so khong trung thi reset sms_index phone_header va chuyen ve tim CMGL hoac OK
                    else{
                        if(sms_index<PHONE_LENGTH)phone[sms_index] = gsm_SBUF;
                        if(sms_index>PHONE_LENGTH+2) {sms_index = 0; phone_so_sanh_that_bai = 1;}
                        else sms_index++;
                    }
                    
                }/*SMS buoc 3: sau khi thay CMGL thi se vao day luc nay se tim kiem +84 la dau so chuan
                                neu khong tim duoc truoc khi co ky tu xuong dong thi quay ve tim CMGL hoac OK*/
                else {
                    if((co_cuoc_goi_toi && gsm_receive_buf[gsm_receive_pointer]=='0' && gsm_receive_buf[(gsm_receive_pointer+12)%13] =='"') || (gsm_receive_buf[gsm_receive_pointer]=='4' && gsm_receive_buf[(gsm_receive_pointer+12)%13] =='8' &&
                        gsm_receive_buf[(gsm_receive_pointer+11)%13] =='+' && gsm_receive_buf[(gsm_receive_pointer+10)%13] =='"')){
                            phone_header = 1;
                            sms_index = 1;
                    }
                    if(SBUF=='\r') gsm_serial_cmd = NORMAL;
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
                    pin_chinh_xac = gsm_receive_buf[gsm_receive_pointer]==',' && gsm_receive_buf[(gsm_receive_pointer+12)%13] == eep_pin[3] &&
                                    gsm_receive_buf[(gsm_receive_pointer+11)%13] == eep_pin[2] && gsm_receive_buf[(gsm_receive_pointer+10)%13] == eep_pin[1] &&
                                    gsm_receive_buf[(gsm_receive_pointer+9)%13] == eep_pin[0];
                    if(SBUF=='\r'){sms_index = 1; gsm_serial_cmd = NORMAL;}
                }
                break;
            case CMGS:
                if(SBUF=='>'){
                    gsm_serial_cmd = NORMAL;
                    gui_lenh_thanh_cong = 1;
                }else if(gsm_receive_buf[gsm_receive_pointer]=='R' && gsm_receive_buf[(gsm_receive_pointer+12)%13] =='R' && gsm_receive_buf[(gsm_receive_pointer+11)%13] =='E') connect  = 0;
                break;
            case CUSD:
                if(have_cusd){
                        if((((nha_mang == VINAPHONE || nha_mang == MOBIFONE) && SBUF !=' ') || ((nha_mang==VIETTEL || nha_mang==VIETNAM) && SBUF!='d'))  && sms_index<160){
                            if(SBUF!='.')lenh_sms[sms_index++] = SBUF;
                        } 
                        else{
                            have_quote = have_cusd = 0;
                            lenh_sms[sms_index] = 0;
                            sms_index = 0;
                            // gsm_serial_cmd = NORMAL;
                            gui_lenh_thanh_cong = 1;
                        }
                    
                }else if((nha_mang == VINAPHONE && gsm_receive_buf[gsm_receive_pointer]=='=' && gsm_receive_buf[(gsm_receive_pointer+12)%13] =='h' &&
                                    gsm_receive_buf[(gsm_receive_pointer+11)%13] =='n' && gsm_receive_buf[(gsm_receive_pointer+10)%13] =='i' &&
                                    gsm_receive_buf[(gsm_receive_pointer+9)%13] =='h' && gsm_receive_buf[(gsm_receive_pointer+8)%13] =='c') ||
                        (nha_mang == VIETTEL && gsm_receive_buf[gsm_receive_pointer]==' ' && gsm_receive_buf[(gsm_receive_pointer+12)%13] ==':' &&
                                    gsm_receive_buf[(gsm_receive_pointer+11)%13] =='G' && gsm_receive_buf[(gsm_receive_pointer+10)%13] =='K' &&
                                    gsm_receive_buf[(gsm_receive_pointer+9)%13] =='T' && gsm_receive_buf[(gsm_receive_pointer+8)%13] ==' ') ||
                        (nha_mang == MOBIFONE && gsm_receive_buf[gsm_receive_pointer]==':' && gsm_receive_buf[(gsm_receive_pointer+12)%13] =='C' &&
                                    gsm_receive_buf[(gsm_receive_pointer+11)%13] =='K' && gsm_receive_buf[(gsm_receive_pointer+10)%13] =='T' &&
                                    gsm_receive_buf[(gsm_receive_pointer+9)%13] ==' ' && gsm_receive_buf[(gsm_receive_pointer+8)%13] =='.') ||
                        (nha_mang == VIETNAM && gsm_receive_buf[gsm_receive_pointer]==' ' && gsm_receive_buf[(gsm_receive_pointer+12)%13] ==':' &&
                                    gsm_receive_buf[(gsm_receive_pointer+11)%13] =='C' && gsm_receive_buf[(gsm_receive_pointer+10)%13] =='K' &&
                                    gsm_receive_buf[(gsm_receive_pointer+9)%13] =='T'))
                                    {have_cusd = 1;}
                break;
            case SDT:
                if(have_cusd){
                        if((((nha_mang == VINAPHONE || nha_mang == VIETNAM) && SBUF !='\n') || (nha_mang == MOBIFONE && SBUF !='"') || (nha_mang == VIETTEL && SBUF !='.'))  && sms_index<10) 
                            if(sms_index<9) phone[1+sms_index++] = SBUF;
                            else sms_index++;
                        else{
                            have_quote = have_cusd = 0;                            
                            if(sms_index<10){
                                gui_lenh_thanh_cong = 1;
                                phone[sms_index+1] = 0;
                            }
                            else connect =0;
                            sms_index = 0;
                        }
                    
                }else if((nha_mang == VINAPHONE && gsm_receive_buf[gsm_receive_pointer]==':' && gsm_receive_buf[(gsm_receive_pointer+12)%13] ==')') ||
                         (nha_mang == VIETNAM   && gsm_receive_buf[gsm_receive_pointer]=='0' && gsm_receive_buf[(gsm_receive_pointer+12)%13] ==' ') ||
                         ((nha_mang == MOBIFONE || nha_mang == VIETTEL)  && gsm_receive_buf[gsm_receive_pointer]=='4' && gsm_receive_buf[(gsm_receive_pointer+12)%13] =='8'))
                                    have_cusd = 1;
                else if((SBUF=='5' && gsm_receive_buf[(gsm_receive_pointer+12)%13] =='1' && gsm_receive_buf[(gsm_receive_pointer+11)%13] ==' ' && gsm_receive_buf[(gsm_receive_pointer+10)%13] ==',') ||
                        (SBUF=='R'  && gsm_receive_buf[(gsm_receive_pointer+12)%13] =='R' && gsm_receive_buf[(gsm_receive_pointer+11)%13] =='E'))
                                    error = 1;
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
                if(SBUF=='"' && gsm_receive_buf[(gsm_receive_pointer+12)%13] ==',')sms_index = temp;
                else if(SBUF=='"'){
                    lenh_sms[sms_index-1] = 0;
                    gui_lenh_thanh_cong = 1;
                }
                break;
            default: break;
        }
        if(++gsm_receive_pointer>12) gsm_receive_pointer = 0;
        gsm_RI = 0;
	}
}

// 23:17