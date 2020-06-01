

void LCD_guichuoi(u8 *vanban) __reentrant;
void LCD_guilenh(u8 lenh);
void LCD_guidulieu(u8 dulieu);
void LCD_Init();
void LCD_guigio(u8 vitri, u8 *chuoi, u8 gio, u8 phut, u8 giay,__bit haicham);
void LCD_guingay(u8 vitri, u8 nam, u8 thang, u8 ngay);
void delay_ms(unsigned int t);
void delay_us(unsigned int t);
void LCD_xoa(u8 hang);
void LCD_chop(u8 hang,u8 *vanban);
void LCD_blinkXY(u8 hang, u8 cot);
void LCD_noblink();

extern __bit chop;

#define     TREN        0x80
#define     DUOI        0xc0
#define     TATCA       0


/************ LCD ******************/
#define		lcd_pw		P06
#define		LCD_rs		P05
#define		LCD_en		P04

#define		LCD_D4		P03
#define		LCD_D5		P02
#define		LCD_D6		P01
#define		LCD_D7		P00

#define		PORT_LCD 	P0
