// declarations.hpp

#ifndef DECLARATIONS
#define DECLARATIONS

#include "libutils/src/file.hpp"
#include "libutils/src/funcs.hpp"
#include "libutils/src/color.hpp"
#include "libutils/src/log.hpp"
#include "libutils/src/table.hpp"

#include <string>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <sstream>

using funcs::print;

enum class UI_state
{
	MAIN_MENU,
	SETTINGS,
	VIEW_USER_INFO,
	EXIT_PROGRAM
};

struct FilePaths
{
	std::string home_dir,
		account_info,
		transaction_history,
		logs;
};

struct UserAccountInfo
{
	long double balance = 0.0L;
	std::string currency = "USD";
	std::string user_name = "User";
};

struct Globals
{
	FilePaths file_paths;
	UI_state ui_state = UI_state::MAIN_MENU;
	std::string VERSION;
	UserAccountInfo user_account_info;
	size_t columns = 10;
};

std::string formatNumber(long long num)
{
    std::string numStr = std::to_string(num);
    if (numStr.length() <= 3)
        return numStr;

    std::string formattedNum;
    formattedNum.reserve(numStr.length() + numStr.length() / 3);

    int count = 0;
    for (int i = (int)numStr.length() - 1; i >= 0; --i)
    {
        formattedNum += numStr[i];
        if (++count == 3 && i > 0)
        {
            formattedNum += ',';
            count = 0;
        }
    }

    std::reverse(formattedNum.begin(), formattedNum.end());
    return formattedNum;
}

std::string formatBalance(long double amount)
{
	const bool negative = amount < 0;
	if (negative)
	{
		amount *= -1;
	}

	std::ostringstream ss;
	ss << std::fixed << std::setprecision(2) << amount;
	std::string text = ss.str();

	const size_t decimal_at = text.find('.');
	const std::string integer_part = (decimal_at == std::string::npos) ? text : text.substr(0, decimal_at);
	const std::string fractional_part = (decimal_at == std::string::npos) ? "" : text.substr(decimal_at);

	std::string formatted_integer;
	formatted_integer.reserve(integer_part.size() + integer_part.size() / 3);
	int count = 0;
	for (int i = static_cast<int>(integer_part.size()) - 1; i >= 0; --i)
	{
		formatted_integer += integer_part[static_cast<size_t>(i)];
		if (++count == 3 && i > 0)
		{
			formatted_integer += ',';
			count = 0;
		}
	}
	std::reverse(formatted_integer.begin(), formatted_integer.end());

	std::string formatted = formatted_integer + fractional_part;
	return negative ? "-" + formatted : formatted;
}

void LOG(const Globals &globals, const std::string &msg)
{
	File::m_insertline(globals.file_paths.logs, funcs::currentTime() + " -> " + msg, 0);
}

#endif
