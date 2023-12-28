#include "ScreenHelper.h" 
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
    char *hex = html + 1; // remove the #
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