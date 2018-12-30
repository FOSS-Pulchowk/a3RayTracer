#pragma once

namespace x {

// NOTE(Zero): These are macrofied in 'Core.h'
enum LogType
{
	LogTypeStatus, LogTypeWarn, LogTypeError
};

/*
	* NOTE(Zero): Current list of supported data types
	* char, string(s8 or char*)
	* i32(32 bit integers or smaller)
	* u32(32 bit unsigned integers or smaller)
	* f32(float) while f64(double) are converted into floats when displayed
	* v2, v3, v4

	* USAGE(Zero) @ 12/30/2018 20:44
	* Don't use this function directly unless you have a good reason to
		and you know what you are doing
	* The format specifiers work the following way:
	xLog("Simple status message"); // display as is, break line is placed automatically at the end
	xLogError("Error message, code = {i}", 55); // displays `Error message, code = 55`
	// for hexadecimal display 'x' in place of 'i'
	// similarly 'o' for octal and 'b' for binary
	// 'u' is used for unsigned integer
	// 'f' for floats and doubles
	// 'c' for character and 's' for string
	// 'v2', 'v3' and 'v4' can be used for resp vectors, example:
	xLogWarn("This vec {v2} may cause errors!", { 5, 6, 7 });
*/

void Log(LogType type, const char*, ...);

}