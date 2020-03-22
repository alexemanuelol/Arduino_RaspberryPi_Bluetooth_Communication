/*
 *      Author:     Alexander Emanuelsson
 *
 *      Brief:      Source code for Arduino Nano door detector
 *
 *      Usage:      When connected to the raspberry pi via bluetooth, the raspberry pi will send "0x1"
 *                  indicating that it want to start a connection. When Arduino Nano detects this event
 *                  it will enter listen mode for the sensor and send "0x2" when the door opens, "0x3"
 *                  when the door closes. "0x4" can be sent from the raspberry pi to check if the bluetooth
 *                  connection to the Arduino Nano is still present. It can also be sent from the Arduino
 *                  Nano to see if the connection is still present.
 */

#include <SoftwareSerial.h>
#include <string.h>

#define     SERIAL_RX               10
#define     SERIAL_TX               11
#define     BAUDRATE                9600
#define     MAX_BUFFER              100
#define     RECEIVE_TIMEOUT_MS      3000

#define     BUTTON_PIN              4

#define     LED_PIN                 13      /* LED on Arduino Nano */

#define     RGB_RED_PIN             9
#define     RGB_GREEN_PIN           6
#define     RGB_BLUE_PIN            5

#define     ON                      HIGH
#define     OFF                     LOW
#define     BUTTON_DOWN             HIGH
#define     BUTTON_UP               LOW

#define     EVENT_CONNECTED         "0x1"   /* Event sent from master */
#define     EVENT_OPENED            "0x2"   /* Event sent when door opened */
#define     EVENT_CLOSED            "0x3"   /* Event sent when door closed */
#define     EVENT_IS_CONNECTION     "0x4"   /* Event sent to see if still BT connection */

#define     IS_CONNECTION_CHECK_MS  10000   /* How often to check the BT connection */
#define     CHECK_BT_CONNECTION     1

enum TState {
    CLOSED = 0,
    OPEN_TRIGGER,
    OPENED,
    CLOSE_TRIGGER
};

SoftwareSerial btSerial (SERIAL_RX, SERIAL_TX);

static TState state = CLOSED;
static int buttonState = BUTTON_UP;


/*
 *      Setup function
 */
void setup ()
{
    /* Init LED/ BUTTON pins */
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT);

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
 *
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

        buttonState = digitalRead(BUTTON_PIN);

        switch(state)
        {
            case CLOSED:
                if (buttonState == BUTTON_DOWN)
                {
                    state = OPEN_TRIGGER;
                }
                break;

            case OPEN_TRIGGER:
                digitalWrite(LED_PIN, ON);
                btSerial.write(EVENT_OPENED);   /* Send EVENT_OPENED over BT */
                if (!waitForResponse("OK", RECEIVE_TIMEOUT_MS))
                {
                    state = CLOSED;
                    return false;
                }
                state = OPENED;
                break;

            case OPENED:
                if (buttonState == BUTTON_UP)
                {
                    state = CLOSE_TRIGGER;
                }
                break;

            case CLOSE_TRIGGER:
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
 *      Find a substring withing a string
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
