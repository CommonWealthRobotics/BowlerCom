#ifndef BOWLERCONFIG_H_
#define BOWLERCONFIG_H_


#include <Arduino.h>

#include <BowlerStack/include/DyIO/BOLWER_EEPROM.h>
#ifdef __cplusplus
extern "C"{
#endif
#define StartCritical() noInterrupts()
#define EndCritical()   interrupts()


#ifdef ARDUINO_ARCH_ARC32

#endif
#ifdef ARDUINO_ARCH_AVR
	#include <avr/pgmspace.h>

	void showString (PGM_P s,Print_Level l,char newLine);

	#undef print_nnl
	#undef b_println

	/**
	 * print the null terminated string with no new lines
	 */
	#define print_nnl(A,B) showString(PSTR(A),B,0)

	/**
	 * print the null terminated string with a newline inserted at the begining of the string
	 */
	#define b_println(A,B) showString(PSTR(A),B,1)
#endif


#if (TOTAL_PINS*4+2)>FullPacketDataSize
	#warning This device has too many pins TOTAL_PINS
	#undef TOTAL_PINS
	#define TOTAL_PINS ((FullPacketDataSize-2)/4)

#endif

#ifdef __cplusplus
}
#endif
#endif
