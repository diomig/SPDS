/*
 *  Copyright 2010 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

#define AIC3204_I2C_ADDR 0x18
#include "usbstk5515.h"
#include "usbstk5515_gpio.h"
#include "usbstk5515_i2c.h"
#include "stdio.h"
#include "usbstk5515.h"
extern Int16 AIC3204_rset( Uint16 regnum, Uint16 regval);
#define Rcv 0x08
#define Xmit 0x20


#define DELTA0 0x4000
#define DELTA_MIN 0x2000
#define DELTA_MAX 0x6000

#define KB 2391
#define a1_IIR_2kHz 21479
#define a1_IIR_4kHz 0
#define a1_IIR_6kHz -21479

#define alpha_fc_10 32640 //0.99608837  (Q15)
#define alpha_fc_100 31530 //0.962213946  (Q15)
typedef Int16 fixed16;




Int16 yn=0, yn1=0, yn2=0;
Int16 xn=0, xn1=0, xn2=0;
Int16 e;
Int16 i, f;
Int16 gain;
Int16 y, y1, y2;
short ramp;
Int16 Kf, ef;



Int16 iir_filter();
Int16 loop_filter(Int16 xD, Int16 e);
Int16 nco(Int16 delta);
Int16 phase_detector(Int16 x, Int16 xo);

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  main( )                                                                 *
 *                                                                          *
 * ------------------------------------------------------------------------ */
void main( void )
{
    /* Initialize BSL */
    USBSTK5515_init( );

    /* Configure AIC3204 */

       AIC3204_rset( 0, 0 );          // Select page 0
       AIC3204_rset( 1, 1 );          // Reset codec
       AIC3204_rset( 0, 1 );          // Select page 1
       AIC3204_rset( 1, 8 );          // Disable crude AVDD generation from DVDD
       AIC3204_rset( 2, 1 );          // Enable Analog Blocks, use LDO power
       AIC3204_rset( 0, 0 );          // Select page 0
       /* PLL and Clocks config and Power Up  */
       AIC3204_rset( 27, 0x0d );      // BCLK and WCLK is set as o/p to AIC3204(Master)
       AIC3204_rset( 28, 0x00 );      // Data ofset = 0
       AIC3204_rset( 4, 3 );          // PLL setting: PLLCLK <- MCLK, CODEC_CLKIN <-PLL CLK
       AIC3204_rset( 6, 7 );          // PLL setting: J=7
       AIC3204_rset( 7, 0x06 );       // PLL setting: HI_BYTE(D=1680)
       AIC3204_rset( 8, 0x90 );       // PLL setting: LO_BYTE(D=1680)
       AIC3204_rset( 30, 0x88 );      // For 32 bit clocks per frame in Master mode ONLY
                                      // BCLK=DAC_CLK/N =(12288000/8) = 1.536MHz = 32*fs
       AIC3204_rset( 5, 0x91 );       // PLL setting: Power up PLL, P=1 and R=1
       AIC3204_rset( 13, 0 );         // Hi_Byte(DOSR) for DOSR = 128 decimal or 0x0080 DAC oversamppling
       AIC3204_rset( 14, 0x80 );      // Lo_Byte(DOSR) for DOSR = 128 decimal or 0x0080
       AIC3204_rset( 20, 0x80 );      // AOSR for AOSR = 128 decimal or 0x0080 for decimation filters 1 to 6
       AIC3204_rset( 11, 0x86 );      // Power up NDAC and set NDAC value to 6 (fs=16kHz)
       AIC3204_rset( 12, 0x87 );      // Power up MDAC and set MDAC value to 7
       AIC3204_rset( 18, 0x87 );      // Power up NADC and set NADC value to 7
       AIC3204_rset( 19, 0x86 );      // Power up MADC and set MADC value to 6 (fs=16kHz)
       /* DAC ROUTING and Power Up */
       AIC3204_rset(  0, 0x01 );      // Select page 1
       AIC3204_rset( 12, 0x08 );      // LDAC AFIR routed to HPL
       AIC3204_rset( 13, 0x08 );      // RDAC AFIR routed to HPR
       AIC3204_rset(  0, 0x00 );      // Select page 0
       AIC3204_rset( 64, 0x02 );      // Left vol=right vol
       AIC3204_rset( 65, 0x00 );      // Left DAC gain to 0dB VOL; Right tracks Left
       AIC3204_rset( 63, 0xd4 );      // Power up left,right data paths and set channel
       AIC3204_rset(  0, 0x01 );      // Select page 1
       AIC3204_rset( 16, 0x00 );      // Unmute HPL , 0dB gain
       AIC3204_rset( 17, 0x00 );      // Unmute HPR , 0dB gain
       AIC3204_rset(  9, 0x30 );      // Power up HPL,HPR
       AIC3204_rset(  0, 0x00 );      // Select page 0
       USBSTK5515_wait( 500 );        // Wait

       /* ADC ROUTING and Power Up */
       AIC3204_rset( 0, 1 );          // Select page 1
       AIC3204_rset( 0x34, 0x30 );    // STEREO 1 Jack
                                   // IN2_L to LADC_P through 40 kohm
       AIC3204_rset( 0x37, 0x30 );    // IN2_R to RADC_P through 40 kohmm
       AIC3204_rset( 0x36, 3 );       // CM_1 (common mode) to LADC_M through 40 kohm
       AIC3204_rset( 0x39, 0xc0 );    // CM_1 (common mode) to RADC_M through 40 kohm
       AIC3204_rset( 0x3b, 0x18 );       // MIC_PGA_L unmute
       AIC3204_rset( 0x3c, 0x18 );       // MIC_PGA_R unmute
       AIC3204_rset( 0, 0 );          // Select page 0
       AIC3204_rset( 0x51, 0xc0 );    // Powerup Left and Right ADC
       AIC3204_rset( 0x52, 0 );       // Unmute Left and Right ADC

       AIC3204_rset( 0, 0 );
       USBSTK5515_wait( 200 );        // Wait
       /* I2S settings */
       I2S0_SRGR = 0x0;
       I2S0_CR = 0x8010;    // 16-bit word, slave, enable I2C
       I2S0_ICMR = 0x3f;    // Enable interrupts




       Int16 DataInLeft, DataInRight;
       Int16 DataOutLeft, DataOutRight;


       Int16 xD, xo;

       while(1) {
                    /* Read Digital audio */
                    while((Rcv & I2S0_IR) == 0);  // Wait for interrupt pending flag
                    DataInRight = I2S0_W0_MSW_R;  // 16 bit right channel received audio data
                    DataInLeft = I2S0_W1_MSW_R;  // 16 bit left channel received audio data
                    /* Write Digital audio */
                    while((Xmit & I2S0_IR) == 0);  // Wait for interrupt pending flag
                    I2S0_W0_MSW_W = DataOutRight;  // 16 bit right channel transmit audio data
                    I2S0_W1_MSW_W = DataOutLeft;  // 16 bit left channel transmit audio data

//--------------------------------------------------------------------------------------------------------------------
 // Your program here!!!
//--------------------------------------------------------------------------------------------------------------------

                    xn = DataInRight;


                    xo = nco(e);
                    xD = phase_detector(xn, xo);

                    e=loop_filter(xD, e);




                    DataOutRight = yn;
                    //ef = DataInRight;
                    //Kf = (((long)ef * (-25736))<<1)>>16; //Q14

                    //yn = iir_filter();
//--------------------------------------------------------------------------------------------------------------------
// Your program here!!!
//--------------------------------------------------------------------------------------------------------------------

  }

 } // main()



