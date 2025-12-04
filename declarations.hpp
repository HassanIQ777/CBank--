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
	long double balance;
	std::string currency;
	std::string user_name;
};

struct Globals
{
	FilePaths file_paths;
	UI_state ui_state;
	std::string VERSION;
	UserAccountInfo user_account_info;
	size_t columns;
};

std::string formatNumber(int num)
{
	std::string numStr = std::to_string(num);
	if (numStr.length() <= 3)
	{
		return numStr;
	}

	std::string formattedNum = "";
	int count = 0;
	for (size_t i = numStr.length() - 1; i >= 0; i--)
	{
		formattedNum += numStr[i];
		count++;
		if (count == 3 && i > 0)
		{
			formattedNum += ',';
			count = 0;
		}
	}

	std::reverse(formattedNum.begin(), formattedNum.end());
	return formattedNum;
}

void LOG(const Globals &globals, const std::string &msg)
{
	static const std::string fp = globals.file_paths.logs;
	File::m_insertline(fp, funcs::currentTime() + " -> " + msg, 0);
}

#endif