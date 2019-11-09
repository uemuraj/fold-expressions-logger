#include <clocale>
#include "logger.h"

using logger::hex;
using logger::what;

int main()
{
	std::setlocale(LC_ALL, "");

	logger::Initialize(3);

	// TODO cassert で簡単なテストを追加する

	INFO("INFO", hex(0x01234567));
	WARN("WARN", hex(0x01234567));
	FAIL("FAIL", hex(0x01234567));

	INFO("INFO", what(0));
	WARN("WARN", what(1));
#if  defined(WIN32)
	FAIL("FAIL", what(E_FAIL));
#else
	FAIL("FAIL", what(-1));
#endif

	return 0;
}