Int16 iir_filter() {
    Int16 a1, a2=-27987, b0=KB, b2=-KB;

    a1 = a1_IIR_4kHz;


    yn = (((((long)a1*yn1)<<1) + (long)a2*yn2 + (long)b0*xn + (long)b2*xn2)<<1)>>16;
    xn2 = xn1;
    xn1 = xn;
    yn2 = yn1;
    yn1 = yn;

    return yn;

}


Int16 loop_filter(Int16 xD, Int16 e) { //receive previous value of e (e[n-1]) and current value of xD
    Int16 alpha = alpha_fc_100;
    Int16 aux = 0x7FFF-alpha;  //1-alpha in Q15

    return ((((long)alpha*e +(long)xD*(aux))<<1)>>16);
}



Int16 nco(Int16 delta){
    Int16 lut[32] = {0,3212,6393,9512,12539,15446,18204,20787,23170,25329,27245,28898,30273,31356,32137,32609,32767,32609,32137,31356,30273,28898,27245,25329,23170,20787,18204,15446,12539,9512,6393,3212};
    Int16 y, y1, y2;
    ramp += delta;

    i = (ramp>>10) & 0x001F;
    f = (ramp<<5)  & 0x7FFF;
    gain = 0x4000; // 16384   ---> 1/2  in Q15
    //DataOutRight = (((long)lut[i] * gain)<<1)>>16; // * gain;

    y1 = lut[i];
    y2 = lut[i+1];

    y = (((long)(y2-y1) * f)<<1)>>16;
    y = y1 + y;

    if (ramp < 0){
        y = -y;
    }


    return y;
}



Int16 phase_detector(Int16 x, Int16 xo){
    return ((((long)x*xo)<<1)>>16);
}



