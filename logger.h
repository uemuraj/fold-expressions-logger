#pragma once

#if !(defined(__cpp_fold_expressions) && defined(__cpp_inline_variables) && defined(__cpp_variable_templates))
#error
#endif

#include <sstream>

#define LOGGER_MACRO_NUMBER2TEXT(n)	#n
#define LOGGER_MACRO_HEADER(file, line)	file "(" LOGGER_MACRO_NUMBER2TEXT(line) "):"

#if defined(_UNICODE)
#define FAIL(...)	if (logger::g_Level1<wchar_t>) { logger::Output(logger::g_Level1<wchar_t>, LOGGER_MACRO_HEADER(__FILE__, __LINE__), __func__, __VA_ARGS__); }
#define WARN(...)	if (logger::g_Level2<wchar_t>) { logger::Output(logger::g_Level2<wchar_t>, LOGGER_MACRO_HEADER(__FILE__, __LINE__), __func__, __VA_ARGS__); }
#define INFO(...)	if (logger::g_Level3<wchar_t>) { logger::Output(logger::g_Level3<wchar_t>, LOGGER_MACRO_HEADER(__FILE__, __LINE__), __func__, __VA_ARGS__); }
#else
#define FAIL(...)	if (logger::g_Level1<char>) { logger::Output(logger::g_Level1<char>, LOGGER_MACRO_HEADER(__FILE__, __LINE__), __func__, __VA_ARGS__); }
#define WARN(...)	if (logger::g_Level2<char>) { logger::Output(logger::g_Level2<char>, LOGGER_MACRO_HEADER(__FILE__, __LINE__), __func__, __VA_ARGS__); }
#define INFO(...)	if (logger::g_Level3<char>) { logger::Output(logger::g_Level3<char>, LOGGER_MACRO_HEADER(__FILE__, __LINE__), __func__, __VA_ARGS__); }
#endif

namespace logger
{
	void Initialize(int level);

	template <typename char_type>
	using Log = void (*)(std::basic_stringstream<char_type> & msg);

	template <typename char_type>
	inline Log<char_type> g_Level1 = nullptr;

	template <typename char_type>
	inline Log<char_type> g_Level2 = nullptr;

	template <typename char_type>
	inline Log<char_type> g_Level3 = nullptr;

	template <typename char_type>
	struct message
	{
		Log<char_type> m_log;

		std::basic_stringstream<char_type> m_buf;

		message(Log<char_type> log, const char * header) : m_log(log)
		{
			m_buf << header;
		}

		~message()
		{
			m_log(m_buf);
		}

		template <typename param_type>
		message & operator<<(param_type && param)
		{
			m_buf << ' ' << param;
			return *this;
		}
	};

	template <typename char_type, typename ... param_types>
	void Output(Log<char_type> log, const char * header, param_types && ... params)
	{
		(message<char_type>{ log, header } << ... << std::move(params));
	}

	template <typename char_type>
	void print(std::basic_ostream<char_type> & os, std::uint8_t value)
	{
		os << "0123456789ABCDEF"[value / 16];
		os << "0123456789ABCDEF"[value % 16];
	}

	template <>
	void print(std::basic_ostream<wchar_t> & os, const std::uint8_t value)
	{
		os << L"0123456789ABCDEF"[value / 16];
		os << L"0123456789ABCDEF"[value % 16];
	}

	template <typename char_type>
	void print(std::basic_ostream<char_type> & os, const std::uint16_t value)
	{
		print(os, static_cast<std::uint8_t>(value >> 8));
		print(os, static_cast<std::uint8_t>(value));
	}

	template <typename char_type>
	void print(std::basic_ostream<char_type> & os, const std::uint32_t value)
	{
		print(os, static_cast<std::uint16_t>(value >> 16));
		print(os, static_cast<std::uint16_t>(value));
	}

	template <typename char_type>
	void print(std::basic_ostream<char_type> & os, const std::uint64_t value)
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
	using make_uint_t = typename make_uint<std::numeric_limits<T>::digits>::type;

	template <typename T>
	struct hex
	{
		make_uint_t<std::make_unsigned_t<T>> value;

		hex(const T v) : value(v) {}
	};

	template <typename char_type, typename T>
	std::basic_ostream<char_type> & operator<<(std::basic_ostream<char_type> & os, const hex<T> & hex)
	{
		os << "0x";
		print(os, hex.value);
		return os;
	}
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
				g_Level3<wchar_t> = [](std::basic_stringstream<wchar_t> & msg) { msg << std::endl; ::OutputDebugStringW(msg.str().c_str()); };
			case 2:
				g_Level2<wchar_t> = [](std::basic_stringstream<wchar_t> & msg) { msg << std::endl; ::OutputDebugStringW(msg.str().c_str()); };
			case 1:
				g_Level1<wchar_t> = [](std::basic_stringstream<wchar_t> & msg) { msg << std::endl; ::OutputDebugStringW(msg.str().c_str()); };
			}
#else
			switch (level)
			{
			case 3:
				g_Level3<char> = [](std::basic_stringstream<char> & msg) { msg << std::endl; ::OutputDebugStringA(msg.str().c_str()); };
			case 2:
				g_Level2<char> = [](std::basic_stringstream<char> & msg) { msg << std::endl; ::OutputDebugStringA(msg.str().c_str()); };
			case 1:
				g_Level1<char> = [](std::basic_stringstream<char> & msg) { msg << std::endl; ::OutputDebugStringA(msg.str().c_str()); };
			}
#endif
		}
	}
}

#endif
