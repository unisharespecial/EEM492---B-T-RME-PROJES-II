// R1 RX2, R4 TX2
// RHT04 PINS: 1 VCC, 2 DATA, 3 NC, 4 GND

// LCDIRS

#define PIC_ID 9
#define RX_1 0			// UART
/*

// 32MHz if (bit_time > 30)
// 8Mhz  if (bit_time > 10)
// 4Mhz  if (bit_time > 5)
// 2Mhz  if (bit_time > 5)

// 3.67mA 32MHz
// 1.75mA 8MHz
// 1.37mA 4MHz

SEG13	1F 1E
SEG17	1A 1G 1D
SEG16	2F 2E
SEG12	2A 2G 2D
SEG37	3F 3E
SEG36	3A 3G 3D
SEG29	4F 4E
SEG11	4A 4G 4D
SEG10	4B 4C
SEG09	3B 3C 3P
SEG08	2B 2C 2P
SEG30	1B 1C 1P

SEG42	1f 1e
SEG45	1a 1g 1d
SEG26	2f 2e
SEG25	2a 2g 2d
SEG24	3f 3e
SEG23	3a 3g 3d
SEG22	4f 4e
SEG21	4a 4g 4d
SEG20	4b 4c
SEG19	3b 3c 3p
SEG41	2b 2c 2p
SEG35	1b 1c 1p

****************************

SEG08		2B 2C 2P
SEG09		3B 3C 3P
SEG10		4B 4C
SEG11		4A 4G 4D
SEG12		2A 2G 2D
SEG13		1F 1E
SEG16		2F 2E
SEG17		1A 1G 1D
SEG19		3b 3c 3p
SEG20		4b 4c
SEG21		4a 4g 4d
SEG22		4f 4e
SEG23		3a 3g 3d
SEG24		3f 3e
SEG25		2a 2g 2d
SEG26		2f 2e
SEG29		4F 4E
SEG30		1B 1C 1P
SEG35		1b 1c 1p
SEG36		3A 3G 3D
SEG37		3F 3E
SEG41		2b 2c 2p
SEG42		1f 1e
SEG45		1a 1g 1d

*/

// 1 -> yesil
// 2 -> sari
// 3 -> kahve
// 4 -> beyaz

#include <pic.h>
#include <stdlib.h>
#include <time.h>
#include <string.H>

#define _XTAL_FREQ 	4000000UL


#define OPTION_REG_INIT          0b10000101
// __CONFIG(0x3FE4);  // 11 1111 1110 0100 eski
// __CONFIG(0x3F7C);  // 11 1111 0111 1100 CP
__CONFIG(0x3FFC);  // 11 1111 0111 1100 NOCP

// CONFIGURATION WORD 1
// FCMEN    -> 1 = Fail-Safe Clock Monitor is enabled
// IESO     -> 1 = Internal/External Switchover mode is enabled
// CLKOUTEN -> 1 = CLKOUT function is disabled. I/O or oscillator function on RA6/CLKOUT
// BOREN<1:0> 11 = BOR enabled
// CPD      -> 1 = Data memory code protection is disabled
// 		CP       -> 0 = Program memory code protection is enabled
// MCLRE    -> If LVP bit = 1, This bit is ignored.
// MCLRE    -> If LVP bit = 0, 1 =RE3/MCLR/VPP pin function is MCLR; Weak pull-up enabled.
// PWRTE    -> 1 = PWRT disabled
// 		WDTE<1:0>  11 = WDT enabled
// FOSC<2:0> 100 = INTOSC oscillator: I/O function on RA7/OSC1/CLKIN

__CONFIG(0x1EFF);  // 01 1110 1111 1111

// CONFIGURATION WORD 2
// LVP -> 0 = High-voltage on MCLR/VPP must be used for programming
// DEBUG -> 1 = In-Circuit Debugger disabled, RB6/ICSPCLK and RB7/ICSPDAT are general purpose I/O pins
// bit 11 -> Unimplemented: Read as 1
// BORV -> 1 = Brown-out Reset voltage set to 1.9V
// STVREN -> 1 = Stack Overflow or Underflow will cause a Reset
// PLLEN -> 0 = 4xPLL disabled
// bit 7-6 Unimplemented: Read as 11
// VCAPEN<1:0> 11 = No capacitor on VCAP pin
// bit 3-2 Unimplemented: Read as 11
// WRT<1:0> 11 = Write protection off

typedef enum _BOOL { FALSE = 0, TRUE } BOOL;    /* Undefined size */
typedef enum _BIT { CLEAR = 0, SET } BIT;

typedef signed int          INT;
typedef signed char         INT8;
typedef signed short int    INT16;
typedef signed long int     INT32;

typedef unsigned int        UINT;
typedef unsigned char       UINT8;
typedef unsigned short int  UINT16;

void    rtcc_init(void);
void    rtcc_handler(void);

UINT8 eksi, komut_isle, nemi_oku;
UINT8 bekleme = 0;
static bit icon;

char display_int(int t);
char display_int2(int t);
volatile char   blink;
time_t      	time_out;

/*

 AAAAAA
F      B
F      B
F      B
 GGGGGG
E      C
E      C
E      C
 DDDDDD  DP
 
*/
#define SEG_A   0x01
#define SEG_B   0x02
#define SEG_C   0x04
#define SEG_D   0x08
#define SEG_E   0x10
#define SEG_F   0x20
#define SEG_G   0x40
#define SEG_DP  0x80

