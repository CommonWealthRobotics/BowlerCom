/**
 * Bowler Communication System
 */
#include <BowlerCom.h>
#include <Platform/include/arch/Arduino/IOAbstraction.hpp>

static BowlerCom * ref;
static Stream * serialPort = NULL;

static Stream * comPort = NULL;
static boolean comsStarted = false;
//DATA_STRUCT functionData[TOTAL_PINS];
//AbsPID mypidGroups[NUM_PID_GROUPS];
//DYIO_PID mydyPid[NUM_PID_GROUPS];
//PidLimitEvent mylimits[NUM_PID_GROUPS];
float getMs(void) {
	return ((float) millis()) * 1.0f;
}
void putCharDebug(char a) {
	// none
	if (serialPort != NULL) {
		serialPort->print(a);
		serialPort->flush();
	}

}
void EnableDebugTerminal() {
	//none
}
BowlerCom::BowlerCom() {
	ref = this;
	addedDyIO = false;
	addedPID = false;
	enableAsync=true;
	timeout.MsTime=getMs();
	timeout.setPoint=5000;
	comPort = NULL;
	Bowler_Init();
}
/**
 * This Function is used to alter the mode of the pin (See DYIO_def.h for modes)
 * Setting the higest bit (|=0x80) will enable the mode as async
 *
 * @param pin The pin refers to the dyio channel to be altered
 * @param mode the mode to set the pin to
 * @return If the mode change was successfull
 */
boolean BowlerCom::setMode(uint8_t pin, uint8_t mode) {
	return setMode(pin, mode);
}

boolean BowlerCom::SetChanVal(uint8_t pin, int32_t bval, float time) {
	return SetChanVal(pin, bval, time);
}
int32_t BowlerCom::GetChanVal(uint8_t pin) {
	return GetChanVal(pin);
}

/* begin method for overriding default serial bitrate */
void BowlerCom::begin(Stream * port) {
	begin_local(port);
}

/* begin method for overriding default serial bitrate */
void BowlerCom::begin(long speed) {
	Serial.begin(speed);
	while (!Serial)
		;    // wait for the serial port to open
	begin_local(&Serial);
}

void BowlerCom::begin_local(Stream * port) {
	InitByteFifo(&store, privateRXCom, comBuffSize);
	comPort = port;

}
#if defined(__MBED__)

#else
void showString(PGM_P s,Print_Level l,char newLine) {
	if(!okToprint(l)) {
		return;
	}
	if(newLine) {
		putCharDebug('\n');
		putCharDebug('\r');
	}
	setColor(l);
	char c;
	while ((c = pgm_read_byte(s++)) != 0)
		putCharDebug(c);
}
#endif
uint16_t putStream(uint8_t * buffer, uint16_t datalength) {
	uint16_t i;
	for (i = 0; i < datalength; i++) {
		//Grab the response packet one byte at a time and push it out the physical layer
		comPort->write((char) buffer[i]);
	}
	comPort->flush();
//	BowlerPacket* Packet = (BowlerPacket*) buffer;
//	if(Packet->use.head.Method!= BOWLER_ASYN)
//		printPacket(Packet,INFO_PRINT);
	return true;
}
boolean PutBowlerPacketDummy(BowlerPacket * Packet) {
	//do nothing
	return true;
}

void BowlerCom::server(void) {
	byte err;
	byte newByte = 0;
	if(comsStarted)
		if(RunEvery(&timeout)>0){
			println_E("Socket Timed out, clearing buffer");
			//clear the buffer
			InitByteFifo(&store, privateRXCom, comBuffSize);
			comsStarted = false;
		}
	while (comPort->available() > 0) {
		newByte = comPort->read();
		//println_I("Adding byte: ");prHEX8(newByte,INFO_PRINT);
		FifoAddByte(&store, (char) newByte, &err);
		timeout.MsTime=getMs();// reset the timeout timer
	}
	if (GetBowlerPacket(&Packet, &store)) {
		//printPacket(&Packet, WARN_PRINT);
		//Now the Packet struct contains the parsed packet data
		process(&Packet);

		// The call backs for processing the packet have been called
		// and the Packet struct now contains the data
		// to be sent back to the client as a response.
		PutBowlerPacket(&Packet);
		if (Packet.use.head.RPC != GetRPCValue((char*)"_pwr")
				&& Packet.use.head.RPC != GetRPCValue((char*)"_png")
				&& Packet.use.head.RPC != GetRPCValue((char*)"_rpc") &&
				Packet.use.head.RPC != GetRPCValue((char*)"_nms") &&
				Packet.use.head.RPC != GetRPCValue((char*)"args")) {//Ignore Power Packet
			b_println("Response:", INFO_PRINT);
			printPacket(&Packet, INFO_PRINT);
		}
		comsStarted = true;
	}
	if (comsStarted&&enableAsync)
		RunNamespaceAsync(&Packet, &PutBowlerPacket);
	else
		RunNamespaceAsync(&Packet, &PutBowlerPacketDummy);
}
void BowlerCom::addDyIO() {
	if (addedDyIO) {
		return;
	}
	addedDyIO = true;
	println_I("Initializing Pins");
	InitPinFunction(new DATA_STRUCT[TOTAL_PINS]);
	println_I("Adding IO Namespace");
	addNamespaceToList(get_bcsIoNamespace());
	println_I("Adding IO.Setmode Namespace");
	addNamespaceToList(get_bcsIoSetmodeNamespace());
	println_I("Adding DyIO Namespace");
	addNamespaceToList(get_neuronRoboticsDyIONamespace());
	println_I("Adding Safe Namespace");
	addNamespaceToList(get_bcsSafeNamespace());

}
void BowlerCom::addDyIOPID() {
	if (addedPID) {
		return;
	}
	addedPID = true;
	addDyIO();
//	InitPID(mypidGroups,
//			mydyPid,
//			mylimits
//			);
	println_I("Initializing PID");
	InitPID(new AbsPID[NUM_PID_GROUPS], new DYIO_PID[NUM_PID_GROUPS],
			new PidLimitEvent[NUM_PID_GROUPS]);
	println_I("Adding PID Namespace");
	addNamespaceToList(getBcsPidNamespace());
	println_I("Adding DyIO PID Namespace");
	addNamespaceToList(get_bcsPidDypidNamespace());
}
void startDebugPint(Stream * port) {
	if (port == NULL)
		return;
	serialPort = port;
	setPrintLevelInfoPrint();
	println_I("\n\n\n##############################");
	println_I("  Welcome To Arduino Bowler!");
	println_I("##############################\n");
	println_E("Error Prints");
	println_W("Warning Prints");
	println_I("Info Prints");
}
