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

void user_menu() {
	while(1) {
		string initial_user_menu[] = { "Book a movie", "Manage purchases", "Manage account", "Go back" };
		switch (print_menu("User menu", initial_user_menu, sizeof(initial_user_menu) / sizeof(string))) {
		case 1:
			book_movie();
			break;
		case 2:
			logged_in_session.manage_book_data();
			break;
		case 3:
			user_management();
			break;
		case 4:
			return;
		}
	}
}

static void user_management() {
	string user_management_menu[] = { "Change my password", "Change my email", "Delete my account", "Go back" };
	while(1) {
		switch (print_menu("User management", user_management_menu, sizeof(user_management_menu) / sizeof(string))) {
		case 1:
			reset_password(logged_in_session.get_username());
			break;
		case 2:
			reset_email(logged_in_session.get_username());
			break;
		case 3:
			account::remove_data(logged_in_session.get_username());
		case 4:
			return;
		}
	}
}
