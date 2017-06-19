/*
 * Namespace_bcs_pid.c
 *
 *  Created on: Mar 7, 2013
 *      Author: hephaestus
 */

#include <BowlerCom.h>

//const char pidNSName[]   = "bcs.pid.*;1.0;;";

boolean pidAsyncEventCallbackLocal(BowlerPacket *Packet, boolean(*pidAsyncCallbackPtr)(BowlerPacket *Packet)) {

    //    println_I("\n");
    //    printPIDvals(0);
	//println_I("PID COms");
    RunPIDComs(Packet, pidAsyncCallbackPtr);
    return false;
}

//Get RPC's
  RPC_LIST bcsPid_APID = {BOWLER_GET,
    "apid",
    &processPIDGet,
     {0}, // Calling arguments
    BOWLER_POST, // response method
     { BOWLER_I32STR, //All current values
        0}, // Calling arguments
    NULL //Termination
};
  RPC_LIST bcsPid__PID = {BOWLER_GET,
    "_pid",
    &processPIDGet,
   { BOWLER_I08, //channel
        0}, // Calling arguments
    BOWLER_POST, // response method
     { BOWLER_I08, //channel
        BOWLER_I32, //current position
        0}, // Calling arguments
    NULL //Termination
};
  RPC_LIST bcsPid_CPID = {BOWLER_GET,
    "cpid",
    &processPIDGet,
    { BOWLER_I08, //channel
        0}, // Calling arguments
    BOWLER_POST, // response method
     { BOWLER_I08, //channel
        BOWLER_I08, //enabled
        BOWLER_I08, //Polarity
        BOWLER_I08, //async
        BOWLER_FIXED100, //Kp
        BOWLER_FIXED100, //Ki
        BOWLER_FIXED100, //Kd
        BOWLER_I32, //Latch value
        BOWLER_I08, //Use index latch
        BOWLER_I08, //Stop On latch
        BOWLER_FIXED1K, //stop
        BOWLER_FIXED1K, //upper
        BOWLER_FIXED1K, //lower
        0}, // Calling arguments
    NULL //Termination
};
  RPC_LIST bcsPid_CPDV = {BOWLER_GET,
    "cpdv",
    &processPIDGet,
    { BOWLER_I08, //channel
        0}, // Calling arguments
    BOWLER_POST, // response method
     { BOWLER_I08, //channel
        BOWLER_FIXED100, //Kp
        BOWLER_FIXED100, //Kd
        0}, // Calling arguments
    NULL //Termination
};
  RPC_LIST bcsPid_GPDC = {BOWLER_GET,
    "gpdc",
    &processPIDGet,
    {0}, // Calling arguments
    BOWLER_POST, // response method
    { BOWLER_I32, //channel
        0}, // Calling arguments
    NULL //Termination
};

//Post RPC's
  RPC_LIST bcsPid_APID_p = {BOWLER_POST,
    "apid",
    &processPIDPost,
    { BOWLER_I32, //Time in ms for transition to take
        BOWLER_I32STR, //All current set point values
        0}, // Calling arguments
    BOWLER_STATUS, // response method
    { BOWLER_I08, //location
        BOWLER_I08, //trace
        0}, // Calling arguments, // Calling arguments
    NULL //Termination
};
  RPC_LIST bcsPid__PID_p = {BOWLER_POST,
    "_pid",
    &processPIDPost,
    { BOWLER_I08, //channel
        BOWLER_I32, //set point value
        BOWLER_I32, //Time in ms for transition to take
        0}, // Calling arguments, // Calling arguments
    BOWLER_STATUS, // response method
    { BOWLER_I08, //location
        BOWLER_I08, //trace
        0}, // Calling arguments, // Calling arguments
    NULL //Termination
};
  RPC_LIST bcsPid__VPD = {BOWLER_POST,
    "_vpd",
    &processPIDPost,
    { BOWLER_I08, //channel
        BOWLER_I32, //velocity set point value
        BOWLER_I32, //Time in ms for transition to take
        0}, // Calling arguments, // Calling arguments
    BOWLER_STATUS, // response method
     { BOWLER_I08, //location
        BOWLER_I08, //trace
        0}, // Calling arguments, // Calling arguments
    NULL //Termination
};
  RPC_LIST bcsPid_RPID = {BOWLER_POST,
    "rpid",
    &processPIDPost,
     { BOWLER_I08, //channel
        BOWLER_I32, //value to reset encoding to
        0}, // Calling arguments, // Calling arguments
    BOWLER_STATUS, // response method
    { BOWLER_I08, //location
        BOWLER_I08, //trace
        0}, // Calling arguments, // Calling arguments
    NULL //Termination
};

  RPC_LIST bcsPid_ACAL = {BOWLER_CRIT,
    "acal",
    &processRunAutoCal,
     { BOWLER_I08, //group
        0}, // no arguments, kills all PID's
    BOWLER_STATUS, // response method
    { BOWLER_I08, //location
        BOWLER_I08, //trace
        0}, // Calling arguments
    NULL //Termination
};

