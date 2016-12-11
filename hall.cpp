#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include "common.hpp"
using namespace std;

hall::hall(int hall_number, int price, HALL_TYPE screen_type) {
	// hall object is being newly created, initialize the members
	for (int x = 0; x < available_x; x++) {
		for (int y = 0; y < available_y; y++) {
			for (int z = 0; z < available_z; z++) {
				available[x][y][z] = true;
			}
		}
	}
	for (int x = 0; x < timetable_x; x++) {
		for (int y = 0; y < timetable_y; y++) {
			timetable[60 * x + y] = 0;
		}
	}

	this->hall_number = hall_number;
	this->price = price;
	this->screen_type = screen_type;
}
// Overloaded constructor : create an object from the saved hall_dat file
hall::hall(string hall_dat_name) {
	ifstream hall_dat;
	hall_dat.open(hall_dat_name);

	hall_dat.read(reinterpret_cast<char *>(this), sizeof(hall));
	hall_dat.close();
}

int hall::get_hall_number() const {
	return hall_number;
}
int hall::get_price() const {
	return price;
}
HALL_TYPE hall::get_screen_type() const {
	return screen_type;
}
int hall::get_x() const {
	return (screen_type == SCREEN_IMAX ? 25 : 20);
}
int hall::get_y() const {
	return (screen_type == SCREEN_IMAX ? 15 : 10);
}
int hall::get_available_seats(int index) const {
	int ret = 0;

	for (int i = 0; i < get_x(); i++)
		for (int j = 0; j < get_y(); j++)
			if (!available[index][i][j])
				ret++;

	return ret;
}
int hall::show_all_timetable(bool prompt) const {
	vector<string> print;
	vector<bool> to_prompt;
	int movie_id = 0; // Marker for the starting time of a movie
	bool first = true; // First item on the list
	int retval = 0;

	for (int x = 0; x < timetable_x; x++) {
		for (int y = 0; y < timetable_y; y++) {
			if (timetable[60 * x + y] && (movie_id != timetable[60 * x + y])) {
				movie_id = timetable[60 * x + y];
				movie movie_obj("movies/movie-" + to_string(movie_id) + ".dat");

				int tmp = 60 * x + y;
				tmp += movie_obj.get_runtime();

				if (first) {
					first = false;
				} else {
					to_prompt.push_back(false);
					print.push_back(" ");
					to_prompt.push_back(false);
					print.push_back(" ");
				}
				to_prompt.push_back(false);
				print.push_back("From " + to_string(x)        + ":" + to_string(y) +
				                 " to " + to_string(tmp / 60) + ":" + to_string(tmp % 60));
				// When passing to print_menu(), add number to the title only
				to_prompt.push_back(true);
				print.push_back(movie_obj.get_title());
			}
		}
	}

	string menu_title = "Hall " + to_string(get_hall_number()) + "'s timetable";
	if (print.size()) {
		string* print_strings = vtoa(print);
		retval = print_menu(menu_title, print_strings, print.size(), prompt, to_prompt);
		if (retval == print.size())
			retval = 0; // Go back
		delete[] print_strings;
	} else {
		string print_string[] = { "No schedule!" };
		print_menu(menu_title, print_string, 1, false);
	}

	return retval;
}
void hall::set_available_seat(bool val, int index, int x, int y) {
	available[index][x][y] = val;
}
bool hall::set_timetable(movie movie_obj, int hr, int mn) {
	int id = movie_obj.get_id();
	int runtime = movie_obj.get_runtime();

	// Check if the requested timetable is taken first
	for (int i = 60 * hr + mn; i <= (60 * hr + mn + runtime); i++)
		if (timetable[i])
			return false;

	// Check passed, now update the timetable
	for (int i = 60 * hr + mn; i <= (60 * hr + mn + runtime); i++)
		timetable[i] = id;

	return true;
}
void hall::remove_item_from_timetable(int index) {
	int tmp = 0;

	for (int x = 0; x < timetable_x; x++) {
		for (int y = 0; y < timetable_y; y++) {
			if (timetable[60 * x + y]) {
				if (tmp == index) {
					// Mark it free
					timetable[60 * x + y] = 0;
				} else {
					tmp++;
				}
			}
		}
	}
}
void hall::reset_timetable() {
	for (int x = 0; x < timetable_x; x++) {
		for (int y = 0; y < timetable_y; y++) {
			timetable[60 * x + y] = 0;
		}
	}
}
void hall::save_to_hall_dat() const {
	fstream hall_dat;
	string hall_dat_name = "halls/hall-" + to_string(hall_number) + ".dat";
	hall_dat.open(hall_dat_name, ios::in | ios::out);

	if (!hall_dat.is_open()) {
		// hall.dat does not exist
		hall_dat.close();
		hall_dat.open(hall_dat_name, ios::in | ios::out | ios::trunc);
	}

	hall_dat.write(reinterpret_cast<const char *>(this), sizeof(hall));
	hall_dat.close();
}

