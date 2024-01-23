#include "ScreenHelper.h"

PNG png;
#define MAX_IMAGE_WIDTH 100  // Adjust for your images

int16_t xpos = 0;
int16_t ypos = 0;
uint16_t *pPNGImagePtr = 0;  // Has to be global as it is used in the callback function

/**
* @brief This function reads chunks of 2 bytes of data from a
         file and returns the data.
*
* @param &f
*
* @return uint16_t
*
* @note litte-endian
*/
uint16_t read16(fs::File &f)
{
    uint16_t result;
    ((uint8_t *)&result)[0] = f.read();  // LSB
    ((uint8_t *)&result)[1] = f.read();  // MSB
    return result;
}

/**
* @brief This function reads chunks of 4 bytes of data from a
         file and returns the data.
*
* @param &f
*
* @return uint32_t
*
* @note little-endian
*/
uint32_t read32(fs::File &f)
{
    uint32_t result;
    ((uint8_t *)&result)[0] = f.read();  // LSB
    ((uint8_t *)&result)[1] = f.read();
    ((uint8_t *)&result)[2] = f.read();
    ((uint8_t *)&result)[3] = f.read();  // MSB
    return result;
}

/**
* @brief This functions accepts a HTML including the # colour code
         (eg. #FF00FF)  and returns it in RGB888 format.
*
* @param *html char (including #)
*
* @return unsigned long
*
* @note none
*/
unsigned long convertHTMLtoRGB888(char *html)
{
    char *hex = html + 1;  // remove the #
    unsigned long rgb = strtoul(hex, NULL, 16);
    return rgb;
}

/**
* @brief This functions accepts a RGB888 format color and returns an HTML including the # colour code
         (eg. #FF00FF).
*
* @param unsigned long
*
* @return *html char (including #)
*
* @note none
*/
void convertRGB888toHTML(unsigned long rgb, char *html, unsigned int htmlsize)
{
    snprintf(html, htmlsize, "#%6.6X", rgb & 0x00ffffff);
}

/**
 * @brief This function converts RGB888 to RGB565.
 *
 * @param rgb unsigned long
 *
 * @return unsigned int
 *
 * @note none
 */
unsigned int convertRGB888ToRGB565(unsigned long rgb)
{
    return (((rgb & 0xf80000) >> 8) | ((rgb & 0xfc00) >> 5) | ((rgb & 0xf8) >> 3));
}

/**
 * @brief This function converts RGB565 to RGB888.
 *
 * @param rgb unsigned int
 *
 * @return unsigned long
 *
 * @note none
 */
unsigned long convertRGB565ToRGB888(unsigned int rgb)
{
    return (((rgb & 0xf800) << 8) | ((rgb & 0x7e0) << 5) | ((rgb & 0x1f) << 3));
}

/**
 * @brief This function converts HTML color code to RGB565.
 *
 * @param *html char (including #)
 *
 * @return unsigned int
 *
 * @note none
 */
unsigned int convertHTMLtoRGB565(char *html)
{
    unsigned long rgb888 = convertHTMLtoRGB888(html);
    return convertRGB888ToRGB565(rgb888);
}

/**
 * @brief This function converts RGB565 to HTML color code.
 *
 * @param unsigned int
 *
 * @return *html char (including #)
 *
 * @note none
 */
void convertRGB565toHTML(unsigned int rgb, char *html, unsigned int htmlsize)
{
    unsigned long rgb888 = convertRGB565ToRGB888(rgb);
    convertRGB888toHTML(rgb888, html, htmlsize);
}


// Here are the callback functions that the decPNG library
// will use to open files, fetch data and close the file.

fs::File pngfile;

void *pngOpen(const char *filename, int32_t *size)
{
    // MSG_DEBUG1("Attempting to open %s\n", filename);
    pngfile = FILESYSTEM.open(filename, "r");
    *size = pngfile.size();
    return &pngfile;
}

void pngClose(void *handle)
{
    File pngfile = *((File *)handle);
    if (pngfile)
        pngfile.close();
}

int32_t pngRead(PNGFILE *page, uint8_t *buffer, int32_t length)
{
    if (!pngfile)
        return 0;
    page = page;  // Avoid warning
    return pngfile.read(buffer, length);
}

