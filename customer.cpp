#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdio>
#include "common.hpp"
using namespace std;

customer::customer(string username) {
	ifstream user_dat;
	user_dat.open("users/user-" + username + ".dat");

	// Try reusing the save data
	if (user_dat.is_open()) {
		user_dat.read(reinterpret_cast<char *>(this), sizeof(customer));
		user_dat.close();
		return;
	}

	// customer object is being newly created, initialize the members
	for (int x = 0; x < timetable_x; x++) {
		for (int y = 0; y < timetable_y; y++) {
			booked_hall_id[x][y] = 0;
			booked_movie_id[x][y] = 0;
		}
	}
	username.copy(this->username, username.size());

	// Create skeleton file
	save_to_user_dat();
}
string customer::get_username() const {
	return string(username);
}
void customer::book(int id, int hall_id, int start_hr, int start_mn) {
	booked_movie_id[start_hr][start_mn] = id;
	booked_hall_id[start_hr][start_mn] = hall_id;
	save_to_user_dat();
}
void customer::save_to_user_dat() const {
	fstream user_dat;
	string user_dat_name = "users/user-" + string(username) + ".dat";
	user_dat.open(user_dat_name, ios::in | ios::out | ios::trunc);

	user_dat.write(reinterpret_cast<const char *>(this), sizeof(customer));
	user_dat.close();
}
void customer::manage_book_data() {
	vector<string> menu_vec;
	vector<bool> to_prompt;

	// Vectors for storing start time of an index
	vector<int> start_hr_vec;
	vector<int> start_mn_vec;

	int total = 0;

	for (int x = 0; x < timetable_x; x++) {
		for (int y = 0; y < timetable_y; y++) {
			if (booked_movie_id[x][y]) {
				movie movie_obj("movies/movie-" + to_string(booked_movie_id[x][y]) + ".dat");

				int tmp = 60 * x + y;
				tmp += movie_obj.get_runtime();
				// Insert 0 at the beginning if it's single digit
				string tmp_1 = to_string(x);
				string tmp_2 = to_string(y);
				string tmp_3 = to_string(tmp / 60);
				string tmp_4 = to_string(tmp % 60);
				if (tmp_1.size() == 1) tmp_1.insert(0, "0");
				if (tmp_2.size() == 1) tmp_2.insert(0, "0");
				if (tmp_3.size() == 1) tmp_3.insert(0, "0");
				if (tmp_4.size() == 1) tmp_4.insert(0, "0");
				start_hr_vec.push_back(x);
				start_mn_vec.push_back(y);
				string final = "From " + tmp_1 + ":" + tmp_2 +
						" to " + tmp_3 + ":" + tmp_4;
				to_prompt.push_back(false);
				menu_vec.push_back(final);
				to_prompt.push_back(true);
				menu_vec.push_back(movie_obj.get_title());
				total++;
			}
		}
	}

	to_prompt.push_back(false);
	menu_vec.push_back("");
	to_prompt.push_back(true);
	menu_vec.push_back("Go back");
	total++;

	string* menu_arr = vtoa(menu_vec);
	int user_choice = print_menu("Delete booked data", menu_arr, menu_vec.size(), true, to_prompt);
	delete menu_arr;

	if (user_choice == total)
		return; // "Go back"

	user_choice--; // Vector starts from 0

	// Clear booked data
	int start_hr = start_hr_vec[user_choice];
	int start_mn = start_mn_vec[user_choice];

	hall tmp("halls/hall-" + to_string(booked_hall_id[start_hr][start_mn]) + ".dat");
	tmp.set_available_seat(true, start_hr_vec[user_choice], start_mn_vec[user_choice],
			seat_x[start_hr][start_mn], seat_y[start_hr][start_mn]);
	 booked_hall_id[start_hr][start_mn] = 0;
	booked_movie_id[start_hr][start_mn] = 0;
}
