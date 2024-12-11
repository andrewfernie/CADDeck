#include "SerialCommands.h"

void serial_commands()
{
    // Check if there is data available on the serial input that needs to be handled.

    if (Serial.available()) {
        String command = Serial.readStringUntil(' ');
        MSG_SERIAL_DEBUGLN(command);

        if (command == "cal") {
            MSG_INFOLN("[INFO] received command cal");
            FILESYSTEM.remove(CALIBRATION_FILE);
            ESP.restart();
        }
        else if (command == "setssid") {
            String value = Serial.readString();
            MSG_INFO1("[INFO] received command setssid ", value.c_str());
            if (saveWifiSSID(value)) {
                MSG_INFO1F("[INFO] Saved new SSID: %s\n", value.c_str());
                loadMainConfig();
                MSG_INFOLN("[INFO] New configuration loaded");
            }
        }
        else if (command == "setpassword") {
            String value = Serial.readString();
            MSG_INFO1("[INFO] received command setpassword ", value.c_str());
            if (saveWifiPW(value)) {
                MSG_INFO1F("[INFO] Saved new Password: %s\n", value.c_str());
                loadMainConfig();
                MSG_INFOLN("[INFO] New configuration loaded");
            }
        }
        else if (command == "setwifimode") {
            String value = Serial.readString();
            MSG_INFO1("[INFO] received command setwifimode ", value.c_str());
            if (saveWifiMode(value)) {
                MSG_INFO1F("[INFO] Saved new WiFi Mode: %s\n", value.c_str());
                loadMainConfig();
                MSG_INFOLN("[INFO] New configuration loaded");
            }
        }
        else if (command == "setwifimodesta") {
            String value = "WIFI_STA";
            MSG_INFO1("[INFO] received command setwifimode ", value.c_str());
            if (saveWifiMode(value)) {
                MSG_INFO1F("[INFO] Saved new WiFi Mode: %s\n", value.c_str());
                loadMainConfig();
                MSG_INFOLN("[INFO] New configuration loaded");
            }
        }
        else if (command == "setwifimodeap") {
            String value = "WIFI_AP";
            MSG_INFO1("[INFO] received command setwifimode ", value.c_str());
            if (saveWifiMode(value)) {
                MSG_INFO1F("[INFO] Saved new WiFi Mode: %s\n", value.c_str());
                loadMainConfig();
                MSG_INFOLN("[INFO] New configuration loaded");
            }
        }
        else if (command == "restart") {
            MSG_WARNLN("[WARNING]: Restarting");
            ESP.restart();
        }
        else if (command == "reset") {
            String file = Serial.readString();
            MSG_INFO1F("[INFO] Resetting %s.json now\n", file.c_str());
            resetconfig(file);
        }
        else if (command == "dir") {
            String foldername = Serial.readString();
            // MSG_SERIAL_DEBUG1F("[INFO] Contents of data folder %s\n", foldername.c_str());
            SERIAL_COMMAND_PORT.println("<<<<<");
            uint8_t status = listDir(FILESYSTEM, foldername.c_str(), 1, true);
            SERIAL_COMMAND_PORT.println(">>>>>");
        }
        else if(command == "getfile"){
            String filename = Serial.readString();
            SERIAL_COMMAND_PORT.println("<<<<<");
            uint8_t status = readFile(FILESYSTEM, filename.c_str(), true);
            SERIAL_COMMAND_PORT.println(">>>>>");
        }

        else {
            for (size_t i = 0; i < NUM_PAGES; i++) {
                char menuName[16];
                sprintf(menuName, "menu%d", i);

                if (command == menuName && pageNum != i && pageNum != WEB_REQUEST_PAGE) {
                    uint8_t status = pageHistoryStack.push(pageNum);
                    if (status == STACK_STATUS_FULL) {
                        MSG_INFOLN("[INFO] Page History Stack is full. Dropped oldest value..");
                    }
                    pageNum = i;

                    drawKeypad();
                    MSG_INFO("[Info] Switched to ");
                    MSG_INFOLN(menuName);
                }
            }
        }
    }
}

