#pragma once
#include <iostream>

class Logger {
   public:
    Logger() = default;

    static Logger &getInstance() {
        static Logger instance;
        return instance;
    }

    std::ostream &log();
    std::ostream &err();
};

inline Logger &logger() { return Logger::getInstance(); }
inline std::ostream &log() { return logger().log(); }
inline std::ostream &err() { return logger().err(); }