int32_t pngSeek(PNGFILE *page, int32_t position)
{
    if (!pngfile)
        return 0;
    page = page;  // Avoid warning
    return pngfile.seek(position);
}

//=========================================v==========================================
//                                      pngDraw
//====================================================================================
// This next function will be called during decoding of the png file. We will need two
// versions:
// The first version will draw the image directly to the TFT screen.
void pngDraw(PNGDRAW *pDraw)
{
    uint16_t lineBuffer[MAX_IMAGE_WIDTH];
    png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xffffffff);
    tft.pushImage(xpos, ypos + pDraw->y, pDraw->iWidth, 1, lineBuffer);
}
// The second version will render the image to a buffer in PSRAM. It will be quicker to draw
// the image to the tft from PSRAM than to read it from FLASH and decode it each time
// it is needed.
void pngDrawToPSRAM(PNGDRAW *pDraw)
{
    uint16_t lineBuffer[MAX_IMAGE_WIDTH];
    png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_LITTLE_ENDIAN, 0xffffffff);
    memcpy(pPNGImagePtr, lineBuffer, pDraw->iWidth * 2);
    pPNGImagePtr += pDraw->iWidth;
}


uint8_t getImageType(const char *filename)
{
    uint8_t imageType = ImageType::ImageType_UNKNOWN;

    String filenameStr = String(filename);
    String extension = filenameStr.substring(filenameStr.lastIndexOf(".") + 1);

    if (extension == "bmp") {
        imageType = ImageType::ImageType_BMP;
    }
    else if (extension == "png") {
        imageType = ImageType::ImageType_PNG;
    }
    else {
        MSG_ERROR1("[ERROR] getImageType: Unknown image type: ", extension.c_str());
    }

    return imageType;
}

/**
 * @brief This function draws an image on the TFT screen according
 *        to the given x and y coordinates. It identifies the image type (bmp or png)
 *        and calls the appropriate function.`
 *
 * @param  *filename
 * @param x int16_t
 * @param y int16_t
 * @param transparent uint8_t
 *
 * @return none
 *
 * @note If transparent is true then black pixels are not drawn (i.e. they are transparent). If false,
 *       black pixels are drawn.
 */
void drawImage(const char *filename, int16_t x, int16_t y, uint8_t transparent)
{
    if ((x >= tft.width()) || (y >= tft.height()))
        return;

    uint8_t imageType = getImageType(filename);

    if (imageType == ImageType::ImageType_BMP) {
        drawBmp(filename, x, y, transparent);
    }
    else if (imageType == ImageType::ImageType_PNG) {
        drawPng(filename, x, y, transparent);
    }
    else {
        MSG_ERROR1("[ERROR] drawImage: Unknown image type: ", filename);
    }
}

