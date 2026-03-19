#include "server_logger.h"

#include <filesystem>

#include <iostream>
#include "server_global.h"
#include "shared_utils.h"

#define COLOR_RESET "\033[0m"
#define COLOR_RED "\033[31m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_GREEN "\033[32m"
#define COLOR_CYAN "\033[36m"

using std::cout;
using std::endl;
using namespace shared::utils;

void create_log_file() {
  using std::to_string;

  auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

  std::tm tm;

  #ifdef _WIN32
    // Windows (MSVC)
    localtime_s(&tm, &t);
  #else
    // Linux / Unix
    localtime_r(&t, &tm);
  #endif

  std::filesystem::create_directory("logs");

  string log_file_name =
    "logs/log_" +
    to_string(tm.tm_year + 1900) + "-" +
    to_string(tm.tm_mon + 1) + "-" +
    to_string(tm.tm_mday) + "_" +
    to_string(tm.tm_hour) + "-" +
    to_string(tm.tm_min) + "-" +
    to_string(tm.tm_sec) + ".log";

  global::log_file.open(log_file_name);
  global::log_file << "[" << get_current_time() << " INFO]: " << "SERVER LOG FILE" << endl;

}

void log_info(const string& text) {
  global::log_file << "[" << get_current_time() << " INFO]: " << text << endl;
  cout << "[" << get_current_time() << COLOR_GREEN " INFO" << COLOR_RESET << "]: " << text << COLOR_RESET << endl;
}

void log_warn(const string& text) {
  global::log_file << "[" << get_current_time() << " WARN]: " << text << endl;
  cout << "[" << get_current_time() << COLOR_YELLOW " WARN" << COLOR_RESET << "]: " << COLOR_YELLOW << text << COLOR_RESET << endl;
}

void log_error(const string& text) {
  global::log_file << "[" << get_current_time() << " ERROR]: " << text << endl;
  cout << "[" << get_current_time() << COLOR_RED " ERROR" << COLOR_RESET << "]: " << COLOR_RED << text << COLOR_RESET << endl;
}

void log_debug(const string& text) {
  if (global::config::debug) {
    global::log_file << "[" << get_current_time() << " DEBUG]: " << text << endl;
    cout << "[" << get_current_time() << COLOR_CYAN " DEBUG" << COLOR_RESET << "]: " << COLOR_CYAN << text << COLOR_RESET << endl;
  }
}

