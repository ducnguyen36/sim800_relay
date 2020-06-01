void xu_ly_tin_nhan(){
    //lenh DH
    lenh_khong_hop_le = 0;
    // u8 giodelta,phutdelta, kim_index;
    u8 i = 0;
    while(lenh_sms[i++])WATCHDOG;
    while(i<160)lenh_sms[i++] = 0;
    i=0;
    if((lenh_sms[0]=='?'||lenh_sms[1]=='?') && !phone_so_sanh_that_bai)gui_huong_dan();
    else{ 
        switch(lenh_sms[0]){
            case 'h':
            case 'H':
                baocaolichsu();
                break;
            case 'S':
            case 's':
                Relay2 = 1;
                delay_ms(100);
                Relay2 = 0;
                baocaosms("\rDung cua cuon");
                break;
            case 'K':
            case 'k':
                Relay2 = 1;
                phone[10] = 0;
                luu_lich_su(phone,3);
                baocaosms("\rKhoa cua cuon");
                break;
            case 'R':
            case 'r':
                baocaosms("\rreset gsm sau10s");
                gsm_pw = 0; 
                break;
            case 'P':
            case 'p':
                if(!phone_master) break;
                if(lenh_sms[4]>='0' && lenh_sms[4]<='9' && lenh_sms[5]>='0' && lenh_sms[5]<='9'
                && lenh_sms[6]>='0' && lenh_sms[6]<='9' && lenh_sms[7]>='0' && lenh_sms[7]<='9'){
                    IAP_xoasector(SECTOR1);
                    IAP_ghibyte(PIN_EEPROM  ,lenh_sms[4]);
                    IAP_ghibyte(PIN_EEPROM+1,lenh_sms[5]);
                    IAP_ghibyte(PIN_EEPROM+2,lenh_sms[6]);
                    IAP_ghibyte(PIN_EEPROM+3,lenh_sms[7]);
                    baocaosms("\rDoi ma pin thanh cong");
                }else baocaosms("\rLenh khong hop le");
                break;
            case 'D':
            case 'd':
                if(!phone_master) break;
                baocaodanhba();
                break;
            case 'X':
            case 'x':
                if(lenh_sms[1] == 'o' || lenh_sms[1] == 'O'){
                    if(!phone_master) break;
                    if(lenh_sms[4]>='0' && lenh_sms[4]<='9' && ((lenh_sms[5]>='0' && lenh_sms[5]<='9') || lenh_sms[5] == 0)
                    && ((lenh_sms[6]>='0' && lenh_sms[6]<='9') || lenh_sms[6] == 0)){
                        if(lenh_sms[4]=='0') xoadanhba(0);
                        else if(!lenh_sms[5]) xoadanhba(lenh_sms[4]-'0');
                        else if(!lenh_sms[6]) xoadanhba((lenh_sms[4]-'0')*10+lenh_sms[5]-'0');
                        else if(((lenh_sms[5]-'0')*10+lenh_sms[6]-'0'>50) && lenh_sms[4]>1) {baocaosms("\rLenh khong hop le");break;}
                        else xoadanhba((lenh_sms[4]-'0')*100 + (lenh_sms[5]-'0')*10 + lenh_sms[6]-'0'); 
                        baocaosms("\rXoa danh ba thanh cong");
                    }else baocaosms("\rLenh khong hop le");
                }else if(lenh_sms[1] == 'u' || lenh_sms[1] == 'U'){
                    Relay3 = 1;
                    delay_ms(100);
                    Relay3 = 0;
                    phone[10] = 0;
                    luu_lich_su(phone,2);
                    baocaosms("\rXuong cua cuon");
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
                        gsm_themdanhba(lenh_sms+4,(lenh_sms[15]!='m' && lenh_sms[15]!= 't')?'u':lenh_sms[15]);
                        baocaosms("\rthem danh ba thanh cong");
                        
                    }else baocaosms("\rSo dt khong hop le");
                }else if(lenh_sms[1] == 'e' || lenh_sms[1] == 'E'){
                    Relay1 = 1;
                    delay_ms(100);
                    Relay1 = 0;
                    phone[10] = 0;
                    luu_lich_su(phone,1);
                    baocaosms("\rMo cua cuon");
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