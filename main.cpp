#include <iostream>
#include <fstream>
#include "common.hpp"
using namespace std;

int main() {
	fstream pass_dat;
	pass_dat.open("pass.dat", ios::in | ios::out);

	if (!pass_dat.is_open()) {
		// pass.dat does not exist
		make_new_user_account(true);
	}

	string menu[] = { "Login", "New account", "Find password", "Quit" };

	while(1) {
		switch (print_menu("Start", menu, sizeof(menu) / sizeof(string)))
		{
		case 1:
			switch (login())
			{
			case LOGIN_ADMIN:
				// admin_menu();
				break;
			case LOGIN_USER:
				book_movie();
				break;
			case LOGIN_FAILED:
			default:
				// Do nothing
				break;
			}
			break;
		case 2:
			make_new_user_account();
			break;
		/*case 3:
			find_password();
			break;*/
		case 4:
			return 0;
		default:
			cout << "Retry please" << endl;
			break;
		}
	}

	return 0;
}
