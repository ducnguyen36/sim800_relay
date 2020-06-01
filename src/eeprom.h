

#define		SECTOR1    		    0x0000
#define     MOVC_START_ADDRESS  0xF000

#define        PIN_EEPROM                   SECTOR1 
#define        INDEX_HISTORY_EEPROM         PIN_EEPROM+4 
#define        HISTORY_EEPROM               INDEX_HISTORY_EEPROM+1 


#define     SECTOR1_LENGTH      HISTORY_EEPROM+400

#define pinEEprom                           PIN_EEPROM+MOVC_START_ADDRESS
#define index_historyEEprom                 INDEX_HISTORY_EEPROM+MOVC_START_ADDRESS
#define historyEEprom                       HISTORY_EEPROM+MOVC_START_ADDRESS


__code __at        pinEEprom                    u8 eep_pin[4];
__code __at        index_historyEEprom          u8 eep_index_history;
__code __at        historyEEprom                u8 eep_history[400];


extern u8  __xdata eeprom_buf[SECTOR1_LENGTH];

#define	CMD_IDLE		0
#define	CMD_READ		1
#define	CMD_PROGRAM		2
#define	CMD_ERASE		3

void IAP_ghibyte(u16 diachi,u8 dulieu);
void IAP_ghisector1();
void IAP_docxoasector1();
void IAP_xoasector(u16 sector);

/********** Eeprom *****************/

//#define ENABLE_IAP 0x80 //if SYSCLK<30MHz
//#define ENABLE_IAP 0x81 //if SYSCLK<24MHz
//#define ENABLE_IAP 0x82 //if SYSCLK<20MHz
#define ENABLE_IAP 0x83 //if SYSCLK<12MHz
//#define ENABLE_IAP 0x84 //if SYSCLK<6MHz
//#define ENABLE_IAP 0x85 //if SYSCLK<3MHz
//#define ENABLE_IAP 0x87 //if SYSCLK<1MHz
//#define ENABLE_IAP 0x86 //if SYSCLK<2MHz