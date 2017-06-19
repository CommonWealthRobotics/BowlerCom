/**
 * Bowler Communication System
 */

#ifndef ARDUBCS_H
#define ARDUBCS_H
#define comBuffSize 256
#ifdef __MBED__
	#include <mbed.h>
#else
	#include <Arduino.h>
	#ifdef __cplusplus
		#include <SoftwareSerial.h>
		#include <Servo.h>
	#endif
#endif


#include "BowlerStack/include/Bowler/Bowler.h"
#include "BowlerStack/include/DyIO/PID.h"
#include "BowlerStack/include/DyIO/DyIO_def.h"
#include "BowlerStack/include/Namespace/AsyncManager.h"
#include "BowlerStack/include/Namespace/Namespace_bcs_io_setmode.h"
#include "BowlerStack/include/Namespace/Namespace_bcs_io.h"


#ifdef __cplusplus
/**
 * Start the serial debugger
 */
void startDebugPint(Stream * port);
/**
 * BowlerCom Class
 * This class is for handeling the Bowler Communications on a stream.
 */
	class BowlerCom {
	public:
		BowlerCom();
		void server(void);
		/* Arduino constructors */
		void begin(Stream * port);
		void begin(long);
		boolean PutBowlerPacketLocal(BowlerPacket * p);
		void addDyIO();
		void addDyIOPID();

		byte privateRXCom[comBuffSize];
		BYTE_FIFO_STORAGE store;
		BowlerPacket Packet;
		boolean SetChanVal(uint8_t pin, int32_t bval, float time);
		int32_t GetChanVal(uint8_t pin);
		boolean enableAsync;
		/**
		 * This Function is used to alter the mode of the pin (See DYIO_def.h for modes)
		 * Setting the higest bit (|=0x80) will enable the mode as async
		 *
		 * @param pin The pin refers to the dyio channel to be altered
		 * @param mode the mode to set the pin to
		 * @return If the mode change was successfull
		 */
		boolean setMode(uint8_t pin,uint8_t mode);
	private:
		boolean addedDyIO;
		boolean addedPID;
		RunEveryData timeout;
		void begin_local(Stream * port);


	};
#endif
#endif
