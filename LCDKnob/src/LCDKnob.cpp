/*                                          LCDKnob

    Author: Andrew Fernie
    Date: 2023-12-20

    This is the main program for the LCDKnob project. It is a 1.28" round LCD touch panel located on the top of the knob
    of a CADDeck combined joystick and touch panel display suitable for use with common CAD programs.

    See https://github.com/andrewfernie/CADDeck for more information about CADDeck.

*/

#include "Arduino.h"
#include "LCDKnob.h"

const char *versionnumber = "V1.0.1";
/*
 * Version V1.0.1 
 *                   - Circular region for button press
 * 
 * Version V1.0.0 
 *                   - Initial version
 *
 * */

// Loop timing control
const unsigned long LOOP_INTERVAL = 30;

unsigned long previousMillis = 0;
long last_loop_start = 0;
long loop_count = 0;
long this_loop_start = 0;
long loop_100_count = 0;
long loop_100_time = 0;

bool move = true;

// placeholder for the pagenumber we are on (0 indicates home)
int pageNum = 0;

char jsonFileFail[LEN_FILENAME] = "";

// Create an instance of the display and touch screen classes.
LGFX_TFT tft(SCREEN_WIDTH, SCREEN_HEIGHT, TFT_BL);
CST816S touch(I2C_SDA, I2C_SCL, TP_RST, TP_INT);

// Create instances of the structs
Menu menu[NUM_PAGES];

Config generalconfig;

SerialChannel serialChannel(115200, SERIAL_8N1, UART1_RX_PIN, UART1_TX_PIN);

// path to the directory the logo are in ! including leading AND trailing / !
char logopath[64] = "/logos/";

void setup()
{
    // Delay to allow serial port to be available
    delay(1000);

    Serial.begin(115200); /* prepare for possible serial debug */

    MSG_INFOLN("LCDKnob " + String(versionnumber) + " starting up...");

    // ticker.attach(1, tcr1s);

    tft.init();
    tft.initDMA();
    tft.startWrite();
    tft.setColor(0, 0, 0);

    tft.setCursor(20, 100);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.print("LCDKnob " + String(versionnumber));
    tft.setCursor(20, 120);
    tft.print(+" starting up...");
    delay(5000);
    tft.clearDisplay();

    // -------------- Start filesystem ----------------------

    if (!FILESYSTEM.begin())
    {
        MSG_ERRORLN("[ERROR] LittleFS initialisation failed!");
        // drawErrorMessage("Failed to init LittleFS! Did you upload the data folder?");
        while (1)
            yield(); // We stop here
    }
    MSG_INFOLN("[INFO] LittleFS initialised.");

    MSG_INFOLN("[INFO] Check for general.json");
    // Let's first check if all the files we need exist
    if (!checkfile("/config/general.json", true))
    {
        MSG_ERRORLN("[ERROR] /config/general.json not found!");
        tft.printf("[ERROR] /config/general.json not found!\n");
        while (1)
            yield(); // Stop!
    }

    for (size_t i = 0; i < NUM_PAGES; i++)
    {
        char filename[LEN_FILENAME];
        char filenumber[4];
        strlcpy(filename, "/config/menu", sizeof(filename));
        sprintf(filenumber, "%d", i);
        strlcat(filename, filenumber, sizeof(filename));
        strlcat(filename, ".json", sizeof(filename));
        MSG_INFO1("[INFO] Check for ", filename);

        if (!checkfile(filename, false))
        {
            MSG_WARN2("[WARNING]: ", filename, " not found. Initializing to default.json");
            tft.printf("[WARNING]: %s not found. Initializing to default.json\n", filename);

            if (CopyFile("/config/default.json", filename))
            {
                MSG_INFO2("[WARN] Successful initialization of ", filename, " to default.json");
            }
            else
            {
                MSG_ERROR1("[ERROR] Failed to initialize ", filename);
                checkfile(filename, true); // This will force a message to the TFT screen

                while (1)
                    yield(); // Stop!
            }
        }
    }

    // Now load the config files, starting with general.json
    if (!loadConfigGeneral())
    {
        MSG_WARNLN("[WARNING]: general.json failed to load!");
        pageNum = SPECIAL_PAGE_LOADERROR;
    }
    else
    {
        // Menu to display on startup
        pageNum = constrain(generalconfig.startup_menu, 0, NUM_PAGES - 1);
    }

    // Next, load the menu pages
    for (size_t i = 0; i < NUM_PAGES; i++)
    {
        char menuName[LEN_FILENAME];
        char menuNumber[4];
        char menuNumberZeroIndex[4];
        strlcpy(menuName, "menu", sizeof(menuName));
        sprintf(menuNumber, "%d", i);
        strlcat(menuName, menuNumber, sizeof(menuName));

        if (!loadConfigMenu(menuName))
        {
            Serial.print("[WARNING]: menu");
            sprintf(menuNumberZeroIndex, "%d", i);
            Serial.print(menuNumberZeroIndex);
            MSG_INFOLN(".json seems to be corrupted!");
            Serial.print("[WARNING]: To reset to default type 'reset ");
            Serial.print(menuName);
            MSG_INFOLN("'.");
            strlcpy(jsonFileFail, menuName, sizeof(jsonFileFail));
            pageNum = SPECIAL_PAGE_LOADERROR;
        }
    }

    if (pageNum >= 0 && pageNum < NUM_PAGES)
    {
        MSG_INFOLN("[INFO] All configs loaded");
    }
    else
    {
        MSG_ERRORLN("[ERROR] Configs failed to load");
        while (1)
            yield(); // Stop!
    }

    // Check for free space

    MSG_INFO("[INFO] Free Space: ");
    MSG_INFOLN(FILESYSTEM.totalBytes() - FILESYSTEM.usedBytes());

    MSG_INFOLN("[INFO] Start touch screen");
    touch.begin();

    // Setup UART for communication with the CADDeck
    serialChannel.Begin();

    // LCD backlight on
    tft.BLOn();

    MSG_INFOLN("[INFO] Initial screen content");
    drawKeypad();
    drawGrid();

    // move = true;
    // draw_logo(SCREEN_CENTER_X, SCREEN_CENTER_Y, "/logos/move_64.png", true);

    MSG_INFOLN("Setup done");
}

