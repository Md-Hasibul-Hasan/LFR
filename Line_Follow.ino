#include "Globals.h"


enum class RobotState
{
    FOLLOW_LINE,
    SEARCH_LINE,
    GAP,
    JUNCTION,
    TURN_LEFT,
    TURN_RIGHT,
    STOP
};


RobotState robotState = RobotState::FOLLOW_LINE;



// ==================================================
// TIMING
// ==================================================

unsigned long gapStartTime = 0;
unsigned long turnStartTime = 0;
unsigned long finishStartTime = 0;


int searchDirection = 1;   // -1 = LEFT, +1 = RIGHT

bool junctionLocked = false;
bool turnLocked = false;

bool finishCandidate = false;


// ==================================================
// INIT
// ==================================================

void RobotInit()
{
    pinMode(S0, OUTPUT);
    pinMode(S1, OUTPUT);
    pinMode(S2, OUTPUT);
    pinMode(S3, OUTPUT);

    pinMode(LEFT_FWD, OUTPUT);
    pinMode(LEFT_BWD, OUTPUT);

    pinMode(RIGHT_FWD, OUTPUT);
    pinMode(RIGHT_BWD, OUTPUT);

    pinMode(LEFT_PWM, OUTPUT);
    pinMode(RIGHT_PWM, OUTPUT);
}


void RobotStop()
{
    DriveMotor(0, 0);
}


// ==================================================
// CHOOSE JUNCTION TURN
// ==================================================

int ChooseJunctionTurn(bool left,bool center,bool right){
    /*
        Return:

        -1 = LEFT
         0 = STRAIGHT
        +1 = RIGHT
    */


    /*
       llrIdx:

       0 = LEFT priority
       1 = RIGHT priority
       2 = STRAIGHT priority

       This matches your current menu assumption.
    */


    // ---------------- LEFT PRIORITY ----------------

    if (llrIdx == 0)
    {
        if (left)   return -1;
        if (center) return 0;
        if (right)  return 1;
    }


    // ---------------- RIGHT PRIORITY ----------------

    else if (llrIdx == 1)
    {
        if (right)  return 1;
        if (center) return 0;
        if (left)   return -1;
    }


    // ---------------- STRAIGHT PRIORITY ----------------

    else
    {
        if (center) return 0;
        if (left)   return -1;
        if (right)  return 1;
    }


    return 10;
}


// ==================================================
// FOLLOW LINE
// ==================================================

void FollowLineState()
{
    /*
        Priority:

        Junction
            ↓
        Hard Left
            ↓
        Hard Right
            ↓
        Line Lost
            ↓
        PID
    */

    // no needfor  current competition.. but need in future
    // if (IsFinishTrack())
    // {
    //     if (!finishCandidate)
    //     {
    //         finishCandidate = true;
    //         finishStartTime = millis();
    //     }

    //     if (millis() - finishStartTime >= FINISH_CONFIRM_TIME)
    //     {
    //         ResetPID();
    //         robotState = RobotState::STOP;
    //         return;
    //     }
    // }
    // else
    // {
    //     finishCandidate = false;
    // }


    // ------------------------------------------
    // Release junction lock after leaving it
    // ------------------------------------------

    if (junctionLocked)
    {
        if (activeSensorCount <= 3)
        {
            junctionLocked = false;
        }
    }


    // ------------------------------------------
    // Junction
    // ------------------------------------------

    if (!junctionLocked && IsJunction())
    {
        ResetPID();
        robotState = RobotState::JUNCTION;
        return;
    }


    // Turn lock release
    if (turnLocked)
    {
        bool center =
            norValue[3] > SENSOR_THRESHOLD ||
            norValue[4] > SENSOR_THRESHOLD;

        bool outer =
            norValue[0] > SENSOR_THRESHOLD ||
            norValue[1] > SENSOR_THRESHOLD ||
            norValue[6] > SENSOR_THRESHOLD ||
            norValue[7] > SENSOR_THRESHOLD;

        // Robot normal line-এ ফিরে এসেছে
        if (center && !outer)
        {
            turnLocked = false;
        }
    }


    // Hard Left
    if (!turnLocked && IsHardLeft())
    {
        ResetPID();
        turnStartTime = millis();
        robotState = RobotState::TURN_LEFT;
        return;
    }


    // Hard Right
    if (!turnLocked && IsHardRight())
    {
        ResetPID();
        turnStartTime = millis();
        robotState = RobotState::TURN_RIGHT;
        return;
    }


    // ------------------------------------------
    // Line Lost / Gap
    // ------------------------------------------

    if (IsLineLost())
    {
        ResetPID();

        gapStartTime = millis();
        robotState = RobotState::GAP;

        return;
    }


    // ------------------------------------------
    // Normal PID
    // ------------------------------------------

    SetMotorSpeed(CalculatePID(error));
}


