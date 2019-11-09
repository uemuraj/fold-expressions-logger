#pragma once

#if !(defined(__cpp_fold_expressions) && defined(__cpp_inline_variables) && defined(__cpp_variable_templates))
#error
#endif

#include <cstring>
#include <cwchar>
#include <sstream>
#include <system_error>

#define LOGGER_MACRO_NUMBER2TEXT(n)	#n
#define LOGGER_MACRO_HEADER(file, line)	file "(" LOGGER_MACRO_NUMBER2TEXT(line) "):"

#if defined(_UNICODE)
#define FAIL(...)	if (logger::g_level1<wchar_t>) { logger::output(logger::g_level1<wchar_t>, LOGGER_MACRO_HEADER(__FILE__, __LINE__), __func__, __VA_ARGS__); }
#define WARN(...)	if (logger::g_level2<wchar_t>) { logger::output(logger::g_level2<wchar_t>, LOGGER_MACRO_HEADER(__FILE__, __LINE__), __func__, __VA_ARGS__); }
#define INFO(...)	if (logger::g_level3<wchar_t>) { logger::output(logger::g_level3<wchar_t>, LOGGER_MACRO_HEADER(__FILE__, __LINE__), __func__, __VA_ARGS__); }
#else
#define FAIL(...)	if (logger::g_level1<char>) { logger::output(logger::g_level1<char>, LOGGER_MACRO_HEADER(__FILE__, __LINE__), __func__, __VA_ARGS__); }
#define WARN(...)	if (logger::g_level2<char>) { logger::output(logger::g_level2<char>, LOGGER_MACRO_HEADER(__FILE__, __LINE__), __func__, __VA_ARGS__); }
#define INFO(...)	if (logger::g_level3<char>) { logger::output(logger::g_level3<char>, LOGGER_MACRO_HEADER(__FILE__, __LINE__), __func__, __VA_ARGS__); }
#endif

namespace logger
{
	void Initialize(int level);

	template <typename char_type>
	using Log = void (*)(std::basic_stringstream<char_type> & msg);

	template <typename char_type>
	inline Log<char_type> g_level1 = nullptr;

	template <typename char_type>
	inline Log<char_type> g_level2 = nullptr;

	template <typename char_type>
	inline Log<char_type> g_level3 = nullptr;

	template <typename char_type>
	struct message
	{
		std::basic_ostream<char_type> & os;

		template <typename param_type>
		message & operator<<(param_type && param)
		{
			os << ' ' << param;
			return *this;
		}
	};

	template <typename char_type, typename ... param_types>
	void output(Log<char_type> log, const char * header, param_types && ... params)
	{
		std::basic_stringstream<char_type> buf;
		(message<char_type>{ (buf << header) } << ... << std::move(params));
		log(buf);
	}

	template <typename char_type>
	void print(std::basic_ostream<char_type> & os, std::uint8_t value)
	{
		os << "0123456789ABCDEF"[value / 16];
		os << "0123456789ABCDEF"[value % 16];
	}

	template <>
	void print(std::basic_ostream<wchar_t> & os, std::uint8_t value)
	{
		os << L"0123456789ABCDEF"[value / 16];
		os << L"0123456789ABCDEF"[value % 16];
	}

	template <typename char_type>
	void print(std::basic_ostream<char_type> & os, std::uint16_t value)
	{
		print(os, static_cast<std::uint8_t>(value >> 8));
		print(os, static_cast<std::uint8_t>(value));
	}

	template <typename char_type>
	void print(std::basic_ostream<char_type> & os, std::uint32_t value)
	{
		print(os, static_cast<std::uint16_t>(value >> 16));
		print(os, static_cast<std::uint16_t>(value));
	}

	template <typename char_type>
	void print(std::basic_ostream<char_type> & os, std::uint64_t value)
	{
		print(os, static_cast<std::uint32_t>(value >> 32));
		print(os, static_cast<std::uint32_t>(value));
	}

	template <int digits>
	struct make_uint {};

	template <>
	struct make_uint<8> {
		using type = std::uint8_t;
	};

	template <>
	struct make_uint<16> {
		using type = std::uint16_t;
	};

