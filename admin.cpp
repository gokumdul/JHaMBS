#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include "common.hpp"
using namespace std;

static void user_management();
static int show_user_list(bool ret, bool email);

void admin_menu() {
	while(1) {
		string initial_admin_menu[] = { "Schedule management", "User management", "Go back" };
		switch (print_menu("Admin menu", initial_admin_menu, sizeof(initial_admin_menu) / sizeof(string))) {
		case 1:
			// TODO : implement schedule management
			break;
		case 2:
			user_management();
			break;
		case 3:
			return;
		}
	}
}

static void user_management() {
	string user_management_menu[] = { "Set authority", "Show user list", "Delete user", "Go back" };
	while(1) {
	switch (print_menu("User management", user_management_menu, sizeof(user_management_menu) / sizeof(string))) {
		case 1:
			// TODO : implement admin/user switch
			break;
		case 2:
			show_user_list(false, true);
			break;
		case 3:
		{ // Limit new declarations within the scope
			int del = show_user_list(true, false);
			if (!del)
				break;

			// Create a temporary file excluding the line to remove
			fstream tmp_file;
			tmp_file.open("tmp_pass.dat", ios::in | ios::out | ios::trunc);

			ifstream pass_dat;
			pass_dat.open("pass.dat");

			int i = 1;
			string tmp;
			string username;

			// Copy the contents excluding the line to remove
			while (getline(pass_dat, tmp)) {
				if (i++ == del) {
					username = tmp.substr(0, tmp.find(" "));
					continue;
				}
				tmp_file << tmp << endl;
			}

			pass_dat.close();
			tmp_file.close();

			// Close global account_file fstream, if opened
			if (account_file && account_file.is_open())
				account_file.close();

			// Remove user data
			remove(string(username + ".dat").c_str());

			// Rename tmp_pass.dat to pass.dat
			remove("pass.dat");
			rename("tmp_pass.dat", "pass.dat");

			break;
		}
		case 4:
			return;
		}
	}
}

static int show_user_list(bool ret, bool email) {
	vector<string> vec;

	string read_username;
	string read_password; // unused
	string read_email;

	ifstream pass_dat;
	pass_dat.open("pass.dat");
	int size = 0;

	if (pass_dat.is_open()) {
		string tmp;
		while (pass_dat >> read_username >> read_password >> read_email) {
			size++;
			tmp = read_username;
			if (email && read_email != "null")
				tmp += " - " + caesar_cipher(read_email.c_str(), read_username, false);
			if (!(ret && read_username == "admin"))
				vec.push_back(tmp);
			else
				size--;
		}
		pass_dat.close();
	}

	if (ret && size == 0) {
		cerr << "No users to list!" << endl;
		return 0;
	}

	string* list = vtoa(vec, ret);
	if (ret)
		list[size] = "Go back";
	int retval = print_menu("User list", list, size + ret, ret);
	// Free the pointer
	free(list);

	return retval;
}
