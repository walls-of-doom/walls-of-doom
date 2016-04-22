#ifdef __linux__

#include <unistd.h>
void rest_for_milliseconds(unsigned int milliseconds) {
    usleep(milliseconds * 1000);
}

#elif _WIN32

#include <windows.h>
void rest_for_milliseconds(unsigned int milliseconds) {
    Sleep(milliseconds);
}

#else

#error Platform not supported!

#endif
