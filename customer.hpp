#include <string>
#include "common.hpp"
using namespace std;

class customer {
private:
	// Store just the user name here, credentials are stored in pass.dat
	// Don't allow username to be more than 20 characters
	static const int username_len = 20;
	char username[username_len + 1];

	static const int timetable_x = 24;
	static const int timetable_y = 60;
	// Stores movie id on its starting time
	int booked_movie_id[timetable_x][timetable_y];
	// Stores hall id on the starting time
	int booked_hall_id[timetable_x][timetable_y];
	// Stores seat X on the starting time
	int seat_x[timetable_x][timetable_y];
	// Stores seat y on the starting time
	int seat_y[timetable_x][timetable_y];

public:
	// Default constructor for empty initialization
	customer() {};
	customer(string username);
	string get_username() const;
	void book(int id, int hall_id, int start_hr, int start_mn);
	void save_to_user_dat() const;
	void manage_book_data();
};
