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

static void schedule_management();
static void schedule_management_sub(); // Called from schedule_management()
static void user_management();
static int show_user_list(bool ret, bool email);

void admin_menu() {
	while(1) {
		string initial_admin_menu[] = { "Schedule management", "User management", "Go back" };
		switch (print_menu("Admin menu", initial_admin_menu, sizeof(initial_admin_menu) / sizeof(string))) {
		case 1:
			schedule_management();
			break;
		case 2:
			user_management();
			break;
		case 3:
			return;
		}
	}
}

static void schedule_management() {
	while(1) {
		string schedule_management_menu[] = {
			"Add a new hall",
			"Manage a specific hall",
			"Go back"
		};
		int schedule_management_menu_size =
			sizeof(schedule_management_menu) / sizeof(string);

		// Print basic hall info first
		vector<string> hall_dats = listdir("halls", "hall-", ".dat");
		vector<string>::iterator it_string;

		vector<hall> hall_vec;
		vector<hall>::iterator it_hall;

		for (it_string = hall_dats.begin(); it_string != hall_dats.end(); it_string++)
			hall_vec.push_back(hall("halls/" + *it_string));

		vector<string> hall_info;
		for (it_hall = hall_vec.begin(); it_hall != hall_vec.end(); it_hall++) {
			// Write basic hall number
			string tmp = "Hall " + to_string(it_hall->get_hall_number());

			// Write screen type
			tmp += " - ";
			switch (it_hall->get_screen_type()) {
			case SCREEN_2D:
				tmp += "2D";
				break;
			case SCREEN_3D:
				tmp += "3D";
				break;
			case SCREEN_IMAX:
				tmp += "IMAX";
				break;
			}

			// Save to vector
			hall_info.push_back(tmp);
		}

		if (!hall_info.empty()) {
			string* hall_info_menu = vtoa(hall_info);
			print_menu("Hall information", hall_info_menu, hall_info.size(), false);
			delete[] hall_info_menu;
		} else {
			// Move "Go back" to the upper choice
			schedule_management_menu_size--;
			schedule_management_menu[schedule_management_menu_size - 1] =
				schedule_management_menu[schedule_management_menu_size];
		}

		// Show the control menu

		switch (print_menu("Schedule management",
				schedule_management_menu,
				schedule_management_menu_size)) {
		case 1:
			create_new_hall();
			break;
		case 2:
			// case 2 is "Go back", if hall_info.empty() is true
			if (hall_info.empty())
				return;

			schedule_management_sub();
			break;
		case 3:
			return;
		}
	}
}

static void schedule_management_sub() {
	int hall_number;
	do {
		cout << "Please choose a hall number : ";
		cin >> hall_number;

		ifstream check;
		check.open("halls/hall-" + to_string(hall_number) + ".dat");
		if (check.is_open()) {
			check.close();
			break;
		}

		cerr << "Unable to find a hall with the number " << hall_number << "!" << endl;
	} while(1);

	// Open the corresponding hall.dat file
	string hall_dat_name = "halls/hall-" + to_string(hall_number) + ".dat";
	hall hall_obj(hall_dat_name);

	string schedule_management_sub_menu[] = {
		"Show schedule",
		"Add a new movie timetable",
		"Delete an existing movie timetable",
		"Delete the entire schedule",
		"Delete this hall altogether",
		"Go back"
	};
	while(1) {
		switch (print_menu("Hall management", schedule_management_sub_menu, sizeof(schedule_management_sub_menu) / sizeof(string))) {
		case 1:
			hall_obj.show_all_timetable();
			break;
		case 2:
			hall_add_movie_timetable(hall_obj);
			break;
		case 3:
			hall_remove_movie_timetable(hall_obj);
			break;
		case 4:
			hall_remove_movie_timetable(hall_obj, true);
			break;
		case 5:
			remove(hall_dat_name.c_str());
			break;
		case 6:
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

			account::remove_data(del);

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
