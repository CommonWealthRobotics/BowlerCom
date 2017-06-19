/**
 *
 * Copyright 2009 Neuron Robotics, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <BowlerCom.h>
#if defined(__PIC32MX__)
extern MAC_ADDR MyMAC __attribute__((section(".scs_global_var")));
#else
extern MAC_ADDR MyMAC;
#endif

void LoadCorePacket(BowlerPacket * Packet) {
    //SetColor(0,1,0);
    uint8_t i;
    Packet->use.head.ProtocolRevision = BOWLER_VERSION;
    for (i = 0; i < 6; i++) {
        Packet->use.head.MAC.v[i] = 0;
    }
    Packet->use.head.MessageID = 1;
    Packet->use.head.ResponseFlag = 1;
    Packet->use.head.Method = BOWLER_STATUS;
    Packet->use.head.RPC = GetRPCValue("****");
    Packet->use.head.DataLegnth = 4;
}
/*
 * Calculate waht the CRC should be for a given data section
 */
uint8_t CalcCRC(BowlerPacket *Packet) {
    uint32_t v = 0;
    int i;
    for (i = 0; i < 10; i++)
        v += (uint32_t) Packet->stream[i];
    return (uint8_t)(v & 0x000000ff);
}
/*
 * Returns true if the data crc in the packet matches the one calculated fromthe packet
 */
uint8_t CheckCRC(BowlerPacket *Packet) {
    uint8_t v = CalcCRC(Packet);
    if (Packet->use.head.CRC == v)
        return true;
    return false;
}
/*
 * Calculates and sets the CRC in the packet
 */
void SetCRC(BowlerPacket * Packet) {
    uint8_t v = CalcCRC(Packet);
    Packet->use.head.CRC = v;
}
/*
 * Calculate waht the CRC should be for a given data section
 */
uint8_t CalcDataCRC(BowlerPacket *Packet) {
    uint32_t v = 0;
    int i;
    for (i = 0; i < Packet->use.head.DataLegnth; i++)
        v +=(uint32_t) Packet->stream[i+_BowlerHeaderSize];
    return (uint8_t)(v & 0x000000ff);
}
/*
 * Returns true if the data crc in the packet matches the one calculated fromthe packet
 */
uint8_t CheckDataCRC(BowlerPacket *Packet) {
    uint8_t v = CalcDataCRC(Packet);
    if (GetDataCRC( Packet) == v)
        return true;
    return false;
}
/*
 * Calculates and sets the CRC in the packet
 */
void SetDataCRC(BowlerPacket * Packet) {
	//println_W("data CRC");
    uint8_t v = CalcDataCRC(Packet);

    Packet->stream[Packet->use.head.DataLegnth+_BowlerHeaderSize] = v;
}

/*
 * Calculates and sets the CRC in the packet
 */
uint8_t GetDataCRC(BowlerPacket * Packet) {
   return Packet->stream[Packet->use.head.DataLegnth+_BowlerHeaderSize] ;
}

inline unsigned long GetRPCValue(char * data) {
    UINT32_UNION l;
    l.byte.FB = data[3];
    l.byte.TB = data[2];
    l.byte.SB = data[1];
    l.byte.LB = data[0];
    return l.Val;
}

uint16_t READY(BowlerPacket *Packet, uint8_t code, uint8_t trace) {
    Packet->use.head.Method = BOWLER_STATUS;
    Packet->use.head.RPC = GetRPCValue("_rdy");
    Packet->use.head.MessageID = 0;
    Packet->use.head.DataLegnth = 6;
    Packet->use.data[0] = code;
    Packet->use.data[1] = trace;
    return 6;
}

uint16_t ERR(BowlerPacket *Packet, uint8_t code, uint8_t trace) {
    Packet->use.head.Method = BOWLER_STATUS;
    Packet->use.head.RPC = GetRPCValue("_err");
    Packet->use.head.MessageID = 0;
    Packet->use.head.DataLegnth = 6;
    Packet->use.data[0] = code;
    Packet->use.data[1] = trace;
    return 6;
}

uint16_t SetPacketLegnth(BowlerPacket *Packet, uint8_t len) {
    //Packet.stream[DataSizeIndex]=(BYTE)(len&0x00ff);
    Packet->use.head.DataLegnth = len;
    return len;
}

uint16_t GetPacketLegnth(BowlerPacket *Packet) {
    return _BowlerHeaderSize + Packet->use.head.DataLegnth+1;
}

uint16_t DataLegnthFromPacket(BowlerPacket *Packet) {
    return Packet->use.head.DataLegnth;
}

void copyPacket(BowlerPacket * from, BowlerPacket * to) {
    uint8_t i;
    for (i = 0; i < GetPacketLegnth(from); i++) {
        to->stream[i] = from->stream[i];
    }
}

uint32_t Bytes2Int32(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    return ( (((uint32_t) a) << 24) + (((uint32_t) b) << 16) + (((uint32_t) c) << 8) + ((uint32_t) d));
}

uint8_t CheckAddress(uint8_t * one, uint8_t * two) {
    int i;
    for (i = 0; i < 6; i++) {
        if (one[i] != two[i])
            return false;
    }
    return true;
}

