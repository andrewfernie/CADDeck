#include "ConfigLoad.h"
/**
* @brief This function loads the menu configuration.
*
* @param String the config to be loaded
*
* @return none
*
* @note Options for values are: colors, menu1, menu2, menu3
         menu4, and menu5
*/
bool loadConfigGeneral()
{
    char fileNameType[LEN_FILENAME];

    File configfile = FILESYSTEM.open("/config/general.json", "r");

    DynamicJsonDocument doc(512);

    DeserializationError error = deserializeJson(doc, configfile);

    // Parsing colors
    const char *menubuttoncolor = doc["menubuttoncolor"] | "#009bf4";         // Get the colour for the menu and back home buttons.
    const char *functionbuttoncolor = doc["functionbuttoncolor"] | "#00efcb"; // Get the colour for the function buttons.
    const char *latchcolor = doc["latchcolor"] | "#fe0149";                   // Get the colour to use when latching.
    const char *bgcolor = doc["background"] | "#000000";                      // Get the colour for the background.

    char menubuttoncolorchar[64];
    strcpy(menubuttoncolorchar, menubuttoncolor);
    unsigned long rgb888menubuttoncolor = convertHTMLtoRGB888(menubuttoncolorchar);
    generalconfig.menuButtonColour = convertRGB888ToRGB565(rgb888menubuttoncolor);

    char colorbuf[64];
    snprintf(colorbuf, sizeof(colorbuf), "[INFO] menuButtonColour (RGB565) = 0x%06X", generalconfig.menuButtonColour);
    MSG_INFOLN(colorbuf);

    char functionbuttoncolorchar[64];
    strcpy(functionbuttoncolorchar, functionbuttoncolor);
    unsigned long rgb888functionbuttoncolor = convertHTMLtoRGB888(functionbuttoncolorchar);
    generalconfig.functionButtonColour = convertRGB888ToRGB565(rgb888functionbuttoncolor);

    snprintf(colorbuf, sizeof(colorbuf), "[INFO] functionButtonColour (RGB565) = 0x%06X", generalconfig.functionButtonColour);
    MSG_INFOLN(colorbuf);

    char latchcolorchar[64];
    strcpy(latchcolorchar, latchcolor);
    unsigned long rgb888latchcolor = convertHTMLtoRGB888(latchcolorchar);
    generalconfig.latchedColour = convertRGB888ToRGB565(rgb888latchcolor);

    snprintf(colorbuf, sizeof(colorbuf), "[INFO] latchedColour (RGB565) = 0x%06X", generalconfig.latchedColour);
    MSG_INFOLN(colorbuf);

    char backgroundcolorchar[64];
    strcpy(backgroundcolorchar, bgcolor);
    unsigned long rgb888backgroundcolor = convertHTMLtoRGB888(backgroundcolorchar);
    generalconfig.backgroundColour = convertRGB888ToRGB565(rgb888backgroundcolor);

    snprintf(colorbuf, sizeof(colorbuf), "[INFO] backgroundColour (RGB565) = 0x%06X", generalconfig.backgroundColour);
    MSG_INFOLN(colorbuf);

    // Loading general settings

    bool sleepenable = doc["sleepenable"] | false;
    if (sleepenable)
    {
        generalconfig.sleepenable = true;
    }
    else
    {
        generalconfig.sleepenable = false;
    }

    uint16_t sleeptimer = doc["sleeptimer"] | 60;
    generalconfig.sleeptimer = sleeptimer;

    uint8_t modifier1 = doc["modifier1"] | 0;
    generalconfig.modifier1 = modifier1;

    uint8_t modifier2 = doc["modifier2"] | 0;
    generalconfig.modifier2 = modifier2;

    uint8_t modifier3 = doc["modifier3"] | 0;
    generalconfig.modifier3 = modifier3;

    uint16_t startup_menu = doc["startup_menu"] | 0;
    generalconfig.startup_menu = startup_menu;

    configfile.close();

    if (error)
    {
        MSG_ERROR2("[ERROR] general.json deserializeJson() error", error.c_str(), doc.memoryUsage());
        return false;
    }
    else
    {
        MSG_INFO1("[INFO] general.json load_config() success. Memory usage: ", doc.memoryUsage());
    }
    return true;
}

