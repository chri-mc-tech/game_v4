#include "server_logger.h"
#include "shared_utils.h"
#include "server_global.h"
#include <iostream>

#define COLOR_RESET "\033[0m"
#define COLOR_RED "\033[31m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_GREEN "\033[32m"
#define COLOR_CYAN "\033[36m"

using std::cout;
using std::endl;


void log_info(const string& text) {
    cout << "[" << shared::utils::get_current_time() << COLOR_GREEN " INFO" << COLOR_RESET << "]: " << text << COLOR_RESET << endl;
}

void log_warn(const string& text) {
    cout << "[" << shared::utils::get_current_time() << COLOR_YELLOW " WARN" << COLOR_RESET << "]: " << COLOR_YELLOW << text << COLOR_RESET << endl;
}

void log_error(const string& text) {
    cout << "[" << shared::utils::get_current_time() << COLOR_RED " ERROR" << COLOR_RESET << "]: " << COLOR_RED << text << COLOR_RESET << endl;
}

void log_debug(const string& text) {
    if (global::debug) {
        cout << "[" << shared::utils::get_current_time() << COLOR_CYAN " DEBUG" << COLOR_RESET << "]: " << COLOR_CYAN << text << COLOR_RESET << endl;
    }
}