/**
* @brief This function draws a BMP on the TFT screen according
         to the given x and y coordinates.
*
* @param  *filename
* @param x int16_t
* @param y int16_t
* @param transparent uint8_t
*
* @return none
*
* @note If transparent is true then black pixels are not drawn (i.e. they are transparent). If false,
*       black pixels are drawn.
*/
void drawBmp(const char *filename, int16_t x, int16_t y, uint8_t transparent)
{
    if ((x >= tft.width()) || (y >= tft.height())) {
        MSG_ERROR3("[ERROR] drawBmp: Invalid x,y coordinates: ", x, y, filename);
        return;
    }

    fs::File bmpFS;

    bmpFS = FILESYSTEM.open(filename, "r");

    if (!bmpFS) {
        MSG_WARNLN("[WARNING]: Bitmap not found: ");
        MSG_WARNLN(filename);
        filename = "/logos/question.bmp";
        bmpFS = FILESYSTEM.open(filename, "r");
    }

    uint32_t seekOffset;
    uint16_t w, h, row;
    uint8_t r, g, b;

    if (read16(bmpFS) == 0x4D42) {
        read32(bmpFS);
        read32(bmpFS);
        seekOffset = read32(bmpFS);
        read32(bmpFS);
        w = read32(bmpFS);
        h = read32(bmpFS);

        if ((read16(bmpFS) == 1) && (read16(bmpFS) == 24) && (read32(bmpFS) == 0)) {
            y += h - 1;

            bool oldSwapBytes = tft.getSwapBytes();
            tft.setSwapBytes(true);
            bmpFS.seek(seekOffset);

            uint16_t padding = (4 - ((w * 3) & 3)) & 3;
            uint8_t lineBuffer[w * 3 + padding];

            for (row = 0; row < h; row++) {
                bmpFS.read(lineBuffer, sizeof(lineBuffer));
                uint8_t *bptr = lineBuffer;
                uint16_t *tptr = (uint16_t *)lineBuffer;
                // Convert 24 to 16 bit colours
                for (uint16_t col = 0; col < w; col++) {
                    b = *bptr++;
                    g = *bptr++;
                    r = *bptr++;
                    *tptr++ = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
                }

                // Push the pixel row to screen, pushImage will crop the line if needed
                // y is decremented as the BMP image is drawn bottom up

                if (transparent) {
                    tft.pushImage(x, y--, w, 1, (uint16_t *)lineBuffer, TFT_BLACK);
                }
                else {
                    tft.pushImage(x, y--, w, 1, (uint16_t *)lineBuffer);
                }
            }
            tft.setSwapBytes(oldSwapBytes);
        }
        else
            MSG_ERROR1("[ERROR] BMP format not recognized for file: ", filename);
    }
    bmpFS.close();
}

/**
* @brief This function draws a PNG image on the TFT screen according
         to the given x and y coordinates.
*
* @param  *filename
* @param x int16_t
* @param y int16_t
* @param transparent uint8_t
*
* @return none
*
* @note If transparent is true then black pixels are not drawn (i.e. they are transparent). If false,
*       black pixels are drawn.
*/
void drawPng(const char *filename, int16_t x, int16_t y, uint8_t transparent)
{
    if ((x >= tft.width()) || (y >= tft.height()))
    {
        MSG_ERROR3("[ERROR] drawPng: Invalid x,y coordinates: ", x, y, filename);
        return;
    }

    xpos = x;
    ypos = y;

    fs::File pngFS;

    int16_t rc = png.open(filename, pngOpen, pngClose, pngRead, pngSeek, pngDraw);
    if (rc == PNG_SUCCESS) {
        tft.startWrite();
//        Serial.printf("image specs: (%d x %d), %d bpp, pixel type: %d\n", png.getWidth(), png.getHeight(), png.getBpp(), png.getPixelType());
        uint32_t dt = millis();
        if (png.getWidth() > MAX_IMAGE_WIDTH) {
            MSG_ERROR1("[ERROR] Image too wide for allocated line buffer size for ", filename);
        }
        else {
            rc = png.decode(NULL, 0);
            png.close();
        }
        tft.endWrite();
    }
    else {
        MSG_ERROR1("[ERROR] Error opening PNG file: ", filename);
    }
}

/**
* @brief This function draws a BMP on the TFT screen according
         to the given x and y coordinates.
*
* @param pImage uint16_t*
* @param x int16_t
* @param y int16_t
* @param transparent uint8_t
*
* @return uint8_t 0 = success, other = error
*
* @note If transparent is true then black pixels are not drawn (i.e. they are transparent). If false,
*       black pixels are drawn.
*/
uint8_t drawLogoFromPSRAM(uint16_t *pImage, int16_t x, int16_t y, uint8_t transparent)
{
    uint16_t *pImagePtr;
    uint16_t w, h, row;

    uint8_t status = ReturnSuccess;

    if ((x >= tft.width()) || (y >= tft.height()))
        return ReturnFail;

    if (pImage != nullptr) {
        w = pImage[0];
        h = pImage[1];
        pImagePtr = pImage + 2;

        // y += h - 1;

        bool oldSwapBytes = tft.getSwapBytes();
        tft.setSwapBytes(true);

        if (transparent) {
            tft.pushImage(x, y, w, h, pImagePtr, TFT_BLACK);
        }
        else {
            tft.pushImage(x, y, w, h, pImagePtr);
        }

        tft.setSwapBytes(oldSwapBytes);
    }

    return status;
}