bool loadConfigMenu(String menuToLoad)
{
    char fileNameType[LEN_FILENAME];
    int fileNameMenuNumber = 0;

    // templogopath is used to hold the complete path of an image. It is empty for now.
    char templogopath[64] = "";

    int numConverted = 0;
    numConverted = sscanf(menuToLoad.c_str(), "%4s%d", fileNameType, &fileNameMenuNumber);
    MSG_INFO2("[INFO] load_config parameter", menuToLoad.c_str(), numConverted);

    // --------------------- Loading menu ----------------------
    if ((numConverted == 2) && (strncmp("menu", menuToLoad.c_str(), 4) == 0) && (fileNameMenuNumber >= 0) && (fileNameMenuNumber < NUM_PAGES))
    {
        char configFileName[LEN_FILENAME];
        char menuFileNameRoot[10];

        snprintf(configFileName, sizeof(configFileName), "/config/menu%d.json", fileNameMenuNumber);

        MSG_INFO1("[INFO] load_config opening file ", configFileName);
        File configfile = FILESYSTEM.open(configFileName, "r");

        DynamicJsonDocument doc(5000);

        DeserializationError error = deserializeJson(doc, configfile);

        snprintf(menuFileNameRoot, sizeof(menuFileNameRoot), "menu%d", fileNameMenuNumber);
        const char *menuName = doc["menuname"] | menuFileNameRoot;
        MSG_INFO1("[INFO] load_config menuname is ", menuName);
        strlcpy(menu[fileNameMenuNumber].name, menuName, sizeof(menu[fileNameMenuNumber].name));

        for (uint8_t buttonIndex = 0; buttonIndex < BUTTONS_PER_PAGE; buttonIndex++)
        {

            char objectName[10];

            snprintf(objectName, sizeof(objectName), "button%d", buttonIndex + 1);

            const char *logo = doc[objectName]["logo"] | "question.bmp";

            strlcpy(templogopath, logopath, sizeof(templogopath));
            strlcat(templogopath, logo, sizeof(templogopath));
            strlcpy(menu[fileNameMenuNumber].button[buttonIndex].logo, templogopath,
                    sizeof(menu[fileNameMenuNumber].button[buttonIndex].logo));
            MSG_INFO2("[INFO] load_config loading logo", objectName, templogopath);

            const char *latchlogo = doc[objectName]["latchlogo"] | "question.bmp";

            menu[fileNameMenuNumber].button[buttonIndex].latch = doc[objectName]["latch"] | false;

            strlcpy(templogopath, logopath, sizeof(templogopath));
            strlcat(templogopath, latchlogo, sizeof(templogopath));
            strlcpy(menu[fileNameMenuNumber].button[buttonIndex].latchlogo, templogopath,
                    sizeof(menu[fileNameMenuNumber].button[buttonIndex].latchlogo));
            MSG_INFO2("[INFO] load_config loading latchlogo", objectName, templogopath);

            menu[fileNameMenuNumber].button[buttonIndex].imageBGColourValid = false;
            menu[fileNameMenuNumber].button[buttonIndex].latchImageBGColourValid = false;

            menu[fileNameMenuNumber].button[buttonIndex].state = ButtonState::ButtonState_NoAction;
            menu[fileNameMenuNumber].button[buttonIndex].islatched = false;
        }

        configfile.close();

        if (error)
        {
            MSG_ERROR1("[ERROR] deserializeJson() error", error.c_str());
            MSG_ERROR2("[ERROR] Will initialize ", configFileName, " to default.json");

            if (CopyFile("/config/default.json", configFileName))
            {
                MSG_INFO2("[INFO] Successful initialization of ", configFileName, " to default.json");
            }
            else
            {
                MSG_ERROR1("[ERROR] Failed to initialize ", configFileName);
                checkfile(configFileName, true); // This will force a message to the TFT screen

                while (1)
                    yield(); // Stop!

                return false;
            }
        }
        return true;
    }
    else
    {
        MSG_ERROR1("[ERROR] Invalid call to loadConfig(). Argument was ", menuToLoad.c_str());
        return false;
    }
}
