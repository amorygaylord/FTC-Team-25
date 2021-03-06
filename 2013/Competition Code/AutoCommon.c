
void initializeRobot()
{
  	// Place code here to sinitialize servos to starting positions.
  	// Sensors are automatically configured and setup by ROBOTC. They may need a brief time to stabilize.
  	servo[gravityShelf] = SHELFDOWN;
  	servo[IRServo] = IRDOWN;
    servo[Ramp] = RAMP_START;

	/*
	 * Assume lined up perpendicular to the pegs.
	 */
	HTMCsetTarget(HTMC);

    initializeMotors();

    /*
     * Do not let the motors coast
     */
    bFloatDuringInactiveMotorPWM = false;

	// the default DSP mode is 1200 Hz.
	tHTIRS2DSPMode mode = DSP_1200;

	// set the DSP to the new mode
	// HTIRS2setDSPMode(IRSeeker, mode);

  	return;
}

/*
 * lookForIRBeacon
 *
 * Drive sideways until we see the beacon in segment 5
 * of the IR receiver.
 */
direction_t lookForIRBeacon(void)
{
	int segment;
    direction_t moved_dir;
	int strength1, strength2, strength3, strength4, strength5;
    bool bypass;

    bypass = false;

	segment = HTIRS2readACDir(IRSeeker);

	if (segment != BEACON_CENTER) {
        if (segment > BEACON_CENTER) {
		    moveSideRightOn(-40);
            moved_dir = RIGHT;
        } else {
            moved_dir = LEFT;
            moveSideLeftOn(40)
        }
	} else {
		return NO_DIR;
	}

    HTIRS2readAllACStrength(IRSeeker, strength1, strength2, strength3, strength4, strength5);

    nMotorEncoder[driveSide] = 0;
	while (segment != BEACON_CENTER) {
        if (nMotorEncoder[driveSide] >= (12 * ENCPERINCH)) {
            bypass = true;
            break;
        }
    	segment = HTIRS2readACDir(IRSeeker);
	}

    /*
     * We are in the correct segment, so move until the strength
     * is balanced.
     */
    if (!bypass) {
	    HTIRS2readAllACStrength(IRSeeker, strength1, strength2, strength3, strength4, strength5);

	    while (strength3 > strength2) {
	        HTIRS2readAllACStrength(IRSeeker, strength1, strength2, strength3, strength4, strength5);
	    }
    }

	sidewaysMovement(0);

    if (bypass) {
        moveForwardOn(2);
    }
    pauseDebug("ir found", 1);

    return (moved_dir);
}

direction_t whereIsTheBeacon(void)
{
    int segment;

    segment = HTIRS2readACDir(IRSeeker);
    nxtDisplayCenteredBigTextLine(4, "Dir: %d", segment);

	if (segment != BEACON_CENTER) {
        if ((segment > BEACON_CENTER) || (segment == 0)) {
		    return RIGHT;
        } else {
            return LEFT;
        }
	} else {
		return NO_DIR;
	}
}

void moveForwardToIRBeacon(int strength)
{
    int val;
    char tmp[50];

    moveForwardOn(70);

    val = getStrength();
    while (val <= strength) {
        val = getStrength();
        sprintf(tmp, "strength %d", val);
        nxtDisplayCenteredTextLine(2, tmp);
   }

    moveForwardOff;
}

/*
 * lookForWhiteLine
 *
 * Look for an edge of the white line by
 * moving sideways until we see the transistion.
 */
void lookForWhiteLine(direction_t dir)
{
	int val;

	LSsetActive(lightSensor);

	switch (dir) {
		case LEFT:
			sidewaysMovement(15);
	        // Look for the first right edge.
		   	val = LSvalNorm(lightSensor);
			while (val <= 22) {
				val = LSvalNorm(lightSensor);
			}
			break;
		case RIGHT:
	        // Look for first the left edge and then the right
			sidewaysMovement(-15);
		   	val = LSvalNorm(lightSensor);
			while (val <= 22) {
				val = LSvalNorm(lightSensor);
			}

		    while (val >=20) {
		        val = LSvalNorm(lightSensor);
		    }
			break;
        case FORWARD:
            moveForwardOn(10);
		   	val = LSvalNorm(lightSensor);
			while (val <= 28) {
				val = LSvalNorm(lightSensor);
			}
            break;
		case NO_DIR:
		default:
			return;
	}

	LSsetInactive(lightSensor);

    moveForwardOff();

    pauseDebug("on white line", 1);
}

/*
 * Move forward until we have detected a white line.
 * dist is the distance to move full speed.  We then
 * slow down so that we don't overshoot the line.  e.g.
 * You know you won't find the line befor dist inches.
 */
void moveForwardToWhiteLine(int dist)
{
    moveForward(dist-8);
    moveForward(8, 40);
    lookForWhiteLine(FORWARD);
}

/*
 * alignToPeg
 *
 * We may have moved off perpendicular to the
 * peg during our travels.  If so, rotate back.
 */
void alignToPeg(direction_t dir)
{
	int bearing;
    int marked;
    char str[48];

    bearing = HTMCreadHeading(HTMC);
    marked = getMarkedHeading();

    sprintf(str, "Compass %d, Marked %d", bearing, marked);
    pauseDebug(str, 5);


	if (bearing == getMarkedHeading()) {
		return;
	}

	if (dir == LEFT) {
		rotateCounterClockwise(20);
	} else {
		rotateClockwise(20);
	}

	while (HTMCreadHeading(HTMC) != marked) {
		showHeading();
	}

  	moveForwardOff();
}

void moveForwardToPushStop()
{
    int val;

    raiseShelfToAutoPushStopPosition();

    //servoChangeRate[IRServo] = 1;
    servo[IRServo] = IR_DEPLOY_RING;
    //wait1Msec(3000);
    //servoChangeRate[IRServo] = 10;

    wait1Msec(1000);

    // We are aligned.  Move forward until the
    // the touch sensor is depressed.
    moveForwardOn(10);
    val = SensorValue[touchSensor];
	while (val != 1) {
        val = SensorValue[touchSensor];
        nxtDisplayCenteredBigTextLine(3, "Move: %d", val);
    }
    moveForwardOff();
}

void findPeg(void)
{
    moveForwardToPushStop();
    moveBackwardHalf(2,20);
}

/*
 * placeRing
 *
 * Completely automated place functionality for putting
 * a ring on a peg.
 *
 * Assumes we are on the platform and in front of the
 * peg we want to place the ring on.
 */
void placeRing(direction_t dir)
{
    servo[IRServo] = IRDOWN;

	raiseShelfToAutoPlacePosition(dir);

    //servo[IRServo] = IRUP;
    pauseDebug("Prepping to move forward", 1);
    servo[IRServo] = IRRING;
    moveForwardHalf(7,20);

    //lowerShelfToDischargePosition();
    pauseDebug("shelf raised, servo deployed", 1);
    //turn(5,15);

    //moveSideways(RIGHT, 10, 15);

    servo[gravityShelf] = SHELFDOWN;

    //moveSideways(RIGHT, 5, 15);

	moveBackward(3);
}
