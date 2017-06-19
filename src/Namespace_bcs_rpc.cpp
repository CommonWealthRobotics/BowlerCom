#include <BowlerCom.h>
#if !defined(NULL)
#define NULL 0
#endif
//const char rpcNSName[] = "bcs.rpc.*;0.3;;";

boolean _rpc(BowlerPacket * Packet) {
	//printBowlerPacketDEBUG(&Packet,WARN_PRINT);
    Packet->use.head.Method = BOWLER_POST;
    Packet->use.head.RPC = GetRPCValue("_rpc");
    int index = 0;
    int nsIndex = Packet->use.data[index++];
    int rpcIndex = Packet->use.data[index++];

    Packet->use.data[index++] = getNumberOfRpcs(nsIndex);
    NAMESPACE_LIST * list = getNamespaceAtIndex(nsIndex);
    if (list == NULL) {
        ERR(Packet, 0, 9);
        return false;
    }
    RPC_LIST * rpc = getRpcByIndex(list, rpcIndex);
    if (rpc == NULL) {
        ERR(Packet, 0, 10);
        return false;
    }
    UINT32_UNION rpcValue;
    rpcValue.Val = GetRPCValue((char*) rpc->rpc);
    int i;
    for (i = 0; i < 4; i++) {
        Packet->use.data[index++] = rpcValue.v[i];
    }

    Packet->use.head.DataLegnth = 4 + index;
    //printBowlerPacketDEBUG(&Packet,ERROR_PRINT);
    return true;
}

boolean _rpcArgs(BowlerPacket * Packet) {
    Packet->use.head.Method = BOWLER_POST;
    Packet->use.head.RPC = GetRPCValue("args");
    int index = 0;
    int nsIndex = Packet->use.data[index++];
    int rpcIndex = Packet->use.data[index++];

    RPC_LIST * rpc = getRpcByIndex(getNamespaceAtIndex(nsIndex), rpcIndex);
    if (rpc == NULL) {
        ERR(Packet, 0, 10);
        return false;
    }
    //UINT32_UNION rpcValue;
    //rpcValue.Val = GetRPCValue((char*)rpc->rpc);
    int i;
    Packet->use.data[(index++)] = rpc->bowlerMethod;
    int argNumIndex = index;
    Packet->use.data[(index++)] = 0; // place holder for number of arguments
    if (rpc->arguments != NULL) {
        //add non NULL arguments
        i = 0;
        while (rpc->arguments[i] != 0) {
            Packet->use.data[(index++)] = rpc->arguments[i++];
            Packet->use.data[argNumIndex]++;
        }
    }
    //Packet->use.data[(index++)] = 0;

    Packet->use.data[(index++)] = rpc->responseMethod;
    argNumIndex = index;
    Packet->use.data[(index++)] = 0; // place holder for number of arguments
    if (rpc->responseArguments != NULL) {
        //add non NULL arguments
        i = 0;
        while (rpc->responseArguments[i] != 0) {
            Packet->use.data[(index++)] = rpc->responseArguments[i++];
            Packet->use.data[argNumIndex]++;
        }
    }
    //Packet->use.data[(index++)] = 0;

    Packet->use.head.DataLegnth = 4 + index;
    return true;
}
//Get RPC's
  RPC_LIST bcsRpc__RPC = {BOWLER_GET,
    "_rpc",
    &_rpc,
    {   BOWLER_I08,
        BOWLER_I08,
        0}, // Calling arguments
    BOWLER_POST, // response method
    {   BOWLER_I08,
        BOWLER_I08,
        BOWLER_I08,
        BOWLER_ASCII,
        0}, // Calling arguments
    NULL //Termination
};
//Get RPC's
  RPC_LIST bcsRpc_ARGS = {BOWLER_GET,
    "args",
    &_rpcArgs,
    {BOWLER_I08, //namespace index
        BOWLER_I08, //rpc index
        0}, // Calling arguments
    BOWLER_POST, // response method
   { BOWLER_I08, //namespace index
        BOWLER_I08, //rpc index
        BOWLER_I08, //Downstream method
        BOWLER_STR, //downstream arguments
        BOWLER_I08, //upstream method
        BOWLER_STR, //upstream arguments
        0}, // Calling arguments
    NULL //Termination
};


  NAMESPACE_LIST bcsRpc = {"bcs.rpc.*;0.3;;", // The string defining the namespace
    NULL, // the first element in the RPC list
    NULL, // async for this namespace
    NULL// no initial elements to the other namesapce field.
};



  boolean BcsRpcnamespcaedAdded = false;

NAMESPACE_LIST * getBcsRpcNamespace() {
    if (!BcsRpcnamespcaedAdded) {
        //GET
        addRpcToNamespace(&bcsRpc, & bcsRpc__RPC);
        addRpcToNamespace(&bcsRpc, & bcsRpc_ARGS);
        BcsRpcnamespcaedAdded = true;
    }

    return &bcsRpc;
}