// ==================================================
// GAP
// ==================================================

void GapState()
{
    // Gap হলে কিছুক্ষণ straight যাবে
    DriveMotor(baseSpeed, baseSpeed);

    // আবার line পেয়ে গেছে
    if (!IsLineLost())
    {
        ResetPID();
        robotState = RobotState::FOLLOW_LINE;
        return;
    }


    // অনেকক্ষণেও line না পেলে SEARCH করবে
    if (millis() - gapStartTime >= GAP_TIMEOUT)
    {
        // Lock first search direction
        if (lastPosition < 0)
            searchDirection = -1; // LEFT
        else
            searchDirection = 1; // RIGHT

        robotState = RobotState::SEARCH_LINE;
        return;
    }
}


// ==================================================
// SEARCH LINE
// ==================================================

void SearchLineState()
{
    // ------------------------------------------
    // Rotate toward last known line direction
    // ------------------------------------------

    if (searchDirection == -1)
    {
        // LEFT
        DriveMotor(-baseSpeed, baseSpeed);
    }
    else
    {
        // RIGHT
        DriveMotor(baseSpeed, -baseSpeed);
    }


    // ------------------------------------------
    // Line found
    // ------------------------------------------

    if (!IsLineLost())
    {
        ResetPID();

        robotState = RobotState::FOLLOW_LINE;
        return;
    }
}

// ==================================================
// TURN LEFT
// ==================================================



void TurnLeftState()
{
    unsigned long elapsed = millis() - turnStartTime;

    // LEFT rotate
    DriveMotor(-baseSpeed, baseSpeed);

    // Minimum time পর্যন্ত কোনো sensor দেখে exit করবে না
    if (elapsed < TURN_MIN_TIME)
        return;

    // Minimum turn শেষ হওয়ার পর
    // center line পাওয়া গেলে PID-এ ফিরে যাও
    bool center =
        norValue[3] > SENSOR_THRESHOLD ||
        norValue[4] > SENSOR_THRESHOLD;

    if (center)
    {
        turnLocked = true;
        ResetPID();
        robotState = RobotState::FOLLOW_LINE;
        return;
    }

    // Safety timeout
    if (elapsed >= TURN_TIMEOUT)
    {
        turnLocked = true;
        ResetPID();
        robotState = RobotState::FOLLOW_LINE;
        return;
    }
}


// ==================================================
// TURN RIGHT
// ==================================================


void TurnRightState()
{
    unsigned long elapsed = millis() - turnStartTime;

    // RIGHT rotate
    DriveMotor(baseSpeed, -baseSpeed);

    // Minimum time পর্যন্ত exit না
    if (elapsed < TURN_MIN_TIME)
        return;

    bool center =
        norValue[3] > SENSOR_THRESHOLD ||
        norValue[4] > SENSOR_THRESHOLD;

    if (center)
    {
        turnLocked = true;
        ResetPID();
        robotState = RobotState::FOLLOW_LINE;
        return;
    }

    // Safety timeout
    if (elapsed >= TURN_TIMEOUT)
    {
        turnLocked = true;
        ResetPID();
        robotState = RobotState::FOLLOW_LINE;
        return;
    }
}

// ==================================================
// JUNCTION
// ==================================================

