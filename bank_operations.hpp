// bank_operations.hpp

#ifndef BANK_OPERATIONS
#define BANK_OPERATIONS

#include "declarations.hpp"

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

	long double balance = std::stold(File::m_getFromINI(fp, "balance"));
	std::string currency = File::m_getFromINI(fp, "currency");
	std::string user_name = File::m_getFromINI(fp, "user_name");
	size_t columns = std::stoul(File::m_getFromINI(fp, "columns"));

	globals.user_account_info.balance = balance;
	globals.user_account_info.currency = currency;
	globals.user_account_info.user_name = user_name;
	globals.columns = columns;
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
		Log::m_info("Canceled operation.");
		funcs::getKeyPress();
		return;
	}

	print("Details: ");
	std::string details;
	std::getline(std::cin, details);
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
		Log::m_info("Canceled operation.");
		funcs::getKeyPress();
		return;
	}

	print("Details: ");
	std::string details;
	std::getline(std::cin, details);
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
	for (size_t i = 0; i < (size_t)std::min(contents.size(), globals.columns); i++) // each line is a transaction
	{
		std::string line = contents[i];
		std::vector<std::string> v = funcs::split(line, ',');
		table.m_addRow(v[0], v[1], v[2], v[3]);
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

	print("\n\n", color::TXT_GREEN, "Cumulative of last ", globals.columns, " transactions", color::_RESET, " = ");

	for (size_t i = 0; i < (size_t)std::min(contents.size(), globals.columns); i++) // each line is a transaction
	{
		std::string line = contents[i];
		std::vector<std::string> v = funcs::split(line, ',');

		std::string type = v[0];
		long double amount = std::stold(v[1]);
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
	print("Balance: ", color::TXT_RED, formatNumber((int)globals.user_account_info.balance), globals.user_account_info.currency, color::_RESET, "\n");

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
		Log::m_info("Canceled operation.");
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
		Log::m_info("Canceled operation.");
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
		Log::m_info("Canceled operation.");
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

	if (amount < 0)
	{
		Log::m_info("Canceled operation.");
		funcs::getKeyPress();
		return;
	}

	globals.columns = std::clamp(amount, 1UL, File::m_numlines(globals.file_paths.transaction_history));

	writeUserAccountInfo(globals);
	
	LOG(globals, "Set columns to " + column);
}

#endif