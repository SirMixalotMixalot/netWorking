#pragma once
#include <iostream>
#include <string_view>
#include <fstream>
#include "../console/console.h"


const ConsoleStyle warning = { ForeGroundColor::Yellow };
const ConsoleStyle log_error = { ForeGroundColor::Red };
const ConsoleStyle ok = { ForeGroundColor::Green };



struct Logger {
	Logger() : out{ std::cout }, err{std::cerr} {
		prepare_console(ENABLE_VIRTUAL_TERMINAL_PROCESSING, 0);
	}
	Logger(std::ofstream o) : stream_file_resource{ std::move(o) }, out{ stream_file_resource }, err{stream_file_resource} {}
	void warn(const char* text) {
		write_styled_text(err, text, warning);
	}
	void error(const char* text) {
		write_styled_text(err, text, log_error);

	}
	void info(const char* text) {
		warn(text);
	}
	void success(const char* text) {
		write_styled_text(out, text, ok);
	}
	~Logger() {
		if (stream_file_resource.is_open()) {
			stream_file_resource.close();
		}
	}

	std::ofstream stream_file_resource;
	std::ostream& out;
	std::ostream& err;

};