#define DIGIT_0 (SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F)
#define DIGIT_1 (SEG_B | SEG_C)
#define DIGIT_2 (SEG_A | SEG_B | SEG_G | SEG_E | SEG_D)
#define DIGIT_3 (SEG_A | SEG_B | SEG_C | SEG_G | SEG_D)
#define DIGIT_4 (SEG_F | SEG_G | SEG_C | SEG_B)
#define DIGIT_5 (SEG_A | SEG_F | SEG_G | SEG_C | SEG_D)
#define DIGIT_6 (SEG_A | SEG_F | SEG_G | SEG_C | SEG_D | SEG_E)
#define DIGIT_7 (SEG_A | SEG_B | SEG_C)
#define DIGIT_8 (SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G)
#define DIGIT_9 (SEG_A | SEG_B | SEG_C | SEG_F | SEG_G)
#define DIGIT_A (SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G)
#define DIGIT_B (SEG_F | SEG_G | SEG_C | SEG_D | SEG_E)
#define DIGIT_C (SEG_A | SEG_F | SEG_E | SEG_D)
#define DIGIT_D (SEG_B | SEG_C | SEG_D | SEG_E | SEG_G)
#define DIGIT_E (SEG_A | SEG_F | SEG_G | SEG_E | SEG_D)
#define DIGIT_F (SEG_A | SEG_F | SEG_G | SEG_E)

typedef union
{
    unsigned char   val;

    struct
    {
        unsigned    seg_dp : 1;
        unsigned    seg_g : 1;
        unsigned    seg_f : 1;
        unsigned    seg_e : 1;
        unsigned    seg_d : 1;
        unsigned    seg_c : 1;
        unsigned    seg_b : 1;
        unsigned    seg_a : 1;
    };
} SEVEN_SEGMENT_TYPE;

typedef union
{
    UINT16    val;
    struct
    {
        unsigned    digit0 : 4;
        unsigned    digit1 : 4;
        unsigned    digit2 : 4;
        unsigned    digit3 : 4;
    };
} BCD_TYPE;

void    lcd_init(void);
BOOL    lcd_display_digits(BCD_TYPE);
BOOL    lcd_display_digits2(BCD_TYPE);

// #define SEG0COM0  BANKMASK(LCDDATA0), 0
// as16LF1947.h
// as16F1947.h

#define B2	SEG8COM0
#define C2	SEG8COM1
#define P2	SEG8COM2

#define B3	SEG9COM0
#define C3	SEG9COM1
#define P3	SEG9COM2

#define B4	SEG10COM0
#define C4	SEG10COM1

#define A4	SEG11COM0
#define G4	SEG11COM1
#define D4	SEG11COM2

#define A2	SEG12COM0
#define G2	SEG12COM1
#define D2	SEG12COM2

#define F1	SEG13COM0
#define E1	SEG13COM1

#define F2	SEG16COM0
#define E2	SEG16COM1

#define A1	SEG17COM0
#define G1	SEG17COM1
#define D1	SEG17COM2

#define b3	SEG19COM0
#define c3	SEG19COM1
#define p3	SEG19COM2

#define b4	SEG20COM0
#define c4	SEG20COM1

#define a4	SEG21COM0
#define g4	SEG21COM1
#define d4	SEG21COM2

#define f4	SEG22COM0
#define e4	SEG22COM1

#define a3	SEG23COM0
#define g3	SEG23COM1
#define d3	SEG23COM2

#define f3	SEG24COM0
#define e3	SEG24COM1

#define a2	SEG25COM0
#define g2	SEG25COM1
#define d2	SEG25COM2

#define f2	SEG26COM0
#define e2	SEG26COM1

#define F4	SEG29COM0
#define E4	SEG29COM1

#define B1	SEG30COM0
#define C1	SEG30COM1
#define P1	SEG30COM2

#define b1	SEG35COM0
#define c1	SEG35COM1
#define p1	SEG35COM2

#define A3	SEG36COM0
#define G3	SEG36COM1
#define D3	SEG36COM2

#define F3	SEG37COM0
#define E3	SEG37COM1

#define b2	SEG41COM0
#define c2	SEG41COM1
#define p2	SEG41COM2

#define f1	SEG42COM0
#define e1	SEG42COM1

#define a1	SEG45COM0
#define g1	SEG45COM1
#define d1	SEG45COM2

const SEVEN_SEGMENT_TYPE   segment_data[] = {DIGIT_0, DIGIT_1, DIGIT_2, DIGIT_3, DIGIT_4, DIGIT_5, DIGIT_6, DIGIT_7, DIGIT_8, DIGIT_9, DIGIT_A, DIGIT_B, DIGIT_C, DIGIT_D, DIGIT_E, DIGIT_F};

/****************************************************************************
  Summary: Initializes the LCD peripheral for the F1 Evaluation Platform

  Description:
    This function configures the LCD peripheral for:
    Type B Waveform,
    Timer 1 Oscillator, and 
    1:1 prescaler
    All segment data is cleared.  The segments enables are configured for
    the F1 Evaluation Platform hardware.
 
  Remarks:
    Type B waverorm was used to allow the LCDIF flag to be the RTCC source.
    This feature was needed when operating the BLDC motors because Timer 1
    is needed for accurate commutation time and cannot be used for RTCC.
  ***************************************************************************/
