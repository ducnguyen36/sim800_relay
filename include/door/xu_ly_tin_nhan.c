void xu_ly_tin_nhan(){
    //lenh DH
    lenh_khong_hop_le = 0;
    u8 i = 0;
    while(lenh_sms[i++])WATCHDOG;
    while(i<160)lenh_sms[i++] = 0;
    i=0;
    if((lenh_sms[0]=='?'||lenh_sms[1]=='?') && !phone_so_sanh_that_bai)gui_huong_dan();
    else{ 
        switch(lenh_sms[0]){
            case 'S':
            case 's':
                if((eep_khoa&1) && !phone_master) break;
                if(eep_khoa&1){
                    IAP_docxoasector1();
                    eeprom_buf[KHOA_EEPROM] &= 2;
                    IAP_ghisector1();
                }
                rfprocess = 1;
                Relay2 = 1;
                relay2giu = 0;
                delay_ms(100);
                rfprocess = Relay2 = 0;
                if(eep_baocao) baocaosms("\rDung cua cuon");
                else gsm_sendandcheck("\032",3,1,"  TAT BAO CAO  ");
                break;
            case 'K':
            case 'k':
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
                    
                }else{
                    IAP_docxoasector1();
                    eeprom_buf[KHOA_EEPROM] |= 1;
                    IAP_ghisector1();
                    Relay2 = 1;relay2giu = 1;
                    phone[10] = 0;
                    baocaosms("\rKhoa cua cuon");
                }
                break;
            case 'R':
            case 'r':
                if(!phone_master) break;
                if(lenh_sms[2] == lenh_sms[4] && (lenh_sms[2] == 'r' || lenh_sms[2] == 'R') && phone_super){
                     baocaosms("\rKhoi tao lai thiet bi");
                    phone_del(0);
					IAP_xoasector(SECTOR2);
					IAP_ghibyte(RFINDEX_EEPROM,0);
					IAP_docxoasector1();
					eeprom_buf[PIN_EEPROM] = eeprom_buf[PIN_EEPROM+1] = eeprom_buf[PIN_EEPROM+2] = eeprom_buf[PIN_EEPROM+3] = '0';
					eeprom_buf[KHOA_EEPROM] = 0;
                    eeprom_buf[HUONG_MOTOR] = 0;
                    eeprom_buf[UPS_EEPROM] = 0;
                    eeprom_buf[BAOCAO_EEPROM] = 1; // mac dinh BAT bao cao
                    IAP_ghisector1();
					have_master = 0;
                }else{
                    baocaosms("\rreset gsm sau 10s");
                    gsm_pw = 0; 
                }
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
                if(lenh_sms[1] == 'o' || lenh_sms[1] == 'O'){
                    if(!phone_master) break;
                    if(lenh_sms[4]>='0' && lenh_sms[4]<='9' && ((lenh_sms[5]>='0' && lenh_sms[5]<='9') || lenh_sms[5] == 0)
                    && ((lenh_sms[6]>='0' && lenh_sms[6]<='9') || lenh_sms[6] == 0)){
                        u8 idx;
                        if(lenh_sms[4]=='0') idx = 0;
                        else if(!lenh_sms[5]) idx = lenh_sms[4]-'0';
                        else if(!lenh_sms[6]) idx = (lenh_sms[4]-'0')*10+lenh_sms[5]-'0';
                        else if(((lenh_sms[5]-'0')*10+lenh_sms[6]-'0'>50) && lenh_sms[4]>1) {baocaosms("\rLenh khong hop le");break;}
                        else idx = (lenh_sms[4]-'0')*100 + (lenh_sms[5]-'0')*10 + lenh_sms[6]-'0';
                        // Xoa,0 = xoa tat ca (giu nguyen). Xoa,N (N>0): khong cho
                        // xoa so co vai tro M/m (master) qua SMS - giong menu.
                        if(idx && idx<=eep_phone_count &&
                        (eep_phone[(idx-1)*PHONE_ENTRY+9]=='M' || eep_phone[(idx-1)*PHONE_ENTRY+9]=='m')){
                            baocaosms("\rKhong the xoa so Master qua SMS");
                            break;
                        }
                        phone_del(idx);
                        baocaosms("\rXoa danh ba thanh cong");
                    }else baocaosms("\rLenh khong hop le");
                }else if(lenh_sms[1] == 'u' || lenh_sms[1] == 'U'){
                    if(relay2giu){
                        baocaosms("\rCua cuon dang khoa");
                        break;
                    }
                    rfprocess = 1;
                    if(eep_huong){
                        Relay1 = 1;
                        delay_ms(100);
                        rfprocess =  Relay1 = 0;
                    }else{
                        Relay3 = 1;
                        delay_ms(100);
                        rfprocess =  Relay3 = 0;
                    }
                    phone[10] = 0;
                    if(eep_baocao) baocaosms("\rXuong cua cuon");
                    else gsm_sendandcheck("\032",3,1,"  TAT BAO CAO  ");
                }else{
                    baocaosms("\rLenh Khong Hop Le");
                }
                
                break;
            case 'L':
            case 'l':
                if(lenh_sms[1] == 'u' || lenh_sms[1] == 'U'){
                    if(!phone_master) break;
                    if(lenh_sms[4] >='0' && lenh_sms[4] <='9' && lenh_sms[5] >='0' && lenh_sms[5] <='9' && lenh_sms[6] >='0' && lenh_sms[6] <='9' && lenh_sms[7] >='0' && lenh_sms[7] <='9'
                    && lenh_sms[8] >='0' && lenh_sms[8] <='9' && lenh_sms[9] >='0' && lenh_sms[9] <='9' && lenh_sms[10]>='0' && lenh_sms[10]<='9' && lenh_sms[11]>='0' && lenh_sms[11]<='9'    
                    && lenh_sms[12]>='0' && lenh_sms[12]<='9' && lenh_sms[13]>='0' && lenh_sms[13]<='9'){
                        lenh_sms[14] = 0;
                        // luu 9 chu so (bo so 0 dau) + vai tro; 'm' = master, con lai = user
                        if(phone_find(lenh_sms+5))
                            baocaosms("\rSo nay da co trong danh ba");
                        else if(phone_add(lenh_sms+5,(lenh_sms[15]=='m')?'m':'u'))
                            baocaosms("\rthem danh ba thanh cong");
                        else baocaosms("\rBang so da day");

                    }else baocaosms("\rSo dt khong hop Le");
                }else if(lenh_sms[1] == 'e' || lenh_sms[1] == 'E'){
                    if(relay2giu){
                        baocaosms("\rCua cuon dang khoa");
                        break;
                    }
                    rfprocess = 1;
                    // Mo cua: dung relay nguoc voi lenh xuong (Relay3 khi huong=1, Relay1 khi =0)
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
                    if(eep_baocao) baocaosms("\rMo cua cuon");
                    else gsm_sendandcheck("\032",3,1,"  TAT BAO CAO  ");
                }else{
                    baocaosms("\rLenh Khong Hop Le");
                }
               
                break;
            case 'u':
            case 'U':
                if(!phone_master) break;
                if(lenh_sms[4] == 'B' || lenh_sms[4] == 'b' || lenh_sms[5] == 'n' || lenh_sms[5] == 'N'){
                   IAP_docxoasector1();
                   eeprom_buf[UPS_EEPROM] = 10;
                   IAP_ghisector1();
                   Relay4 = 1;
                   baocaosms("\rBat UPS");
                }else if(lenh_sms[4] == 'T' || lenh_sms[4] == 't' || lenh_sms[5] == 'f' || lenh_sms[5] == 'F'){
                   IAP_docxoasector1();
                   eeprom_buf[UPS_EEPROM] = 0;
                   IAP_ghisector1();
                   Relay4 = 0;
                   baocaosms("\rTat UPS");
                }else{
                    baocaosms("\rLenh Khong Hop Le");
                }
                break;
            case 'b':
            case 'B':
                if(!phone_master) break;
                if(!lenh_sms[3] || !lenh_sms[2]){
                    baocaosms("\rBao cao thanh cong");
                }else if(lenh_sms[3] == 'B' || lenh_sms[3] == 'b' || lenh_sms[4] == 'n' || lenh_sms[4] == 'N'){
                    IAP_docxoasector1();
                    eeprom_buf[BAOCAO_EEPROM] = 1;
                    IAP_ghisector1();
                    baocaosms("\rBat bao cao");
                 }else if(lenh_sms[3] == 'T' || lenh_sms[3] == 't' || lenh_sms[4] == 'f' || lenh_sms[4] == 'F'){
                    baocaosms("\rTat bao cao");
                    IAP_docxoasector1();
                    eeprom_buf[BAOCAO_EEPROM] = 0;
                    IAP_ghisector1();
                 }else{
                    baocaosms("\rLenh Khong Hop Le");
                }
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
                            baocaosms("\rDa chinh gio");
                        }
                    }else baocaosms("\rGio khong hop le");
                }else{
                    baocaosms("\rLenh Khong Hop Le");
                }
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