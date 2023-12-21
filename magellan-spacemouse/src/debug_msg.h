#pragma once

#define MSG_PRINT_COMMAND printf

#ifndef LOG_MSG_BASIC
// #define LOG_MSG_BASIC 0       // off
#define LOG_MSG_BASIC 1  // on
#endif

#ifndef LOG_MSG_LEVEL
// #define LOG_MSG_LEVEL 0         // off
#define LOG_MSG_LEVEL 1  // ERROR
// #define LOG_MSG_LEVEL 2         // ERROR, WARN
// #define LOG_MSG_LEVEL 3         // ERROR, WARN, INFO
#endif

#ifndef LOG_MSG_DEBUG
#define LOG_MSG_DEBUG 0  // off
// #define LOG_MSG_DEBUG 1         // on
#endif

#define MSG_BASICLN(x)       \
    if (LOG_MSG_BASIC) {     \
        MSG_PRINT_COMMAND(x);MSG_PRINT_COMMAND("\n"); \
    }
#define MSG_BASIC(x)       \
    if (LOG_MSG_BASIC) {   \
        MSG_PRINT_COMMAND(x); \
    }
#define MSG_BASIC1(x, y)         \
    if (LOG_MSG_BASIC) {         \
        MSG_PRINT_COMMAND(x);    \
        MSG_PRINT_COMMAND(" ");  \
        MSG_PRINT_COMMAND(y);    \
        MSG_PRINT_COMMAND("\n"); \
    }
#define MSG_BASIC1F(x, y) \
    if (LOG_MSG_BASIC) {  \
        MSG_PRINT_COMMAND(x);      \
        MSG_PRINT_COMMAND(y);      \
    }
#define MSG_BASIC2(x, y, z)  \
    if (LOG_MSG_BASIC) {     \
        MSG_PRINT_COMMAND(x);   \
        MSG_PRINT_COMMAND(" "); \
        MSG_PRINT_COMMAND(y);   \
        MSG_PRINT_COMMAND(" "); \
        MSG_PRINT_COMMAND(z);MSG_PRINT_COMMAND("\n"); \
    }
#define MSG_BASIC3(x, y, z, w) \
    if (LOG_MSG_BASIC) {       \
        MSG_PRINT_COMMAND(x);     \
        MSG_PRINT_COMMAND(" ");   \
        MSG_PRINT_COMMAND(y);     \
        MSG_PRINT_COMMAND(" ");   \
        MSG_PRINT_COMMAND(z);     \
        MSG_PRINT_COMMAND(" ");   \
        MSG_PRINT_COMMAND(w);MSG_PRINT_COMMAND("\n");   \
    }

#define MSG_ERRORLN(x)       \
    if (LOG_MSG_LEVEL > 0) { \
        MSG_PRINT_COMMAND(x);MSG_PRINT_COMMAND("\n"); \
    }
#define MSG_ERROR(x)         \
    if (LOG_MSG_LEVEL > 0) { \
        MSG_PRINT_COMMAND(x);   \
    }
#define MSG_ERROR1(x, y)     \
    if (LOG_MSG_LEVEL > 0) { \
        MSG_PRINT_COMMAND(x);   \
        MSG_PRINT_COMMAND(" "); \
        MSG_PRINT_COMMAND(y);MSG_PRINT_COMMAND("\n"); \
    }
#define MSG_ERROR1F(x, y)    \
    if (LOG_MSG_LEVEL > 0) { \
        MSG_PRINT_COMMAND(x);         \
        MSG_PRINT_COMMAND(y);         \
    }
#define MSG_ERROR2(x, y, z)  \
    if (LOG_MSG_LEVEL > 0) { \
        MSG_PRINT_COMMAND(x);   \
        MSG_PRINT_COMMAND(" "); \
        MSG_PRINT_COMMAND(y);   \
        MSG_PRINT_COMMAND(" "); \
        MSG_PRINT_COMMAND(z);MSG_PRINT_COMMAND("\n"); \
    }
#define MSG_ERROR3(x, y, z, w) \
    if (LOG_MSG_LEVEL > 0) {   \
        MSG_PRINT_COMMAND(x);     \
        MSG_PRINT_COMMAND(" ");   \
        MSG_PRINT_COMMAND(y);     \
        MSG_PRINT_COMMAND(" ");   \
        MSG_PRINT_COMMAND(z);     \
        MSG_PRINT_COMMAND(" ");   \
        MSG_PRINT_COMMAND(w);MSG_PRINT_COMMAND("\n");   \
    }

#define MSG_WARNLN(x)        \
    if (LOG_MSG_LEVEL > 1) { \
        MSG_PRINT_COMMAND(x);MSG_PRINT_COMMAND("\n"); \
    }
#define MSG_WARN(x)          \
    if (LOG_MSG_LEVEL > 1) { \
        MSG_PRINT_COMMAND(x);   \
    }
#define MSG_WARN1(x, y)      \
    if (LOG_MSG_LEVEL > 1) { \
        MSG_PRINT_COMMAND(x);   \
        MSG_PRINT_COMMAND(" "); \
        MSG_PRINT_COMMAND(y);MSG_PRINT_COMMAND("\n"); \
    }
