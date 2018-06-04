/*
 * Simplified Virtual Wire compatible library for PIC microcontrollers.
 * It is based on the original VirtualWire 1.20 available for Arduino at
 * http://www.airspayce.com/mikem/arduino/
 *
 * Some simplifications were made to make it fit inside PIC16 mid-range devices.
 * Half-duplex mode only, with a shared buffer for TX and RX.
 *
 * Original license:
 ** This software is Copyright (C) 2008 Mike McCauley. Use is subject to license
 ** conditions. The main licensing options available are GPL V2 or Commercial:
 *
 ** Open Source Licensing GPL V2
 *
 ** This is the appropriate option if you want to share the source code of your
 ** application with everyone you distribute it to, and you also want to give them
 ** the right to share who uses it. If you wish to use this software under Open
 ** Source Licensing, you must contribute all your source code to the open source
 ** community in accordance with the GPL Version 2 when your application is
 ** distributed. See http://www.gnu.org/copyleft/gpl.html
 **
 ** Commercial Licensing
 *
 ** This is the appropriate option if you are creating proprietary applications
 ** and you are not prepared to distribute and share the source code of your
 ** application. Contact info@open.com.au for details.
 */

#ifndef TEA_ENCRYPTION_H
#define TEA_ENCRYPTION_H

#include <stdint.h>

//void btea(uint32_t *v, int n, uint32_t const key[4]);

void encrypt(uint32_t *data, uint8_t len);
void decrypt(uint32_t *data, uint8_t len);
//void encipher(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4]);
//void decipher(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4]);

#endif
