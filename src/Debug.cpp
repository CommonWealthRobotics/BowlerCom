/**
 * "getPidGroupDataTable(i)->"
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
void sendStr(const char *str);
Print_Level level = NO_PRINT;

#define bufferSize 10

boolean DebugINIT = false;
const char AsciiHex[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
//#if !defined(NO_PRINTING)
//const char  packet[] = "\tPacket = \t";
//const char get[] = "Get";
//const char post[]="Post ";
//const char stat[]= "Status";
//const char crit[]="Critical";
//const char unknown[] = "Unknown ";
//const char ver[] ="\tVersion = \t";
//const char mac[] = "\tMAC = \t\t";
//const char meth[] = "\tMethod = \t";
//const char id[] = "\tNamespace Index = \t";
//const char dataSise[] ="\tData Size = \t";
//const char crcval []= "\tCRC Value = \t";
//	const char dval[] = "\tData = \t\t";
//	const char rpc []="\tRPC code = \t";
//	const char nodata[] = " no data";
//const char streamsize[] = " Stream: size=";
//#endif

const char errorColor[] = "\033[31m";
const char infoColor[] = "\033[94m";
const char warningColor[] = "\033[93m";
const char debugColor[]="\033[95m ";
const char clearErrorColor[] = "\033[39m";
int (*sendToStream)(uint8_t *, int);

void setColor(Print_Level l) {
    switch (l) {
        case NO_PRINT:
            return;
        case INFO_PRINT:
            sendStr(infoColor);
            return;
        case WARN_PRINT:
            sendStr(warningColor);
            return;
         case DEBUG_PRINT:
            sendStr(debugColor);
            return;
        case ERROR_PRINT:
            sendStr(errorColor);
            return;
        default:
            return;
    }
}

int sendToStreamMine(uint8_t * data, int num) {
    int i;
    i = 0;
    if (num >= bufferSize) {
        num = bufferSize - 1;
    }
    for (i = 0; i < num; i++) {

        putCharDebug(data[i]);
    }
    return i;
}

void setPrintStream(int (*sendToStreamPtr)(uint8_t *, int)) {
    if (sendToStreamPtr == 0) {
        sendToStreamPtr = &sendToStreamMine;
    }
    sendToStream = sendToStreamPtr;
}

void sendToStreamLocal(uint8_t * data, int num) {
    if (sendToStream == 0) {
        sendToStream = &sendToStreamMine;
    }
    sendToStream(data, num);
}

Print_Level getPrintLevel() {
    return level;
}

Print_Level setPrintLevel(Print_Level l) {
#if defined(NO_PRINTING)
    level = NO_PRINT;
#else
    level = l;
#endif
    return getPrintLevel();
}

boolean okToprint(Print_Level l) {

    if (getPrintLevel() >= l) {
        if (DebugINIT == false) {
            DebugINIT = true;
            EnableDebugTerminal();
        }
        return true;
    }

    return false;
}
//
//void enableDebug(void){
//	setPrintLevel(INFO_PRINT);
//}
//void disableDebug(void){
//	setPrintLevel(NO_PRINT);
//}

char GetLowNib(uint8_t b) {
    return AsciiHex[b & 0x0f];
}

char GetHighNib(uint8_t b) {
    return AsciiHex[((b & 0xF0) >> 4)];
}

void printfDEBUG(char *str, Print_Level l) {
    if (!okToprint(l)) {
        return;
    }

    putCharDebug('\n');
    putCharDebug('\r');
    printfDEBUG_NNL(str, l);
    //sendToStreamLocal(data,i);
}

void sendStr(const char *str) {
    int x = 0;
    while (str[x] != '\0') {
        putCharDebug(str[x++]);
    }
}

void printfDEBUG_BYTE(char b, Print_Level l) {
    if (!okToprint(l)) {
        return;
    }
    setColor(l);
    putCharDebug(b);
    //sendToStreamLocal((uint8_t *)&b,1);
}

void printfDEBUG_NNL(char *str, Print_Level l) {
    if (!okToprint(l)) {
        return;
    }
    setColor(l);
    sendStr(str);
    //sendToStreamLocal(data,i);
}

void printfDEBUG_INT(int32_t val, Print_Level l) {
    if (!okToprint(l)) {
        return;
    }
    setColor(l);
    int x;
    x = 0;
    if (val < 0) {
        val *= -1;
        putCharDebug('-');
    }
    uint8_t byteStr[12];
    ultoaMINE(val, byteStr);
    while (byteStr[x] != '\0') {
        putCharDebug(byteStr[x++]);
        if (x == 12) {
            if (l == ERROR_PRINT) {
                sendStr(clearErrorColor);
            }
            return;
        }
    }
    //sendToStreamLocal(data,i);
}

void printfDEBUG_FL(float f, Print_Level l) {
    if (!okToprint(l)) {
        return;
    }
    if(isnan(f)){
    	print_nnl("NaN", l);
    	return;
    }
    int32_t upper = (int32_t) f; // set up the upper section of the decimal by int casting to clip  off the decimal places
    int32_t shift = (int32_t) (f * 1000.0f); //shift up the decaml places as a float 3 places
    int32_t clip = upper * 1000; //clip off the upper section of the decimal
    printfDEBUG_INT(upper, l);
    printfDEBUG_BYTE('.', l);
    int32_t dec = shift - clip;
    //make positive and print zeros
    if (dec < 0)
        dec *= -1;
    if (dec < 100)
        printfDEBUG_BYTE('0', l);
    if (dec < 10)
        printfDEBUG_BYTE('0', l);
    printfDEBUG_INT(dec, l);
}

#if defined(BOWLERSTRUCTDEF_H_)

void printPIDvalsPointer(AbsPID * conf) {


    print_nnl(" Enabled=", INFO_PRINT);
    p_int(conf->config.Enabled, INFO_PRINT);
    print_nnl(" Polarity=", INFO_PRINT);
    p_int(conf->config.Polarity, INFO_PRINT);
    print_nnl(" SET=", INFO_PRINT);
    p_int(conf->SetPoint, INFO_PRINT);
    print_nnl(" Kp=", INFO_PRINT);
    p_fl(conf->config.K.P, INFO_PRINT);
    print_nnl(" Ki=", INFO_PRINT);
    p_fl(conf->config.K.I, INFO_PRINT);
    print_nnl(" Kd=", INFO_PRINT);
    p_fl(conf->config.K.D, INFO_PRINT);
    print_nnl("\t Setpoint=", INFO_PRINT);
    p_fl(conf->SetPoint, INFO_PRINT);
    print_nnl("\t Current State=", INFO_PRINT);
    p_fl(conf->CurrentState, INFO_PRINT);
    print_nnl("\t Offset=", INFO_PRINT);
    p_fl(conf->config.offset, INFO_PRINT);
    print_nnl("\t Control Output: ", INFO_PRINT);
    p_fl(conf->Output, INFO_PRINT);
    print_nnl("\t Output Set: ",INFO_PRINT); p_fl(conf->OutputSet ,INFO_PRINT);

}

//void printPIDvals(int i) {
//
//    b_println("PID chan=", INFO_PRINT);
//    p_int(i, INFO_PRINT);
//    printPIDvalsPointer(getPidGroupDataTable(i));
//}

void printBowlerPacketDEBUG(BowlerPacket * Packet, Print_Level l) {
#if !defined(NO_PRINTING)
    if (!okToprint(l)) {
        return;
    }
    int i;
    uint8_t s;
    b_println("\tPacket = \t", l);
    s = GetPacketLegnth(Packet);
    printfDEBUG_BYTE('[', l);
    for (i = 0; i < s; i++) {
    	//print_nnl("0x", l);
        prHEX8(Packet->stream[i], l);
//        print_nnl(" ( ", l);
//        p_int(Packet->stream[i],l);
//        print_nnl(" ) ", l);
        if (i < s - 1)
            printfDEBUG_BYTE(',', l);
    }
    printfDEBUG_BYTE(']', l);
    b_println("\tVersion = \t\t0x", l);
    prHEX8(Packet->stream[0], l);
    b_println("\tMAC = \t\t\t", l);
    for (i = 0; i < 6; i++) {
    	//print_nnl("0x", l);
        prHEX8(Packet->stream[1 + i], l);
        if (i < 5)
            printfDEBUG_BYTE(':', l);
    }
    b_println("\tMethod = \t\t", l);
    switch (Packet->stream[MethodIndex]) {
        case BOWLER_STATUS:
            print_nnl(" Status", l);
            break;
        case BOWLER_GET:
            print_nnl(" Get", l);
            break;
        case BOWLER_POST:
            print_nnl(" Post ", l);
            break;
        case BOWLER_CRIT:
            print_nnl(" Critical", l);
            break;
        case BOWLER_ASYN:
            print_nnl("ASYNCHRONUS", l);
            break;
        default:
            print_nnl("Unknown 0x", l);
            prHEX8(Packet->stream[MethodIndex], l);
            break;
    }
    b_println("\tNs Index = \t", l);
    p_int((Packet->stream[SessionIDIndex]&0x7f), l);
    b_println("\tSize = \t\t", l);
    p_int((Packet->stream[DataSizeIndex]), l);

    //CRC Value
    b_println("\tCRC Value = \t\t0x", l);
    prHEX8((Packet->stream[CRCIndex]), l);
    b_println("\tC CRC = \t0x", l);
    prHEX8(CalcCRC(Packet), l);

    //Data CRC
    b_println("\tData CRC = \t0x", l);
    prHEX8(GetDataCRC(Packet), l);
    b_println("\tC Data CRC = \t0x", l);
    prHEX8(CalcDataCRC(Packet), l);

    if (Packet->use.head.DataLegnth >= 4) {
        b_println("\tRPC code = \t\t", l);
        for (i = 0; i < 4; i++) {
            printfDEBUG_BYTE(Packet->stream[RPCCodeStart + i], l);
        }
    }
    print_nnl(" ( ", l);
    prHEX32(Packet->use.head.RPC,l);
	print_nnl(" ) ", l);
    if (Packet->use.head.DataLegnth > 4) {
        s = (Packet->use.head.DataLegnth - 4);
        b_println("\tData = \t\t\t", l);
        for (i = 0; i < s; i++) {
            prHEX8(Packet->use.data[i], l);
            if (i < (s - 1))
                printfDEBUG_BYTE(',', l);
        }
    } else {
        b_println(" no data", l);
    }

    b_println("\n", l);
#endif
}
#endif

void printByteArray(uint8_t * stream, uint16_t len, Print_Level l) {
    //#if !defined(NO_PRINTING)
    if (!okToprint(l)) {
        return;
    }
    uint16_t i;
    print_nnl(" Stream: size=", l);
    p_int(len, l);
    print_nnl(" [", l);
    for (i = 0; i < len; i++) {
        prHEX8(stream[i], l);
        if (i < (len - 1))
            printfDEBUG_BYTE(',', l);
    }
    print_nnl("]", l);
    //#endif
}

void ultoaMINE(uint32_t Value, uint8_t* Buffer) {
    uint8_t i;
    uint32_t Digit;
    uint32_t Divisor;
    boolean Printed = false;

    if (Value) {
        for (i = 0, Divisor = 1000000000; i < 10; i++) {
            Digit = Value / Divisor;
            if (Digit || Printed) {
                *Buffer++ = '0' + Digit;
                Value -= Digit*Divisor;
                Printed = true;
            }
            Divisor /= 10;
        }
    } else {
        *Buffer++ = '0';
    }

    *Buffer = 0; //null terminator
}