void lcd_init(void)
{

    // Configure LCDPS
    // Wave form type A or B
    // Bias Mode
    // Prescaler
    // 1:16 - 1:1
    LCDPS = 0; // LCD PHASE REGISTER
    WFT = 1; // bora 1;        // WFT: Waveform Type bit, B type
	BIASMD = 0; // bora
    LCDPSbits.LP = 0; // LP<3:0>: LCD Prescaler Selection bits, 0000 = 1:1

    T1OSCEN = 1;    // activate the 32khz oscillator for our clock source
					// 1 = Dedicated Timer1 oscillator circuit enabled (T1CON register)
    
/*******************************************************/
// 1 = Segment function of the pin is enable
// 0 = I/O function of the pin is enabled
    LCDSE0  = 0;
    LCDSE1  = 0b00111111; // seg15 seg14 SEG13 SEG12 SEG11 SEG10 SEG09 SEG08
    LCDSE2  = 0b11111011; // SEG23 SEG22 SEG21 SEG20 SEG19 seg18 SEG17 SEG16
	LCDSE3  = 0b01100111; // seg31 SEG30 SEG29 seg28 seg27 SEG26 SEG25 SEG24
	LCDSE4  = 0b00111000; // seg39 seg38 SEG37 SEG36 SEG35 seg34 seg33 seg32
	LCDSE5  = 0b00100110; // seg47 seg46 SEG45 seg44 seg43 SEG42 SEG41 seg40

    /*******************************************************/

    // Configure LCDCON
    // LCD enbled
    // LCD enabled in sleep
    // LCD Clock Select - T1OSC
    // LCD Commons - 1/4
    LCDCON = 0; // LCD CONTROL REGISTER
    SLPEN = 0;  // SLPEN bit -> 0 = LCD Driver module is enabled in Sleep mode
    WERR = 0;   // WERR bit  -> 0 = No LCD write error
    LCDCONbits.CS = 1; // CS<1:0>: Clock Source Select bits -> 01 = T1OSC (Timer1), 00 -> Fosc / 256
    LCDCONbits.LMUX = 2; // bora 3; // LMUX<1:0>: Commons Select bits -> MUX 1/4, bias 1/3

    // clear ALL SEGMENT DATA
    LCDDATA0 = 0;
    LCDDATA1 = 0;
    LCDDATA2 = 0;
    LCDDATA3 = 0;
    LCDDATA4 = 0;
    LCDDATA5 = 0;
    LCDDATA6 = 0;
    LCDDATA7 = 0;
    LCDDATA8 = 0;
    LCDDATA9 = 0;
    LCDDATA10 = 0;
    LCDDATA11 = 0;
    LCDDATA12 = 0;
    LCDDATA13 = 0;
    LCDDATA14 = 0;
    LCDDATA15 = 0;
    LCDDATA16 = 0;
    LCDDATA17 = 0;
    LCDDATA18 = 0;
    LCDDATA19 = 0;
    LCDDATA20 = 0;
    LCDDATA21 = 0;
    LCDDATA22 = 0;
    LCDDATA23 = 0;

    LCDIF = 0; // PIR2: PERIPHERAL INTERRUPT REQUEST REGISTER 2
               // LCDIF: LCD Module Interrupt Flag bit -> 0 = Interrupt is not pending

    // Reference ladder control
    LCDRL = 0x30; // LCD REFERENCE LADDER CONTROL REGISTER
                  // LRLBP<1:0> -> 11 = Internal LCD Reference Ladder is powered in High-Power mode

    // Configure LCDREF
    // Internal reference enabled
    // Internal Reference Source
    // Internal Reference always on
    LCDREF = 0; // Internal LCD Reference is disabled
    LCDIRE = 1; // 1 = Internal LCD Reference is enabled and connected to the Internal Contrast Control circuit
    LCDIRS = 0; // 0 = Internal LCD Contrast Control is powered by VDD
    LCDIRI = 0; // 0 = The LCD Internal FVR Buffer ignores the LCD Reference Ladder Power mode

    LCDCST = 0; // max contrast = 0, min contrast = 7
    LCDEN = 1;  // LCDCON register, 1 = LCD Driver module is enabled 
}

/****************************************************************************
  Summary: Render the 4 BCD digits 0-9,A-F passed in BCD_TYPE

  Description:
    This function checks the Write Allow bit to determine if it is ok to modify
    the LCD segments registers.  Write Allow could be false in Type B waveforms.
    If the Write Allow flag is true, this function accepts the numbers for
    display on the LCD. It renders the 7 segment data as quickly as possible by 
    a 2 level mapping of digits to segments and then segments to LCD segments.
    This function returns TRUE or FALSE based upon the state of WA.
 
  Parameters:
    BCD_TYPE b : digits to render

  Returns:
    BOOL TRUE if display was updated.
         FALSE if the display was not updated.

  Remarks:
    It is possible that this function will return false and thereby leave the
    in the old state. 
    If you are modifying other icons on the glass (decimal points, symbols)
    test the result of this function and only modify the segments if this returns true.
    As long as you are quick, the WA bit will still be true and allow writes.
  ***************************************************************************/

