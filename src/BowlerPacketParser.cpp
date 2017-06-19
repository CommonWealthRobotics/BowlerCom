/*
 * Bowler_Transport.c
 *
 *  Created on: May 27, 2010
 *      Author: hephaestus
 */
#include <BowlerCom.h>
//#define minSize ((BowlerHeaderSize)+4)
#define minSize 1

void allign(BowlerPacket * Packet,BYTE_FIFO_STORAGE * fifo){

	int first = 0;
	do
	{
		FifoReadByteStream(Packet->stream,1,fifo);// peak but dont read yet
		if((Packet->use.head.ProtocolRevision != BOWLER_VERSION)){
			if(first==0){
				b_println("bad first byte. Fifo=",INFO_PRINT);  // SPI ISR shits out messages when 0xAA fails to match. making this info.
				p_int(calcByteCount(fifo),INFO_PRINT);
				print_nnl(" [",INFO_PRINT);
			}
			first++;
			print_nnl(" 0x",INFO_PRINT);
			prHEX8(Packet->use.head.ProtocolRevision,INFO_PRINT);
			uint8_t b;
			if(getNumBytes(fifo)==0)
				return;
			//StartCritical();
			FifoGetByte(fifo,& b);// dropping bad byte
			//EndCritical();

		}
	}while(getNumBytes(fifo)>0 && (Packet->use.head.ProtocolRevision != BOWLER_VERSION));
	if(first>0){
		b_println("] ##Junked total:",INFO_PRINT);p_int(first,INFO_PRINT);
	}
}

boolean _getBowlerPacket(BowlerPacket * Packet,BYTE_FIFO_STORAGE * fifo, boolean debug){
	boolean PacketCheck=false; 
	//uint16_t PacketLegnth=0;
        Packet->stream[0]=0;
	if (getNumBytes(fifo) == 0 ) {
		return false; //Not enough bytes to even be a header, try back later
	}

	allign(Packet,fifo);

	if (getNumBytes(fifo) < ((_BowlerHeaderSize)+4)) {
		if(debug){
			//b_println("Current num bytes: ",ERROR_PRINT);p_int(getNumBytes(fifo),ERROR_PRINT);
		}
		return false; //Not enough bytes to even be a header, try back later
	}
	FifoReadByteStream(Packet->stream,_BowlerHeaderSize,fifo);
	PacketCheck=false; 
	while(PacketCheck==false) {
		if( (Packet->use.head.ProtocolRevision != BOWLER_VERSION)
				|| (CheckCRC(Packet)==false) 

		  ){
			if(Packet->use.head.ProtocolRevision != BOWLER_VERSION){
				b_println("first",ERROR_PRINT);
			}else if(CheckCRC(Packet)==false) {
				b_println("crc",ERROR_PRINT);
			}
			//prHEX8(Packet->use.head.ProtocolRevision,ERROR_PRINT);print_nnl(" Fifo Size=",ERROR_PRINT);p_int(calcByteCount(fifo),ERROR_PRINT);
			uint8_t b;
			if(getNumBytes(fifo)==0)
				return false; 
			//StartCritical();
			getStream(& b,1,fifo);//junk out one
			//EndCritical();
			FifoReadByteStream(Packet->stream,_BowlerHeaderSize,fifo);
		}else{
			if(debug){
				//b_println("Got header");
			}
			PacketCheck=true; 
		}
		if (getNumBytes(fifo) < minSize) {
			b_println("allign packet",ERROR_PRINT);
			allign(Packet,fifo);
			return false; //Not enough bytes to even be a header, try back later
		}
	}
	//PacketLegnth  = Packet->use.head.DataLegnth;

	uint16_t totalLen = GetPacketLegnth(Packet);
	// See if all the data has arived for this packet
	int32_t num = getNumBytes(fifo);
	if (num >=(totalLen) ){
		if(debug){
			//b_println("**Found packet, ");p_int(totalLen);//print_nnl(" Bytes, pulling out of buffer");
		}
		//StartCritical();
		getStream(Packet->stream,totalLen,fifo);
		//EndCritical();
		if(CheckDataCRC(Packet)){
			return  true;
		}else{
			println_E("Data CRC Failed ");printBowlerPacketDEBUG(Packet,ERROR_PRINT);
		}
	}
	if(debug){
		//b_println("Header ready, but data is not. Need: ",INFO_PRINT);p_int(totalLen,INFO_PRINT);print_nnl(" have: ",INFO_PRINT);p_int(num ,INFO_PRINT);
	}
	return false; 
}
boolean GetBowlerPacket(BowlerPacket * Packet,BYTE_FIFO_STORAGE * fifo){
	return _getBowlerPacket(Packet,fifo, false) ;
}
boolean GetBowlerPacketDebug(BowlerPacket * Packet,BYTE_FIFO_STORAGE * fifo){
	//enableDebug();
	return _getBowlerPacket( Packet, fifo, true) ;
}
/**
 * @return returns the number of bytes in the fifo
 */
uint16_t getNumBytes(BYTE_FIFO_STORAGE * fifo){
	return (uint16_t)calcByteCount(fifo);
}
/**
 * get a stream of this length from the connection
 */
uint16_t getStream(uint8_t *packet,uint16_t size,BYTE_FIFO_STORAGE * fifo){
	return FifoGetByteStream(fifo,packet,size);
}

void FixPacket(BowlerPacket * Packet){
	extern MAC_ADDR MyMAC;
	uint8_t i;
	//Ensure the packet going upstream is valid
	for (i=0;i<6;i++){
		Packet->use.head.MAC.v[i]=MyMAC.v[i];
	}
	Packet->use.head.ProtocolRevision=BOWLER_VERSION;
	SetCRC(Packet);
	SetDataCRC(Packet);
}

boolean PutBowlerPacket(BowlerPacket * Packet){
	Packet->use.head.ResponseFlag=1;
	FixPacket(Packet);
//	if(Packet->use.head.Method ==BOWLER_ASYN)
//		printPacket(Packet,WARN_PRINT);
	return putStream(Packet->stream,GetPacketLegnth(Packet));
}
