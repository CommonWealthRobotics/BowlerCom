/*
 * PidHysteresis.c
 *
 *  Created on: Feb 14, 2014
 *      Author: hephaestus
 */
#include <BowlerCom.h>

void incrementHistoresis(int group) {
    getPidGroupDataTable(group)->config.upperHistoresis += 1;
    //calcCenter( group);
}

void decrementHistoresis(int group) {
    getPidGroupDataTable(group)->config.lowerHistoresis -= 1;
}

void calcCenter(int group) {
    int diff = (getPidGroupDataTable(group)->config.upperHistoresis + getPidGroupDataTable(group)->config.lowerHistoresis) / 2;
    getPidGroupDataTable(group)->config.stop = diff;
}

void checkCalibration(int group) {
    if (getPidGroupDataTable(group)->calibration.calibrated != true) {
        getPidGroupDataTable(group)->config.upperHistoresis = 0;
        getPidGroupDataTable(group)->config.lowerHistoresis = 0;
        getPidGroupDataTable(group)->config.stop = 0;
        getPidGroupDataTable(group)->calibration.calibrated = true;
    }
}

int getUpperPidHistoresis(int group) {
    checkCalibration(group);
    return getPidGroupDataTable(group)->config.upperHistoresis;
}

int getLowerPidHistoresis(int group) {
    checkCalibration(group);
    return getPidGroupDataTable(group)->config.lowerHistoresis;
}

int getPidStop(int group) {
    checkCalibration(group);
    return getPidGroupDataTable(group)->config.stop;
}

boolean processRunAutoCal(BowlerPacket * Packet) {
    int group = Packet->use.data[0];

    runPidHysterisisCalibration(group);

    READY(Packet, 0, 0);
    return true;
}

void runPidHysterisisCalibration(int group) {

    if (!getPidGroupDataTable(group)->config.Enabled) {
        println_E("Axis disabled for calibration #");
        p_int_E(group);
        getPidGroupDataTable(group)->config.Enabled = true;
    }
    getPidGroupDataTable(group)->config.lowerHistoresis = 0;
    getPidGroupDataTable(group)->config.upperHistoresis = 0;
    getPidGroupDataTable(group)->config.stop = 0;
    //    println_I("\tReset PID");
    pidReset(group, 0); // Zero encoder reading
    //   println_I("\tDisable PID Output");
    SetPIDEnabled(group, true);
    SetPIDCalibrateionState(group, CALIBRARTION_hysteresis);

    getPidGroupDataTable(group)->calibration.state = forward;
    //  println_I("\tSetting slow move");
    setOutput(group, -1.0f);
    getPidGroupDataTable(group)->timer.setPoint = 2000;
    getPidGroupDataTable(group)->timer.MsTime = getMs();

}

CAL_STATE pidHysterisis(int group) {

    if (RunEvery(&getPidGroupDataTable(group)->timer) > 0) {
        Print_Level l = getPrintLevel();
        //setPrintLevelInfoPrint();
        float boundVal = 150.0;
        float extr = GetPIDPosition(group);
        if (bound(0, extr, boundVal, boundVal)) {// check to see if the encoder has moved
            //we have not moved
            //          println_I("NOT moved ");p_fl_I(extr);
            if (getPidGroupDataTable(group)->calibration.state == forward) {
                incrementHistoresis(group);
            } else if (getPidGroupDataTable(group)->calibration.state == backward) {
                decrementHistoresis(group);
            }
            int historesisBound = 25;
            if (getPidGroupDataTable(group)->config.lowerHistoresis < (-historesisBound) &&
                    getPidGroupDataTable(group)->calibration.state == backward) {
                println_E("Backward Motor seems damaged, more then counts of historesis #");
                p_int_I(group);
                getPidGroupDataTable(group)->calibration.state = forward;
            }
            if (getPidGroupDataTable(group)->config.upperHistoresis > (historesisBound) &&
                    getPidGroupDataTable(group)->calibration.state == forward) {
                println_E("Forward Motor seems damaged, more then counts of historesis #");
                p_int_I(group);
                getPidGroupDataTable(group)->calibration.state = done;
            }
        } else {
            pidReset(group, 0);
            setOutput(group, 0);
            println_E("Moved ");
            p_fl_E(extr);
            if (getPidGroupDataTable(group)->calibration.state == forward) {
                println_I("Backward Calibrated for link# ");
                p_int_I(group);
                getPidGroupDataTable(group)->calibration.state = backward;
            } else {
                println_I("Calibration done for link# ");
                p_int_I(group);
                getPidGroupDataTable(group)->calibration.state = done;

                float offset = .9;
                getPidGroupDataTable(group)->config.lowerHistoresis *= offset;
                getPidGroupDataTable(group)->config.upperHistoresis *= offset;
                calcCenter(group);
            }

        }
        if (getPidGroupDataTable(group)->calibration.state == forward) {
            setOutput(group, 1.0f);
        } else if (getPidGroupDataTable(group)->calibration.state == backward) {
            setOutput(group, -1.0f);
        }
        setPrintLevel(l);
    }
    if (getPidGroupDataTable(group)->calibration.state == done)
        SetPIDCalibrateionState(group, CALIBRARTION_DONE);
    return getPidGroupDataTable(group)->calibration.state;
}

