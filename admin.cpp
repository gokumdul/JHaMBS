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
		{ // Limit new declarations within the scope
			int to_switch = show_user_list(true, false);
			if (!to_switch)
				break;

			ifstream pass_dat;
			pass_dat.open("pass.dat");

			// to_switch here counts after the admin account, so it's (to_switch), not (to_switch - 1)
			pass_dat.seekg(sizeof(account) * to_switch);

			account user;
			pass_dat.read(reinterpret_cast<char *>(&user), sizeof(account));
			pass_dat.close();

			cout << "Changing " << user.get_username()
			     << " from " <<  (user.is_admin() ? "admin" : "regular user")
			     << " to "   << (!user.is_admin() ? "admin" : "regular user")
			     << endl;

			user.set_admin(!user.is_admin());
			user.save_to_pass_dat();

			break;
		}
		case 2:
			show_user_list(false, true);
			break;
		case 3:
		{ // Limit new declarations within the scope
			int del = show_user_list(true, false);
			if (!del)
				break;

			account::remove_data(del + 1);

			break;
		}
		case 4:
			return;
		}
	}
}

static int show_user_list(bool ret, bool email) {
	vector<string> vec;
	ifstream pass_dat;
	int size = 0;

	// Trigger garbage collection in pass.dat to avoid number collision
	account::gc();

	pass_dat.open("pass.dat");
	if (pass_dat.is_open()) {
		string tmp;
		account user;
		while (pass_dat.read(reinterpret_cast<char *>(&user), sizeof(account))) {
			tmp = user.get_username();

			size++;
			if (email && user.get_email() != "null")
				tmp += " - " + user.get_email();

			/*
			 * Do not use user.is_admin() here as
			 * multiple users can have administrator permissions.
			 * We don't want the "admin" user to be deleted
			 */
			if (!(ret && user.get_username() == "admin"))
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
	delete[] list;

	// Check if retval is "Go back"
	if (ret && retval == size + 1)
		retval = 0;

	return retval;
}
