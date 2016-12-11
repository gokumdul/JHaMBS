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
	 * 3D-array : available[index][x][y]
	 * Reserve enough space to have a consistent class size
	 *
	 * index   : index'th movie of the day,
	 *           allow max 12 movies to be played in one day.
	 *           Note that this is not the movie id.
	 *  x *  y : seat(x,y)'s availability on the index'th movie
	 * 20 x 15 : IMAX
	 * 15 x 10 : 3D, 2D
	 */
	static const int available_x = 12;
	static const int available_y = 20;
	static const int available_z = 15;
	bool available[available_x][available_y][available_z];
	/*
	 * timetable[0~23 * 0~59]
	 *
	 * Avoid using 2D-array for easier calculation
	 * Contains the movie id for 00:00 ~ 23:59
	 */
	static const int timetable_x = 24;
	static const int timetable_y = 60;
	int timetable[60 * timetable_x + timetable_y];

public:
	hall(int hall_number, int price, HALL_TYPE screen_type);
	// Overloaded constructor : create an object from the saved hall_dat file
	hall(string hall_dat_name);

	int get_hall_number() const;
	int get_price() const;
	HALL_TYPE get_screen_type() const;
	int get_x() const;
	int get_y() const;
	int get_available_seats(int index) const;
	void show_all_timetable() const;
	void set_available_seat(bool val, int index, int x, int y);
	// Returns false if requested timetable is already taken
	bool set_timetable(movie movie_obj, int hr, int mn);
	void save_to_hall_dat() const;
};
