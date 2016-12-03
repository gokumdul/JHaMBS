#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include "common.hpp"
using namespace std;

// account.dat file
fstream account_file;

void make_new_user_account(bool admin) {
	string username;
	string password;
	string user_email;

	if (admin) {
		username = "admin";
	} else {
		cout << "Enter user name : ";
		getline(cin, username);
	}

	string read_username;
	string read_password; // unused
	string read_email; // unused

	fstream pass_dat;
	pass_dat.open("pass.dat", ios::in | ios::out);

	bool match = false;
	if (pass_dat.is_open()) {
		while (!pass_dat.eof()) {
			pass_dat >> read_username >> read_password >> read_email;
			if (username == read_username) {
				match = true;
				break;
			}
		}
	}
	if (match) {
		cerr << "Account already exists!" << endl;
		return;
	}

	cout << "Enter " << username << "'s password : ";
	getline(cin, password);

	// Admin's email might be publicly available,
	// do not allow password restoration if admin.
	if (!admin) {
		cout << "Enter " << username << "'s Email : ";
		getline(cin, user_email);
	}

	if (!pass_dat.is_open()) {
		// pass.dat does not exist
		pass_dat.close();
		pass_dat.open("pass.dat", ios::in | ios::out | ios::trunc);
	} else {
		// pass.dat exists, but at EOF state
		pass_dat.clear();
		pass_dat.seekg(0, ios::end);
		pass_dat.seekp(0, ios::end);
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

LOGIN_STATUS login() {
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
		return LOGIN_FAILED;
	}

	if (password != caesar_cipher(read_password, false)) {
		cerr << "Wrong password!" << endl;
		return LOGIN_FAILED;
	}

	cout << "Login successful!" << endl;

	// account_file is a global, static variable
	account_file.open(username + ".dat", ios::in | ios::out);
	if (!account_file.is_open()) {
		// username.dat does not exist
		account_file.open(username + ".dat", ios::in | ios::out | ios::trunc);
	}

	return (username == "admin" ? LOGIN_ADMIN : LOGIN_USER);
}
