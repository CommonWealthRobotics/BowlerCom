/*
 * PidAsync.c
 *
 *  Created on: Feb 14, 2014
 *      Author: hephaestus
 */
#include <BowlerCom.h>

RunEveryData pidAsyncTimer ={0,100};

void updatePidAsync(BowlerPacket *Packet,boolean (*pidAsyncCallbackPtr)(BowlerPacket *Packet)){
    if(RunEvery(&pidAsyncTimer)){
        int i;
        int update = false; 
        for (i=0;i<getNumberOfPidChannels();i++){
                if(getPidGroupDataTable(i)->config.Async){
                        if(getPidGroupDataTable(i)->CurrentState != getPidGroupDataTable(i)->lastPushedValue){
                            //println_E("Async because of ");p_int_E(i);
                                update = true; 
                        }
               }
        }
        if(update){
                pushAllPIDPositions(Packet,pidAsyncCallbackPtr);
        }
    }
}

void pushAllPIDPositions(BowlerPacket *Packet,boolean (*pidAsyncCallbackPtr)(BowlerPacket *Packet)){
	//float time = getMs();
//	for(i=0;i<getNumberOfPidChannels();i++){
//		pushPID(i,getPidGroupDataTable(i)->CurrentState, time);
//	}
        INT32_UNION PID_Temp;
        LoadCorePacket(Packet);
        Packet->use.head.DataLegnth=5;
        Packet->use.head.RPC = GetRPCValue("apid");
        int i;
        for(i=0;i<getNumberOfPidChannels();i++){
                PID_Temp.Val=getPidGroupDataTable(i)->CurrentState;
                getPidGroupDataTable(i)->lastPushedValue = getPidGroupDataTable(i)->CurrentState;
                Packet->use.data[0+(i*4)]=PID_Temp.byte.FB;
                Packet->use.data[1+(i*4)]=PID_Temp.byte.TB;
                Packet->use.data[2+(i*4)]=PID_Temp.byte.SB;
                Packet->use.data[3+(i*4)]=PID_Temp.byte.LB;
                Packet->use.head.DataLegnth+=4;
        }
        Packet->use.head.Method=BOWLER_ASYN;
        FixPacket(Packet);
		if(pidAsyncCallbackPtr!=NULL)
			pidAsyncCallbackPtr(Packet);
}



void pushPIDLimitEvent(BowlerPacket *Packet,boolean (*pidAsyncCallbackPtr)(BowlerPacket *Packet),PidLimitEvent * event){
	if(event->type == NO_LIMIT){
		return;
	}
	event->latchTickError=0;
	if(event->type == INDEXEVENT){
		if(getPidGroupDataTable(event->group)->config.useIndexLatch){
			event->latchTickError = event->value-GetPIDPosition(event->group);
			event->value = getPidGroupDataTable(event->group)->config.IndexLatchValue;
			if(getPidGroupDataTable(event->group)->config.stopOnIndex){
				pidReset(event->group,event->value);
			}else{
				event->value = pidResetNoStop(event->group,event->value);
			}
		}
	}else{
        //println_I("ABSTRACTPID: End stop limit, setting new PID setpoint");
		event->value=GetPIDPosition(event->group);
		event->latchTickError=0;
		event->time = getMs();
		SetPID(event->group,event->value);
	}

	getPidGroupDataTable(event->group)->lastPushedValue=event->value;

	LoadCorePacket(Packet);
	Packet->use.head.MessageID = 0;
	Packet->use.head.Method=BOWLER_ASYN;
	Packet->use.head.RPC = GetRPCValue("pidl");

	Packet->use.data[0]=event->group;
	Packet->use.data[1]=event->type;
	INT32_UNION tmp;
	tmp.Val=event->value;
	Packet->use.data[2]=tmp.byte.FB;
	Packet->use.data[3]=tmp.byte.TB;
	Packet->use.data[4]=tmp.byte.SB;
	Packet->use.data[5]=tmp.byte.LB;
	tmp.Val=event->time;
	Packet->use.data[6]=tmp.byte.FB;
	Packet->use.data[7]=tmp.byte.TB;
	Packet->use.data[8]=tmp.byte.SB;
	Packet->use.data[9]=tmp.byte.LB;

	tmp.Val=event->latchTickError;
	Packet->use.data[10]=tmp.byte.FB;
	Packet->use.data[11]=tmp.byte.TB;
	Packet->use.data[12]=tmp.byte.SB;
	Packet->use.data[13]=tmp.byte.LB;

	Packet->use.head.DataLegnth = 4+1+1+(4*3);

	event->type = NO_LIMIT;
	FixPacket(Packet);
	if(pidAsyncCallbackPtr!=NULL)
		pidAsyncCallbackPtr(Packet);
}

void pushPID(BowlerPacket *Packet,boolean (*pidAsyncCallbackPtr)(BowlerPacket *Packet),uint8_t chan, int32_t value, float time){
	LoadCorePacket(Packet);
	Packet->use.head.Method=BOWLER_ASYN;
	Packet->use.head.MessageID = 0;
	Packet->use.head.RPC = GetRPCValue("_pid");
	Packet->use.head.DataLegnth = 4+1+4+4+4;
	Packet->use.data[0]=chan;
	INT32_UNION tmp;
	tmp.Val=value;
	Packet->use.data[1]=tmp.byte.FB;
	Packet->use.data[2]=tmp.byte.TB;
	Packet->use.data[3]=tmp.byte.SB;
	Packet->use.data[4]=tmp.byte.LB;
	tmp.Val=time;
	Packet->use.data[5]=tmp.byte.FB;
	Packet->use.data[6]=tmp.byte.TB;
	Packet->use.data[7]=tmp.byte.SB;
	Packet->use.data[8]=tmp.byte.LB;

	float diffTime = time-getPidGroupDataTable(chan)->lastPushedValue;
	float diffVal  = value -getPidGroupDataTable(chan)->lastPushedTime;

	tmp.Val = (int32_t) (diffVal/diffTime);
	Packet->use.data[9]=tmp.byte.FB;
	Packet->use.data[10]=tmp.byte.TB;
	Packet->use.data[11]=tmp.byte.SB;
	Packet->use.data[12]=tmp.byte.LB;


    FixPacket(Packet);
    if(pidAsyncCallbackPtr !=NULL){
    	if(pidAsyncCallbackPtr(Packet)){
            getPidGroupDataTable(chan)->lastPushedValue =value;
            getPidGroupDataTable(chan)->lastPushedTime=time;
        }
    }
}
