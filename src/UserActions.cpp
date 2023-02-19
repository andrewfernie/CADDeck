#include "UserActions.h"
/*
 * User actions you can modify these functions to do custom stuff.
 * Have a look at the 3 examples for some inspiration. Also, have a look
 * at Actions.h to see what actions you can use (eg. print, write, press, etc.)
 *
 */

// After any action you might need a delay, this delay (in ms) is defined here:
#define USER_ACTION_DELAY 50

// Function used to print large pieces of text.
void printLargeString(const char string[]);

void userAction1()
{
    // (All OS) This functions prints a large string of text to the active window.
    printLargeString("This is an example of printing long pieces of text.");
    delay(USER_ACTION_DELAY);
    Keyboard.write(KEY_RETURN);
    printLargeString("After KEY_RETURN it will print on a new line.");
}

void userAction2()
{
    // (Windows Only) This functions opens the calculator and does some simple mathematics.

    Keyboard.write(KEY_MEDIA_CALCULATOR);
    delay(1000);  // after opening an app, a longer delay is required for the app to completely launch.
    Keyboard.print("22/7");
    delay(USER_ACTION_DELAY);
    Keyboard.write(KEY_RETURN);
    delay(USER_ACTION_DELAY);
}

void userAction3()
{
    // (Windows Only) This function rickroll's you.

    Keyboard.press(KEY_LEFT_GUI);
    delay(USER_ACTION_DELAY);
    Keyboard.print("r");
    Keyboard.releaseAll();
    delay(500);
    printLargeString("https://youtu.be/dQw4w9WgXcQ");
    Keyboard.write(KEY_RETURN);
}

void userAction4()
{
    // (Mac Only) This function rickroll's you.

    Keyboard.press(KEY_LEFT_GUI);
    delay(USER_ACTION_DELAY);
    Keyboard.print(" ");
    Keyboard.releaseAll();
    delay(USER_ACTION_DELAY);
    printLargeString("https://youtu.be/dQw4w9WgXcQ");
    Keyboard.write(KEY_RETURN);
}

void userAction5()
{
    // (Mac only) This opens a new file in Sublime (has to be installed off course and pastes the last thing you copied to the clipboard.
    // I use this to select pieces of text and copy them to a new file.
    Keyboard.press(KEY_LEFT_GUI);
    delay(USER_ACTION_DELAY);
    Keyboard.print(" ");
    Keyboard.releaseAll();
    printLargeString("Sublime");
    Keyboard.write(KEY_RETURN);
    delay(500);
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.print("n");
    Keyboard.releaseAll();
    delay(USER_ACTION_DELAY);
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.print("v");
    Keyboard.releaseAll();
}

void userAction6()
{
    // (Windows only) This opens a new file in Sublime (has to be installed off course and pastes the last thing you copied to the clipboard.
    // I use this to select pieces of text and copy them to a new file.
    Keyboard.press(KEY_LEFT_GUI);
    delay(USER_ACTION_DELAY);
    Keyboard.print("r");
    Keyboard.releaseAll();
    delay(500);
    printLargeString("notepad");
    Keyboard.write(KEY_RETURN);
    delay(500);
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.print("v");
    Keyboard.releaseAll();
}

void userAction7()
{
    // Nothing yet, create your own!
}

/* A simple function to print large strings using Keyboard. The function has a delay
 * between keypresses to not miss any, or hang on 1.
 */

void printLargeString(const char string[])
{
    for (int i = 0; i < strlen(string); i++) {
        char c = string[i];
        Keyboard.print(c);
        delay(10);  // 10ms is on most systems enough to not miss a character
    }
}
