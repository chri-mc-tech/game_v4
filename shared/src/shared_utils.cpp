#include "shared_utils.h"

#include <iostream>
#include <chrono>

using std::string;

namespace shared::utils {
    string get_current_time() {
        time_t t = time(nullptr);
        tm* now = localtime(&t);
        char buf[9];
        strftime(buf, sizeof(buf), "%H:%M:%S", now);
        return string(buf);
    }
}