uint8_t listDir(fs::FS &fs, const char *dirname, uint8_t levels, uint8_t raw)
{
    uint8_t status = ReturnSuccess;

    File root = fs.open(dirname);
    if (!root) {
        MSG_ERROR1("[ERROR] Listing directory ", dirname);
        MSG_ERRORLN(" - failed to open directory.");
        status = ReturnFail;
    }
    if (!root.isDirectory()) {
        MSG_ERROR1("[ERROR] Listing directory ", dirname);
        MSG_ERRORLN(" - not a directory.");
        status = ReturnFail;
    }

    if (status == ReturnSuccess) {
        File file = root.openNextFile();
        while (file) {
            if (file.isDirectory()) {
                if (!raw) {
                    MSG_SERIAL_DEBUG("  DIR : ");
                    MSG_SERIAL_DEBUGLN(file.name());
                }

                if (levels) {
                    listDir(fs, file.path(), levels - 1, raw);
                }
            }
            else {
                if (raw) {
                    SERIAL_COMMAND_PORT.print(dirname);
                    if (strlen(dirname) > 1) {
                        SERIAL_COMMAND_PORT.print("/");
                    }
                    SERIAL_COMMAND_PORT.println(file.name());
                }
                else {
                    MSG_SERIAL_DEBUG("  FILE: ");
                    MSG_SERIAL_DEBUG(dirname);
                    MSG_SERIAL_DEBUG(file.name());
                    MSG_SERIAL_DEBUG("\tSIZE: ");
                    MSG_SERIAL_DEBUGLN(file.size());
                }
            }
            file = root.openNextFile();
        }
    }
    return status;
}

uint8_t createDir(fs::FS &fs, const char *path)
{
    uint8_t status = ReturnSuccess;
    Serial.printf("Creating Dir: %s\n", path);
    if (fs.mkdir(path)) {
        Serial.println("Dir created");
    }
    else {
        Serial.println("mkdir failed");
        status = ReturnFail;
    }

    return status;
}

uint8_t removeDir(fs::FS &fs, const char *path)
{
    uint8_t status = ReturnSuccess;
    Serial.printf("Removing Dir: %s\n", path);
    if (fs.rmdir(path)) {
        Serial.println("Dir removed");
    }
    else {
        Serial.println("rmdir failed");
        status = ReturnFail;
    }
    return status;
}

uint8_t readFile(fs::FS &fs, const char *path, uint8_t raw)
{
    uint8_t status = ReturnSuccess;
    if(!raw){
        SERIAL_COMMAND_PORT.printf("Reading file: %s\r\n", path);
    }

    File file = fs.open(path);
    if (!file || file.isDirectory()) {
        SERIAL_COMMAND_PORT.println("- failed to open file for reading");
        status = ReturnFail;
    }
    else {
        SERIAL_COMMAND_PORT.println("- read from file:");
        while (file.available()) {
            SERIAL_COMMAND_PORT.write(file.read());
        }
        SERIAL_COMMAND_PORT.println();
        file.close();
    }
    return status;
}

uint8_t writeFile(fs::FS &fs, const char *path, const char *message)
{
    uint8_t status = ReturnSuccess;
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if (!file) {
        Serial.println("- failed to open file for writing");
        status = ReturnFail;
    }
    else {
        if (file.print(message)) {
            Serial.println("- file written");
        }
        else {
            Serial.println("- write failed");
            status = ReturnFail;
        }
        file.close();
    }
    return status;
}

uint8_t appendFile(fs::FS &fs, const char *path, const char *message)
{
    uint8_t status = ReturnSuccess;
    Serial.printf("Appending to file: %s\r\n", path);

    File file = fs.open(path, FILE_APPEND);
    if (!file) {
        Serial.println("- failed to open file for appending");
        status = ReturnFail;
    }
    else {
        if (file.print(message)) {
            Serial.println("- message appended");
        }
        else {
            Serial.println("- append failed");
            status = ReturnFail;
        }
        file.close();
    }
    return status;
}

uint8_t renameFile(fs::FS &fs, const char *path1, const char *path2)
{
    uint8_t status = ReturnSuccess;
    Serial.printf("Renaming file %s to %s\r\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("- file renamed");
    }
    else {
        Serial.println("- rename failed");
        status = ReturnFail;
    }
    return status;
}

uint8_t deleteFile(fs::FS &fs, const char *path)
{
    uint8_t status = ReturnSuccess;
    Serial.printf("Deleting file: %s\r\n", path);
    if (fs.remove(path)) {
        Serial.println("- file deleted");
    }
    else {
        Serial.println("- delete failed");
        status = ReturnFail;
    }
    return status;
}