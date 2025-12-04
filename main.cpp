/* CBank++ by HassanIQ777
Created     : 2025-Aug-30
Last update : 2025-Nov-12 */

//    ___________________                __
//    \_   ___ \______   \_____    ____ |  | __    .__         .__
//    /    \  \/|    |  _/\__  \  /    \|  |/ /  __|  |___   __|  |___
//    \     \___|    |   \ / __ \|   |  \    <  /__    __/  /__    __/
//     \______  /______  /(____  /___|  /__|_ \    |__|        |__|
//            \/       \/      \/     \/     \/

#include "bank_operations.hpp"

#include "libutils/src/table.hpp"
#include "libutils/src/cliparser.hpp"

// ##########################################
// 		Function declarations
void assignPaths(Globals &globals);
void parseArgs(CLIParser &parser, Globals &globals);

void stateMachine(Globals &globals);
void mainMenu(Globals &globals);
void settingsMenu(Globals &globals);
void viewUserInfoMenu(Globals &globals);

void parseMainMenuOption(Globals &globals, const std::string &option_str);
void parseSettingsMenuOption(Globals &globals, const std::string &option_str);
void parseViewUserInfoMenuOption(Globals &globals, const std::string &option_str);

void createFiles(const Globals &globals);
void printLogo(const Globals &globals);
// ###########################################

int main(int argc, char *argv[])
{
	CLIParser parser(argc, argv);

	Globals globals;
	globals.VERSION = "1.2";
	parseArgs(parser, globals);
	assignPaths(globals);
	createFiles(globals);
	globals.ui_state = UI_state::MAIN_MENU;
	readUserAccountInfo(globals);

	bool running = true;
	while (running)
	{
		std::cout.flush();
		funcs::clearTerminal();

		stateMachine(globals);
	}
}

void assignPaths(Globals &globals)
{
	globals.file_paths.account_info = fs::path(globals.file_paths.home_dir) / fs::path("account_info.ini");
	globals.file_paths.transaction_history = fs::path(globals.file_paths.home_dir) / fs::path("transaction_history.csv");
	globals.file_paths.logs = fs::path(globals.file_paths.home_dir) / fs::path("logs.txt");
}

void parseArgs(CLIParser &parser, Globals &globals)
{
	auto printHelp = [&]() -> void {
		print("Usage: ", parser.m_getArg(0), " <DIRECTORY>\n");
		print("Usage: ", parser.m_getArg(0), " [options]\n");
		print("Options:\n");
		print("  -h   Show this help message\n");
		print("  -v   Show program version\n");
	};

	if (parser.m_getArgc() != 2)
	{
		Log::m_error("Two arguments only are required.", false); //don't exit just now
		printHelp();
		exit(-1);
	}

	else if (parser.m_hasFlag("-h"))
	{
		printHelp();
		exit(0);
	}

	else if (parser.m_hasFlag("-v"))
	{
		print("CBank++ version ", globals.VERSION,"\n");
		exit(0);
	}

	else if (!File::m_isdirectory(parser.m_getArg(1)))
	{
		Log::m_error("'" + parser.m_getArg(1) + "' is not a directory.", false);
		exit(-2);
	}

	globals.file_paths.home_dir = parser.m_getArg(1);
}

void stateMachine(Globals &globals)
{
	switch (globals.ui_state)
	{
	case UI_state::MAIN_MENU:
		mainMenu(globals);
		break;

	case UI_state::SETTINGS:
		settingsMenu(globals);
		break;

	case UI_state::VIEW_USER_INFO:
		viewUserInfoMenu(globals);
		break;

	case UI_state::EXIT_PROGRAM:
		std::exit(EXIT_SUCCESS);
		break; // useless but whatever
	}
}

void mainMenu(Globals &globals)
{
	printLogo(globals);

	print("\n", color::TXT_GREEN, color::_BOLD, "Main Menu", color::_RESET, "\n\n");
	print(color::TXT_YELLOW, "1) ", color::TXT_CYAN, "Deposit Money", color::_RESET, '\n');
	print(color::TXT_YELLOW, "2) ", color::TXT_CYAN, "Withdraw Money", color::_RESET, '\n');
	print(color::TXT_YELLOW, "3) ", color::TXT_CYAN, "View Account Info", color::_RESET, '\n');

	print("\n");

	print(color::TXT_YELLOW, "9) ", color::TXT_CYAN, "Settings", color::_RESET, '\n');
	print(color::TXT_YELLOW, "0) ", color::TXT_CYAN, "Logout", color::_RESET, '\n');

	print(color::TXT_BLUE, "\nSelect option:", color::_RESET, " _");

	std::string option = funcs::getKeyPress();
	parseMainMenuOption(globals, option);
}

void parseMainMenuOption(Globals &globals, const std::string &option_str)
{
	int option;
	try
	{
		option = stoi(option_str);
	}
	catch (...)
	{
		return;
	}

	switch (option) // Not a crazy ass optimization. Just want the code to look good :3
	{
	case 1:
		bankDeposit(globals);
		break;

	case 2:
		bankWithdraw(globals);
		break;

	case 3:
		globals.ui_state = UI_state::VIEW_USER_INFO;
		break;

	case 9:
		globals.ui_state = UI_state::SETTINGS;
		break;

	case 0:
		globals.ui_state = UI_state::EXIT_PROGRAM;
		break;
	}
}

