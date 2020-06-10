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
                if(!phone_master) break;
                baocaolichsu();
                break;
            case 'S':
            case 's':
                if(eep_khoa && !phone_master) break;
                if(eep_khoa){
                    IAP_docxoasector1();
                    eeprom_buf[KHOA_EEPROM] = 0;
                    IAP_ghisector1();
                }
                Relay4 = 0;
                Relay2 = 1;
                delay_ms(100);
                Relay2 = 0;
                baocaosms("\rDung cua cuon");
                break;
            case 'K':
            case 'k':
                if(!phone_master) break;
                IAP_docxoasector1();
                eeprom_buf[KHOA_EEPROM] = 1;
                IAP_ghisector1();
                Relay4 = 1;
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
            // case 'u':
            // case 'U':
            //     if(!phone_master) break;
            //     if(lenh_sms[4] == 'B' || lenh_sms[4] == 'b' || lenh_sms[5] == 'n' || lenh_sms[5] == 'N'){
            //        IAP_docxoasector1();
            //        eeprom_buf[UPS_EEPROM] = 10;
            //        IAP_ghisector1();
            //        Relay4 = 1;
            //        baocaosms("\rBat UPS");
            //     }else if(lenh_sms[4] == 'T' || lenh_sms[4] == 't' || lenh_sms[5] == 'f' || lenh_sms[5] == 'F'){
            //        IAP_docxoasector1();
            //        eeprom_buf[UPS_EEPROM] = 0;
            //        IAP_ghisector1();
            //        Relay4 = 0;
            //        baocaosms("\rTat UPS");
            //     }
            //     break;
            case 'b':
            case 'B':
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