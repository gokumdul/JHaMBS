#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
using namespace std;

// TODO : Generalize this function with other user account create function
void make_new_admin_account() {
	string admin_pw;

	cout << "Enter admin password : ";
	getline(cin, admin_pw);

	fstream pass_dat;
	pass_dat.open("pass.dat", ios::in | ios::out | ios::binary | ios::trunc);

	pass_dat << "admin " << admin_pw;
}

void login() {
	fstream pass_dat;
	pass_dat.open("pass.dat", ios::in | ios::out | ios::binary);

	if (!pass_dat.is_open()) {
		// pass.dat does not exist
		make_new_admin_account();
	}

	pass_dat.open("pass.dat", ios::in | ios::out | ios::binary);

	string username;
	string password;
	
	cout << "Username : ";
	getline(cin, username);

	cout << "Password : ";
	getline(cin, password);

	// debug
	cout << username << " " << password << endl;
}