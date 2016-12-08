#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdio>
#include "common.hpp"
using namespace std;

account::account(string username, string password, string email, bool is_admin) {
	/*
	 * Username : Store without encryption
	 * Password : Store with Caesar's cipher on top of crc32 hash
	 *            so that the program itself cannot decrypt password
	 *            2-stage is also used for the same reason as below
	 * Email    : Brute-forcing email decryption is way too easy since '@'
	 *            and commonly-used domains are always used,
	 *            so store email with 2-stage Caesar's cipher
	 *            with the username as the offset for the 2nd round
	 *
	 * Seperator : space
	 */

	// Store username
	username.copy(this->username, username.size());
	// string::copy does not append a null character at the end of the copied content.
	this->username[username.size()] = '\0';

	// Store password
	set_password(password);

	// Store email
	strcpy(this->email, caesar_cipher(email.c_str(), username).c_str());

	// Set admin
	this->admin = is_admin;
}
string account::get_username() const {
	return string(username);
}
bool account::is_pw_match(string cmp) const {
	return (to_string(calc_crc32(cmp.c_str())) ==
		caesar_cipher(password, username, false, 80));
}
string account::get_email() const {
	return caesar_cipher(email, username, false);
}
bool account::is_admin() const {
	return admin;
}
void account::set_admin(bool admin) {
	this->admin = admin;
}
void account::set_password(string password) {
	strcpy(this->password,
		caesar_cipher(to_string(calc_crc32(password.c_str())),
				username,
				true,
				80)
			.c_str()
		);
}
void account::pw_recovery() {
	// TODO : implement
}
void account::save_to_pass_dat() const {
	fstream pass_dat;
	pass_dat.open("pass.dat", ios::in | ios::out);

	if (!pass_dat.is_open()) {
		// pass.dat does not exist
		pass_dat.close();
		pass_dat.open("pass.dat", ios::in | ios::out | ios::trunc);
	}

	// Try to save at the existing location
	int index = exists_in_pass_dat(username);
	if (index)
		pass_dat.seekp(sizeof(account) * (index - 1));
	else
		pass_dat.seekp(0, ios::end);
	pass_dat.write(reinterpret_cast<const char *>(this), sizeof(account));

	pass_dat.close();
}
int account::exists_in_pass_dat(string username, bool get, account *user) {
	ifstream pass_dat;
	pass_dat.open("pass.dat");

	if (!pass_dat.is_open())
		return 0;

	if (!user || user == nullptr)
		user = new account();
	int index = 0;

	while (pass_dat.read(reinterpret_cast<char *>(user), sizeof(account))) {
		index++;
		if (user->get_username() == username) {
			pass_dat.close();
			if (!get)
				free(user);

			return index;
		}
	}

	pass_dat.close();
	if (!get)
		free(user);

	return 0;
}
void account::gc() {
	ofstream tmp_dat;
	tmp_dat.open("tmp_pass.dat");

	ifstream pass_dat;
	pass_dat.open("pass.dat");

	if (!pass_dat.is_open())
		return;

	// Declare an empty char array
	char arr[sizeof(account)] = "";

	while (pass_dat.read(arr, sizeof(account))) {
		// strlen returns 0(false) when arr is empty
		if (strlen(arr))
			tmp_dat.write(arr, sizeof(account));
	}

	pass_dat.close();
	tmp_dat.close();

	rename("tmp_pass.dat", "pass.dat");
}

// account.dat file
fstream account_file;

void make_new_user_account(bool admin) {
	string username;
	string password;
	string user_email;

	if (admin) {
		username = "admin";
		user_email = "null";
	} else {
		cout << "Enter user name : ";
		getline(cin, username);
	}

	if (account::exists_in_pass_dat(username)) {
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

	account new_account(username, password, user_email, admin);
	new_account.save_to_pass_dat();
}

LOGIN_STATUS login() {
	ifstream pass_dat;
	pass_dat.open("pass.dat");

	if (!pass_dat.is_open()) {
		// pass.dat does not exist
		make_new_user_account(true);
	} else {
		pass_dat.close();
	}

	string username;
	string password;

	cout << "Username : ";
	getline(cin, username);

	cout << "Password : ";
	getline(cin, password);

	account *user = new account();
	if (!account::exists_in_pass_dat(username, true, user)) {
		cerr << "No such username!" << endl;
		if (user)
			delete user;
		return LOGIN_FAILED;
	}

	if (!user->is_pw_match(password)) {
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

	LOGIN_STATUS retval = (user->is_admin() ? LOGIN_ADMIN : LOGIN_USER);

	delete user;

	return retval;
}
