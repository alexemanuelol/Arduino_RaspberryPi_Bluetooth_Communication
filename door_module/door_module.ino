/*
 *      Author:     Alexander Emanuelsson
 *
 *      Brief:      Source code for Arduino door detector
 *
 *      Usage:      When connected to the raspberry Pi via bluetooth, the Raspberry Pi can send "0x1",
 *                  indicating that it want to start a connection, or it can send "0x4", indicating that
 *                  it want to check if it is connected to the Arduino. The Arduino will respond with "OK\r\n".
 *
 *                  When Arduino detects the connection event, it will turn the RGB LED green and enter
 *                  listening mode. In listening mode, the Arduino will wait for the reed switch to be
 *                  active/ inactive. When active, "0x2" will be sent to the Raspberry Pi, indicating that
 *                  the door opened. Raspberry Pi will respond with "OK\r\n". When inactive, "0x3" will be sent
 *                  to the Raspberry Pi, indicating that the door closed. Raspberry Pi will respond with "OK\r\n".
 *
 *                  During runtime, the Arduino will send "0x4" every 10 second in which the Raspberry Pi must
 *                  answer "OK\r\n". If the answer is not provided, the Arduino will enter
 *                  waitForConnectionEstablished function and the RGB LED will become red.
 */

/*
 *      Includes
 */

#include <SoftwareSerial.h>
#include <string.h>


/*
 *      Defines
 */

#define     SERIAL_RX_PIN           10
#define     SERIAL_TX_PIN           11
#define     BAUDRATE                9600
#define     MAX_BUFFER              100     /* The max size of the serial buffer in bytes */
#define     RECEIVE_TIMEOUT_MS      3000    /* The amount of time to wait before timeout */

#define     SWITCH_PIN              4       /* Reed Switch & Debug Button Switch*/

#define     LED_PIN                 13      /* LED on Arduino */

#define     RGB_RED_PIN             9
#define     RGB_GREEN_PIN           6
#define     RGB_BLUE_PIN            5

#define     ON                      HIGH
#define     OFF                     LOW
#define     DOOR_CLOSED             HIGH
#define     DOOR_OPEN               LOW

#define     EVENT_CONNECTED         "0x1"   /* Event sent from master */
#define     EVENT_OPENED            "0x2"   /* Event sent when door opened */
#define     EVENT_CLOSED            "0x3"   /* Event sent when door closed */
#define     EVENT_IS_CONNECTION     "0x4"   /* Event sent to see if still BT connection */

#define     IS_CONNECTION_CHECK_MS  10000   /* How often to check the BT connection */
#define     CHECK_BT_CONNECTION     1       /* Enable/ Disable continuous connection check */


/*
 *      Enums
 */

enum DoorStateType {
    CLOSED = 0,
    OPEN_TRIGGER,
    OPEN,
    CLOSED_TRIGGER
};


SoftwareSerial btSerial(SERIAL_RX_PIN, SERIAL_TX_PIN);

static DoorStateType state = CLOSED;
static int doorState = DOOR_CLOSED;


/*
 *      Setup function
 */
void setup ()
{
    /* Init LED/ SWITCH pins */
    pinMode(LED_PIN, OUTPUT);
    pinMode(SWITCH_PIN, INPUT_PULLUP);

    /* Init RGB pins */
    pinMode(RGB_RED_PIN, OUTPUT);
    pinMode(RGB_GREEN_PIN, OUTPUT);
    pinMode(RGB_BLUE_PIN, OUTPUT);

    /* Init serial communication */
    btSerial.begin(BAUDRATE);

    /* Check that the connection is up and running */
    RGB_color(255, 0, 0); /* RED */
    waitForConnectionEstablished();
    RGB_color(0, 255, 0); /* GREEN */
}


/*
 *      Main loop function
 */
void loop()
{
    if (!stateMachine())
    {
        RGB_color(255, 0, 0); /* RED */
        waitForConnectionEstablished();
        RGB_color(0, 255, 0); /* GREEN */
    }
}