void settingsMenu(Globals &globals)
{
	printLogo(globals);

	print("\n", color::TXT_GREEN, color::_BOLD, "Settings Menu", color::_RESET, "\n\n");
	print(color::TXT_YELLOW, "1) ", color::TXT_CYAN, "Set user name", color::_RESET, '\n');
	print(color::TXT_YELLOW, "2) ", color::TXT_CYAN, "Set currency", color::_RESET, '\n');
	print(color::TXT_YELLOW, "3) ", color::TXT_CYAN, "Set balance", color::_RESET, '\n');
	print(color::TXT_YELLOW, "4) ", color::TXT_CYAN, "Transaction History Entries = ", globals.columns, color::_RESET, '\n');
	
	print("\n");

	print(color::TXT_YELLOW, "9) ", color::TXT_CYAN, "Go Back", color::_RESET, '\n');

	print(color::TXT_BLUE, "\nSelect option:", color::_RESET, " _");

	std::string option = funcs::getKeyPress();
	parseSettingsMenuOption(globals, option);
}

void parseSettingsMenuOption(Globals &globals, const std::string &option_str)
{
	int option;
	try
	{
		option = stoi(option_str);
	}
	catch (...)
	{
		return;
	}

	switch (option)
	{
	case 1:
		setUserName(globals);
		break;

	case 2:
		setCurrency(globals);
		break;

	case 3:
		setBalance(globals);
		break;
		
	case 4:
		setColumns(globals);
		break;

	case 9:
		globals.ui_state = UI_state::MAIN_MENU;
		break;
	}
}

void viewUserInfoMenu(Globals &globals)
{
	printLogo(globals);

	print("\n", color::TXT_GREEN, color::_BOLD, "User Info Menu", color::_RESET, "\n\n");
	printUserInfo(globals);

	print("\n", color::TXT_YELLOW, "1) ", color::TXT_CYAN, "View Transaction History", color::_RESET, '\n');
	print(color::TXT_YELLOW, "2) ", color::TXT_CYAN, "View Cumulative", color::_RESET, '\n');
	
	print("\n");

	print(color::TXT_YELLOW, "9) ", color::TXT_CYAN, "Go Back", color::_RESET, '\n');

	print(color::TXT_BLUE, "\nSelect option:", color::_RESET, " _");

	std::string option = funcs::getKeyPress();
	parseViewUserInfoMenuOption(globals, option);
}

void parseViewUserInfoMenuOption(Globals &globals, const std::string &option_str)
{
	int option;
	try
	{
		option = stoi(option_str);
	}
	catch (...)
	{
		return;
	}

	switch (option)
	{
	case 1:
		bankViewTransactionHistory(globals);
		break;
	case 2:
		bankShowCumulative(globals);
		break;

	case 9:
		globals.ui_state = UI_state::MAIN_MENU;
		break;
	}
}

void createFiles(const Globals &globals)
{
	if (!File::m_isfile(globals.file_paths.logs))
	{
		std::string fp = globals.file_paths.logs;
		File::m_createfile(fp);

		LOG(globals,"Successfully created '" + fp + "'");
	}

	if (!File::m_isfile(globals.file_paths.account_info))
	{
		std::string fp = globals.file_paths.account_info;
		File::m_createfile(fp);
		File::m_appendline(fp, "balance=0");
		File::m_appendline(fp, "currency= USD");
		File::m_appendline(fp, "user_name=User");
		File::m_appendline(fp, "columns=10");

		LOG(globals,"Successfully created '" + fp + "'");
	}

	if (!File::m_isfile(globals.file_paths.transaction_history))
	{
		std::string fp = globals.file_paths.transaction_history;
		File::m_createfile(fp);

		LOG(globals,"Successfully created '" + fp + "'");
	}
}

void printLogo(const Globals &globals)
{
	std::string time_hours_mins = funcs::currentTime().substr(11, 5);
	std::string user_name = globals.user_account_info.user_name;

	print(color::_BOLD);
	funcs::printLeftMiddleRight(user_name, "", time_hours_mins);
	print(color::_RESET);

	std::cout << color::_BOLD << color::TXT_YELLOW
			  << R"(		            ______  ______                    __                       
	 .' ___  ||_   _ \                  [  |  _   .-.      .-.    
	/ .'   \_|  | |_) |  ,--.   _ .--.   | | / ]__| |__  __| |__  
	| |         |  __'. `'_\ : [ `.-. |  | '' <|__   __||__   __| 
	\ `.___.'\ _| |__) |// | |, | | | |  | |`\ \  | |      | |    
	 `.____ .'|_______/ \'-;__/[___||__][__|  \_] '-'      '-'    )";

	print(color::_RESET, "\n");

	std::cout.flush();
}