#define MSG_WARN1F(x, y)     \
    if (LOG_MSG_LEVEL > 1) { \
        MSG_PRINT_COMMAND(x);         \
        MSG_PRINT_COMMAND(y);         \
    }
#define MSG_WARN2(x, y, z)   \
    if (LOG_MSG_LEVEL > 1) { \
        MSG_PRINT_COMMAND(x);   \
        MSG_PRINT_COMMAND(" "); \
        MSG_PRINT_COMMAND(y);   \
        MSG_PRINT_COMMAND(" "); \
        MSG_PRINT_COMMAND(z);MSG_PRINT_COMMAND("\n"); \
    }
#define MSG_WARN3(x, y, z, w) \
    if (LOG_MSG_LEVEL > 1) {  \
        MSG_PRINT_COMMAND(x);    \
        MSG_PRINT_COMMAND(" ");  \
        MSG_PRINT_COMMAND(y);    \
        MSG_PRINT_COMMAND(" ");  \
        MSG_PRINT_COMMAND(z);    \
        MSG_PRINT_COMMAND(" ");  \
        MSG_PRINT_COMMAND(w);MSG_PRINT_COMMAND("\n");  \
    }

#define MSG_INFOLN(x)        \
    if (LOG_MSG_LEVEL > 2) { \
        MSG_PRINT_COMMAND(x);MSG_PRINT_COMMAND("\n"); \
    }
#define MSG_INFO(x)          \
    if (LOG_MSG_LEVEL > 2) { \
        MSG_PRINT_COMMAND(x);   \
    }
#define MSG_INFO1(x, y)      \
    if (LOG_MSG_LEVEL > 2) { \
        MSG_PRINT_COMMAND(x);   \
        MSG_PRINT_COMMAND(" "); \
        MSG_PRINT_COMMAND(y);MSG_PRINT_COMMAND("\n"); \
    }
#define MSG_INFO1F(x, y)     \
    if (LOG_MSG_LEVEL > 2) { \
        MSG_PRINT_COMMAND(x);         \
        MSG_PRINT_COMMAND(y);         \
    }
#define MSG_INFO2(x, y, z)   \
    if (LOG_MSG_LEVEL > 2) { \
        MSG_PRINT_COMMAND(x);   \
        MSG_PRINT_COMMAND(" "); \
        MSG_PRINT_COMMAND(y);   \
        MSG_PRINT_COMMAND(" "); \
        MSG_PRINT_COMMAND(z);MSG_PRINT_COMMAND("\n"); \
    }
#define MSG_INFO3(x, y, z, w) \
    if (LOG_MSG_LEVEL > 2) {  \
        MSG_PRINT_COMMAND(x);    \
        MSG_PRINT_COMMAND(" ");  \
        MSG_PRINT_COMMAND(y);    \
        MSG_PRINT_COMMAND(" ");  \
        MSG_PRINT_COMMAND(z);    \
        MSG_PRINT_COMMAND(" ");  \
        MSG_PRINT_COMMAND(w);MSG_PRINT_COMMAND("\n");  \
    }

#define MSG_DEBUGLN(x)       \
    if (LOG_MSG_DEBUG > 0) { \
        MSG_PRINT_COMMAND(x);MSG_PRINT_COMMAND("\n"); \
    }
#define MSG_DEBUG(x)         \
    if (LOG_MSG_DEBUG > 0) { \
        MSG_PRINT_COMMAND(x);   \
    }
#define MSG_DEBUG1(x, y)     \
    if (LOG_MSG_DEBUG > 0) { \
        MSG_PRINT_COMMAND(x);   \
        MSG_PRINT_COMMAND(" "); \
        MSG_PRINT_COMMAND(y);MSG_PRINT_COMMAND("\n"); \
    }
#define MSG_DEBUG1F(x, y)    \
    if (LOG_MSG_DEBUG > 0) { \
        MSG_PRINT_COMMAND(x);         \
        MSG_PRINT_COMMAND(y);         \
    }
#define MSG_DEBUG2(x, y, z)  \
    if (LOG_MSG_DEBUG > 0) { \
        MSG_PRINT_COMMAND(x);   \
        MSG_PRINT_COMMAND(" "); \
        MSG_PRINT_COMMAND(y);   \
        MSG_PRINT_COMMAND(" "); \
        MSG_PRINT_COMMAND(z);MSG_PRINT_COMMAND("\n"); \
    }
#define MSG_DEBUG3(x, y, z, w) \
    if (LOG_MSG_DEBUG > 0) {   \
        MSG_PRINT_COMMAND(x);     \
        MSG_PRINT_COMMAND(" ");   \
        MSG_PRINT_COMMAND(y);     \
        MSG_PRINT_COMMAND(" ");   \
        MSG_PRINT_COMMAND(z);     \
        MSG_PRINT_COMMAND(" ");   \
        MSG_PRINT_COMMAND(w);MSG_PRINT_COMMAND("\n");   \
    }