void loop()
{
    long time_now = millis();
    if (time_now - last_loop_start > LOOP_INTERVAL)
    {
        this_loop_start = time_now;

        processTouchEvents();

        uint8_t eventType = serialChannel.ReceiveData();

        switch (eventType)
        {
        case LCDKNOB_SEND_SET_BUTTON_STATE:
        {
            uint8_t buttonNumber = serialChannel.GetLastEventButtonNumber();
            uint8_t buttonState = serialChannel.GetLastEventState();

            MSG_DEBUGLN("Set button state received for button " + String(buttonNumber) + " state " + String(buttonState));

            if (buttonState == 1)
            {
                menu[pageNum].button[buttonNumber].state = ButtonState::ButtonState_Latched;
                menu[pageNum].button[buttonNumber].islatched = true;
            }
            else
            {
                menu[pageNum].button[buttonNumber].state = ButtonState::ButtonState_NoAction;
                menu[pageNum].button[buttonNumber].islatched = false;
            }

            drawButtonNumber(pageNum, buttonNumber);
            drawGrid();
            break;
        }

        case LCDKNOB_SEND_BUTTON_STATE_REQUEST:
        {
            uint8_t buttonNumber = serialChannel.GetLastEventButtonNumber();
            MSG_DEBUGLN("Button state request received for button " + String(buttonNumber));

            if (menu[pageNum].button[buttonNumber].islatched)
                serialChannel.SendButtonState(buttonNumber, true);
            else
                serialChannel.SendButtonState(buttonNumber, false);

            break;
        }

        case LCDKNOB_SEND_SET_MENU:
        {
            uint8_t menuNumber = serialChannel.GetLastEventMenuNumber();
            MSG_DEBUGLN("Set menu received for menu " + String(menuNumber));
            pageNum = menuNumber;
            drawKeypad();
            drawGrid();
            break;
        }

        default:
            break;
        }

        last_loop_start = this_loop_start;
    }
}
