//
// Created by Matthias Hofstätter on 16.02.23.
//

#include <atomic>
#include <thread>
#include <iostream>
#include "gui.h"
#include "glog/logging.h"
#include "../log/log.h"
#include <fstream>

namespace mpp::gui {
    bool ENABLED = false;
    int TERMINAL_WIDTH = 80;
    int TERMINAL_HEIGHT = 24;
    std::chrono::duration<int, std::milli> TERMINAL_REFRESH_RATE(500); // in ms

    std::atomic_bool running = false;
    std::atomic_bool second_frame = false;
    std::thread tScreen;

    void init() {
        LOG(INFO) << "Init gui...";

        initscr();
    }

    void start() {
        LOG(INFO) << "Starting gui...";

        running.store(true, std::memory_order_release);
        tScreen = std::thread(&mpp::gui::run);
    }

    void run() {
        LOG(INFO) << "Gui running...";

        while(running.load(std::memory_order_acquire)) {
            clear();
            mpp::gui::display();
            refresh();

            bool frame = second_frame.load(std::memory_order_acquire);
            second_frame.store(!frame, std::memory_order_release);

            std::this_thread::sleep_for(mpp::gui::TERMINAL_REFRESH_RATE);
        }
    }

    void stop() {
        LOG(INFO) << "Stopping gui...";

        running.store(false, std::memory_order_release);
        tScreen.join();

        endwin();

        LOG(INFO) << "Gui stopped.";
    }

    void display() {
        for (int i = 0; i < 3; i++) {
            display_connection();
        }
        display_log(mpp::gui::TERMINAL_HEIGHT - 9);
        std::cout << std::flush;
    }

    void display_connection() {
        std::string conname = "dsl";
        int datarate_in = rand();
        int datarate_out = rand();
        int packets_in = rand();
        int packets_out = rand();

        std::string metrics = "in: " + std::to_string(datarate_in) + " out: " + std::to_string(datarate_out) + " pkt_in: " + std::to_string(packets_in) + " pkt_out: " + std::to_string(packets_out);
        printw((std::string((mpp::gui::TERMINAL_WIDTH - conname.length()) / 2, ' ') + conname + '\n').c_str());
        if(second_frame.load(std::memory_order_acquire)) {
            printw("LOCAL     <- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - >    REMOTE\n");
        } else {
            printw("LOCAL     < - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ->    REMOTE\n");
        }
        printw((std::string((mpp::gui::TERMINAL_WIDTH - metrics.length()) / 2, ' ') + metrics + '\n').c_str());
    }

    void display_log(int lines_max) {
        std::ifstream logfile;
        logfile.open(log::FILE);

        std::string log;

        std::string line;
        if (logfile.is_open()) {
            while (std::getline(logfile, line)) {
                log.append(line + '\n');
            }
            logfile.close();
        } else {
            LOG(ERROR) << "Couldn't open logfile " + log::FILE;
        }

        printw(log.c_str());
    }
}