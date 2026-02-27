// bank_operations.hpp

#ifndef BANK_OPERATIONS
#define BANK_OPERATIONS

#include "declarations.hpp"
#include <limits>

namespace
{
std::string trim(std::string text)
{
	const size_t first = text.find_first_not_of(" \t\r\n");
	if (first == std::string::npos)
	{
		return "";
	}
	const size_t last = text.find_last_not_of(" \t\r\n");
	return text.substr(first, (last - first) + 1);
}

bool tryParseLongDouble(const std::string &text, long double &value_out)
{
	try
	{
		const std::string cleaned = trim(text);
		if (cleaned.empty())
		{
			return false;
		}
		size_t consumed = 0;
		value_out = std::stold(cleaned, &consumed);
		return consumed == cleaned.size();
	}
	catch (...)
	{
		return false;
	}
}

bool tryParseSizeT(const std::string &text, size_t &value_out)
{
	try
	{
		const std::string cleaned = trim(text);
		size_t consumed = 0;
		const unsigned long long parsed = std::stoull(cleaned, &consumed);
		if (consumed != cleaned.size())
		{
			return false;
		}
		if (parsed > static_cast<unsigned long long>(std::numeric_limits<size_t>::max()))
		{
			return false;
		}
		value_out = static_cast<size_t>(parsed);
		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool parseTransactionRow(const std::string &line, std::vector<std::string> &fields_out)
{
	fields_out.clear();
	const size_t c1 = line.find(',');
	if (c1 == std::string::npos)
	{
		return false;
	}
	const size_t c2 = line.find(',', c1 + 1);
	if (c2 == std::string::npos)
	{
		return false;
	}
	const size_t c3 = line.find(',', c2 + 1);
	if (c3 == std::string::npos)
	{
		return false;
	}

	fields_out.push_back(line.substr(0, c1));
	fields_out.push_back(line.substr(c1 + 1, c2 - c1 - 1));
	fields_out.push_back(line.substr(c2 + 1, c3 - c2 - 1));
	fields_out.push_back(line.substr(c3 + 1));
	return true;
}

std::string sanitizeDetails(std::string details)
{
	for (char &ch : details)
	{
		if (ch == '\n' || ch == '\r')
		{
			ch = ' ';
		}
	}
	return details;
}
} // namespace

void writeUserAccountInfo(Globals &globals)
{
	/* static */ std::string fp = globals.file_paths.account_info;

	long double balance = globals.user_account_info.balance;
	std::string currency = globals.user_account_info.currency;
	std::string user_name = globals.user_account_info.user_name;
	size_t columns = globals.columns;

	File::m_writeToINI(fp, "balance", funcs::str(balance));
	File::m_writeToINI(fp, "currency", currency);
	File::m_writeToINI(fp, "user_name", user_name);
	File::m_writeToINI(fp, "columns", funcs::str(columns));
}

void readUserAccountInfo(Globals &globals)
{
	/* static */ std::string fp = globals.file_paths.account_info;

	long double balance = 0.0L;
	size_t columns = 10;
	bool config_changed = false;

	const std::string raw_balance = File::m_getFromINI(fp, "balance");
	if (!tryParseLongDouble(raw_balance, balance))
	{
		balance = 0.0L;
		config_changed = true;
	}

	std::string currency = trim(File::m_getFromINI(fp, "currency"));
	if (currency.empty())
	{
		currency = "USD";
		config_changed = true;
	}

	std::string user_name = trim(File::m_getFromINI(fp, "user_name"));
	if (user_name.empty())
	{
		user_name = "User";
		config_changed = true;
	}

	const std::string raw_columns = File::m_getFromINI(fp, "columns");
	if (!tryParseSizeT(raw_columns, columns))
	{
		columns = 10;
		config_changed = true;
	}
	columns = std::max<size_t>(1, columns);

	globals.user_account_info.balance = balance;
	globals.user_account_info.currency = currency;
	globals.user_account_info.user_name = user_name;
	globals.columns = columns;

	if (config_changed)
	{
		writeUserAccountInfo(globals);
		LOG(globals, "Detected invalid account_info.ini values; restored defaults.");
	}
}

void bankDeposit(Globals &globals)
{
	std::string amount_str;
	print("\n\nDeposit amount: ", color::_ITALIC);
	std::cin >> amount_str;
	std::cin.ignore();
	print(color::_RESET);

	long double amount;
	try
	{
		amount = std::stold(amount_str);
	}
	catch (...)
	{
		Log::m_warn("Failed transaction.");
		funcs::getKeyPress();
		return;
	}

	if (amount < 0)
	{
		Log::m_warn("Failed transaction.");
		funcs::getKeyPress();
		return;
	}

	else if (amount == 0)
	{
		Log::m_info("Cancelled operation.");
		funcs::getKeyPress();
		return;
	}

	print("Details: ");
	std::string details;
	std::getline(std::cin, details);
	details = sanitizeDetails(details);
	if (details.size() < 2)
	{
		details = " ";
	}
	std::string date = funcs::currentTime();
	std::string fp = globals.file_paths.transaction_history;
	// "Type", "Amount", "Date", "Details"
	File::m_insertline(fp, "Deposit," + funcs::str(amount) + "," + date + "," + details, 0);

	globals.user_account_info.balance += amount;
	writeUserAccountInfo(globals);

	LOG(globals, "Deposited " + funcs::str(amount) + " " + globals.user_account_info.currency);
}

void bankWithdraw(Globals &globals)
{
	std::string amount_str;
	print("\n\nWithdraw amount: ", color::_ITALIC);
	std::cin >> amount_str;
	std::cin.ignore();
	print(color::_RESET);

	long double amount;
	try
	{
		amount = std::stold(amount_str);
	}
	catch (...)
	{
		Log::m_warn("Failed transaction.");
		funcs::getKeyPress();
		return;
	}

	if (amount < 0)
	{
		Log::m_warn("Failed transaction.");
		funcs::getKeyPress();
		return;
	}
	else if (amount == 0)
	{
		Log::m_info("Cancelled operation.");
		funcs::getKeyPress();
		return;
	}

	print("Details: ");
	std::string details;
	std::getline(std::cin, details);
	details = sanitizeDetails(details);
	if (details.size() < 2)
	{
		details = " ";
	}
	std::string date = funcs::currentTime();
	std::string fp = globals.file_paths.transaction_history;
	// "Type", "Amount", "Date", "Details"
	File::m_insertline(fp, "Withdraw," + funcs::str(amount) + "," + date + "," + details, 0);

	globals.user_account_info.balance -= amount;
	writeUserAccountInfo(globals);

	LOG(globals, "Withdrew " + funcs::str(amount) + " " + globals.user_account_info.currency);
}

void bankViewTransactionHistory(Globals &globals)
{
	std::string fp = globals.file_paths.transaction_history;
	Table table;
	//table.m_importCSV(fp);

	table.m_setHeader("Type", "Amount", "Date", "Details");

	print("\n\n", color::TXT_GREEN, "Showing latest ", globals.columns, " transactions", color::_RESET);

	std::vector<std::string> contents = File::m_readfile(fp);
	std::vector<std::string> row;
	for (size_t i = 0; i < (size_t)std::min(contents.size(), globals.columns); i++) // each line is a transaction
	{
		std::string line = contents[i];
		if (!parseTransactionRow(line, row))
		{
			continue;
		}
		table.m_addRow(row[0], row[1], row[2], row[3]);
	}

	std::cout.flush();
	print("\n\n");
	print(table);
	funcs::getKeyPress();
}

void bankShowCumulative(Globals &globals)
{
	std::string fp = globals.file_paths.transaction_history;
	std::vector<std::string> contents = File::m_readfile(fp);
	long double cum = 0;
	std::vector<std::string> row;

	print("\n\n", color::TXT_GREEN, "Cumulative of last ", globals.columns, " transactions", color::_RESET, " = ");

	for (size_t i = 0; i < (size_t)std::min(contents.size(), globals.columns); i++) // each line is a transaction
	{
		std::string line = contents[i];
		if (!parseTransactionRow(line, row))
		{
			continue;
		}

		std::string type = row[0];
		long double amount = 0.0L;
		if (!tryParseLongDouble(row[1], amount))
		{
			continue;
		}
		if (type == "Withdraw")
		{
			amount *= -1;
		}
		cum += amount;
	}

	print(color::TXT_YELLOW, color::_BOLD, cum, color::_RESET, globals.user_account_info.currency, "\n");
	funcs::getKeyPress();
}

void printUserInfo(Globals &globals)
{
	print(color::TXT_GREEN, "────────────────────────", color::_RESET, "\n");

	print("User Name: ", color::TXT_RED, globals.user_account_info.user_name, color::_RESET, "\n");
	print("Balance: ", color::TXT_RED, formatBalance(globals.user_account_info.balance), globals.user_account_info.currency, color::_RESET, "\n");

	print(color::TXT_GREEN, "────────────────────────", color::_RESET, "\n");
}

void setUserName(Globals &globals)
{
	readUserAccountInfo(globals);

	print("\nNew user name:\n> ", color::_ITALIC);
	std::string new_user_name;
	std::getline(std::cin, new_user_name);

	if (new_user_name.size() < 2)
	{
		Log::m_info("Cancelled operation.");
		funcs::getKeyPress();
		return;
	}

	globals.user_account_info.user_name = new_user_name;
	writeUserAccountInfo(globals);

	print(color::_RESET);

	LOG(globals, "Set user name to \"" + new_user_name + "\"");
}

void setCurrency(Globals &globals)
{
	readUserAccountInfo(globals);

	print("\nNew currency:\n> ", color::_ITALIC);
	std::string new_currency;
	std::getline(std::cin, new_currency);

	if (new_currency.size() < 2)
	{
		Log::m_info("Cancelled operation.");
		funcs::getKeyPress();
		return;
	}

	globals.user_account_info.currency = new_currency;
	writeUserAccountInfo(globals);

	print(color::_RESET);
	
	LOG(globals, "Set currency to \"" + new_currency + "\"");
}

void setBalance(Globals &globals)
{
	readUserAccountInfo(globals);

	print("\n");
	Log::m_warn("This is a dangerous operation.");

	print("\nNew balance:\n> ", color::_ITALIC);
	std::string new_balance_str;
	std::cin >> new_balance_str;
	std::cin.ignore();
	print(color::_RESET);

	long double amount;
	try
	{
		amount = std::stold(new_balance_str);
	}
	catch (...)
	{
		Log::m_warn("Failed transaction.");
		funcs::getKeyPress();
		return;
	}

	if (amount < 0)
	{
		Log::m_info("Cancelled operation.");
		funcs::getKeyPress();
		return;
	}

	globals.user_account_info.balance = amount;
	writeUserAccountInfo(globals);
	
	LOG(globals, "Set balance to " + new_balance_str);
}

void setColumns(Globals &globals)
{
	readUserAccountInfo(globals);

	print("\n");

	print("\nNew max column number:\n> ", color::_ITALIC);
	std::string column;
	std::cin >> column;
	std::cin.ignore();
	print(color::_RESET);

	size_t amount;
	try
	{
		amount = std::stoul(column);
	}
	catch (...)
	{
		Log::m_warn("Failed transaction.");
		funcs::getKeyPress();
		return;
	}

	const size_t max_entries = std::max<size_t>(1, File::m_numlines(globals.file_paths.transaction_history));
	globals.columns = std::clamp(amount, static_cast<size_t>(1), max_entries);

	writeUserAccountInfo(globals);
	
	LOG(globals, "Set columns to " + column);
}

#endif