/**
 * @brief This function draws an image on the TFT screen according
 *        to the given x and y coordinates. It identifies the image type (bmp or png)
 *        and calls the appropriate function.`
 *
 * @param  *filename
 * @param x int16_t
 * @param y int16_t
 * @param transparent uint8_t
 *
 * @return none
 *
 * @note If transparent is true then black pixels are not drawn (i.e. they are transparent). If false,
 *       black pixels are drawn.
 */
uint8_t loadImageToPSRAM(const char *filename, uint16_t **pImage)
{
    uint8_t status = ReturnFail;

    uint8_t imageType = getImageType(filename);

    if (imageType == ImageType::ImageType_BMP) {
        status = loadBMPToPSRAM(filename, pImage);
    }
    else if (imageType == ImageType::ImageType_PNG) {
        status = loadPNGToPSRAM(filename, pImage);
    }
    else {
        MSG_ERROR1("[ERROR] loadImageToPSRAM: Unknown image type: ", filename);
        status = ReturnFail;
    }

    return status;
}
/**
* @brief This function loads a BMP logo file into a buffer allocated in PSRAM. This gets all of the (slow)
         filesystem accesses out of the way once. After that the logo can be drawn directly from PSRAM.
*
* @param  *filename
* @param pImage *uint16_t
*
* @return 0 for success, other for failure
*
* @note Make sure to check the status of the return value. If it is not equal to zero, then the logo was not loaded.
*/
uint8_t loadBMPToPSRAM(const char *filename, uint16_t **pImage)
{
    uint8_t status = ReturnSuccess;

    // MSG_DEBUG1("loadBmpToPSRAM logo: ", filename);

    fs::File bmpFS;
    bmpFS = FILESYSTEM.open(filename, "r");

    if (!bmpFS) {
        MSG_WARN1("[WARN]: Bitmap not found: ", filename);
        filename = "/logos/question.bmp";
        MSG_WARN1("[WARN]: Will use : ", filename);
        bmpFS = FILESYSTEM.open(filename, "r");
    }

    if (!bmpFS) {
        status = ReturnFail;
        MSG_ERROR1("[ERROR] Failed to open file: ", filename);
    }
    else {
        uint32_t seekOffset;
        uint16_t w, h;
        uint8_t r, g, b;
        uint16_t *pImagePtr = 0;

        if (read16(bmpFS) == 0x4D42) {
            read32(bmpFS);
            read32(bmpFS);
            seekOffset = read32(bmpFS);
            read32(bmpFS);
            w = read32(bmpFS);
            h = read32(bmpFS);

            if ((read16(bmpFS) == 1) && (read16(bmpFS) == 24) && (read32(bmpFS) == 0)) {
                uint16_t imageSize = 2 + 2 + (w * h * 2);  // 2 bytes for width, 2 bytes for height, 2 bytes per pixel
                uint16_t *pLogoImage = (uint16_t *)ps_malloc(imageSize);

                if (pLogoImage == NULL) {
                    MSG_ERROR1("[ERROR] Could not allocate memory for image: ", filename);
                    status = ReturnFail;
                }
                else {
                    pLogoImage[0] = w;
                    pLogoImage[1] = h;
                    pImagePtr = pLogoImage + 2;
                }

                if (status == ReturnSuccess) {
                    bmpFS.seek(seekOffset);

                    uint16_t padding = (4 - ((w * 3) & 3)) & 3;
                    uint8_t lineBuffer[w * 3 + padding];

                    for (int row = h - 1; row >= 0; row--) {
                        bmpFS.read(lineBuffer, sizeof(lineBuffer));
                        uint8_t *bptr = lineBuffer;
                        uint16_t *tptr = (uint16_t *)lineBuffer;
                        // Convert 24 to 16 bit colours
                        for (int col = 0; col < w; col++) {
                            b = *bptr++;
                            g = *bptr++;
                            r = *bptr++;
                            pImagePtr[row * w + col] = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
                        }
                    }
                    *pImage = pLogoImage;
                }
            }
            else {
                MSG_ERROR1("[ERROR] BMP format not recognized for file: ", filename);
                status = ReturnFail;
            }
        }
        bmpFS.close();
    }

    return status;
}

