#include <iostream>
#include <fstream>
#include "common.hpp"
using namespace std;

int main() {
	ifstream pass_dat;
	pass_dat.open("pass.dat");

	if (!pass_dat.is_open()) {
		// pass.dat does not exist
		make_new_user_account(true);
	} else {
		pass_dat.close();
	}

	string menu[] = { "Login", "New account", "Find password", "Quit" };

	while(1) {
		switch (print_menu("Start", menu, sizeof(menu) / sizeof(string)))
		{
		case 1:
			switch (login())
			{
			case LOGIN_ADMIN:
				admin_menu();
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
		case 3:
			reset_password();
			break;
		case 4:
			// Trigger garbage collection in pass.dat
			account::gc();
			return 0;
		}
	}

	return 0;
}
