#pragma once

namespace x {

enum LogType
{
	LogTypeStatus, LogTypeWarn, LogTypeError
};

void Log(LogType type, const char*, ...);

}