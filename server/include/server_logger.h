#pragma once

#include <iostream>


using std::string;

void create_log_file();
void console_init();

void log_info(const string& text);
void log_warn(const string& text);
void log_error(const string& text);
void log_debug(const string& text);