#pragma once

// Define the storage to be used. This must be done before including <LovyanGFX.hpp> if you needed
// to use any of the lgfx functions that use the file system.
#define FILESYSTEM_LITTLEFS

#ifdef FILESYSTEM_LITTLEFS
#define FILESYSTEM LittleFS
#include <LittleFS.h> // https://github.com/lorol/LITTLEFS
#else
#define FILESYSTEM SPIFFS
#include <SPIFFS.h> // Filesystem support header
#endif

#include <ArduinoJson.h> // Using ArduinoJson to read and write config files

#include <LovyanGFX.hpp>
#include "CST816S.h"

#include "lgfx_tft.h"

// length of filenames (e.g. for logos, menus, etc). Includes the file extension
#define LEN_FILENAME 32
// length of menu button descriptions
#define LEN_DESCRIPTION 32
// length of menu name
#define LEN_NAME 32

#include "drawhelper.h"
#include "ConfigLoad.h"
#include "ConfigSave.h"
#include "ScreenHelper.h"
#include "action.h"
#include "ConfigHelper.h"
#include "ProcessTouchscreen.h"
#include "SerialChannel.h"

#define I2C_SDA 4
#define I2C_SCL 5
#define TP_INT 0
#define TP_RST 1
#define TFT_BL 3

const uint16_t SCREEN_WIDTH = 240;
const uint16_t SCREEN_HEIGHT = 240;

const uint16_t SCREEN_CENTER_X = SCREEN_WIDTH / 2;
const uint16_t SCREEN_CENTER_Y = SCREEN_HEIGHT / 2;

const uint8_t LOGO_SIZE_X = 64;
const uint8_t LOGO_SIZE_Y = 64;

const uint16_t BUTTON_CIRCLE_RADIUS = 87;

// Basic macros for debug and info messages to the serial port
// If too many messages are being sent the web server used for the configurator may not work as well.
#define LOG_MSG_BASIC 1
#define LOG_MSG_LEVEL 3 // 1=ERROR, 2=ERROR+WARN, 3=ERROR+WARN+INFO
#define LOG_MSG_DEBUG 1
#define LOG_MSG_TOUCH_DEBUG 0 // Use as needed for specific issue debug
#include "std_defs.h"

// Project version number
extern const char *versionnumber;

// Text Button Label Font
#define LABEL_FONT &FreeSansBold12pt7b

extern unsigned long previousMillis;
extern unsigned long Interval;

extern LGFX_TFT tft;
extern CST816S touch;

extern char logopath[];

// UART1
const uint8_t UART1_RX_PIN = 20;
const uint8_t UART1_TX_PIN = 21;

extern SerialChannel serialChannel;

// placeholder for the pagenumber we are on (0 indicates home)
extern int pageNum;

// Button layout and number of pages with buttons
const uint8_t BUTTONS_PER_PAGE = 7;
const uint8_t NUM_PAGES = 2; // Includes Menu0 which is the home page
const uint8_t SPECIAL_PAGE_LOADERROR = (NUM_PAGES + 1);
const uint8_t MAX_PAGE_NUM = (NUM_PAGES + 1); // update this to the highest special page number

// Struct Action: actions and value
struct Actions
{
    uint8_t action;
    uint8_t value;
    char symbol[64];
};

// Each button has an action struct in it
struct Button
{
    Actions actions[3];
    bool latch;
    bool islatched;
    char logo[LEN_FILENAME];
    char latchlogo[LEN_FILENAME];
    char description[LEN_DESCRIPTION];
    uint16_t imageBGColour;
    uint16_t latchImageBGColour;
    uint8_t imageBGColourValid;
    uint8_t latchImageBGColourValid;
    uint8_t state;
};

// Each menu has an array of buttons
struct MenuLayout
{
    uint16_t logo_size_x_y;
    uint16_t num_buttons;
    uint16_t button_size_x_y;
    uint16_t key_spacing_x;
    uint16_t key_spacing_y;
};

struct Menu
{
    char name[LEN_NAME];
    MenuLayout layout;
    Button button[BUTTONS_PER_PAGE];
};

// Struct to hold the general logos.
struct Generallogos
{
    char homebutton[LEN_FILENAME];
    char configurator[LEN_FILENAME];
};

// Struct to hold the general config like colours.
struct Config
{
    uint16_t menuButtonColour;
    uint16_t functionButtonColour;
    uint16_t backgroundColour;
    uint16_t latchedColour;
    bool sleepenable;
    uint16_t sleeptimer;
    uint8_t modifier1;
    uint8_t modifier2;
    uint8_t modifier3;
    uint16_t startup_menu;
};

// Create instances of the structs
extern Config generalconfig;

// And finally, we have an array of menus
extern Menu menu[NUM_PAGES];

extern unsigned long previousMillis;
extern unsigned long Interval;
extern bool displayinginfo;
extern bool displayingIOValues;
extern bool displayingButtonDescriptions;
extern char jsonFileFail[32];

//--------- Internal references ------------
enum SpecialFn
{
    SpecialFn_NoAction = 0,
    SpecialFn_ConfigMode = 1,
    SpecialFn_DisplayBrightnessDown = 2,
    SpecialFn_DisplayBrightnessUp = 3,
    SpecialFn_SleepEnable = 4,
    SpecialFn_HomePage = 5,
    SpecialFn_SaveConfig = 6
};

enum ButtonState
{
    ButtonState_NoAction = 0,
    ButtonState_Pressed = 1,
    ButtonState_Released = 2,
    ButtonState_Latched = 3
};
