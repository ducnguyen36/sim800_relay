#include "true.h"
#include "common.h"

/*********** Sim 900 ***************/
#define		gsm_pw			P20
// Baurate general = Timer 1
#define		gsm_RI			RI
#define		gsm_TI			TI
#define		gsm_SBUF		SBUF

#define		gsm_BAUD		38400

void gsm_init();
void send_gsm_byte(u8 dulieu);
void send_gsm_cmd(u8 *cmd);

extern u8 __xdata    gsm_serial_cmd;

#define     PHU         0
#define     CHINH       1

#define     NORMAL      0
#define     CFUN        1
#define     CLK         2
#define     PHONE       3
#define     PIN         4
#define     CMD         5
#define     CFUN1       6
#define     CUSD        7
#define     CPBR        8
#define     CMGS        9
#define     CUS2        10

#define     CHOXULY     1
#define     DANGXULY    2