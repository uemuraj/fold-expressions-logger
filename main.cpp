#include <clocale>
#include "logger.h"

using logger::hex;

int main()
{
	std::setlocale(LC_ALL, "");

	logger::Initialize(1);

	// TODO cassert で簡単なテストを追加する

	INFO("INFO", hex(0x01234567));
	WARN("WARN", hex(0x01234567));
	FAIL("FAIL", hex(0x01234567));

	return 0;
}
