#pragma once
#include "CADDeck.h"

void serial_commands();

uint8_t listDir(fs::FS &fs, const char *dirname, uint8_t levels, uint8_t raw);
uint8_t createDir(fs::FS &fs, const char *path);
uint8_t removeDir(fs::FS &fs, const char *path);
uint8_t readFile(fs::FS &fs, const char *path, uint8_t raw);
uint8_t writeFile(fs::FS &fs, const char *path, const char *message);
uint8_t appendFile(fs::FS &fs, const char *path, const char *message);
uint8_t renameFile(fs::FS &fs, const char *path1, const char *path2);
uint8_t deleteFile(fs::FS &fs, const char *path);
extern bool in_joystick_pan;
extern bool in_joystick_rotate;
extern bool in_joystick_zoom;