BOOL lcd_display_digits(BCD_TYPE b) // nem
{

    UINT8   val,val1,val2,val3,val4;

	val4 = segment_data[b.digit0].val;
	val3 = segment_data[b.digit1].val;
	val2 = segment_data[b.digit2].val;
	val1 = segment_data[b.digit3].val;


    if(WA) // make sure it is OK to write the data
           // LCDPS: LCD PHASE REGISTER
           // WA: LCD Write Allow Status bit -> 1 = Writing to the LCDDATAn registers is allowed
    {

		val  = val1;

		if(b.digit3 == 0) { A1 = B1 = C1 = D1 = E1 = F1 = G1 = 0; }
         else {
        	if(val & SEG_A)		A1 = 1;
        	if(!(val & SEG_A))	A1 = 0;
        	if(val & SEG_B)		B1 = 1;
        	if(!(val & SEG_B))	B1 = 0;
        	if(val & SEG_C)		C1 = 1;
        	if(!(val & SEG_C))	C1 = 0;
        	if(val & SEG_D)		D1 = 1;
        	if(!(val & SEG_D))	D1 = 0;
        	if(val & SEG_E)		E1 = 1;
        	if(!(val & SEG_E))	E1 = 0;
        	if(val & SEG_F)		F1 = 1;
        	if(!(val & SEG_F))	F1 = 0;
        	if(val & SEG_G)		G1 = 1;
        	if(!(val & SEG_G))	G1 = 0;
		}

		val  = val2;
		if((b.digit3 == 0) && (b.digit2 == 0)) { A2 = B2 = C2 = D2 = E2 = F2 = G2 = 0; }
		 else {
        	if(val & SEG_A)		A2 = 1;
        	if(!(val & SEG_A))	A2 = 0;
        	if(val & SEG_B)		B2 = 1;
        	if(!(val & SEG_B))	B2 = 0;
        	if(val & SEG_C)		C2 = 1;
        	if(!(val & SEG_C))	C2 = 0;
        	if(val & SEG_D)		D2 = 1;
        	if(!(val & SEG_D))	D2 = 0;
        	if(val & SEG_E)		E2 = 1;
        	if(!(val & SEG_E))	E2 = 0;
        	if(val & SEG_F)		F2 = 1;
        	if(!(val & SEG_F))	F2 = 0;
        	if(val & SEG_G)		G2 = 1;
        	if(!(val & SEG_G))	G2 = 0;
		}

		val  = val3;

        	if(val & SEG_A)		A3 = 1;
        	if(!(val & SEG_A))	A3 = 0;
        	if(val & SEG_B)		B3 = 1;
        	if(!(val & SEG_B))	B3 = 0;
        	if(val & SEG_C)		C3 = 1;
        	if(!(val & SEG_C))	C3 = 0;
        	if(val & SEG_D)		D3 = 1;
        	if(!(val & SEG_D))	D3 = 0;
        	if(val & SEG_E)		E3 = 1;
        	if(!(val & SEG_E))	E3 = 0;
        	if(val & SEG_F)		F3 = 1;
        	if(!(val & SEG_F))	F3 = 0;
        	if(val & SEG_G)		G3 = 1;
        	if(!(val & SEG_G))	G3 = 0;


		val  = val4;

        if(val & SEG_A) 	A4 = 1;
        if(!(val & SEG_A))	A4 = 0;
        if(val & SEG_B)		B4 = 1;
        if(!(val & SEG_B))	B4 = 0;
        if(val & SEG_C)	 	C4 = 1;
        if(!(val & SEG_C))	C4 = 0;
        if(val & SEG_D)		D4 = 1;
        if(!(val & SEG_D))	D4 = 0;
        if(val & SEG_E)		E4 = 1;
        if(!(val & SEG_E))	E4 = 0;
        if(val & SEG_F)		F4 = 1;
        if(!(val & SEG_F))	F4 = 0;
        if(val & SEG_G)		G4 = 1;
        if(!(val & SEG_G))	G4 = 0;


        return (1);
    }

    return (0);
}

BOOL lcd_display_digits2(BCD_TYPE b)  // sicaklik
{

    UINT8   val,val1,val2,val3,val4;

	val4 = segment_data[b.digit0].val;
	val3 = segment_data[b.digit1].val;
	val2 = segment_data[b.digit2].val;
	val1 = segment_data[b.digit3].val;

    if(WA) // make sure it is OK to write the data
           // LCDPS: LCD PHASE REGISTER
           // WA: LCD Write Allow Status bit -> 1 = Writing to the LCDDATAn registers is allowed
    {

		val  = val1;

		if(b.digit3 == 0) { a1 = b1 = c1 = d1 = e1 = f1 = g1 = 0; }
         else {
        	if(val & SEG_A)		a1 = 1;
        	if(!(val & SEG_A))	a1 = 0;
        	if(val & SEG_B)		b1 = 1;
        	if(!(val & SEG_B))	b1 = 0;
        	if(val & SEG_C)		c1 = 1;
        	if(!(val & SEG_C))	c1 = 0;
        	if(val & SEG_D)		d1 = 1;
        	if(!(val & SEG_D))	d1 = 0;
        	if(val & SEG_E)		e1 = 1;
        	if(!(val & SEG_E))	e1 = 0;
        	if(val & SEG_F)		f1 = 1;
        	if(!(val & SEG_F))	f1 = 0;
        	if(val & SEG_G)		g1 = 1;
        	if(!(val & SEG_G))	g1 = 0;
		}

		val  = val2;
		if((b.digit3 == 0) && (b.digit2 == 0)) { a2 = b2 = c2 = d2 = e2 = f2 = g2 = 0; }
		 else {
        	if(val & SEG_A)		a2 = 1;
        	if(!(val & SEG_A))	a2 = 0;
        	if(val & SEG_B)		b2 = 1;
        	if(!(val & SEG_B))	b2 = 0;
        	if(val & SEG_C)		c2 = 1;
        	if(!(val & SEG_C))	c2 = 0;
        	if(val & SEG_D)		d2 = 1;
        	if(!(val & SEG_D))	d2 = 0;
        	if(val & SEG_E)		e2 = 1;
        	if(!(val & SEG_E))	e2 = 0;
        	if(val & SEG_F)		f2 = 1;
        	if(!(val & SEG_F))	f2 = 0;
        	if(val & SEG_G)		g2 = 1;
        	if(!(val & SEG_G))	g2 = 0;
		}
		
		if(eksi) {
			if((b.digit3 == 0) && (b.digit2 == 0)) g2 = 1;
			if((b.digit3 == 0) && (b.digit2 != 0)) g1 = 1;
		}

		val  = val3;

        	if(val & SEG_A)		a3 = 1;
        	if(!(val & SEG_A))	a3 = 0;
        	if(val & SEG_B)		b3 = 1;
        	if(!(val & SEG_B))	b3 = 0;
        	if(val & SEG_C)		c3 = 1;
        	if(!(val & SEG_C))	c3 = 0;
        	if(val & SEG_D)		d3 = 1;
        	if(!(val & SEG_D))	d3 = 0;
        	if(val & SEG_E)		e3 = 1;
        	if(!(val & SEG_E))	e3 = 0;
        	if(val & SEG_F)		f3 = 1;
        	if(!(val & SEG_F))	f3 = 0;
        	if(val & SEG_G)		g3 = 1;
        	if(!(val & SEG_G))	g3 = 0;


		val  = val4;

        if(val & SEG_A) 	a4 = 1;
        if(!(val & SEG_A))	a4 = 0;
        if(val & SEG_B)		b4 = 1;
        if(!(val & SEG_B))	b4 = 0;
        if(val & SEG_C)	 	c4 = 1;
        if(!(val & SEG_C))	c4 = 0;
        if(val & SEG_D)		d4 = 1;
        if(!(val & SEG_D))	d4 = 0;
        if(val & SEG_E)		e4 = 1;
        if(!(val & SEG_E))	e4 = 0;
        if(val & SEG_F)		f4 = 1;
        if(!(val & SEG_F))	f4 = 0;
        if(val & SEG_G)		g4 = 1;
        if(!(val & SEG_G))	g4 = 0;

        return (1);
    }

    return (0);
}