	template <>
	struct make_uint<32> {
		using type = std::uint32_t;
	};

	template <>
	struct make_uint<64> {
		using type = std::uint64_t;
	};

	template <typename T>
	using make_uint_t = typename make_uint<std::numeric_limits<std::make_unsigned_t<T>>::digits>::type;

	template <typename T>
	struct hex
	{
		make_uint_t<T> value;

		hex(const T v) : value(v) {}
	};

	template <typename char_type, typename T>
	std::basic_ostream<char_type> & operator<<(std::basic_ostream<char_type> & os, const hex<T> & hex)
	{
		os << "0x";
		print(os, hex.value);
		return os;
	}

	std::basic_ostream<wchar_t> & operator<<(std::basic_ostream<wchar_t> & os, const char * ptr)
	{
		std::mbstate_t state{};
		wchar_t wc{};

		std::size_t ret, len = std::strlen(ptr);

		while (static_cast<int>(ret = std::mbrtowc(&wc, ptr, len, &state)) > 0)
		{
			ptr += ret, len -= ret;
			os << wc;
		}

		return os;
	}

	struct what
	{
		int value;

		what(int v) : value(v) {}
	};

	template <typename char_type>
	std::basic_ostream<char_type> & operator<<(std::basic_ostream<char_type> & os, const what & what)
	{
		if (what.value >= 0)
			os << what.value << ' ';
		else
			os << hex(what.value) << ' ';

		return os << std::system_error(what.value, std::system_category()).what();
	}
#if 0
	template <>
	std::basic_ostream<wchar_t> & operator<<(std::basic_ostream<wchar_t> & os, const what & what)
	{
		if (what.value >= 0)
			os << what.value << L' ';
		else
			os << hex(what.value) << L' ';

		std::system_error syserr(what.value, std::system_category());
		std::mbstate_t state{};
		wchar_t wc{};

		const char * ptr = syserr.what();
		std::size_t ret, len = std::strlen(ptr);

		while (static_cast<int>(ret = std::mbrtowc(&wc, ptr, len, &state)) > 0)
		{
			ptr += ret, len -= ret;
			os << wc;
		}

		return os;
	}
#endif
}

#if defined(_WIN32)

#define _WIN32_WINNT _WIN32_WINNT_WIN7
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace logger
{
	inline void Initialize(int level)
	{
		if (::IsDebuggerPresent())
		{
#if defined(_UNICODE)
			switch (level)
			{
			case 3:
				g_level3<wchar_t> = [](std::basic_stringstream<wchar_t> & msg) { msg << std::endl; ::OutputDebugStringW(msg.str().c_str()); };
			case 2:
				g_level2<wchar_t> = [](std::basic_stringstream<wchar_t> & msg) { msg << std::endl; ::OutputDebugStringW(msg.str().c_str()); };
			case 1:
				g_level1<wchar_t> = [](std::basic_stringstream<wchar_t> & msg) { msg << std::endl; ::OutputDebugStringW(msg.str().c_str()); };
			}
#else
			switch (level)
			{
			case 3:
				g_level3<char> = [](std::basic_stringstream<char> & msg) { msg << std::endl; ::OutputDebugStringA(msg.str().c_str()); };
			case 2:
				g_level2<char> = [](std::basic_stringstream<char> & msg) { msg << std::endl; ::OutputDebugStringA(msg.str().c_str()); };
			case 1:
				g_level1<char> = [](std::basic_stringstream<char> & msg) { msg << std::endl; ::OutputDebugStringA(msg.str().c_str()); };
			}
#endif
		}
	}
}

#else

#include <iostream>

namespace logger
{
	inline void Initialize(int level)
	{
		switch (level)
		{
		case 3:
			g_level3<char> = [](std::basic_stringstream<char> & msg) { std::cout << msg.str() << std::endl; };
		case 2:
			g_level2<char> = [](std::basic_stringstream<char> & msg) { std::cout << msg.str() << std::endl; };
		case 1:
			g_level1<char> = [](std::basic_stringstream<char> & msg) { std::cout << msg.str() << std::endl; };
		}
	}
}

#endif
