#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
using namespace std;

extern unsigned int calc_crc32(const char* contents);
extern string caesar_cipher(string val, bool encrypt = true);

void make_new_user_account(bool admin = false) {
	string username;
	string password;
	string user_email;

	if (admin) {
		username = "admin";
	} else {
		cout << "Enter user name : ";
		getline(cin, username);
	}

	cout << "Enter " << username << "'s password : ";
	getline(cin, password);

	// Admin's email might be publicly available,
	// do not allow password restoration if admin.
	if (!admin) {
		cout << "Enter " << username << "'s Email : ";
		getline(cin, user_email);
	}

	fstream pass_dat;
	pass_dat.open("pass.dat", ios::in | ios::out | ios::app);
	
	if (!pass_dat.is_open()) {
		// pass.dat does not exist
		pass_dat.close();
		pass_dat.open("pass.dat", ios::in | ios::out | ios::trunc | ios::app);
	}

	/*
	 * Store username without encryption
	 * Store password with Caesar's cipher
	 * Store email with Caesar's cipher on top of crc32 hash
	 *
	 * Seperator : space
	 */
	pass_dat
		<< username << ' '
		<< caesar_cipher(password) << ' '
		<< (admin ? "null" : caesar_cipher(to_string(calc_crc32(user_email.c_str())))) << '\n';
}

void login() {
	fstream pass_dat;
	pass_dat.open("pass.dat", ios::in | ios::out);

	if (!pass_dat.is_open()) {
		// pass.dat does not exist
		make_new_user_account(true);
	}

	string username;
	string password;
	string read_username;
	string read_password;
	string read_email; // unused

	cout << "Username : ";
	getline(cin, username);

	cout << "Password : ";
	getline(cin, password);

	// Close and reopen
	pass_dat.close();
	pass_dat.open("pass.dat", ios::in | ios::out);

	bool match = false;
	while (!pass_dat.eof()) {
		pass_dat >> read_username >> read_password >> read_email;
		if (username == read_username) {
			match = true;
			break;
		}
	}

	if (!match) {
		cerr << "No such username!" << endl;
		return;
	}

	if (password != caesar_cipher(read_password, false)) {
		cout << "Wrong password!" << endl;
		return;
	}

	cout << "Login successful!" << endl;
}