// UART
#define BAUD 9600      								// UART
#define DIVIDER ((int)(_XTAL_FREQ/(16UL * BAUD) -1))		// UART
#define HIGH_SPEED 1								// UART

#if HIGH_SPEED == 1									// UART
	#define SPEED 0x4								// UART
#else												// UART
	#define SPEED 0									// UART
#endif												// UART

// TRISC<7:0>: PORTC Tri-State Control bits, 
// 1 = PORTC pin configured as an input (tri-stated)
// 0 = PORTC pin configured as an output
#if RX_1 == 1 										// UART
	#define RX_PIN TRISC7							// UART
	#define TX_PIN TRISC6							// UART
#else												// UART
	#define RX_PIN TRISG2							// UART
	#define TX_PIN TRISG1							// UART
#endif												// UART

#include <htc.h>
#include <stdio.h>

void putch(unsigned char byte) // TX one byte
{
// PIR1: PERIPHERAL INTERRUPT REQUEST REGISTER 1, TXIF: USART Transmit Interrupt Flag bit
// 1 = Interrupt is pending, 0 = Interrupt is not pending

#if RX_1 == 1 				// UART
	while(!TXIF) continue;	// UART
	TX1REG = byte; 			// UART
#else
	while(!TX2IF) continue;	// UART
	TX2REG = byte; 			// UART
#endif						// UART

}


unsigned int sayac; // Bora
unsigned char RX[20], RX_index=0, RX_timeout=0;	// UART

// #define LED_PIN PIN_A3

unsigned char ROM1[8]; 
unsigned char nem[5],dene[800];
UINT16 busy_say;
UINT16 nem_deger;
INT16 sicaklik_deger; 
 


void nem_oku()
{
      unsigned char i, bit_time, bit_temp,sensor_yok;
	  UINT16 lowda_bekle;

	  sensor_yok = 0;
      RE1 = 0; TRISE1 = 0;
      __delay_ms( 1 ); // 
      TRISE1 = 1; // 1-wire bir yap�l�r,
      __delay_us( 150 );
      while(RE1==1) ; // low olana kadar bekle.
    
      for(i=0; i<40; i++) {
       bit_time = 0;
       CLRWDT();
	   lowda_bekle = 0;
       while(RE1==0) { lowda_bekle++; if(lowda_bekle > 100UL) {sensor_yok=1; break;}} // low iken bekle
       while(RE1==1)  { bit_time++; } // sureyi olc
       //dene[i]=bit_time; // DEBUG
       if (bit_time > 10) bit_temp = 1; else bit_temp = 0;
       nem[i/8] = (nem[i/8] << 1) | bit_temp;
      }

	RE1 = 1; TRISE1 = 0; // output high

	   if(sensor_yok) {nem[0]=0xFF; nem[1]=0xFF; nem[2]=0xFF; nem[3]=0xFF;}

	   if((UINT8) (nem[0]+nem[1]+nem[2]+nem[3]) == nem[4]) { // CHKSUM tutuyorsa
         nem_deger = (nem[0] << 8) + nem[1];
		 sicaklik_deger = (nem[2] << 8) + nem[3];
		 if(sicaklik_deger & 0x8000UL) { sicaklik_deger = sicaklik_deger & 0x7FFFUL; eksi = 1;} else eksi = 0;
       }    
	   
}

