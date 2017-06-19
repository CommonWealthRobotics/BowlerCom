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

#define __WASP2_C

MAC_ADDR Broadcast = {
    {0, 0, 0, 0, 0, 0}
};
MAC_ADDR MyMAC = {
    {0x74, 0xf7, 0x26, 0x01, 0x01, 0x01}
};

boolean ignoreAddressing = true;

MAC_ADDR * getMac(){
	return &MyMAC;
}

void setIgnoreAddressing(boolean v){
	ignoreAddressing =v;
}

float lastPacketTime=0;

float getLastPacketTime() {
    if (lastPacketTime > getMs())
        lastPacketTime = 0;
    return lastPacketTime;
}

void Process_Self_Packet(BowlerPacket * Packet) {
    int namespaceIndex = 0;
    int foundRpc = 0;
    int currentNamespaceIndexForPacket = namespaceIndex;
    NAMESPACE_LIST * tmp = getBcsCoreNamespace();
    // First locate all Namespaces for the given RPC
    do {
        //Start the list with the first one
        RPC_LIST * rpc = getRpcByID(tmp, Packet->use.head.RPC, Packet->use.head.Method);
        //null check
        if (rpc != NULL) {
            //Found matching RPC and Method to parse
            foundRpc++;
            currentNamespaceIndexForPacket = namespaceIndex;
//            println_I("Rpc: ");
//            printfDEBUG_NNL((char *)rpc->rpc,INFO_PRINT);
//            print_I(" found in namespace: ");
//            printfDEBUG_NNL((char *)tmp->namespaceString,INFO_PRINT);
        }
        //Null check and move to next namespace
        tmp = tmp->next;
        namespaceIndex++;
    } while (tmp != NULL);
    // Now the namespace should have been found or not

    if (foundRpc == 0) {
        Print_Level l = getPrintLevel();
        setPrintLevelErrorPrint();
        println_E("##ERROR Rpc not found!");
        setPrintLevel(l);
        ERR(Packet, 0, 0);
        return;
    } else if (foundRpc > 0) {
        //println_I("Namespace found: ");print_I(getNamespaceAtIndex(currentNamespaceIndexForPacket)->namespaceString);
        if (foundRpc > 1) {
            if (Packet->use.head.MessageID == 0) {
                //RPC overlap but no resolution defined
                println_E("##ERROR Rpc not resolved! Multiple implementations");
                printPacket(Packet, ERROR_PRINT);
                ERR(Packet, 0, 1);
                return;
            } else {
                //RPC resolution is specified
                currentNamespaceIndexForPacket = Packet->use.head.MessageID;
                //println_I("Rpc resolved to: ");print_I(getNamespaceAtIndex(currentNamespaceIndexForPacket)->namespaceString);
            }
        }

        RPC_LIST * rpc = getRpcByID(getNamespaceAtIndex(currentNamespaceIndexForPacket),
                Packet->use.head.RPC,
                Packet->use.head.Method);

        if (rpc != NULL) {
            rpc->callback(Packet);
            Packet->use.head.DataLegnth = 4;
            if (rpc->responseArguments != 0) {
                int argIndex = 0;
                int dataIndex = 0;
                int packetDataStart;
                while (rpc->responseArguments[argIndex] != 0) {
                    switch (rpc->responseArguments[argIndex]) {
                        case BOWLER_ASCII:
                            dataIndex = 0;
                            packetDataStart = Packet->use.head.DataLegnth - 4;
                            while (Packet->use.data[packetDataStart + dataIndex++]) {
                                Packet->use.head.DataLegnth++;
                            }
                            Packet->use.head.DataLegnth++; // null terminator
                            break;
                        case BOWLER_BOOL:
                        case BOWLER_I08:
                            Packet->use.head.DataLegnth++;
                            break;
                        case BOWLER_I16:
                            Packet->use.head.DataLegnth += 2;
                            break;
                        case BOWLER_I32:
                        case BOWLER_FIXED100:
                        case BOWLER_FIXED1K:
                            Packet->use.head.DataLegnth += 4;
                            break;
                        case BOWLER_STR:
                            packetDataStart = Packet->use.head.DataLegnth - 4;
                            Packet->use.head.DataLegnth += Packet->use.data[packetDataStart] + 1;
                            break;
                        case BOWLER_I32STR:
                        case BOWLER_FIXED1K_STR:
                            packetDataStart = Packet->use.head.DataLegnth - 4;
                            Packet->use.head.DataLegnth += Packet->use.data[packetDataStart]*4 + 1;
                            break;
                    }
                    argIndex++;
                }
            }
        }
        Packet->use.head.MessageID = currentNamespaceIndexForPacket;
        Packet->use.head.ResponseFlag = 1;
        FixPacket(Packet);
        lastPacketTime = getMs();
    }


}//finish processing the Packet

void Bowler_Init(void) {
    addNamespaceToList( getBcsCoreNamespace());
    addNamespaceToList( getBcsRpcNamespace());
}

boolean process(BowlerPacket * Packet) {
    int i;
    //if(debug){
    if (Packet->use.head.RPC != GetRPCValue("_pwr") &&
            Packet->use.head.RPC != GetRPCValue("_png") &&
            Packet->use.head.RPC != GetRPCValue("_rpc") &&
                        Packet->use.head.RPC != GetRPCValue("_nms") &&
                        Packet->use.head.RPC != GetRPCValue("args")
            ) {//Ignore Power Packet
        b_println("Got:", INFO_PRINT);
        printPacket(Packet, INFO_PRINT);
    }
    //}
    if (    CheckAddress(MyMAC.v, Packet->use.head.MAC.v) == true ||
            CheckAddress(Broadcast.v, Packet->use.head.MAC.v) == true||
            ignoreAddressing ==true
            ) {
        Process_Self_Packet(Packet);
        for (i = 0; i < 6; i++) {
            Packet->use.head.MAC.v[i] = MyMAC.v[i];
        }
        SetCRC(Packet);
        return true;
    } else {

        b_println("Packet not addressed to me: ", ERROR_PRINT);
        printByteArray(Packet->use.head.MAC.v, 6, ERROR_PRINT);
        print_nnl(" is not mine: ", ERROR_PRINT);
        printByteArray(MyMAC.v, 6, ERROR_PRINT);

    }
    return false;
}

/**
 * Run an instance of the server. This uses user defined memory
 */
uint8_t Bowler_Server_Static(BowlerPacket * Packet, BYTE_FIFO_STORAGE * fifo) {

    boolean back = GetBowlerPacket(Packet, fifo);

    if (back) {
        return process(Packet);
        ;
    }//Have a packet

    return false;
}

uint8_t Bowler_Server(BowlerPacket * Packet, boolean debug) {

    boolean back = GetBowlerPacket_arch(Packet);

    if (back) {
        //SetColor(0, 1, 0);
        if (process(Packet)) {
            //Packet found, sending
            PutBowlerPacket(Packet);
            if (Packet->use.head.RPC != GetRPCValue("_pwr") &&
                    Packet->use.head.RPC != GetRPCValue("_png")&&
                    Packet->use.head.RPC != GetRPCValue("_rpc") &&
                                        //Packet->use.head.RPC != GetRPCValue("_nms") &&
                                        Packet->use.head.RPC != GetRPCValue("args")
                    ) {//Ignore Power Packet
                b_println("Response:", INFO_PRINT);
                printPacket(Packet, INFO_PRINT);
            }
            //SetColor(0, 0, 1);
            return true;
        }
    }//Have a packet

    return false;
}