//Critical
  RPC_LIST bcsPid_KPID = {BOWLER_CRIT,
    "kpid",
    &processPIDCrit,
    {0}, // no arguments, kills all PID's
    BOWLER_STATUS, // response method
    { BOWLER_I08, //location
        BOWLER_I08, //trace
        0}, // Calling arguments, // Calling arguments
    NULL //Termination
};
  RPC_LIST bcsPid_CPID_c = {BOWLER_CRIT,
    "cpid",
    &processPIDCrit,
    { BOWLER_I08, //channel
        BOWLER_I08, //enabled
        BOWLER_I08, //Polarity
        BOWLER_I08, //async
        BOWLER_FIXED100, //Kp
        BOWLER_FIXED100, //Ki
        BOWLER_FIXED100, //Kd
        BOWLER_I32, //Latch value
        BOWLER_I08, //Use index latch
        BOWLER_I08, //Stop On latch
        BOWLER_FIXED1K, //stop
        BOWLER_FIXED1K, //upper
        BOWLER_FIXED1K, //lower
        0}, // Calling arguments
    BOWLER_STATUS, // response method
   { BOWLER_I08, //location
        BOWLER_I08, //trace
        0}, // Calling arguments, // Calling arguments
    NULL //Termination
};
  RPC_LIST bcsPid_CPDV_c = {BOWLER_CRIT,
    "cpdv",
    &processPIDCrit,
    { BOWLER_I08, //channel
        BOWLER_FIXED100, //Kp
        BOWLER_FIXED100, //Kd
        0}, // Calling arguments
    BOWLER_STATUS, // response method
     { BOWLER_I08, //location
        BOWLER_I08, //trace
        0}, // Calling arguments, // Calling arguments
    NULL //Termination
};



  NAMESPACE_LIST bcsPid = {"bcs.pid.*;1.0;;", // The string defining the namespace
    NULL, // the first element in the RPC list
    &pidAsyncEventCallbackLocal, // async for this namespace
    NULL// no initial elements to the other namesapce field.
};



  boolean bcsPidnamespcaedAdded = false;

NAMESPACE_LIST * getBcsPidNamespace() {
    if (!bcsPidnamespcaedAdded) {
        //GET
        addRpcToNamespace(&bcsPid, & bcsPid_APID);
        addRpcToNamespace(&bcsPid, & bcsPid__PID);
        addRpcToNamespace(&bcsPid, & bcsPid_CPID);
        addRpcToNamespace(&bcsPid, & bcsPid_CPDV);
        addRpcToNamespace(&bcsPid, & bcsPid_GPDC);
        //Post
        addRpcToNamespace(&bcsPid, & bcsPid_APID_p);
        addRpcToNamespace(&bcsPid, & bcsPid__PID_p);
        addRpcToNamespace(&bcsPid, & bcsPid__VPD);
        addRpcToNamespace(&bcsPid, & bcsPid_RPID);

        //Critical
        addRpcToNamespace(&bcsPid, & bcsPid_KPID);
        addRpcToNamespace(&bcsPid, & bcsPid_CPID_c);
        addRpcToNamespace(&bcsPid, & bcsPid_CPDV_c);
        addRpcToNamespace(&bcsPid, & bcsPid_ACAL);
        bcsPidnamespcaedAdded = true;
    }

    return &bcsPid;
}