int main(void)
{
	unsigned char input, n_TO;
	UINT16 temperature, k;

    unsigned char gelen, i, CHK;
 
	time_t      t;

 	CLRWDT();
	WDTCON = 0x1A; // 8sec watchdog timer
	
	blink = 0;
	eksi = 0;
	komut_isle = 0;		// UART

	nem_deger = 0L; 
	sicaklik_deger = 0L;

	bekleme = 0;

	OPTION_REG = OPTION_REG_INIT;	// 1000 0101
	// WPUEN: Weak Pull-up Enable bit 		-> 1 = All weak pull-ups are disabled (except MCLR, if it is enabled)
	// INTEDG: Interrupt Edge Select bit 	-> 0 = Interrupt on falling edge of INT pin
	// TMR0CS: Timer0 Clock Source Select	-> 0 = Internal instruction cycle clock (FOSC/4)
	// TMR0SE: Timer0 Source Edge Select 	-> 0 = Increment on low-to-high transition on T0CKI pin
	// PSA: Prescaler Assignment bit		-> 0 = Prescaler is assigned to the Timer0 module
	// PS<2:0>: Prescaler Rate Select bits	-> 101 = 1 : 64

	OSCCONbits.IRCF = 0b1101; // 1101 -> 4MHz, 1100 -> 2MHz, 1011 -> 1MHz, 0111 -> 500kHz, 1110 = 8 MHz or 32 MHz HF
	SPLLEN = 0; // 0 = 4x PLL is disabled, (If PLLEN in Configuration Word 1 = 0:)
		    
	// peripheral configuration

    PORTA = 0;
    PORTB = 0;
	PORTC = 0;
	PORTD = 0;
	PORTE = 0;
    PORTF = 0;
    PORTG = 0;

	ANSA0 = 0;   // PIC16F1947
	ANSA1 = 0;       
	ANSA2 = 0;       
	ANSA3 = 0;       
	ANSA5 = 0;       

	ANSE0 = 0;       
	ANSE1 = 0;       
	ANSE2 = 0;       

	ANSF0 = 0;
	ANSF1 = 0;
	ANSF2 = 0;
	ANSF3 = 0;
	ANSF4 = 0;
	ANSF5 = 0;
	ANSF6 = 0;
	ANSF7 = 0;

	ANSG1 = 0;
	ANSG2 = 0;
	ANSG3 = 0;
	ANSG4 = 0;
 
	rtcc_init();
	lcd_init();

	t = time(0);
	
	LCDIE = 1; // PIE2: PERIPHERAL INTERRUPT ENABLE REGISTER 2, LCDIE -> 1 = Enables the LCD module interrupt

#if RX_1 == 1 				// UART
	RCIE =  1; // PIE1: PERIPHERAL INTERRUPT ENABLE REGISTER 1, RCIE ->  1 = Enables the USART receive interrupt
#else
	RC2IE =  1; // PIE1: PERIPHERAL INTERRUPT ENABLE REGISTER 1, RCIE ->  1 = Enables the USART receive interrupt
#endif						// UART
	
	PEIE = 1;  // INTCON: INTERRUPT CONTROL REGISTER, PEIE: Peripheral Interrupt Enable bit -> 1 = Enables all active peripheral interrupts
	GIE = 1;   // INTCON: INTERRUPT CONTROL REGISTER, GIE: Global Interrupt Enable bit -> 1 = Enables all active interrupts
	
    while(!WA);
	// light all segments
	A1 = B1 = C1 = D1 = E1 = F1 = G1 = P1 = 1;
	A2 = B2 = C2 = D2 = E2 = F2 = G2 = P2 = 1;
	A3 = B3 = C3 = D3 = E3 = F3 = G3 = P3 = 1;
	A4 = B4 = C4 = D4 = E4 = F4 = G4 = 1;

	a1 = b1 = c1 = d1 = e1 = f1 = g1 = p1 = 1;
	a2 = b2 = c2 = d2 = e2 = f2 = g2 = p2 = 1;
	a3 = b3 = c3 = d3 = e3 = f3 = g3 = p3 = 1;
	a4 = b4 = c4 = d4 = e4 = f4 = g4 = 1;

	// wait for 2 seconds
	__delay_ms(2000);    
   
	// wait for LCD ready
	while(!WA);
	// clear all segments
	A1 = B1 = C1 = D1 = E1 = F1 = G1 = P1 = 0;
	A2 = B2 = C2 = D2 = E2 = F2 = G2 = P2 = 0;
	A3 = B3 = C3 = D3 = E3 = F3 = G3 = P3 = 0;
	A4 = B4 = C4 = D4 = E4 = F4 = G4 = 0;

	a1 = b1 = c1 = d1 = e1 = f1 = g1 = p1 = 0;
	a2 = b2 = c2 = d2 = e2 = f2 = g2 = p2 = 0;
	a3 = b3 = c3 = d3 = e3 = f3 = g3 = p3 = 0;
	a4 = b4 = c4 = d4 = e4 = f4 = g4 = 0;

	// TRISC<7:0>: PORTC Tri-State Control bits, 
	// 1 = PORTC pin configured as an input (tri-stated)
	// 0 = PORTC pin configured as an output
	RX_PIN = 1;		     // UART
	TX_PIN = 1;  	     // UART

#if RX_1 == 1 		     // UART
	SP1BRGL = DIVIDER;
	RC1STA = 0x90;       // 1001 0000	SPEN = 1, CREN = 1
	TX1STA = 0x04;       // 0010 0100	TXEN = 0, SYNC = 0, BRGH = 1		// TXREG -> USART Transmit Data Register
#else				     // UART
	SP2BRGL = DIVIDER;
	RC2STA = 0x90;       // 1001 0000	SPEN = 1, CREN = 1
	TX2STA = 0x04;       // 0010 0100	TXEN = 0, SYNC = 0, BRGH = 1
#endif					 // UART
 

	RE1 = 1; TRISE1 = 0; // output high

   
   ////////////////////////////////////////// main loop ////////////////////////////////////////////////   
 
   while(1)
   {  
		if(nemi_oku == 1) { 
			nemi_oku = 0; 
			CLRWDT();
			nem_oku(); 

		}

		if(komut_isle) {
              CLRWDT();
			  __delay_ms(100*PIC_ID-50);

#if RX_1 == 1 						// UART
			  TX1STA = 0x24;       	// 0010 0100	TXEN = 1, SYNC = 0, BRGH = 1
#else								// UART
			  TX2STA = 0x24;       	// 0010 0100	TXEN = 1, SYNC = 0, BRGH = 1
#endif								// UART

      		  __delay_ms(20);
       		  putch(0x02);
			  putch(PIC_ID);
       		  for(i=0;i<4;i++) { putch(nem[i]); } 
			  CLRWDT();
			  putch(PIC_ID+nem[0]+nem[1]+nem[2]+nem[3]);
       		  putch(0x03);
		      komut_isle = 0;
			  __delay_ms(20);

#if RX_1 == 1 						// UART
			  TX1STA = 0x04;       	// 0010 0100	TXEN = 0, SYNC = 0, BRGH = 1
#else								// UART
			  TX2STA = 0x04;       	// 0010 0100	TXEN = 0, SYNC = 0, BRGH = 1
#endif								// UART
		}


		if(bekleme == 3) display_int2(PIC_ID);

		if(bekleme > 5) {

	    if(!(sayac%2))
	    	{ 


				if((nem[0]==0xFF) && (nem[1]==0xFF) &&(nem[2]==0xFF) &&(nem[3]==0xFF)) {

					// clear all segments
					while(!WA);
					A1 = B1 = C1 = D1 = E1 = F1 = G1 = P1 = 0;
					A2 = B2 = C2 = D2 = E2 = F2 = G2 = P2 = 0;
					A3 = B3 = C3 = D3 = E3 = F3 = G3 = P3 = 0;
					A4 = B4 = C4 = D4 = E4 = F4 = G4 = 0;

					a1 = b1 = c1 = d1 = e1 = f1 = g1 = p1 = 0;
					a2 = b2 = c2 = d2 = e2 = f2 = g2 = p2 = 0;
					a3 = b3 = c3 = d3 = e3 = f3 = g3 = p3 = 0;
					a4 = b4 = c4 = d4 = e4 = f4 = g4 = 0;

					display_int2(PIC_ID);
					G4 = 1;
					blink = 0;
					CLRWDT();
				}
				else {
                    P3 = 1; p3 = 1;
                    display_int(nem_deger);       // nem_deger
					display_int2(sicaklik_deger); // sicaklik_deger
					blink = 1;   
					CLRWDT();
				}

            } // if(!(sayac%2))
		} // if(bekleme > 6) {

	} // while(1) 	    
	return 0;
}