/**
* @brief This function loads a PNG logo file into a buffer allocated in PSRAM. This gets all of the (slow)
         filesystem accesses out of the way once. After that the logo can be drawn directly from PSRAM.
*
* @param  *filename
* @param pImage *uint16_t
*
* @return 0 for success, other for failure
*
* @note Make sure to check the status of the return value. If it is not equal to zero, then the logo was not loaded.
*/
uint8_t loadPNGToPSRAM(const char *filename, uint16_t **pImage)
{
    uint8_t status = ReturnSuccess;
    uint32_t seekOffset;
    uint16_t w, h;
    uint8_t r, g, b;

    fs::File pngFS;

    int16_t rc = png.open(filename, pngOpen, pngClose, pngRead, pngSeek, pngDrawToPSRAM);

    if (rc == PNG_SUCCESS) {
//        Serial.printf("image specs: (%d x %d), %d bpp, pixel type: %d\n", png.getWidth(), png.getHeight(), png.getBpp(), png.getPixelType());

        uint16_t imageSize = 2 + 2 + (png.getWidth() * png.getHeight() * 2);  // 2 bytes for width, 2 bytes for height, 2 bytes per pixel (will always be 565)
        uint16_t *pLogoImage = (uint16_t *)ps_malloc(imageSize);

        if (pLogoImage == NULL) {
            MSG_ERROR1("[ERROR] Could not allocate PSRAM for image: ", filename);
            status = ReturnFail;
        }
        else {
            pLogoImage[0] = png.getWidth();
            pLogoImage[1] = png.getHeight();
            pPNGImagePtr = pLogoImage + 2;
        }

        if (png.getWidth() > MAX_IMAGE_WIDTH) {
             MSG_ERROR1("[ERROR] Image too wide for allocated line buffer size for ", filename);
        }
        else {
            rc = png.decode(NULL, 0);
            if (rc == PNG_SUCCESS) {
                status = ReturnSuccess;
                *pImage = pLogoImage;
            }
            else {
                MSG_ERROR3("[ERROR] PNG decode fail, filename: ",filename, ", status = ", rc);
                status = ReturnFail;
            }

            png.close();
        }
    }
    else {
        MSG_ERROR1("[ERROR] Error opening PNG file: ", filename);
        status = ReturnFail;
    }

    return status;
}

/**
* @brief This function reads a number of bytes from the given
         file at the given position.
*
* @param *p_file File
* @param position int
* @param nBytes byte
*
* @return int32_t
*
* @note none
*/
int32_t readNbytesInt(File *p_file, int position, byte nBytes)
{
    if (nBytes > 4)
        return 0;

    p_file->seek(position);

    int32_t weight = 1;
    int32_t result = 0;
    for (; nBytes; nBytes--) {
        result += weight * p_file->read();
        weight <<= 8;
    }
    return result;
}

/**
* @brief This function reads the RGB565 colour of the first pixel for a
         given BMP format logo.
*
* @param *filename const char
*
* @return uint16_t
*
* @note Uses readNbytesInt
*/
uint16_t getBMPColor(const char *filename)
{
    uint16_t color = 0x0000;

    // Open File
    File bmpImage;
    bmpImage = FILESYSTEM.open(filename, FILE_READ);

    int32_t dataStartingOffset = readNbytesInt(&bmpImage, 0x0A, 4);
    int16_t pixelsize = readNbytesInt(&bmpImage, 0x1C, 2);

    if (pixelsize != 24) {
        MSG_ERROR2("[ERROR] getBMPColor: ", filename, " Image is not 24 bpp");
        MSG_ERROR2("        logo is *", filename, "*");
        color = 0x0000;
    }
    else {
        // Read first pixel
        bmpImage.seek(dataStartingOffset);  // skip bitmap header

        byte R, G, B;

        B = bmpImage.read();
        G = bmpImage.read();
        R = bmpImage.read();

        bmpImage.close();

        color = tft.color565(R, G, B);
    }

    return color;
}

