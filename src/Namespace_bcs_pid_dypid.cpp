/**
 * @file Namespace_bcs_pid_dypid.c
 *
 * Created on: Mar 16, 2013
 * @author hephaestus
 */
#include <BowlerCom.h>
//char dypidNSName[] = "bcs.pid.dypid.*;0.3;;";
RunEveryData pid = {0, 5};
RunEveryData vel = {0, 100};

boolean bcsPidDypidAsyncEventCallback(BowlerPacket *Packet, boolean(*pidAsyncCallbackPtr)(BowlerPacket *Packet)) {
   //println_I("DyPID Async ");
    if (RunEvery(&pid) > 0) {
        //clearPrint();
        RunPIDControl();
//        Print_Level l = getPrintLevel();
//	setPrintLevelInfoPrint();
//        printPIDvals(0);
//        setPrintLevel(l);
    }
    //println_I("Run Velocity ");
#if defined(USE_VELOCITY)
    if (RunEvery(&vel) > 0) {
        RunVel();
    }
#endif

    //println_W("Done ");//print_W(dypidNSName);
    return false; 
}

boolean bcsPidDypidProcessor_g(BowlerPacket * Packet) {

            GetConfigDyPID(Packet);

    return true; 
}

boolean bcsPidDypidProcessor_c(BowlerPacket * Packet) {

	if (ConfigDyPID(Packet)) {
		READY(Packet, 45, 1);
	} else
		ERR(Packet, 45, 1);


    return true; 
}

RPC_LIST bcsPidDypid_dpid_g = {BOWLER_GET, // Method
    "dpid", //RPC as string
    &bcsPidDypidProcessor_g, //function pointer to a packet parsinf function
    {
        BOWLER_I08, // group
        0
    }, // Calling arguments
    BOWLER_POST, // response method
    {
        BOWLER_I08, // group
        BOWLER_I08, // input channel
        BOWLER_I08, // input mode
        BOWLER_I08, // output channel
        BOWLER_I08, // output mode
        0
    }, // Calling arguments
    NULL //Termination
};

RPC_LIST bcsPidDypid_dpid_c = {BOWLER_CRIT, // Method
    "dpid", //RPC as string
    &bcsPidDypidProcessor_c, //function pointer to a packet parsinf function
      {
        BOWLER_I08, // group
        BOWLER_I08, // input channel
        BOWLER_I08, // input mode
        BOWLER_I08, // output channel
        BOWLER_I08, // output mode
        0
    }, // Calling arguments
    BOWLER_POST, // response method
     {
        0
    }, // Calling arguments
    NULL //Termination
};

NAMESPACE_LIST bcsPidDypid = {"bcs.pid.dypid.*;0.3;;", // The string defining the namespace
    NULL, // the first element in the RPC list
    &bcsPidDypidAsyncEventCallback, // async for this namespace
    NULL// no initial elements to the other namesapce field.
};

boolean bcsPidDypidnamespcaedAdded = false;

NAMESPACE_LIST * get_bcsPidDypidNamespace() {
    if (!bcsPidDypidnamespcaedAdded) {
        //POST
        //Add the RPC structs to the namespace
        addRpcToNamespace(&bcsPidDypid, & bcsPidDypid_dpid_g);
        addRpcToNamespace(&bcsPidDypid, & bcsPidDypid_dpid_c);

        bcsPidDypidnamespcaedAdded = true; 
    }

    return &bcsPidDypid; //Return pointer to the struct
}
