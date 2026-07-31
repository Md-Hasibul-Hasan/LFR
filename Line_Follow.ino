


#include "Globals.h"

enum class RobotState
{
    FOLLOW_LINE,
    SEARCH_LINE,
    GAP,
    JUNCTION,
    TURN_LEFT,
    TURN_RIGHT,
    ROUNDABOUT,
    STOP
};

RobotState robotState = RobotState::FOLLOW_LINE;

const unsigned long GAP_TIMEOUT = 250;
unsigned long gapStartTime = 0;

unsigned long TURN_FORWARD_TIME = 60;
unsigned long turnStartTime = 0;

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

/*====================================================
                    STATE FUNCTIONS
====================================================*/

void FollowLineState()
{
    /*
    -----------------------------------------
    FOLLOW_LINE STATE

    Robot normal PID line following mode.

    Responsibilities:
    1. Follow line using PID.
    2. Continuously monitor sensor patterns.
    3. Change state when a special track
       feature is detected.

    Priority (Highest → Lowest)

        STOP
          ↓
        ROUNDABOUT
          ↓
        JUNCTION
          ↓
        TURN LEFT
          ↓
        TURN RIGHT
          ↓
        GAP
          ↓
        LINE LOST
          ↓
        PID FOLLOW

    -----------------------------------------
    */

    if (IsRoundabout()){
        robotState = RobotState::ROUNDABOUT;
        return;
    }

    if (IsJunction()){
        // cross-junction (+) or T-junction (T) Future implement
        robotState = RobotState::JUNCTION;
        return;
    }

    if (IsHardLeft()){
        turnStartTime = millis();
        robotState = RobotState::TURN_LEFT;
        return;
    }

    if (IsHardRight()){
        turnStartTime = millis();
        robotState = RobotState::TURN_RIGHT;
        return;
    }


    if (IsLineLost()){
        // first check for gap then check for line lost
        gapStartTime = millis();
        robotState = RobotState::GAP;
        return;
    }

    // Continue normal PID line following
    SetMotorSpeed(CalculatePID(error));
}



void GapState(){
    DriveMotor(baseSpeed, baseSpeed);
    if (!IsLineLost()){
        robotState = RobotState::FOLLOW_LINE;
        return;
    }
    if (millis() - gapStartTime > GAP_TIMEOUT){
        robotState = RobotState::SEARCH_LINE;
        return;
    }
}



void SearchLineState()
{
    /*
    -----------------------------------------
    SEARCH_LINE STATE

    The robot failed to recover the line
    during the GAP state.

    Step 1
        Disable PID control.

    Step 2
        Rotate toward the last known
        line position.

    Step 3
        Keep checking the sensors.

    Step 4
        If the line is detected:
            - Detect track color.
            - Return to FOLLOW_LINE.

    Step 5
        If the line is still not found:
            - Continue searching.
            - Future: perform wider search.

    Transition:

        SEARCH_LINE
              │
        Line Found ?
          │      │
         NO     YES
          │      │
     Continue   FOLLOW_LINE
     Searching
    -----------------------------------------
    */

    const int searchSpeed = 120;

    // Search toward the last known line position
    if (lastPosition < 0)
    {
        // Last line was on the left
        DriveMotor(-searchSpeed, searchSpeed);
    }
    else
    {
        // Last line was on the right
        DriveMotor(searchSpeed, -searchSpeed);
    }

    // Line found
    if (!IsLineLost())
    {
        DetectTrack();
        robotState = RobotState::FOLLOW_LINE;
    }
}

void TurnLeftState()
{

    // Step 1 : একটু সামনে যাও
    if (millis() - turnStartTime < TURN_FORWARD_TIME)
    {
        DriveMotor(baseSpeed, baseSpeed);
        return;
    }

    // Step 2 : Left Rotate
    DriveMotor(-baseSpeed, baseSpeed);

    // Step 3 : Line center এ আসলে Stop Turning
    if (norValue[3] > 500 || norValue[4] > 500)
    {
        robotState = RobotState::FOLLOW_LINE;
    }

    // if (!IsLineLost())
    // {
    //     robotState = RobotState::FOLLOW_LINE;
    // }
}


