
#define		SECTOR1    		    0x0000
#define     SECTOR2             0x0200
#define     MOVC_START_ADDRESS  0xF000

#define        PIN_EEPROM                   SECTOR1 
#define        BAOCAO_EEPROM                PIN_EEPROM+4
#define        HUONG_MOTOR                  BAOCAO_EEPROM+1
#define        KHOA_EEPROM                  HUONG_MOTOR+1 
#define        UPS_EEPROM                   KHOA_EEPROM+1
#define        PHONE_COUNT_EEPROM           UPS_EEPROM+1
#define        PHONE_TABLE_EEPROM           PHONE_COUNT_EEPROM+1

#define        PHONE_MAX           20
#define        PHONE_ENTRY         10

#define        RFDATA_EEPROM                SECTOR2
#define        RFINDEX_EEPROM               RFDATA_EEPROM+300
#define        RFLOCK_EEPROM                RFINDEX_EEPROM+1

#define     SECTOR1_LENGTH      PHONE_TABLE_EEPROM+(PHONE_MAX*PHONE_ENTRY)
#define     SECTOR2_LENGTH      RFLOCK_EEPROM+1-SECTOR2
/* Scratch buffer must hold the larger of the two sectors (RF sector ops
   reuse eeprom_buf up to SECTOR2_LENGTH). */
#define     EEPROM_BUF_LENGTH   (SECTOR1_LENGTH>SECTOR2_LENGTH?SECTOR1_LENGTH:SECTOR2_LENGTH)




#define pinEEprom                           PIN_EEPROM+MOVC_START_ADDRESS
#define baocaoEEprom                        BAOCAO_EEPROM+MOVC_START_ADDRESS
#define huongEEprom                         HUONG_MOTOR+MOVC_START_ADDRESS
#define khoaEEprom                          KHOA_EEPROM+MOVC_START_ADDRESS
#define upsEEprom                           UPS_EEPROM+MOVC_START_ADDRESS
#define phone_countEEprom                   PHONE_COUNT_EEPROM+MOVC_START_ADDRESS
#define phone_tableEEprom                   PHONE_TABLE_EEPROM+MOVC_START_ADDRESS

#define rfdataEEprom                        RFDATA_EEPROM+MOVC_START_ADDRESS
#define rfindexEEprom                       RFINDEX_EEPROM+MOVC_START_ADDRESS
#define rflockEEprom                        RFLOCK_EEPROM+MOVC_START_ADDRESS


__code __at        pinEEprom                    u8 eep_pin[4];
__code __at        baocaoEEprom                 u8 eep_baocao;
__code __at        huongEEprom                  u8 eep_huong;
__code __at        khoaEEprom                   u8 eep_khoa;
__code __at        upsEEprom                    u8 eep_ups;
__code __at        phone_countEEprom            u8 eep_phone_count;
__code __at        phone_tableEEprom            u8 eep_phone[PHONE_MAX*PHONE_ENTRY];

__code __at        rfdataEEprom                 u8 eep_rfdata[300];
__code __at        rfindexEEprom                u8 eep_rfindex;
__code __at        rflockEEprom                 u8 eep_rflock;


extern u8  __xdata eeprom_buf[EEPROM_BUF_LENGTH];

#define	CMD_IDLE		0
#define	CMD_READ		1
#define	CMD_PROGRAM		2
#define	CMD_ERASE		3

void IAP_ghibyte(u16 diachi,u8 dulieu);
void IAP_ghisector1();
void IAP_docxoasector1();
void IAP_ghisector2();
void IAP_docxoasector2();
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