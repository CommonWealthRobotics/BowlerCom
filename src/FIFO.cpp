/**
 * @file FIFO.c
 *
 * Created on: May 27, 2010
 * @author hephaestus
 */
#include <BowlerCom.h>
void clearByteFifo(BYTE_FIFO_STORAGE * fifo);

//const char * fifoinit = "Initializing FIFO, Buffer Size: ";
//const char * error = "@##ERROR fifo is overflown! Size, buffer: ";

void printFiFoState(BYTE_FIFO_STORAGE * fifo,Print_Level l){
	int i;
	print_nnl("\nFifo state: \tBytes:",l);
	p_int(calcByteCount(fifo),l);

	print_nnl("\tData [ ",l);
	for(i=0;i<calcByteCount(fifo);i++){
		p_int(FifoReadByteAtIndex(fifo,i),l);print_nnl(" ",l);
	}
	print_nnl(" ]\n",l);
}

boolean lockFifo(BYTE_FIFO_STORAGE * fifo){
//	if(fifo->mutex != false) {
//		return false; 
//	}
//	fifo->mutex=true; 

	return true; 
}
boolean unLockFifo(BYTE_FIFO_STORAGE * fifo){
	//fifo->mutex=false; 
	//EndCritical();
	return true; 
}
void InitByteFifo(BYTE_FIFO_STORAGE * fifo,uint8_t * buff,uint32_t size){
	if(fifo == 0 ||  buff == 0){
		b_println("@#@#FIFO FAILED TO INITIALIZE",ERROR_PRINT);p_int(size,ERROR_PRINT);
	}
	fifo->buffer=buff;
	fifo->bufferSize=size;
	fifo->readPointer=0;
	fifo->writePointer=0;
	//fifo->byteCount=0;
	fifo->mutex=false; 
	clearByteFifo(fifo);
	//b_println(fifoinit);p_int(size);
}
void clearByteFifo(BYTE_FIFO_STORAGE * fifo){
	int i;
	for (i=0;i < fifo->bufferSize;i++){
		fifo->buffer[i]=0;
	}
	//EndCritical();
}

uint8_t ReadFirstByte(BYTE_FIFO_STORAGE * fifo){
	return fifo->buffer[fifo->readPointer];
}

uint32_t calcByteCount(BYTE_FIFO_STORAGE * fifo){
	int w =fifo->writePointer;
	int r = fifo->readPointer;
	if(w>r){
		fifo->byteCount= w-r;
	}else if(w==r){
		fifo->byteCount= 0;
	}else{
		fifo->byteCount= (w+fifo->bufferSize)-r;
	}
	return fifo->byteCount;
}

uint32_t FifoGetByteCount(BYTE_FIFO_STORAGE * fifo){
	return calcByteCount(fifo);

}

uint32_t FifoAddByte(BYTE_FIFO_STORAGE * fifo,uint8_t b, uint8_t * errorCode){
//	if(calcByteCount(fifo) >= (fifo->bufferSize-2)){
//		//b_println(error);p_int(fifo->bufferSize);print_nnl(",");p_int(fifo->byteCount);
//		errorCode[0]=FIFO_OVERFLOW;
//		return 0;
//	}
//	if(lockFifo(fifo)==false) {
//		errorCode[0]=FIFO_FAILED_TO_SET;
//		//return 0;
//	}
//        if(fifo->buffer == NULL){
//            setPrintLevelErrorPrint();
//            println_E("Null buffer in fifo call");
//            //return 0;
//        }

	fifo->buffer[fifo->writePointer]=b;
	fifo->writePointer++;
	if (fifo->writePointer == fifo->bufferSize){
		fifo->writePointer=0;
	}
	//fifo->byteCount++;
	//calcByteCount(fifo);
	errorCode[0]=FIFO_OK;
	//unLockFifo(fifo);
	return (calcByteCount(fifo)<=fifo->bufferSize);
}