void JunctionState()
{
    /*
        Unlike previous version:

        No fixed 25ms forward + second classification.

        Read current available paths and choose
        using menu bias.
    */


    bool left =
        norValue[0] > SENSOR_THRESHOLD ||
        norValue[1] > SENSOR_THRESHOLD;


    bool center =
        norValue[3] > SENSOR_THRESHOLD ||
        norValue[4] > SENSOR_THRESHOLD;


    bool right =
        norValue[6] > SENSOR_THRESHOLD ||
        norValue[7] > SENSOR_THRESHOLD;


    // ------------------------------------------------
    // CROSS +
    //
    // All directions available -> straight
    // ------------------------------------------------

    if (left && center && right && activeSensorCount >= 6)
    {
        junctionLocked = true;

        // DriveMotor(baseSpeed, baseSpeed);
        DriveMotor(50, 50);

        ResetPID();

        robotState = RobotState::FOLLOW_LINE;

        return;
    }


    // ------------------------------------------------
    // Choose available direction
    // ------------------------------------------------

    int junctionTurn = ChooseJunctionTurn(left, center, right);

    // No valid path detected
    if (junctionTurn == 10)
    {
        junctionLocked = true;

        ResetPID();
        robotState = RobotState::FOLLOW_LINE;
        return;
    }

    junctionLocked = true;


    // ---------------- LEFT ----------------

    if (junctionTurn == -1)
    {
        turnStartTime = millis();
        robotState = RobotState::TURN_LEFT;
        return;
    }


    // ---------------- RIGHT ----------------

    if (junctionTurn == 1)
    {
        turnStartTime = millis();
        robotState = RobotState::TURN_RIGHT;
        return;
    }


    // ---------------- STRAIGHT ----------------

    if (junctionTurn == 0 && center)
    {
        DriveMotor(baseSpeed, baseSpeed);

        ResetPID();

        robotState = RobotState::FOLLOW_LINE;

        return;
    }



}


// ==================================================
// STOP
// ==================================================

void StopState()
{
    RobotStop();
}


// ==================================================
// ROBOT UPDATE
// ==================================================

void RobotUpdate()
{

    bool allowTrackDetection =
        robotState == RobotState::FOLLOW_LINE ||
        robotState == RobotState::GAP;

    ReadSensors(allowTrackDetection);
    // ReadSensors();

    switch (robotState)
    {
        case RobotState::FOLLOW_LINE:
            FollowLineState();
            break;


        case RobotState::GAP:
            GapState();
            break;


        case RobotState::SEARCH_LINE:
            SearchLineState();
            break;


        case RobotState::JUNCTION:
            JunctionState();
            break;


        case RobotState::TURN_LEFT:
            TurnLeftState();
            break;


        case RobotState::TURN_RIGHT:
            TurnRightState();
            break;


        case RobotState::STOP:
            StopState();
            break;
    }
}


// ==================================================
// LINE FOLLOW SCREEN
// ==================================================

void LineFollow()
{
    RobotUpdate();


    u8g2.firstPage();

    do
    {
        u8g2.setFont(u8g2_font_6x12_tf);

        u8g2.drawStr(0, 12, "LINE FOLLOW");


        // ---------------- State ----------------

        u8g2.setCursor(0, 26);
        u8g2.print("State:");

        switch (robotState)
        {
            case RobotState::FOLLOW_LINE:
                u8g2.print("PID");
                break;

            case RobotState::SEARCH_LINE:
                u8g2.print("SEARCH");
                break;

            case RobotState::GAP:
                u8g2.print("GAP");
                break;

            case RobotState::JUNCTION:
                u8g2.print("JUNC");
                break;

            case RobotState::TURN_LEFT:
                u8g2.print("LEFT");
                break;

            case RobotState::TURN_RIGHT:
                u8g2.print("RIGHT");
                break;

            case RobotState::STOP:
                u8g2.print("STOP");
                break;
        }


        // ---------------- Position ----------------

        u8g2.setCursor(0, 40);
        u8g2.print("P:");
        u8g2.print(position);


        // ---------------- Track ----------------

        u8g2.setCursor(70, 40);
        u8g2.print("T:");
        u8g2.print(inverseTrack ? "W" : "B");


        // ---------------- Sensor total ----------------

        u8g2.setCursor(0, 54);
        u8g2.print("Tot:");
        u8g2.print(totalNorValue);


        // ---------------- Active sensors ----------------

        u8g2.setCursor(78, 54);
        u8g2.print("A:");
        u8g2.print(activeSensorCount);
    }
    while (u8g2.nextPage());


    // Exit
    if (okEvent == Button::Event::SHORT)
    {
        RobotStop();
        ResetPID();

        // Reset robot state for next run
        robotState = RobotState::FOLLOW_LINE;

        // Reset temporary states
        searchDirection = 1;
        junctionLocked = false;
        turnLocked = false;

        // Finish detection reset
        finishCandidate = false;
        finishStartTime = 0;

        ResetTrackDetection();

        currentScreen = Screen::DASHBOARD;
    }
}