void startHomingLink(int group, PidCalibrationType type, float homedValue) {
    float speed = 20.0;
    if (type == CALIBRARTION_home_up)
        speed *= 1.0;
    else if (type == CALIBRARTION_home_down)
        speed *= -1.0;
    else {
        println_E("Invalid homing type");
        return;
    }
    getPidGroupDataTable(group)->config.tipsScale = 1;
    SetPIDCalibrateionState(group, type);
    setOutput(group, speed);
    getPidGroupDataTable(group)->timer.MsTime = getMs();
    getPidGroupDataTable(group)->timer.setPoint = 1000;
    getPidGroupDataTable(group)->homing.homingStallBound = 20;
    getPidGroupDataTable(group)->homing.previousValue = GetPIDPosition(group);
    getPidGroupDataTable(group)->homing.lastTime = getMs();
    getPidGroupDataTable(group)->homing.homedValue = homedValue;
    SetPIDEnabled(group,true);
}

void checkLinkHomingStatus(int group) {
    if (!(GetPIDCalibrateionState(group) == CALIBRARTION_home_down ||
            GetPIDCalibrateionState(group) == CALIBRARTION_home_up ||
            GetPIDCalibrateionState(group) == CALIBRARTION_home_velocity
            )
            ) {
        return; //Calibration is not running
    }
    float current = GetPIDPosition(group);
    float currentTime = getMs();
    if (RunEvery(&getPidGroupDataTable(group)->timer) > 0) {
        //println_W("Check Homing ");
        if (GetPIDCalibrateionState(group) != CALIBRARTION_home_velocity) {
            float boundVal = getPidGroupDataTable(group)->homing.homingStallBound;

            if (bound(getPidGroupDataTable(group)->homing.previousValue,
                    current,
                    boundVal,
                    boundVal
                    )
                    ) {
                pidReset(group, getPidGroupDataTable(group)->homing.homedValue);
                //after reset the current value will have changed
                current = GetPIDPosition(group);
                getPidGroupDataTable(group)->config.tipsScale = 1;
                println_W("Homing Velocity for group ");
                p_int_W(group);
                print_W(", Resetting position to: ");
                p_fl_W(getPidGroupDataTable(group)->homing.homedValue);
                print_W(" current ");
                p_fl_W(current);

                float speed = -20.0;
                if (GetPIDCalibrateionState(group) == CALIBRARTION_home_up)
                    speed *= 1.0;
                else if (GetPIDCalibrateionState(group) == CALIBRARTION_home_down)
                    speed *= -1.0;
                else {
                    println_E("Invalid homing type");
                    return;
                }
                Print_Level l = getPrintLevel();
                //setPrintLevelInfoPrint();
                setOutput(group, speed);
                setPrintLevel(l);
                getPidGroupDataTable(group)->timer.MsTime = getMs();
                getPidGroupDataTable(group)->timer.setPoint = 2000;
                SetPIDCalibrateionState(group, CALIBRARTION_home_velocity);
                getPidGroupDataTable(group)->homing.lastTime = currentTime;
            }           
        } else {
            current = GetPIDPosition(group);
            float posDiff = current - getPidGroupDataTable(group)->homing.previousValue; //ticks
            float timeDiff = (currentTime - getPidGroupDataTable(group)->homing.lastTime) / 1000.0; //
            float tps = (posDiff / timeDiff);
            getPidGroupDataTable(group)->config.tipsScale = 20 / tps;

            println_E("New scale factor: ");
            p_fl_E(getPidGroupDataTable(group)->config.tipsScale);
            print_E(" speed ");
            p_fl_E(tps);
            print_E(" on ");
            p_int_E(group);
            print_E(" Position difference ");
            p_fl_E(posDiff);
            print_E(" time difference ");
            p_fl_E(timeDiff);


            OnPidConfigure(group);
            SetPIDCalibrateionState(group, CALIBRARTION_DONE);
        }
        getPidGroupDataTable(group)->homing.previousValue = current;
    }
}