uint8_t FifoGetByte(BYTE_FIFO_STORAGE * fifo, uint8_t * errorCode){
	if(lockFifo(fifo)==false) {
		errorCode[0]=FIFO_FAILED_TO_GET;
		return 0;
	}
	uint8_t b = fifo->buffer[fifo->readPointer];
	fifo->readPointer++;
	if (fifo->readPointer==fifo->bufferSize){
		fifo->readPointer=0;
	}
	errorCode[0]=FIFO_OK;
	calcByteCount(fifo);
	unLockFifo(fifo);
	return b;
}

uint8_t FifoReadByteAtIndex(BYTE_FIFO_STORAGE * fifo,uint32_t offset ){


	uint32_t index=fifo->readPointer +offset;
	if(index >=fifo->bufferSize )
		index -= fifo->bufferSize;

	return fifo->buffer[index];
}


uint32_t FifoGetByteStream(BYTE_FIFO_STORAGE * fifo,uint8_t *packet,uint32_t size){
	uint8_t err;
	int i;
	int count = calcByteCount(fifo);
	if(size>count)
		size = count;
	for (i=0;i<size;i++){
		if(count > 0){
			uint8_t b;
			do{
				b = FifoGetByte(fifo,&err);
			}while(err != FIFO_OK);
			packet[i] = b;
		}else{
			packet[i]=0;
		}
	}
	return i;
}


uint32_t FifoReadByteStream(uint8_t *packet,uint32_t size,BYTE_FIFO_STORAGE * fifo){
	uint32_t read=fifo->readPointer;
	uint32_t  count = calcByteCount(fifo);
	int i;
	if(size>count)
		size = count;
	for (i=0;i<size;i++){
		if(count > 0){
			packet[i] = fifo->buffer[read];
			read++;
			if (read==fifo->bufferSize){
				read=0;
			}
		}else{
			packet[i]=0;
		}
	}
	return i;
}


void InitPacketFifo(PACKET_FIFO_STORAGE * fifo,BowlerPacket * buff,uint32_t size){
    	if(fifo == 0 ||  buff == 0){
		b_println("@#@#FIFO FAILED TO INITIALIZE",ERROR_PRINT);p_int(size,ERROR_PRINT);
	}
	fifo->buffer=buff;
	fifo->bufferSize=size;
	fifo->readPointer=0;
	fifo->writePointer=0;
	//fifo->byteCount=0;
	fifo->mutex=false; 
}

uint32_t FifoAddPacket(PACKET_FIFO_STORAGE * fifo,BowlerPacket * toBeAdded){
    	if(FifoGetPacketCount(fifo) >= (fifo->bufferSize)){
		println_E("Packet FIFO overflow");p_int_E(fifo->bufferSize);print_E(",");p_int_E(fifo->byteCount);
		return 0;
	}

        copyPacket(toBeAdded,&fifo->buffer[fifo->writePointer]);

	fifo->writePointer++;
	if (fifo->writePointer == fifo->bufferSize){
		fifo->writePointer=0;
	}
	//fifo->byteCount++;
	FifoGetPacketCount(fifo);
        
        //EndCritical();
	return (FifoGetPacketCount(fifo)<=fifo->bufferSize);
}

uint32_t FifoGetPacketCount(PACKET_FIFO_STORAGE * fifo){
	int w =fifo->writePointer;
	int r = fifo->readPointer;
	if(w>r){
		fifo->byteCount= w-r;
	}else if(w==r){
		fifo->byteCount= 0;
	}else{
		fifo->byteCount= (w+fifo->bufferSize)-r;
	}
	return fifo->byteCount;
}

uint32_t FifoGetPacketSpaceAvailible(PACKET_FIFO_STORAGE * fifo){
        return fifo->bufferSize - FifoGetPacketCount(fifo)-1;
}

uint32_t FifoGetPacket(PACKET_FIFO_STORAGE * fifo,BowlerPacket * retrived){

        if(FifoGetPacketCount(fifo)>0)
            copyPacket(&fifo->buffer[fifo->readPointer],retrived);
        else
            return false; 
	fifo->readPointer++;
	if (fifo->readPointer==fifo->bufferSize){
		fifo->readPointer=0;
	}

	FifoGetPacketCount(fifo);
	//EndCritical();
	return true; 
}