float interpolate(INTERPOLATE_DATA * data, float currentTime) {
    float  totalDistance=0, elapsed=0,currentDistance=0, currentLocation=0;
    float set 			= data->set;
    float start 		= data->start;
    float setTime 		= data->setTime;
    float startTime 	= data->startTime;

    if(isnan(set)){
    	setPrintLevelErrorPrint();
    	println_E("set NaN");
    	return 0;
    }
    if(isnan(start)){
    	setPrintLevelErrorPrint();
    	println_W("start NaN");
    	return set;
    }
    if(isnan(setTime)){
    	setPrintLevelErrorPrint();
    	println_W("setTime NaN");
    	return set; // can not divide by zero
    }
    if(isnan(startTime)){
    	setPrintLevelErrorPrint();
    	println_W("startTime NaN");
    	return set;
    }
    if(isnan(currentTime)){
		setPrintLevelErrorPrint();
		println_W("currentTime NaN");
		return set;
	}

//

    // If the time is imediate, then the target should be returned no matter what.
    if(setTime<=0){
    	if(setTime<0){
    		println_E("FAIL, set time is negative");
			println_E("Time= ");p_fl_E(currentTime);
			print_W(" Set= ");p_fl_W(set);
			print_E(" start= ");p_fl_E(start);
			print_W(" setTime= ");p_fl_W(setTime);
			print_E(" startTime= ");p_fl_E(startTime);

			println_W("elapsedTime = ");p_fl_W(elapsed);
			print_E(" incremental distance = ");p_fl_E(currentDistance);
			print_W(" Target = ");p_fl_W(currentLocation);
    	}
    	return set;
    }

    elapsed = currentTime - (startTime);
    //interpolation is done
    if( elapsed < 0){
    	println_E("FAIL, elapsed time is negative");
		println_E("Time= ");p_fl_E(currentTime);
		print_W(" Set= ");p_fl_W(set);
		print_E(" start= ");p_fl_E(start);
		print_W(" setTime= ");p_fl_W(setTime);
		print_E(" startTime= ");p_fl_E(startTime);

		println_W("elapsedTime = ");p_fl_W(elapsed);
		print_E(" incremental distance = ");p_fl_E(currentDistance);
		print_W(" Target = ");p_fl_W(currentLocation);
		data->setTime=0;
        return set;
    }
    if(elapsed >= setTime ){
    	println_I("Interp Done");
    	data->setTime=0;
        return set;
    }

    totalDistance = set-start;

    // elapsed must always be greater than the set time, current distance will be lower then
    // total distance
    currentDistance = totalDistance * (elapsed/setTime);
    
    // location will be an offset from the start
    currentLocation = currentDistance+start;

    if(isnan(currentLocation)){
    	println_E("FAIL, setpoint calculated as NaN");
		println_E("Time= ");p_fl_E(currentTime);
		print_W(" Set= ");p_fl_W(set);
		print_E(" start= ");p_fl_E(start);
		print_W(" setTime= ");p_fl_W(setTime);
		print_E(" startTime= ");p_fl_E(startTime);

		println_W("elapsedTime = ");p_fl_W(elapsed);
		print_E(" incremental distance = ");p_fl_E(currentDistance);
		print_W(" Target = ");p_fl_W(currentLocation);
		data->setTime=0;
    	return set;
    }
   	if(between(set,currentLocation,start)){
   		//print_W(" interp = ");p_fl_W(currentLocation);
   		return currentLocation;
   	}else{
//    	println_E("FAIL, setpoint outside bounds!");
//		println_E("Time= ");p_fl_E(currentTime);
//		print_W(" Set= ");p_fl_W(set);
//		print_E(" start= ");p_fl_E(start);
//		print_W(" setTime= ");p_fl_W(setTime);
//		print_E(" startTime= ");p_fl_E(startTime);
//
//		println_W("elapsedTime = ");p_fl_W(elapsed);
//		print_E(" incremental distance = ");p_fl_E(currentDistance);
//		print_W(" Target = ");p_fl_W(currentLocation);
   	}
   	data->setTime=0;
   	return set;


}

boolean bound(float target, float actual, float plus, float minus) {
    return ((actual)<(target + plus) && (actual)>(target - minus));
}
boolean between(float targetupper, float actual, float targetLower) {
	if(targetupper>targetLower ){
		return (actual>targetLower) && (actual<targetupper) ;
	}else{
		return (actual<targetLower) && (actual>targetupper) ;
	}
}
void set8bit(BowlerPacket * Packet, uint8_t val, uint8_t offset) {
    Packet->use.data[0 + offset] = val;
}

void set16bit(BowlerPacket * Packet, int16_t val, uint8_t offset) {
    UINT16_UNION wval;
    wval.Val = val;
    Packet->use.data[0 + offset] = wval.byte.SB;
    Packet->use.data[1 + offset] = wval.byte.LB;
}

void set32bit(BowlerPacket * Packet, int32_t val, uint8_t offset) {
    INT32_UNION lval;
    lval.Val = val;
    Packet->use.data[0 + offset] = lval.byte.FB;
    Packet->use.data[1 + offset] = lval.byte.TB;
    Packet->use.data[2 + offset] = lval.byte.SB;
    Packet->use.data[3 + offset] = lval.byte.LB;
}

void setString(BowlerPacket * Packet, char * val, uint8_t offset) {
    int i=0;
    while(val[i]){
        Packet->use.data[offset + i] = val[i];
        i++;
        if(i+offset>=sizeof(BowlerPacket)){
            //println_E("Writing over the size of the packet!!");
            return;
        }
    }
    Packet->use.data[offset + i] = 0;
}

int32_t get32bit(BowlerPacket * Packet, uint8_t offset) {
    INT32_UNION lval;
    lval.byte.FB = Packet->use.data[0 + offset];
    lval.byte.TB = Packet->use.data[1 + offset];
    lval.byte.SB = Packet->use.data[2 + offset];
    lval.byte.LB = Packet->use.data[3 + offset];
    return lval.Val;
}

int32_t get16bit(BowlerPacket * Packet, uint8_t offset) {
    UINT16_UNION wval;
    wval.byte.SB = Packet->use.data[0 + offset];
    wval.byte.LB = Packet->use.data[1 + offset];
    return wval.Val;
}
