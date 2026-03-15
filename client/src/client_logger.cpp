#include "client_logger.h"
#include "shared_utils.h"
#include "client_global.h"
#include <iostream>
#include <windows.h>
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

#define COLOR_RESET "\033[0m"
#define COLOR_RED "\033[31m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_GREEN "\033[32m"
#define COLOR_CYAN "\033[36m"

using std::cout;
using std::endl;

void create_log_file() {
  using std::to_string;
  auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  std::tm tm; localtime_s(&tm, &t);

  global::log_file =
    "logs/log_" +
    to_string(tm.tm_year+1900) + "-" +
    to_string(tm.tm_mon+1) + "-" +
    to_string(tm.tm_mday) + "_" +
    to_string(tm.tm_hour) + "-" +
    to_string(tm.tm_min) + "-" +
    to_string(tm.tm_sec) + ".log";
}

void console_init() {
  AllocConsole();
  FILE* f;
  freopen_s(&f, "CONOUT$", "w", stdout);

  std::cout << "CLIENT DEBUG CONSOLE";
}


void log_info(const string& text) {
  std::ofstream(global::log_file) << "[" << shared::utils::get_current_time() << " INFO]: " << text << endl;
  cout << "[" << shared::utils::get_current_time() << COLOR_GREEN " INFO" << COLOR_RESET << "]: " << text << COLOR_RESET << endl;
}

void log_warn(const string& text) {
  std::ofstream(global::log_file) << "[" << shared::utils::get_current_time() << " WARN]: " << text << endl;
  cout << "[" << shared::utils::get_current_time() << COLOR_YELLOW " WARN" << COLOR_RESET << "]: " << COLOR_YELLOW << text << COLOR_RESET << endl;
}

void log_error(const string& text) {
  std::ofstream(global::log_file) << "[" << shared::utils::get_current_time() << " ERROR]: " << text << endl;
  cout << "[" << shared::utils::get_current_time() << COLOR_RED " ERROR" << COLOR_RESET << "]: " << COLOR_RED << text << COLOR_RESET << endl;
}

void log_debug(const string& text) {
  if (global::config::debug) {
    std::ofstream(global::log_file) << "[" << shared::utils::get_current_time() << " DEBUG]: " << text << endl;
    cout << "[" << shared::utils::get_current_time() << COLOR_CYAN " DEBUG" << COLOR_RESET << "]: " << COLOR_CYAN << text << COLOR_RESET << endl;
  }
}

