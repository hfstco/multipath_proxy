//
// Created by Matthias Hofstätter on 16.02.23.
//

#ifndef MULTIPATH_PROXY_GUI_H
#define MULTIPATH_PROXY_GUI_H


#include <ncurses.h>
#include <chrono>

namespace gui {
    extern bool ENABLED;
    extern int TERMINAL_WIDTH;
    extern int TERMINAL_HEIGHT;
    extern std::chrono::duration<int, std::milli> TERMINAL_REFRESH_RATE; // in ms

    void init();

    void start();

    void run();

    void stop();

    void display();

    void display_connection();

    void display_log(int lines_max);
}


#endif //MULTIPATH_PROXY_GUI_H
