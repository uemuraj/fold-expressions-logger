#include <clocale>
#include "logger.h"

#if defined(_WIN32)

#define _WIN32_WINNT _WIN32_WINNT_WIN7
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void Initialize()
{
	if (::IsDebuggerPresent())
	{
#if defined(_UNICODE)
		logger::g_level3<wchar_t> = [](std::basic_stringstream<wchar_t> & msg) { msg << std::endl; ::OutputDebugStringW(msg.str().c_str()); };
		logger::g_level2<wchar_t> = [](std::basic_stringstream<wchar_t> & msg) { msg << std::endl; ::OutputDebugStringW(msg.str().c_str()); };
		logger::g_level1<wchar_t> = [](std::basic_stringstream<wchar_t> & msg) { msg << std::endl; ::OutputDebugStringW(msg.str().c_str()); };
#else
		logger::g_level3<char> = [](std::basic_stringstream<char> & msg) { msg << std::endl; ::OutputDebugStringA(msg.str().c_str()); };
		logger::g_level2<char> = [](std::basic_stringstream<char> & msg) { msg << std::endl; ::OutputDebugStringA(msg.str().c_str()); };
		logger::g_level1<char> = [](std::basic_stringstream<char> & msg) { msg << std::endl; ::OutputDebugStringA(msg.str().c_str()); };
#endif
	}
}

#else

#include <iostream>

void Initialize()
{
	logger::g_level3<char> = [](std::basic_stringstream<char> & msg) { std::cout << msg.str() << std::endl; };
	logger::g_level2<char> = [](std::basic_stringstream<char> & msg) { std::cout << msg.str() << std::endl; };
	logger::g_level1<char> = [](std::basic_stringstream<char> & msg) { std::cout << msg.str() << std::endl; };
}

#endif

using logger::hex;
using logger::what;

int main()
{
	std::setlocale(LC_ALL, "");

	Initialize();

	// TODO cassert で簡単なテストを追加する
	INFO("INFO", hex(0x01234567));
	WARN("WARN", hex(0x01234567));
	FAIL("FAIL", hex(0x01234567));

	INFO("INFO", what(0));
	WARN("WARN", what(1));
#if  defined(WIN32)
	FAIL("FAIL", what(E_FAIL));
#endif

	return 0;
}