void interrupt isr()
{

	// TMR1IF => PIR1: PERIPHERAL INTERRUPT REQUEST REGISTER 1, TMR1IF: Timer1 Overflow Interrupt Flag bit
	// 1 = Interrupt is pending, 0 = Interrupt is not pending
	// TMR1IE => PIE1: PERIPHERAL INTERRUPT ENABLE REGISTER 1, TMR1IE: Timer1 Overflow Interrupt Enable bit
	// 1 = Enables the Timer1 overflow interrupt, 0 = Disables the Timer1 overflow interrupt
    if(TMR1IF && TMR1IE) // Bu kismi saniyede bir isletiyor
    {
        TMR1IF = 0; 
        rtcc_handler();
        icon = !icon;  //S2 = ~S2;
		sayac++;
		if(bekleme < 10) bekleme++;
        if(sayac % 4 == 0) {nemi_oku=1;}
		if(RX_index) RX_timeout++;
    }
    
	// LCDIF => PIR2: PERIPHERAL INTERRUPT REQUEST REGISTER 2, LCD Module Interrupt Flag bit
    // 0 = Interrupt is not pending, 1 = Interrupt is pending
	// LCDIE => PIE2: PERIPHERAL INTERRUPT ENABLE REGISTER 2, 
    // 1 = Enables the LCD module interrupt, 0 = Disables the LCD module interrupt
    if(LCDIF && LCDIE) // Bu kismi cok hizli isletiyor, SEGMENT degisiklikleri burada yapiliyor
    {
	    LCDIF = 0;
//		LCD_bas = 1;
		if((bekleme == 10) && (blink == 1)) {
	    	if(icon)
	    	{E1 = G1 = C1 = 1; 
			 //A1 = B1 = C1 = D1 = E1 = F1 = G1 = 1;
             //a4 = b4 = c4 = d4 = e4 = f4 = g4 = 0;
            }
	    	else
	    	{E1 = G1 = C1 = 0; 
             //A1 = B1 = C1 = D1 = E1 = F1 = G1 = 0;
             //a4 = b4 = c4 = d4 = e4 = f4 = g4 = 1;
            }
		}
	}

#if RX_1 == 1 				// UART
	if(RCIF && RCIE) 
	{
		RCIF=0;
		if((RC1REG == 'n') || (RC1REG == 'N')) komut_isle = 1; else komut_isle = 0;
		//RX[RX_index++]=RC1REG;
		RX_timeout=0;  // yeniden kur
	}
#else						// UART
	if(RC2IF && RC2IE) 
	{
		RC2IF=0;
		if((RC2REG == 'n') || (RC2REG == 'N')) komut_isle = 1; else komut_isle = 0;
		//RX[RX_index++]=RC2REG;
		RX_timeout=0;  // yeniden kur
	}
#endif						// UART

}


