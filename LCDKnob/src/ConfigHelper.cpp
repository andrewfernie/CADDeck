#include "ConfigHelper.h"

/**
* @brief This function checks if a file exists and returns a boolean accordingly.
         It then prints a debug message to the serial as wel as the tft.
*
* @param filename (const char *)
*
* @return boolean True if succeeded. False otherwise.
*
* @note Pass the filename including a leading /
*/
bool checkfile(const char *filename, bool showMessage)
{
    if (!FILESYSTEM.exists(filename))
    {
        if (showMessage)
        {
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(1, 3);
            tft.setTextFont(2);
            tft.setTextSize(2);
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
#ifdef LANGUAGE_GERMAN
            tft.printf("%s nicht gefunden!\n\n", filename);
            tft.setTextSize(1);
            tft.printf("Sollte dies nach der Konfiguration geschehen sein, kann es sein, dass die Daten auf dem ESP \nbesch�digt sein.");
#else
            tft.printf("%s not found!\n\n", filename);
            tft.setTextSize(1);
            tft.printf("If this has happend after confguration, the data on the ESP may \nbe corrupted.");
#endif
        }
        return false;
    }
    else
    {
        return true;
    }
}

bool resetconfig(String file)
{
    char fileNameType[LEN_FILENAME];
    int fileNameMenuNumber = 0;
    int numConverted = 0;
    numConverted = sscanf("%4s%d", file.c_str(), fileNameType, &fileNameMenuNumber);
    if ((numConverted == 2) && strncmp("menu", file.c_str(), 4) && (fileNameMenuNumber >= 0) && (fileNameMenuNumber < NUM_PAGES))
    {
        // Reset a menu config

        // Delete the corrupted json file
        String filetoremove = "/config/" + file;
        if (!filetoremove.endsWith(".json"))
        {
            filetoremove = filetoremove + ".json";
        }

        FILESYSTEM.remove(filetoremove);

        // Copy default.json to the new config file
        File defaultfile = FILESYSTEM.open("/config/default.json", "r");

        size_t n;
        uint8_t buf[64];

        if (defaultfile)
        {
            File newfile = FILESYSTEM.open(filetoremove, "w");
            if (newfile)
            {
                while ((n = defaultfile.read(buf, sizeof(buf))) > 0)
                {
                    newfile.write(buf, n);
                }
                // Close the newly created file
                newfile.close();
            }

#ifdef LANGUAGE_GERMAN
            MSG_INFOLN("[INFO] Zur�cksetzen abgeschlossen.");
            MSG_INFOLN("[INFO] Geben Sie \"restart\" ein, um die Konfiguration neu zu laden.");
#else
            MSG_INFOLN("[INFO] Done resetting.");
            MSG_INFOLN("[INFO] Type \"restart\" to reload configuration.");
#endif

            // Close the default.json file
            defaultfile.close();
            return true;
        }
    }
    else if (file == "general")
    {
        // Reset the general config
        // For this we do not need to open a default file because we can easily write it ourselfs

        String filetoremove = "/config/" + file;
        if (!filetoremove.endsWith(".json"))
        {
            filetoremove = filetoremove + ".json";
        }

        FILESYSTEM.remove(filetoremove);

        File newfile = FILESYSTEM.open(filetoremove, "w");
        newfile.println("{");
        newfile.println("\"menubuttoncolor\": \"#009bf4\",");
        newfile.println("\"functionbuttoncolor\": \"#00efcb\",");
        newfile.println("\"latchcolor\": \"#fe0149\",");
        newfile.println("\"background\": \"#000000\",");
        newfile.println("\"sleepenable\": true,");
        newfile.println("\"sleeptimer\": 10,");
        newfile.println("\"modifier1\": 130,");
        newfile.println("\"modifier2\": 129,");
        newfile.println("\"modifier3\": 0,");
        newfile.println("\"startup_menu\": 0,");

        newfile.println("}");

        newfile.close();
        MSG_BASICLN("[INFO] Done resetting general config.");
        MSG_BASICLN("[INFO] Type \"restart\" to reload configuration.");
        return true;
    }
    else
    {
        MSG_WARNLN("[WARNING]: Invalid reset option. Choose: general, menu1, menu2, menu3...");
        return false;
    }

    return false;
}

bool CopyFile(String FileOriginal, String FileCopy)
{
    bool success = true;

    uint8_t ibuffer[64]; // declare a buffer

#ifdef LANGUAGE_GERMAN
    MSG_INFOLN("[INFO] CopyFile()");
#else
    MSG_INFOLN("[INFO] In CopyFile()");
#endif

    if (LittleFS.exists(FileCopy) == true) // remove file copy
    {
        LittleFS.remove(FileCopy);
    }

    File f1 = LittleFS.open(FileOriginal, "r"); // open source file to read
    if (f1)
    {
        File f2 = LittleFS.open(FileCopy, "w"); // open destination file to write
        if (f2)
        {
            while (f1.available() > 0)
            {
                size_t i = f1.read(ibuffer, 64); // i = number of bytes placed in buffer from file f1
                f2.write(ibuffer, i);            // write i bytes from buffer to file f2
            }
            f2.close(); // done, close the destination file
        }
        else
        {
            MSG_ERROR1("[ERROR] CopyFile() can't open destination file ", FileCopy.c_str()); // debug
            success = false;
        }

        f1.close(); // done, close the source file
    }
    else
    {
        MSG_ERROR1("[ERROR] CopyFile() can't open source file ", FileOriginal.c_str()); // debug
        success = false;
    }

    if (success)
    {
#ifdef LANGUAGE_GERMAN
        MSG_INFOLN("[INFO] CopyFile() mit Erfolg beendet");
#else
        MSG_INFOLN("[INFO] End CopyFile() with success");
#endif
    }
    else
    {
        MSG_ERRORLN("[ERRROR] End CopyFile() with failure.");
    }

    return success;
}