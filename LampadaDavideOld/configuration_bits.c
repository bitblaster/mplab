/******************************************************************************/
/* Main Files to Include                                                      */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

/******************************************************************************/
/* Configuration Bits                                                         */
/*                                                                            */
/* Refer to your Hi-Tech User Manual in the PICC installation directory       */
/* /doc folder for more information on filling in configuration bits.         */
/* In addition, configuration bit mnemonics can be found in your              */
/* PICC\version\include\<processor name>.h file for your device.  The XC8     */
/* compiler contains documentation on the configuration bit macros within     */
/* the compiler installation /docs folder in a file called                    */
/* pic18_chipinfo.html.                                                       */
/*                                                                            */
/* For additional information about what the hardware configurations mean in  */
/* terms of device operation, refer to the device datasheet.                  */
/*                                                                            */
/* A feature of MPLAB X is the 'Generate Source Code to Output' utility in    */
/* the Configuration Bits window.  Under Window > PIC Memory Views >          */
/* Configuration Bits, a user controllable configuration bits window is       */
/* available to Generate Configuration Bits source code which the user can    */
/* paste into this project.                                                   */
/*                                                                            */
/******************************************************************************/
#if defined(PIC12F683)
    // CONFIG
    #pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA4/OSC2/CLKOUT pin, I/O function on RA5/OSC1/CLKIN)
    #pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
    #pragma config PWRTE = ON      // Power-up Timer Enable bit (PWRT disabled)
    #pragma config MCLRE = ON      // MCLR Pin Function Select bit (MCLR pin function is digital input, MCLR internally tied to VDD)
    #pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
    #pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
    #pragma config BOREN = ON       // Brown Out Detect (BOR enabled)
    #pragma config IESO = OFF       // Internal External Switchover bit (Internal External Switchover mode is disabled)
    #pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
#elif defined(PIC12F675)
    // CONFIG
    #pragma config FOSC = INTRCIO   // Oscillator Selection bits (INTOSC oscillator: I/O function on GP4/OSC2/CLKOUT pin, I/O function on GP5/OSC1/CLKIN)
    #pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
    #pragma config PWRTE = ON       // Power-Up Timer Enable bit (PWRT enabled)
    #pragma config MCLRE = ON      // GP3/MCLR pin function select (GP3/MCLR pin function is digital I/O, MCLR internally tied to VDD)
    #pragma config BOREN = ON       // Brown-out Detect Enable bit (BOD enabled)
    #pragma config CP = OFF         // Code Protection bit (Program Memory code protection is disabled)
    #pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#endif