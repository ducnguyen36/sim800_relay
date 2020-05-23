#ifndef TRUE_H
#define TRUE_H
#include "stc15.h"

/*--------Khong duoc phep thay doi-------*/
#define		FOSC		12000000L

#define		PORT_KEY	P3
#define		PORT_OUT 	P2



/*********** Keys ***************/
#define		phim_mode_vao		P35
#define		phim_back_vao		P33
#define		phim_cong_vao		P34

/************ CAMS ******************/
#define		tinhieuA	    P37
#define		tinhieuD     	P36

/*********** DALAS ***************/
#define		clock_in	P32


/************ MOTOR STATUS ******************/
#define		motor_run_sign	P41

/************ LED ******************/
#define		over_cur_led	P07



/*********** MOTORS DC ***************/
// Timer 0 = motor control
#define		motorS2	  P43
#define		motorS1   P44

/*********** MOTORS DRIVER ***************/
#define     motor1    P27
#define     motor3    P26
#define     motor2    P25
#define     motor4    P24

#define     RelayS1    P27
#define     RelayS2    P26
#define     RelayS3    P25
#define     RelayS4    P24


#define	cam_sign_out	cam_out1
#define	zero_sign_out	cam_out2

/* RELAY */
#define		Relay1  		P23
#define		Relay2      	P22
#define		Relay3  		P21
#define		Relay4  		P42

#endif