/**
* @brief This function SHOULD read the RGB565 colour of the first pixel for a
*         given PNG format logo. However, it is not currently implemented, and returns only 0x0000.
*         But, as long as the PSRAM is loaded correctly, the colour can be read from the image that has
*         already been loaded into PSRAM.
*
* @param *filename const char
*
* @return uint16_t
*
* @note Uses readNbytesInt
*/
uint16_t getPNGColor(const char *filename)
{
    uint16_t color = 0x0000;

    MSG_WARN2("[WARN] getPNGColor: Not implemented for PNG format: ", filename, ". Using black instead.");

    return color;
}

/**
* @brief This function returns the RGB565 colour of the first pixel in a logo for a
         given button.
*
* @param logonumber int
*
* @return uint16_t
*
* @note Uses getBMPColor to read the actual image data.
*/
uint16_t getImageBG(uint8_t page, uint8_t row, uint8_t col)
{
    uint16_t bg_color = 0x0000;
    char *filename = pMenu[page]->button[row][col].logo;

    if ((page >= 0) && (page < NUM_PAGES)) {
        if ((row >= 0) && (row < BUTTON_ROWS) && (col >= 0) && (col < BUTTON_COLS)) {
            uint16_t *pImage = pMenu[page]->button[row][col].pImage;
            if (pImage != nullptr) {
                bg_color = pImage[2];
            }
            else {
                if (getImageType(filename) == ImageType::ImageType_BMP) {
                    bg_color = getBMPColor(pMenu[page]->button[row][col].logo);
                }
                else if (getImageType(filename) == ImageType::ImageType_PNG) {
                    bg_color = getPNGColor(pMenu[page]->button[row][col].logo);
                }
                else {
                    MSG_ERROR1("[ERROR] getImageBG: Unknown image type: ", filename);
                    bg_color = 0x0000;
                }
            }
        }
        else {
            MSG_ERROR3("[ERROR] getImageBG: Invalid logo index ", page, row, col);
            bg_color = 0x0000;
        }
    }
    else {
        MSG_ERROR1("[ERROR] getImageBG: Invalid pageNum", page);
        bg_color = 0x0000;
    }

    return bg_color;
}

/**
 * @brief This function returns the RGB565 colour of the first pixel of the image which
 *          is being latched to for a given the logo number.
 *
 * @param logonumber int
 *
 * @return uint16_t
 *
 * @note Uses getBMPColor to read the actual image data.
 */
uint16_t getLatchImageBG(uint8_t page, uint8_t row, uint8_t col)
{
    uint16_t bg_color;
    char *filename = pMenu[page]->button[row][col].latchlogo;

    if ((page >= 0) && (page < NUM_PAGES)) {
        if ((row >= 0) && (row < BUTTON_ROWS) && (col >= 0) && (col < BUTTON_COLS)) {
            uint16_t *pLatchImage = pMenu[page]->button[row][col].pLatchImage;
            if (pLatchImage != nullptr) {
                bg_color = pLatchImage[2];
            }
            else {
                if (getImageType(filename) == ImageType::ImageType_BMP) {
                    if (strcmp(pMenu[page - 1]->button[row][col].latchlogo, "/logos/") == 0) {
                        bg_color = getBMPColor(pMenu[page - 1]->button[row][col].logo);
                    }
                    else {
                        bg_color = getBMPColor(pMenu[page - 1]->button[row][col].latchlogo);
                    }
                }
                else if (getImageType(filename) == ImageType::ImageType_PNG) {
                    if (strcmp(pMenu[page - 1]->button[row][col].latchlogo, "/logos/") == 0) {
                        bg_color = getPNGColor(pMenu[page - 1]->button[row][col].logo);
                    }
                    else {
                        bg_color = getPNGColor(pMenu[page - 1]->button[row][col].latchlogo);
                    }
                }
                else {
                    MSG_ERROR1("[ERROR] getLatchImageBG: Unknown image type: ", filename);
                    bg_color = 0x0000;
                }
            }
        }

        else {
            MSG_ERRORLN("[ERROR] getLatchImageBG: Invalid latch logo index");
            bg_color = 0x0000;
        }
    }
    else {
        MSG_ERRORLN("[ERROR] getLatchImageBG: Invalid page");
        bg_color = 0x0000;
    }
    return bg_color;
}