void create_new_hall() {
	// Generate hall_number automatically following existing ones
	int hall_number = 1;
	vector<string> hall_dats = listdir("halls", "hall-", ".dat");
	hall_number += hall_dats.size();

	cout << endl;

	// Store price
	int price;
	cout << "Please enter price of a ticket for this hall : ";
	cin >> price;
	cout << endl;

	// Store screen type
	int screen_type;
	cout << "1 : 2D" << endl
	     << "2 : 3D" << endl
	     << "3 : IMAX" << endl
	     << "Please enter the type of this hall : ";
	do {
		cin >> screen_type;
		if (screen_type == 1 ||
		    screen_type == 2 ||
		    screen_type == 3)
			break;

		cerr << "Please choose again!" << endl;
	} while(1);
	cout << endl;

	// Create a new object
	hall hall_obj(hall_number, price, static_cast<HALL_TYPE>(screen_type - 1));

	// Save to file
	hall_obj.save_to_hall_dat();
}

void hall_add_movie_timetable(hall &hall_obj) {
	int movie_choice;
	// class movie does not have a default constructor,
	// so use pointer to use it across different scopes
	movie* movie_obj = nullptr;
	do {
		movie_choice = select_movie();
		movie_obj = new movie("movies/movie-" + to_string(movie_choice) + ".dat");

		if (hall_obj.get_screen_type() == SCREEN_3D && !movie_obj->is_3D()) {
			cerr << "Screen is 3D, but the selected movie is not 3D!" << endl;
			continue;
		}

		if (hall_obj.get_screen_type() == SCREEN_IMAX && !movie_obj->is_IMAX()) {
			cerr << "Screen is IMAX, but the selected movie is not IMAX!" << endl;
			continue;
		}

		break;
	} while(1);

	do {
		// Get the movie starting time
		int hr;
		do {
			cout << endl
			     << "Please enter which hour of the day" << endl
			     << "you want this movie to be played at : ";
			cin >> hr;

			if (0 <= hr && hr <= 23)
				break;

			cerr << "Hour has to be in (0 <= hr <= 23) range!" << endl;
		} while(1);

		int mn;
		do {
			cout << endl
			     << "Please enter which minute" << endl
			     << "you want this movie to be played at : ";
			cin >> mn;

			if (0 <= mn && mn <= 59)
				break;

			cerr << "Minute has to be in (0 <= mn <= 59) range!" << endl;
		} while(1);

		// Mark hall::timetable
		if (!hall_obj.set_timetable(*movie_obj, hr, mn)) {
			cerr << "Timetable is overlapping!" << endl
			     << "Please choose another time!" << endl;
		}
		break;
	} while(1);

	// Free pointer
	delete movie_obj;
}

void hall_remove_movie_timetable(hall &hall_obj, bool entirely) {
	if (entirely) {
		hall_obj.reset_timetable();
		return;
	}

	int user_choice;
	user_choice = hall_obj.show_all_timetable(true);

	if (!user_choice)
		return; // Go back

	// index starts from 0, user_choice starts from 1
	hall_obj.remove_item_from_timetable(user_choice - 1);
}
