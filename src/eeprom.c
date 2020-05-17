#include "true.h"
#include "common.h"
#include "eeprom.h"

void IAP_cho()
{
    IAP_CONTR = 0; //Close IAP function
    IAP_CMD = CMD_IDLE; //Clear command to standby
    IAP_TRIG = 0; //Clear trigger register
    IAP_ADDRH = 0x80; //Data ptr point to non-EEPROM area
    IAP_ADDRL = 0; //Clear IAP address to prevent misuse
    WATCHDOG;
}

u8 IAP_docbyte(u16 diachi){
    u8 dulieu;
    IAP_CONTR = ENABLE_IAP;
    IAP_CMD = CMD_READ;
    IAP_ADDRL = diachi;
    IAP_ADDRH = diachi>>8;
    IAP_TRIG = 0x5a;
    IAP_TRIG = 0xa5;
    _nop_();
    dulieu = IAP_DATA;
    IAP_cho();
    return dulieu;
}

void IAP_ghibyte(u16 diachi,u8 dulieu){
    IAP_CONTR = ENABLE_IAP;
    IAP_CMD = CMD_PROGRAM;
    IAP_ADDRL = diachi;
    IAP_ADDRH = diachi>>8;
    IAP_DATA = dulieu;
    IAP_TRIG = 0x5a;
    IAP_TRIG = 0xa5;
    _nop_();
    IAP_cho();
}

void IAP_xoasector(u16 sector){
    IAP_CONTR = ENABLE_IAP;
    IAP_CMD = CMD_ERASE;
    IAP_ADDRL = sector;
    IAP_ADDRH = sector>>8;
    IAP_TRIG = 0x5a;
    IAP_TRIG = 0xa5;
    _nop_();
    IAP_cho();

}

void IAP_docxoasector1(){
    //4 mt 4 rs 2 charge 48 lon 48 lof 1 ll 48 mp3 48 song 1ml 1ismp3
    u8 __data i=SECTOR1_LENGTH;
    WATCHDOG;
    while(i) eeprom_buf[--i] = 0xff;
    
    while(i<SECTOR1_LENGTH) eeprom_buf[i] = *(eep_pin+i++);
    // while(i<SECTOR1_LENGTH) eeprom_buf[i++] = *(&eep_motor+i);
    IAP_xoasector(SECTOR1);
}

void IAP_ghisector1(){
    u8 __data i=0;
    while(i<SECTOR1_LENGTH){
        IAP_CONTR = ENABLE_IAP;
        IAP_CMD = CMD_PROGRAM;
        IAP_ADDRL = i;
        IAP_ADDRH = 0;
        IAP_DATA = eeprom_buf[i++];
        IAP_TRIG = 0x5a;
        IAP_TRIG = 0xa5;
        _nop_();
        IAP_cho();
    }
}