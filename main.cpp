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

	int index;

	while(1) {
		cout << "1. Login" << endl
			<< "2. New account" << endl
			<< "3. Find password" << endl
			<< "4. Quit" << endl;

		cout << "What do you want to do? : ";
		cin >> index;

		// Clear the buffer for next getline()
		cin.ignore();

		switch (index)
		{
		case 1:
			login();
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
