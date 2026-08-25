#include "true.h"
#include "common.h"
#include "lcd.h"
void delay_us(unsigned int t)
{
        unsigned int i;
		for(i=0;i<t;i++){WATCHDOG;};
}

void delay_ms(unsigned int t)
{
        unsigned int i;
		for (i = 0; i < t; i++)delay_us(1000);
			
}

void LCD_blinkXY(u8 hang, u8 cot){
	LCD_guilenh(hang|cot);
	LCD_guilenh(0x0d);
	
}
void LCD_chop(u8 hang,u8 *vanban){
	if(chop){
		LCD_guilenh(hang);
		LCD_guichuoi(vanban);
	}else LCD_xoa(hang);
}
void LCD_guidulieu(u8 dulieu){
	if(!dulieu)return;
	LCD_rs=1;
	LCD_D4=(dulieu>>4)&1;
	LCD_D5=(dulieu>>5)&1;
	LCD_D6=(dulieu>>6)&1;
	LCD_D7=(dulieu>>7)&1;
	LCD_en =1;
	_nop_();
	LCD_en=0;
	_nop_();
	LCD_D4=dulieu 	  &1;
	LCD_D5=(dulieu>>1)&1;
	LCD_D6=(dulieu>>2)&1;
	LCD_D7=(dulieu>>3)&1;
	LCD_en =1;
	_nop_();
	LCD_en=0;
	delay_us(100);
	LCD_rs=0;
}

void LCD_guilenh(u8 lenh){
	if(!lenh)return;
	LCD_rs=0;
	LCD_D4=(lenh>>4)&1;
	LCD_D5=(lenh>>5)&1;
	LCD_D6=(lenh>>6)&1;
	LCD_D7=(lenh>>7)&1;
	LCD_en =1;
	_nop_();
	LCD_en=0;
	_nop_();
	LCD_D4=lenh 	  &1;
	LCD_D5=(lenh>>1)&1;
	LCD_D6=(lenh>>2)&1;
	LCD_D7=(lenh>>3)&1;
	LCD_en =1;
	_nop_();
	LCD_en=0;
	delay_us(100);
}

void LCD_noblink(){
	LCD_guilenh(0x0c);
}

void LCD_xoa(u8 hang){
	if(hang){
		LCD_guilenh(hang);
		LCD_guichuoi("                ");
	}else{
		LCD_guilenh(0x01);
		delay_us(1600);
	}			  
}


/**************Ctr giao tiep LCD 16x2 4bit**********************/





void LCD_guichuoi(u8 *vanban) __reentrant {
	u8 i=0;
	while (*vanban && i<16)
		if(*vanban<128 && *vanban>31 && ++i)LCD_guidulieu(*vanban++);
		else LCD_guilenh(*vanban++);
}

void LCD_Init(){
	lcd_pw = 0;delay_us(15000);
	P0=0x1c;_nop_();
	LCD_en=0;delay_us(4000);
	LCD_en=1;_nop_();
	LCD_en=0;delay_us(160);
	LCD_en=1;_nop_();
	LCD_en=0;delay_us(160);
	P0=0x14;_nop_();
	LCD_en=0;delay_us(1000);
	LCD_en=1;_nop_();
	LCD_en=0;_nop_();
	P0=0x11;_nop_();
	LCD_en=0;delay_us(40);
	P0=0x10;_nop_();
	LCD_en=0;_nop_();
	P0=0x13;_nop_();
	LCD_en=0;delay_us(40);
	P0=0x10;_nop_();
	LCD_en=0;_nop_();
	P0=0x16;_nop_();
	LCD_en=0;delay_us(40);
	P0=0x10;_nop_();
	LCD_en=0;_nop_();
	P0=0x18;_nop_();
	LCD_en=0;delay_us(1000);
	
}

void LCD_guigio(u8 vitri, u8 *chuoi, u8 gio, u8 phut, u8 giay,__bit haicham){
	if(gio>23) return;
	if(giay>250){
		LCD_guilenh(vitri);
		LCD_guichuoi(chuoi);
		LCD_guidulieu(gio<10?' ':(gio/10+'0'));
		LCD_guidulieu(gio%10+'0');
		if(giay>252) LCD_guidulieu(haicham?':':' ');
		LCD_guidulieu(phut/10+'0');
		LCD_guidulieu(phut%10+'0');
	}else{
		LCD_guilenh(vitri);
		LCD_guichuoi(chuoi);
		LCD_guidulieu(gio<10?' ':(gio/10+'0'));
		LCD_guidulieu(gio%10+'0');
		LCD_guidulieu(haicham?':':' ');
		LCD_guidulieu(phut/10+'0');
		LCD_guidulieu(phut%10+'0');
		LCD_guidulieu(haicham?':':' ');
		LCD_guidulieu(giay/10+'0');
		LCD_guidulieu(giay%10+'0');
		LCD_guidulieu(' ');
	}
	
}

void LCD_guingay(u8 vitri, u8 nam, u8 thang, u8 ngay){
	LCD_guilenh(vitri);
	LCD_guidulieu(ngay/10+'0');
	LCD_guidulieu(ngay%10+'0');
	LCD_guidulieu(thang/10+'0');
	LCD_guidulieu(thang%10+'0');
	LCD_guidulieu(nam/10+'0');
	LCD_guidulieu(nam%10+'0');
	LCD_guidulieu(' ');
}