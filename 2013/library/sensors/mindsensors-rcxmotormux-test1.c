#pragma config(Sensor, S1,     MSMTRMX,             sensorI2CCustomFastSkipStates)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/*
 * $Id: mindsensors-rcxmotormux-test1.c 109 2012-09-25 17:10:26Z xander $
 */

/**
 * mindsensors-rcxmotormux.h provides an API for the Mindsensors RCX Motor MUX.
 * This program demonstrates how to use the driver for the Mindsensors RCX Motor Mux.
 * Plug the mux into sensor port 1, and a motor into motor output 1. The motor will
 * gradually speed up to full power and then back again, and then the same backwards.
 * It will then gradually increase the braking force from nothing to full power, and
 * back to zero, before turning the motor off
 *
 * Changelog:
 * - 0.1: Initial release
 *
 * License: You may use this code as you wish, provided you give credit where it's due.
 *
 * THIS CODE WILL ONLY WORK WITH ROBOTC VERSION 3.51 AND HIGHER.
 * Daniel Playfair Cal (daniel.playfair.cal_at_gmail.com)
 * 20 September 2011
 * version 0.1
 */

#include "drivers/mindsensors-rcxmotormux.h"

task main () {

  // Gradually speed up
  for (int power = 0; power <= 255; power++) {
    MSMTRMX_Control (MSMTRMX, MSMTRMX_M1, power);
    wait1Msec (20);
  }

  // Gradually slow down
  for (int power = 255; power >=1; power--) {
    MSMTRMX_Control (MSMTRMX, MSMTRMX_M1, power);
    wait1Msec (20);
  }

  // The same backwards
  for (int power = 0; power >= -255; power--) {
    MSMTRMX_Control (MSMTRMX, MSMTRMX_M1, power);
    wait1Msec (20);
  }

  // Slow down again
  for (int power = -255; power <= -1; power++) {
    MSMTRMX_Control (MSMTRMX, MSMTRMX_M1, power);
    wait1Msec (20);
  }

  // Gradually increase braking force
  for (int force = 0; force <= 255; force++) {
    MSMTRMX_Brake (MSMTRMX, MSMTRMX_M1, force);
    wait1Msec (20);
  }

  // Gradually reduce braing force
  for (int force = 255; force >=1; force--) {
    MSMTRMX_Brake (MSMTRMX, MSMTRMX_M1, force);
    wait1Msec (20);
  }

  // Turn off the motor
  MSMTRMX_Control (MSMTRMX, MSMTRMX_M1, 0);
}
