#include <string>
#include "common.hpp"
using namespace std;

// Explicitly specify to start from 0
enum HALL_TYPE { SCREEN_2D = 0, SCREEN_3D, SCREEN_IMAX };

class hall {
private:
	// Hall number
	int hall_number;
	// Price of each tickets for this hall
	int price;
	// Screen type; see above
	HALL_TYPE screen_type;
	/*
	 * // Total seats count across horizontal axis
	 * // int x;
	 * // Total seats count across vertical axis
	 * // int y;
	 * These are determined automatically from screen_type
	 */

	/*
	 * timetable[0~23 * 0~59]
	 *
	 * Avoid using 2D-array for easier calculation
	 * Contains the movie id for 00:00 ~ 23:59
	 */
	static const int timetable_x = 24;
	static const int timetable_y = 60;
	int timetable[60 * timetable_x + timetable_y];

	/*
	 * 4D-array : available[start_hr][start_mn][available_x][available_y]
	 * Reserve enough space to have a consistent class size
	 * This is very inefficient,
	 *     but without a proper DB solution, this is the easiest way
	 *
	 *  x *  y : seat(x,y)'s availability on the index'th movie
	 * 20 x 15 : IMAX
	 * 15 x 10 : 3D, 2D
	 */
	static const int available_x = 20;
	static const int available_y = 15;
	bool available[timetable_x][timetable_y][available_x][available_y];

public:
	hall(int hall_number, int price, HALL_TYPE screen_type);
	// Overloaded constructor : create an object from the saved hall_dat file
	hall(string hall_dat_name);

	int get_hall_number() const;
	int get_price() const;
	HALL_TYPE get_screen_type() const;
	string get_screen_type_in_string() const;
	int get_x() const;
	int get_y() const;
	int get_available_seats(int start_hr, int start_mn) const;
	bool check_available_seat(int start_hr, int start_mn, int x, int y) const;
	int show_all_timetable(bool prompt = false,
		bool for_customers = false, int id = 0, int total_offset = 0,
			vector<int> *hall_id_vec  = nullptr,
			vector<int> *start_hr_vec = nullptr,
			vector<int> *start_mn_vec = nullptr) const;
	// Search through all halls and list timetable for a movie
	static void show_movie_timetable(int id, int &hall_id, int &start_hr, int &start_mn);
	void set_available_seat(bool val, int start_hr, int start_mn, int x, int y);
	// Returns false if requested timetable is already taken
	bool set_timetable(movie movie_obj, int hr, int mn);
	// Remove index'th item from timetable
	void remove_item_from_timetable(int index);
	// Reset the whole timetable
	void reset_timetable();
	void save_to_hall_dat() const;
};