/*
 *      The main state machine
 */
int stateMachine(void)
{
    int checkConnectionCounter = 0;
    while (1)
    {
#if CHECK_BT_CONNECTION
        /* Check if there is still a BT connection */
        if (checkConnectionCounter == IS_CONNECTION_CHECK_MS)
        {
            checkConnectionCounter = 0;
            btSerial.write(EVENT_IS_CONNECTION);
            if (!waitForResponse("OK", RECEIVE_TIMEOUT_MS))
            {
                state = CLOSED;
                return false;
            }
        }
        delay(1);
        checkConnectionCounter++;
#endif

        doorState = digitalRead(SWITCH_PIN);

        switch(state)
        {
            case CLOSED:
                if (doorState == DOOR_OPEN)
                {
                    state = OPEN_TRIGGER;
                }
                break;

            case OPEN_TRIGGER:
                digitalWrite(LED_PIN, ON);
                btSerial.write(EVENT_OPENED);       /* Send EVENT_OPENED over BT */
                if (!waitForResponse("OK", RECEIVE_TIMEOUT_MS))
                {
                    state = CLOSED;
                    return false;
                }
                state = OPEN;
                break;

            case OPEN:
                if (doorState == DOOR_CLOSED)
                {
                    state = CLOSED_TRIGGER;
                }
                break;

            case CLOSED_TRIGGER:
                digitalWrite(LED_PIN, OFF);
                btSerial.write(EVENT_CLOSED);         /* Send EVENT_CLOSED over BT */
                if (!waitForResponse("OK", RECEIVE_TIMEOUT_MS))
                {
                    state = CLOSED;
                    return false;
                }
                state = CLOSED;
                break;

            default:
                break;
        }
    }
}


/*
 *      Wait until connection event message is received
 */
void waitForConnectionEstablished(void)
{
    String received = "";
    int newData = false;

    while (1)
    {
        /* If received buffer is too big, clear it */
        if (received.length() >= MAX_BUFFER)
        {
            received = "";
            btSerial.flush();
        }

        while (btSerial.available())
        {
            received += (char) btSerial.read();
            newData = true;
        }

        delay(100);

        if (newData == true)
        {
            newData = false;
            if (findSubstring(received, (String) EVENT_CONNECTED) == true)
            {
                /* Connection event received */
                btSerial.write("OK\r\n");
                return;
            }
            else if (findSubstring(received, (String) EVENT_IS_CONNECTION) == true)
            {
                /* Is connected event received */
                btSerial.write("OK\r\n");
            }
        }
    }
}


/*
 *      Wait for receive str timeout ms
 */
 bool waitForResponse(String str, int timeout)
 {
    String received = "";
    int counter = 0;

    while (btSerial.available() == 0)
    {
        if (counter == timeout)
        {
            return false;
        }
        counter++;
        delay(1);
    }

    delay(10);

    while (btSerial.available())
    {
        received += (char) btSerial.read();
    }

    if (findSubstring(received, str) == true)
    {
        return true;
    }
    else
    {
        return false;
    }
 }


/*
 *      Set RGB LED color function
 */
void RGB_color(int red, int green, int blue)
{
    analogWrite(RGB_RED_PIN, red);
    analogWrite(RGB_GREEN_PIN, green);
    analogWrite(RGB_BLUE_PIN, blue);
}


/*
 *      Find a substring within a string
 */
bool findSubstring(String str, String substr)
{
    int strLength = str.length();
    int substrLength = substr.length();

    if (substrLength > strLength)
    {
        return false;
    }

    int i, j;
    for (i = 0, j = 0; i != strLength; i++)
    {
        if (str[i] == substr[j])
        {
            if ((j + 1) == substrLength)
            {
                return true;
            }
            j++;
        }
        else
        {
            j = 0;
        }
    }

    if ((j + 1) == substrLength)
    {
        return true;
    }
    else
    {
        return false;
    }
}
