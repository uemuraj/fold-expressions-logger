#pragma once

#if !(defined(__cpp_fold_expressions) && defined(__cpp_inline_variables) && defined(__cpp_variable_templates))
#error
#endif

#include <sstream>

#define LOGGER_MACRO_NUMBER2TEXT(n)	#n
#define LOGGER_MACRO_HEADER(file, line)	file "(" LOGGER_MACRO_NUMBER2TEXT(line) "): "

#if defined(_UNICODE)
#define FAIL(...)	if (logger::g_level1<wchar_t>) { logger::output(logger::g_level1<wchar_t>, TEXT(LOGGER_MACRO_HEADER(__FILE__, __LINE__)), __func__, __VA_ARGS__); }
#define WARN(...)	if (logger::g_level2<wchar_t>) { logger::output(logger::g_level2<wchar_t>, TEXT(LOGGER_MACRO_HEADER(__FILE__, __LINE__)), __func__, __VA_ARGS__); }
#define INFO(...)	if (logger::g_level3<wchar_t>) { logger::output(logger::g_level3<wchar_t>, TEXT(LOGGER_MACRO_HEADER(__FILE__, __LINE__)), __func__, __VA_ARGS__); }
#else
#define FAIL(...)	if (logger::g_level1<char>) { logger::output(logger::g_level1<char>, LOGGER_MACRO_HEADER(__FILE__, __LINE__), __func__, __VA_ARGS__); }
#define WARN(...)	if (logger::g_level2<char>) { logger::output(logger::g_level2<char>, LOGGER_MACRO_HEADER(__FILE__, __LINE__), __func__, __VA_ARGS__); }
#define INFO(...)	if (logger::g_level3<char>) { logger::output(logger::g_level3<char>, LOGGER_MACRO_HEADER(__FILE__, __LINE__), __func__, __VA_ARGS__); }
#endif

namespace logger
{
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
	void output(Log<char_type> log, const char_type * header, param_types && ... params)
	{
		std::basic_stringstream<char_type> buf;
		(message<char_type>{ (buf << header) } << ... << std::move(params));
		log(buf);
	}
}

namespace logger
{
	template <int digits>
	struct hexadecimal;

	template <>
	struct hexadecimal<8>
	{
		const std::uint8_t value;
	};

	template <>
	struct hexadecimal<16>
	{
		const std::uint16_t value;
	};

	template <>
	struct hexadecimal<32>
	{
		const std::uint32_t value;
	};

	template <>
	struct hexadecimal<64>
	{
		const std::uint64_t value;
	};

	template <typename T>
	hexadecimal<std::numeric_limits<std::make_unsigned_t<T>>::digits> hex(const T value)
	{
		return { (std::make_unsigned_t<T>) value };
	}

	template <typename char_type>
	std::basic_ostream<char_type> & print(std::basic_ostream<char_type> & os, std::uint8_t value)
	{
		os << "0123456789ABCDEF"[value / 16];
		os << "0123456789ABCDEF"[value % 16];
		return os;
	}

	template <>
	std::basic_ostream<wchar_t> & print(std::basic_ostream<wchar_t> & os, std::uint8_t value)
	{
		os << L"0123456789ABCDEF"[value / 16];
		os << L"0123456789ABCDEF"[value % 16];
		return os;
	}

	template <typename char_type>
	std::basic_ostream<char_type> & print(std::basic_ostream<char_type> & os, std::uint16_t value)
	{
		print(os, static_cast<std::uint8_t>(value >> 8));
		print(os, static_cast<std::uint8_t>(value));
		return os;
	}

	template <typename char_type>
	std::basic_ostream<char_type> & print(std::basic_ostream<char_type> & os, std::uint32_t value)
	{
		print(os, static_cast<std::uint16_t>(value >> 16));
		print(os, static_cast<std::uint16_t>(value));
		return os;
	}

	template <typename char_type>
	std::basic_ostream<char_type> & print(std::basic_ostream<char_type> & os, std::uint64_t value)
	{
		print(os, static_cast<std::uint32_t>(value >> 32));
		print(os, static_cast<std::uint32_t>(value));
		return os;
	}

	template <typename char_type, int digits>
	std::basic_ostream<char_type> & operator<<(std::basic_ostream<char_type> & os, const hexadecimal<digits> & hex)
	{
		return print(os << "0x", hex.value);
	}
}

#include <system_error>
#include <cstring>
#include <cwchar>

namespace logger
{
	struct system_error_code
	{
		const int value;
	};

	system_error_code what(int code)
	{
		return { code };
	};

	template <typename char_type>
	std::basic_ostream<char_type> & print(std::basic_ostream<char_type> & os, const system_error_code & code)
	{
		return os << std::system_error(code.value, std::system_category()).what();
	}

	template <>
	std::basic_ostream<wchar_t> & print(std::basic_ostream<wchar_t> & os, const system_error_code & code)
	{
		std::system_error syserr(code.value, std::system_category());
		const char * ptr = syserr.what();

		std::mbstate_t state{};
		std::size_t len = std::strlen(ptr), ret;
		wchar_t wc{};

		while (static_cast<int>(ret = std::mbrtowc(&wc, ptr, len, &state)) > 0)
		{
			ptr += ret, len -= ret;
			os << wc;
		}

		return os;
	}

	template <typename char_type>
	std::basic_ostream<char_type> & operator<<(std::basic_ostream<char_type> & os, const system_error_code & code)
	{
		if (code.value >= 0)
			return print(os << code.value << ' ', code);
		else
			return print(os << hex(code.value) << ' ', code);
	}
}