char display_int(int t)
{
    BCD_TYPE bcd;
    
    // assumes t is the temperature in degrees C * 10
    bcd.digit0 = t % 10;
    t /= 10;
    bcd.digit1 = t % 10;
    t /= 10;
    bcd.digit2 = t % 10;
    t /= 10;
    bcd.digit3 = t % 10;
    return(lcd_display_digits(bcd));   
}    

char display_int2(int t)
{
    BCD_TYPE bcd;
    
    // assumes t is the temperature in degrees C * 10
    bcd.digit0 = t % 10;
    t /= 10;
    bcd.digit1 = t % 10;
    t /= 10;
    bcd.digit2 = t % 10;
    t /= 10;
    bcd.digit3 = t % 10;
    return(lcd_display_digits2(bcd));   
}   

#warning USING TIMER1 FOR TIMEBASE
#define CLOCKS_PER_SEC  1

volatile time_t device_time;
volatile UINT16 seconds_counter;

/****************************************************************************
  Summary: Initialize the clock calendar driver.

  Description:
    This function configured the basics of a software driven RTCC peripheral.
    It relies upon a periodic event to provide time keeping.  Two event choices
    are available:
    1) Timer 1 Oscillator + Timer 1, and
    2) Timer 1 Oscillator + LCD Peripheral
    In either case, the Timer 1 Oscillator provides a steady 32.768kHz time base.
    If Timer 1 is used, then 0x8000 counts will provide a 1HZ tick.
    CLOCKS_PER_SEC is configured for 1 and all is well.
    If LCDIF is used, then CLOCKS_PER_SEC is 128 so device_time updates every 128
    LCDIF events.  The variable seconds_counter provides this countdown.
 
  Remarks:
    The C stdlib functions understand UNIX time but they need a UNIX time counter.
    UNIX time is the number of seconds elapsed since Midnight Jan 1, 1970
  ***************************************************************************/
void rtcc_init(void)
{

    T1CONbits.TMR1CS = 2; 	// TMR1CS<1:0>: Timer1 Clock Source Select bits, 
							// 10 =Timer1 clock source is pin or oscillator
							// Crystal oscillator on T1OSI/T1OSO pins (If T1OSCEN = 1)

    T1OSCEN = 1; // T1OSCEN: LP Oscillator Enable Control bit, 1 = Dedicated Timer1 oscillator circuit enabled
    nT1SYNC = 0; // 0 = Synchronize external clock input with system clock (FOSC)
    TMR1ON = 1;  // 1 = Enables Timer1
    // TMR1IF => PIR1: PERIPHERAL INTERRUPT REQUEST REGISTER 1, TMR1IF: Timer1 Overflow Interrupt Flag bit
	// 1 = Interrupt is pending, 0 = Interrupt is not pending
	// TMR1IE => PIE1: PERIPHERAL INTERRUPT ENABLE REGISTER 1, TMR1IE: Timer1 Overflow Interrupt Enable bit
	// 1 = Enables the Timer1 overflow interrupt, 0 = Disables the Timer1 overflow interrupt
    TMR1IF = 0;
    TMR1IE = 1;
    
    device_time = 1293861600; // Jan 1 2011
    seconds_counter = CLOCKS_PER_SEC;
}


/****************************************************************************
  Summary: maintain device_time (seconds) using the LCDIF flag/interrupt.

  Description:
    This function decrements seconds_counter until 0 and then increments device_time.
    seconds_counter reloads with CLOCK_PER_SEC.
    This version of the function uses Timer 1 as the time base.
    Timer 1 is reloaded with 0x8000 to cause TMR1IF to overflow ever second.
 
  Remarks:
    TMR1IF is not looked at or modified because there could be other tasks running
    on this interrupt.
  ***************************************************************************/

void rtcc_handler(void)
{
    TMR1ON = 0;		// 0 = Stops Timer1, Clears Timer1 gate flip-flop
    TMR1H = 0x80;	// Holding Register for the Most Significant Byte of the 16-bit TMR1 Register
    TMR1ON = 1;		// 1 = Enables Timer1
    seconds_counter--;
    if(seconds_counter == 0)
    {
        device_time++;
        seconds_counter = CLOCKS_PER_SEC;
    }
}


/****************************************************************************
  Summary: return the current device time.

  Description:
    This function retrieves the device time as either a return value or 
    filling in a variable passed by reference.  Interrupts are disabled
    during the copy and restored on exit.
 
  Parameters:
    time_t *t : A time_t pointer for the current time.

  Returns:
    time_t value of the current time.

  Remarks:
    This function is a prerequisit to supporting the standard C time libraries.
  ***************************************************************************/

/* implement the time(&t) function for the standard libraries */

/* time.h does not implment time as it is application dependent */
time_t time(time_t *t)
{
    BOOL   gie_val;
    time_t  the_time;
    
    gie_val = GIE;  // INTCON: INTERRUPT CONTROL REGISTER, GIE: Global Interrupt Enable bit -> 1 = Enables all active interrupts
    GIE = 1; 		// INTCON: INTERRUPT CONTROL REGISTER, GIE: Global Interrupt Enable bit -> 1 = Enables all active interrupts	
    the_time = device_time;
    GIE = gie_val;  // INTCON: INTERRUPT CONTROL REGISTER, GIE: Global Interrupt Enable bit -> 1 = Enables all active interrupts

    if(t)
    {
        *t = the_time;
    }

    return (the_time);
}
