#include "DrawHelper.h"

uint8_t draw_logo(uint16_t x, uint16_t y, const char *filename, bool erase)
{

    if (erase)
    {
        tft.fillCircle(x, y, 39, TFT_BLACK);
    }

    tft.drawPngFile(FILESYSTEM, filename, x - LOGO_SIZE_X / 2, y - LOGO_SIZE_Y / 2);

    return true;
}

void drawGrid()
{
    tft.drawCircle(SCREEN_CENTER_X, SCREEN_CENTER_Y, 40, TFT_DARKGRAY);
    for (uint8_t i = 0; i < 6; i++)
    {
        uint16_t x0 = SCREEN_CENTER_X + sin(i * 3.1416 / 3) * 40;
        uint16_t y0 = SCREEN_CENTER_Y + cos(i * 3.1416 / 3) * 40;
        uint16_t x1 = SCREEN_CENTER_X + sin(i * 3.1416 / 3) * 110;
        uint16_t y1 = SCREEN_CENTER_Y + cos(i * 3.1416 / 3) * 110;

        tft.drawLine(x0, y0, x1, y1, TFT_DARKGRAY);
    }
}

/**
 * @brief This function checks if the button is active or inactive.
 *        Inactive means that the logo is "blank.bmp" .
 *
 * @param page uint8_t
 * @param button_index uint8_t
 *
 * @return boolean true if active, false if inactive
 *
 * @note Button 0 is at the center, and the remainder are erqually spaced around the center button.
 */
bool isActiveButton(uint8_t page, uint8_t button_index)
{
    bool activeButton = false;
    char logoPathAndName[64];

    strcpy(logoPathAndName, logopath);
    strcat(logoPathAndName, "blank.bmp");

    if (strcmp(menu[pageNum].button[button_index].logo, logoPathAndName) != 0)
    {
        activeButton = true;
    }

    return activeButton;
}

/**
 * @brief This function draws a button, both the logo and the surround box.
 *
 * @param page uint8_t
 * @param button_index uint8_t
 *
 * @return boolean true if active, false if inactive
 *
 * @note Button 0 is at the center, and the remainder are erqually spaced around the center button.
 */
void drawButtonNumber(uint8_t page, uint8_t button_index)
{
    bool drawTransparent;
    uint16_t imageBGColor;
    uint16_t buttonBG = TFT_BLACK;
    uint16_t outlineColor = TFT_BLACK;
    bool activeButton = false;

    buttonBG = TFT_BLACK;
    outlineColor = TFT_BLACK;
    activeButton = false;

    uint8_t status = RETURN_SUCCESS;
    activeButton = isActiveButton(pageNum, button_index);

    // MSG_DEBUGLN("drawButtonNumber: pageNum = " + String(pageNum) + " button = " + String(button_index));

    // After drawing the button outline we call this to draw a logo.
    if (activeButton)
    {

        uint16_t x;
        uint16_t y;

        if (button_index == 0)
        {
            x = SCREEN_CENTER_X;
            y = SCREEN_CENTER_Y;
        }
        else
        {
            float angle = (button_index - 1 + 0.5) * 2.0 * 3.1416 / (BUTTONS_PER_PAGE - 1);
            x = SCREEN_CENTER_X + sin(angle) * BUTTON_CIRCLE_RADIUS;
            y = SCREEN_CENTER_Y + cos(angle) * BUTTON_CIRCLE_RADIUS;
        }

        // MSG_DEBUGLN("...is active button. Drawing at: x = " + String(x) + " y = " + String(y));

        if (menu[pageNum].button[button_index].islatched)
        {
            draw_logo(x, y, menu[pageNum].button[button_index].latchlogo, true);
        }
        else
        {
            draw_logo(x, y, menu[pageNum].button[button_index].logo, true);
        }
    }
}

void drawButtonCircle(uint8_t button_index,uint16_t color)
{
    uint16_t x;
    uint16_t y;

    if (button_index == 0)
    {
        x = SCREEN_CENTER_X;
        y = SCREEN_CENTER_Y;
    }
    else
    {
        float angle = (button_index - 1 + 0.5) * 2.0 * 3.1416 / (BUTTONS_PER_PAGE - 1);
        x = SCREEN_CENTER_X + sin(angle) * BUTTON_CIRCLE_RADIUS;
        y = SCREEN_CENTER_Y + cos(angle) * BUTTON_CIRCLE_RADIUS;
    }
    tft.fillCircle(x, y, 30, TFT_RED);
}


/**
* @brief This function draws the set buttons that are on each page.
         Pagenumber is global and doesn't need to be passed.
*
* @param none
*
* @return none
*
* @note none
*/
void drawKeypad()
{
    // MSG_DEBUGLN("drawKeypad: pageNum = " + String(pageNum));
    if ((pageNum >= 0) && (pageNum < NUM_PAGES))
    {
        // Draw the button outlines and fill them with colours
        for (uint8_t buttonIndex = 0; buttonIndex < BUTTONS_PER_PAGE; buttonIndex++)
        {
            drawButtonNumber(pageNum, buttonIndex);
        }
    }
    else if (pageNum == SPECIAL_PAGE_LOADERROR)
    {
        // Pagenum max + 4 means that a JSON config failed to load completely.
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(5, 50);
        tft.setTextFont(2);
        tft.setTextSize(1);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);

        tft.printf("  %s failed to load.\n", jsonFileFail);
    }
}

void drawErrorMessage(String message)
{
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, SCREEN_HEIGHT / 2);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.println(message);
}

uint32_t usedPSRAM()
{
    return ESP.getPsramSize() - ESP.getFreePsram();
}

/**
 * @brief This function validates a page number
 *
 * @param page uint8_t. The page number to be validated
 *
 * @return true if the page number is valid, false otherwise
 *
 * @note  It is not currently being used, but is provided for potential future use.
 */
uint8_t isValidPageNumber(uint8_t page)
{
    uint8_t valid = false;

    valid = (page >= 0) && (page <= MAX_PAGE_NUM);

    return valid;
}

/**
 * @brief This function determines which button corresponds to the x and y parameters
 *
 * @param x uint16_t. The x coordinate, in pixels, of the point to be checked
 *
 * @param y uint16_t. The y coordinate, in pixels, of the point to be checked
 * 
 * @return uint8_t. The button number. 255 if invalid. 
  */
uint8_t getButtonPressedNumber(uint16_t x, uint16_t y)
{
    uint8_t button = 255;

    for (uint8_t button_index = 0; button_index < BUTTONS_PER_PAGE; button_index++)
    {
        uint16_t button_x;
        uint16_t button_y;
        if (button_index == 0)
        {
            button_x = SCREEN_CENTER_X;
            button_y = SCREEN_CENTER_Y;
        }
        else
        {
            float angle = (button_index - 1 + 0.5) * 2.0 * 3.1416 / (BUTTONS_PER_PAGE - 1);
            button_x = SCREEN_CENTER_X + sin(angle) * BUTTON_CIRCLE_RADIUS;
            button_y = SCREEN_CENTER_Y + cos(angle) * BUTTON_CIRCLE_RADIUS;
        }

        if ((abs(x - button_x) < 50) && (abs(y - button_y) < 50))
        {
            button = button_index;
            break;
        }
    }
    return button;
}