void TurnRightState()
{
    if (millis() - turnStartTime < TURN_FORWARD_TIME)
    {
        DriveMotor(baseSpeed, baseSpeed);
        return;
    }

    DriveMotor(baseSpeed, -baseSpeed);

    if (norValue[3] > 500 || norValue[4] > 500)
    {
        robotState = RobotState::FOLLOW_LINE;
    }

    // if (!IsLineLost())
    // {
    //     robotState = RobotState::FOLLOW_LINE;
    // }
}


void JunctionState()
{
    /*
    -----------------------------------------
    JUNCTION STATE

    Robot detected Cross or T Junction.

    Future implementation:

    Step 1
        Detect junction type.
            - Cross
            - T Junction
            - Left Branch
            - Right Branch

    Step 2
        Apply competition rule.

    Example:
        Left Priority
        Right Priority
        Straight Priority

    Step 3
        Execute turn.

    Step 4
        Line centered again.

    Step 5
        Return FOLLOW_LINE.

    Transition

        JUNCTION
            │
        Decision
            │
        Execute Turn
            │
        FOLLOW_LINE
    -----------------------------------------
    */

    robotState = RobotState::FOLLOW_LINE;
}

void RoundaboutState()
{
    /*
    -----------------------------------------
    ROUNDABOUT STATE

    Robot entered a roundabout.

    Future implementation:

    Step 1
        Detect circle entry.

    Step 2
        Follow circular path.

    Step 3
        Detect exit line.

    Step 4
        Align with exit.

    Step 5
        Return FOLLOW_LINE.

    Transition

        ROUNDABOUT
              │
        Circle Follow
              │
        Exit Found
              │
        FOLLOW_LINE
    -----------------------------------------
    */

    robotState = RobotState::FOLLOW_LINE;
}



void StopState()
{
    /*
    STOP STATE

    Finish line /
    Emergency Stop /
    Manual Stop.

    Motors remain stopped
    until another state
    changes robotState.
    */

    RobotStop();
}

/*====================================================
                    ROBOT UPDATE
======================================================
Robot Update Loop

1. Read Sensors
2. Execute Current State
3. State decides next transition
4. Repeat

====================================================
*/
void RobotUpdate()
{
    ReadSensors();

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

    case RobotState::ROUNDABOUT:
        RoundaboutState();
        break;

    case RobotState::STOP:
        StopState();
        break;
    }
}

/*====================================================
                    LINE FOLLOW SCREEN
====================================================*/

void LineFollow()
{
    RobotUpdate();

    u8g2.firstPage();
    do
    {
        u8g2.setFont(u8g2_font_6x12_tf);

        u8g2.drawStr(0, 12, "LINE FOLLOW");

        u8g2.setCursor(0, 26);
        u8g2.print("State: ");

        switch (robotState)
        {
        case RobotState::FOLLOW_LINE:
            u8g2.print("FOLLOW");
            break;

        case RobotState::SEARCH_LINE:
            u8g2.print("SEARCH");
            break;

        case RobotState::GAP:
            u8g2.print("GAP");
            break;

        case RobotState::JUNCTION:
            u8g2.print("JUNCTION");
            break;

        case RobotState::ROUNDABOUT:
            u8g2.print("ROUND");
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

        u8g2.setCursor(0, 40);
        u8g2.print("Pos: ");
        u8g2.print(position);

        u8g2.setCursor(0, 54);
        u8g2.print("Total: ");
        u8g2.print(totalNorValue);

    } while (u8g2.nextPage());

    if (okEvent == Button::Event::SHORT)
    {
        RobotStop();
        currentScreen = Screen::DASHBOARD;
    }
}