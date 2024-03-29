#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdio>
#include "common.hpp"
using namespace std;

account::account(string username, string password, string email,
	string sec_q, string sec_a,
	bool is_admin) {
	// account object is being newly created, initialize the members
	retries = 0;

	/*
	 * Username : Store without encryption
	 * Password : Store with Caesar's cipher on top of crc32 hash
	 *            so that the program itself cannot decrypt password
	 *            2-stage is also used for the same reason as below
	 * Email    : Brute-forcing email decryption is way too easy since '@'
	 *            and commonly-used domains are always used,
	 *            so store email with 2-stage Caesar's cipher
	 *            with the username as the offset for the 2nd round
	 * Security question : Same as email
	 * Security answer   : Same as password
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

	// Store security question
	strcpy(this->sec_q, caesar_cipher(sec_q.c_str(), username).c_str());

	// Store security answer
	strcpy(this->sec_a,
		caesar_cipher(to_string(calc_crc32(sec_a.c_str())),
				username,
				true,
				80)
			.c_str()
		);

	// Set admin
	this->admin = is_admin;
}
string account::get_username() const {
	return string(username);
}
bool account::is_pw_match(string cmp) {
	bool match = (to_string(calc_crc32(cmp.c_str())) ==
		caesar_cipher(password, username, false, 80));

	if (!match) {
		cls();
		should_clear = false;
		retries++;
		save_to_pass_dat();
	}

	/*
	 * We'd love to enforce security to all users, including admin,
	 * but removing the "admin" account doesn't make sense.
	 *
	 * All other secondary admin accounts
	 * (which has admin permissions but not named "admin") can be removed.
	 */
	if (!match && strcmp(username, "admin") && retries > 5) {
		cerr << endl
		     << "If password match failure happens 10 times," << endl
		     << "the entire user data gets wiped!" << endl
		     << "You have been warned!" << endl
		     << endl;
	}

	if (!match && strcmp(username, "admin") && retries > 10) {
		cerr << endl
		     << "Too many attempts, wiping user data!" << endl
		     << endl;
		remove_data();
	}

	if (match) {
		if (retries > 3) {
			cout << endl
			     << "This account was attempted to be logged in" << endl
			     << retries << " times before with incorrect passwords!" << endl
			     << endl
			     << "If this wasn't you, we recommend you to change your password." << endl
			     << endl;
		}

		// Login successful, reset retries
		retries = 0;
		save_to_pass_dat();
	}

	return match;
}
bool account::valid_email(string email) {
	// Check for '@' inside the string
	if (email.find("@") == string::npos)
		return false;

	// Check for '.' after the '@'
	if (email.substr(email.find("@")).find(".") == string::npos)
		return false;

	return true;
}
string account::get_email() const {
	return caesar_cipher(email, username, false);
}
void account::set_email(string email) {
	email.copy(this->email, email.size());
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
	string email;

	cout << "Type user's email to reset password : ";
	getline(cin, email);

	if (email != get_email()) {
		cls();
		should_clear = false;
		cerr << "Incorrect email address!" << endl;
		return;
	}

	// Ask personal security question
	string sec_a;
	cout << caesar_cipher(sec_q, username, false) << " ";
	getline(cin, sec_a);
	if (to_string(calc_crc32(sec_a.c_str())) !=
		caesar_cipher(this->sec_a, username, false, 80)) {
		cls();
		should_clear = false;
		cerr << "Incorrect answer!" << endl;
		return;
	}

	string pw1; // First  entry
	string pw2; // Second entry

	do {
		cout << "Enter your new password : ";
		getline(cin, pw1);

		cout << "Re-enter your new password : ";
		getline(cin, pw2);

		if (pw1 == pw2)
			break;

		cerr << "Password mismatches!" << endl
		     << "Please try again!" << endl << endl;
	} while (1);

	set_password(pw1);
	this->save_to_pass_dat();

	cout << "Password updated!" << endl;
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
void account::remove_data() const {
	remove_data(this->username);
}
void account::remove_data(string username) {
	remove_data(exists_in_pass_dat(username), username);
}
void account::remove_data(int index, string username) {
	// Replace the account data in pass.dat with empty characters
	fstream pass_dat;
	pass_dat.open("pass.dat", ios::in | ios::out);

	// Move the pointer in front of the data to delete
	pass_dat.seekg(sizeof(account) * (index - 1));
	pass_dat.seekp(sizeof(account) * (index - 1));

	if (username.empty()) {
		// Read first and obtain username
		account user;
		pass_dat.read(reinterpret_cast<char *>(&user), sizeof(account));
		username = user.username;
	}

	// Create an empty character array
	char empty[sizeof(account)] = "";

	// Use it to wipe the data
	pass_dat.write(empty, sizeof(account));
	pass_dat.close();

	// Wipe the seperate user data as well
	username = "users/user-" + username + ".dat";
	remove(username.c_str());
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

// Currently logged-in session
customer logged_in_session;

void make_new_user_account(bool admin) {
	string username;
	string password;
	string user_email;
	string sec_q;
	string sec_a;

	if (admin) {
		username = "admin";
	} else {
		do {
			cout << "Enter user name : ";
			getline(cin, username);

			if (username.size() <= account::username_len)
				break;

			cerr << "Username must be shorter than "
			     << account::username_len << " "
			     << "characters!" << endl;
		} while(1);
	}

	if (account::exists_in_pass_dat(username)) {
		cls();
		should_clear = false;
		cerr << "Account already exists!" << endl;
		return;
	}

	cout << "Enter " << username << "'s password : ";
	getline(cin, password);

	do {
		cout << "Enter " << username << "'s Email : ";
		getline(cin, user_email);

		if (!account::valid_email(user_email)) {
			cerr << "Invalid email address!" << endl;
			continue;
		}
		if (user_email.size() > account::email_len) {
			cerr << "Email must be shorter than "
			     << account::email_len << " "
			     << "characters!" << endl;
			continue;
		}

		break;
	} while(1);

	do {
		cout << "Enter your personal security question : ";
		getline(cin, sec_q);

		if (sec_q.size() <= account::sec_q_len)
			break;

		cerr << "Security question must be shorter than "
		     << account::sec_q_len << " "
		     << "characters!" << endl;
	} while(1);

	cout << "Enter your personal security question's answer : ";
	getline(cin, sec_a);

	account new_account(username, password, user_email, sec_q, sec_a, admin);
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
		cls();
		should_clear = false;
		cerr << "No such username!" << endl;
		if (user)
			delete user;
		return LOGIN_FAILED;
	}

	if (!user->is_pw_match(password)) {
		cls();
		should_clear = false;
		cerr << "Wrong password!" << endl;
		return LOGIN_FAILED;
	}

	cls();
	should_clear = false;
	cout << "Login successful!" << endl;

	if (user->is_admin()) {
		delete user;
		return LOGIN_ADMIN;
	}

	// logged_in_session is a global, static variable
	logged_in_session = customer(user->get_username());
	delete user;
	return LOGIN_USER;
}

// Real password recovery is done within the class(account::pw_recovery())
void reset_password(string username) {
	ifstream pass_dat;
	pass_dat.open("pass.dat");

	if (!pass_dat.is_open()) {
		// pass.dat does not exist
		cls();
		should_clear = false;
		cerr << "No user data available!" << endl;
		return;
	} else {
		pass_dat.close();
	}

	string email;

	if (username.empty()) {
		cout << "Username : ";
		getline(cin, username);
	}

	account *user = new account();
	if (!account::exists_in_pass_dat(username, true, user)) {
		cls();
		should_clear = false;
		cerr << "No such username!" << endl;
		if (user)
			delete user;
		return;
	}

	user->pw_recovery();
	delete user;
}

void reset_email(string username) {
	ifstream pass_dat;
	pass_dat.open("pass.dat");

	if (!pass_dat.is_open()) {
		// pass.dat does not exist
		cls();
		should_clear = false;
		cerr << "No user data available!" << endl;
		return;
	} else {
		pass_dat.close();
	}

	if (username.empty()) {
		cout << "Username : ";
		getline(cin, username);
	}

	account *user = new account();
	if (!account::exists_in_pass_dat(username, true, user)) {
		cls();
		should_clear = false;
		cerr << "No such username!" << endl;
		if (user)
			delete user;
		return;
	}

	string user_email;
	do {
		cout << "Enter " << username << "'s Email : ";
		getline(cin, user_email);

		if (!account::valid_email(user_email)) {
			cerr << "Invalid email address!" << endl;
			continue;
		}
		if (user_email.size() > account::email_len) {
			cerr << "Email must be shorter than "
			     << account::email_len << " "
			     << "characters!" << endl;
			continue;
		}

		break;
	} while(1);

	user->set_email(user_email);
	delete user;
}
