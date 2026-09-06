/* Nhan lai trang thai 4 relay cho nguoi vua gui lenh (phone[] da tro toi so do).
   Rieng cho san pham RELAY (khong nam trong gsm_serial.c dung chung, de khong
   ton flash vo ich cho san pham CUA CUON). */
void baocao_trang_thai(){
    gsm_sendandcheck("AT\r", 15, 1,ver);
    lenh_sms[0] = 0;
    if(!send_sms()) return;
    send_gsm_cmd("\rRELAY R1:");
    send_gsm_byte(Relay1?'1':'0');
    send_gsm_cmd(" R2:");
    send_gsm_byte(Relay2?'1':'0');
    send_gsm_cmd(" R3:");
    send_gsm_byte(Relay3?'1':'0');
    send_gsm_cmd(" R4:");
    send_gsm_byte(Relay4?'1':'0');
    gsm_sendandcheck("\032",120,1,"DANG GUI BAO CAO");
}

void xu_ly_tin_nhan(){
    // Lenh SMS cho hop 4 relay. Cu phap: <pin>,<lenh>
    lenh_khong_hop_le = 0;
    u8 i = 0;
    while(lenh_sms[i++])WATCHDOG;
    while(i<160)lenh_sms[i++] = 0;
    i=0;
    if((lenh_sms[0]=='?'||lenh_sms[1]=='?') && !phone_so_sanh_that_bai)gui_huong_dan();
    else{
        switch(lenh_sms[0]){
            case 'R':
            case 'r':
                // Rn,ON / Rn,OFF : bat/tat relay n (1..4). Cho phep so da luu (master/user).
                if(lenh_sms[1]>='1' && lenh_sms[1]<='4'){
                    if(phone_so_sanh_that_bai) break;
                    {
                        u8 idx = lenh_sms[1]-'1';
                        u8 m = (Relay1?1:0)|(Relay2?2:0)|(Relay3?4:0)|(Relay4?8:0);
                        if(lenh_sms[4]=='F' || lenh_sms[4]=='f') m &= ~(1<<idx);
                        else if(lenh_sms[4]=='N' || lenh_sms[4]=='n') m |= (1<<idx);
                        else { baocaosms("\rLenh khong hop le"); break; }
                        dat_relay(m);
                        luu_relay();
                        baocao_trang_thai();
                    }
                    break;
                }
                // Rr,r : khoi tao lai thiet bi (chi Super M). Nguoc lai: reset module GSM.
                if(!phone_master) break;
                if(lenh_sms[2] == lenh_sms[4] && (lenh_sms[2] == 'r' || lenh_sms[2] == 'R') && phone_super){
                     baocaosms("\rKhoi tao lai thiet bi");
                    phone_del(0);
                    IAP_xoasector(SECTOR2);
                    IAP_ghibyte(RFINDEX_EEPROM,0);
                    IAP_docxoasector1();
                    eeprom_buf[PIN_EEPROM] = eeprom_buf[PIN_EEPROM+1] = eeprom_buf[PIN_EEPROM+2] = eeprom_buf[PIN_EEPROM+3] = '0';
                    eeprom_buf[KHOA_EEPROM] = 0;
                    eeprom_buf[RELAY_STATE_EEPROM] = 0;
                    eeprom_buf[BAOCAO_EEPROM] = 1; // mac dinh BAT bao cao
                    IAP_ghisector1();
                    dat_relay(0);
                    have_master = 0;
                }else{
                    baocaosms("\rreset gsm sau 10s");
                    gsm_pw = 0;
                }
                break;
            case 'A':
            case 'a':
                // ALL,ON / ALL,OFF : bat/tat ca 4 relay
                if(phone_so_sanh_that_bai) break;
                if(lenh_sms[5]=='N' || lenh_sms[5]=='n'){
                    dat_relay(0x0f); luu_relay(); baocao_trang_thai();
                }else if(lenh_sms[5]=='F' || lenh_sms[5]=='f'){
                    dat_relay(0); luu_relay(); baocao_trang_thai();
                }else baocaosms("\rLenh khong hop le");
                break;
            case 'K':
            case 'k':
                // Chi con khoa/mo ban phim (Kp,ON / Kp,OFF). Bo khoa cua cuon.
                if(!phone_master) break;
                if(lenh_sms[1] == 'P' || lenh_sms[1] == 'p' ){
                    IAP_docxoasector1();
                    if(lenh_sms[4] == 'N' || lenh_sms[4] == 'n' ){
                        eeprom_buf[KHOA_EEPROM] |= 2;
                        baocaosms("\rKhoa ban phim");
                    }else{
                        eeprom_buf[KHOA_EEPROM] &= 1;
                        baocaosms("\rMo ban phim");
                    }
                    IAP_ghisector1();
                }else baocaosms("\rLenh khong hop le");
                break;
            case 'P':
            case 'p':
                if(!phone_master) break;
                if(lenh_sms[4]>='0' && lenh_sms[4]<='9' && lenh_sms[5]>='0' && lenh_sms[5]<='9'
                && lenh_sms[6]>='0' && lenh_sms[6]<='9' && lenh_sms[7]>='0' && lenh_sms[7]<='9'){
                    IAP_docxoasector1();
                    eeprom_buf[PIN_EEPROM  ] = lenh_sms[4];
                    eeprom_buf[PIN_EEPROM+1] = lenh_sms[5];
                    eeprom_buf[PIN_EEPROM+2] = lenh_sms[6];
                    eeprom_buf[PIN_EEPROM+3] = lenh_sms[7];
                    IAP_ghisector1();
                    baocaosms("\rDoi ma pin thanh cong");
                }else baocaosms("\rLenh khong hop le");
                break;
            case 'D':
            case 'd':
                if(!phone_master) break;
                baocao_bang_sdt();
                break;
            case 'X':
            case 'x':
                // Xoa so (Xo,n / Xo,0). Bo lenh Xuong cua.
                if(lenh_sms[1] == 'o' || lenh_sms[1] == 'O'){
                    if(!phone_master) break;
                    if(lenh_sms[4]>='0' && lenh_sms[4]<='9' && ((lenh_sms[5]>='0' && lenh_sms[5]<='9') || lenh_sms[5] == 0)
                    && ((lenh_sms[6]>='0' && lenh_sms[6]<='9') || lenh_sms[6] == 0)){
                        if(lenh_sms[4]=='0') phone_del(0);
                        else if(!lenh_sms[5]) phone_del(lenh_sms[4]-'0');
                        else if(!lenh_sms[6]) phone_del((lenh_sms[4]-'0')*10+lenh_sms[5]-'0');
                        else if(((lenh_sms[5]-'0')*10+lenh_sms[6]-'0'>50) && lenh_sms[4]>1) {baocaosms("\rLenh khong hop le");break;}
                        else phone_del((lenh_sms[4]-'0')*100 + (lenh_sms[5]-'0')*10 + lenh_sms[6]-'0');
                        baocaosms("\rXoa danh ba thanh cong");
                    }else baocaosms("\rLenh khong hop le");
                }else baocaosms("\rLenh Khong Hop Le");
                break;
            case 'L':
            case 'l':
                // Luu so (Lu,<so>[,m]). Bo lenh Le (mo cua).
                if(lenh_sms[1] == 'u' || lenh_sms[1] == 'U'){
                    if(!phone_master) break;
                    if(lenh_sms[4] >='0' && lenh_sms[4] <='9' && lenh_sms[5] >='0' && lenh_sms[5] <='9' && lenh_sms[6] >='0' && lenh_sms[6] <='9' && lenh_sms[7] >='0' && lenh_sms[7] <='9'
                    && lenh_sms[8] >='0' && lenh_sms[8] <='9' && lenh_sms[9] >='0' && lenh_sms[9] <='9' && lenh_sms[10]>='0' && lenh_sms[10]<='9' && lenh_sms[11]>='0' && lenh_sms[11]<='9'
                    && lenh_sms[12]>='0' && lenh_sms[12]<='9' && lenh_sms[13]>='0' && lenh_sms[13]<='9'){
                        lenh_sms[14] = 0;
                        // luu 9 chu so (bo so 0 dau) + vai tro; 'm' = master, con lai = user
                        if(phone_add(lenh_sms+5,(lenh_sms[15]=='m')?'m':'u'))
                            baocaosms("\rthem danh ba thanh cong");
                        else baocaosms("\rBang so da day");
                    }else baocaosms("\rSo dt khong hop Le");
                }else baocaosms("\rLenh Khong Hop Le");
                break;
            case 'b':
            case 'B':
                // B (khong tham so): tra ve trang thai 4 relay (so da luu deu xem duoc).
                // BC,Bat / BC,Tat: bat/tat bao cao (chi master).
                if(!lenh_sms[3] || !lenh_sms[2]){
                    if(phone_so_sanh_that_bai) break;
                    baocao_trang_thai();
                    break;
                }
                if(!phone_master) break;
                if(lenh_sms[3] == 'B' || lenh_sms[3] == 'b' || lenh_sms[4] == 'n' || lenh_sms[4] == 'N'){
                    IAP_docxoasector1();
                    eeprom_buf[BAOCAO_EEPROM] = 1;
                    IAP_ghisector1();
                    baocaosms("\rBat bao cao");
                }else if(lenh_sms[3] == 'T' || lenh_sms[3] == 't' || lenh_sms[4] == 'f' || lenh_sms[4] == 'F'){
                    baocaosms("\rTat bao cao");
                    IAP_docxoasector1();
                    eeprom_buf[BAOCAO_EEPROM] = 0;
                    IAP_ghisector1();
                }else baocaosms("\rLenh Khong Hop Le");
                break;
            case 'g':
            case 'G':
                if(!phone_master) break;
                phone[10] = 0;
                gsm_quay_so(phone);
                break;
            case 'C':
            case 'c':
                // Chinh gio thu cong (du phong khi SIM khong lay duoc gio tu mang):
                // <pin>,Cg,hhmmddMMyy  vi du 0000,Cg,1430260826 = 14:30 ngay 26/08/26
                if(lenh_sms[1] == 'g' || lenh_sms[1] == 'G'){
                    if(!phone_master) break;
                    if(lenh_sms[3] >='0' && lenh_sms[3] <='9' && lenh_sms[4] >='0' && lenh_sms[4] <='9'
                    && lenh_sms[5] >='0' && lenh_sms[5] <='9' && lenh_sms[6] >='0' && lenh_sms[6] <='9'
                    && lenh_sms[7] >='0' && lenh_sms[7] <='9' && lenh_sms[8] >='0' && lenh_sms[8] <='9'
                    && lenh_sms[9] >='0' && lenh_sms[9] <='9' && lenh_sms[10]>='0' && lenh_sms[10]<='9'
                    && lenh_sms[11]>='0' && lenh_sms[11]<='9' && lenh_sms[12]>='0' && lenh_sms[12]<='9'){
                        u8 gio_moi    = (lenh_sms[3] -'0')*10 + lenh_sms[4] -'0';
                        u8 phut_moi   = (lenh_sms[5] -'0')*10 + lenh_sms[6] -'0';
                        u8 ngay_moi   = (lenh_sms[7] -'0')*10 + lenh_sms[8] -'0';
                        u8 thang_moi  = (lenh_sms[9] -'0')*10 + lenh_sms[10]-'0';
                        u8 nam_moi    = (lenh_sms[11]-'0')*10 + lenh_sms[12]-'0';
                        if(gio_moi>23 || phut_moi>59 || ngay_moi<1 || ngay_moi>31 || thang_moi<1 || thang_moi>12){
                            baocaosms("\rGio khong hop le");
                        }else{
                            u16 check;
                            hour = gio_moi; minute = phut_moi; second = 0;
                            day = ngay_moi; month = thang_moi; year = nam_moi;
                            check = (23*month/9 + day + (month>2?!(year%4):2) + year + (year+3)/4 + 1);
                            date = check%7+1;
                            gio_hop_le = 1;   // da co gio (thu cong) -> ngung thu lai nhanh
                            baocaosms("\rDa chinh gio");
                        }
                    }else baocaosms("\rGio khong hop le");
                }else baocaosms("\rLenh Khong Hop Le");
                break;
            default:
                    baocaosms("\rLenh khong hop le");

        }
        //reset lenh_sms
        i = 0;
        WATCHDOG;
        while(lenh_sms[i])lenh_sms[i++] = 0;
    }
}
