#pragma once
#define NOGDI
#define NOUSER
#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <windows.h>

using std::string;

void create_log_file();
void console_init();

void log_info(const string& text);
void log_warn(const string& text);
void log_error(const string& text);
void log_debug(const string& text);