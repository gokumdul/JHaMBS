#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include "common.hpp"
#include "movie.hpp"
using namespace std;

movie::movie(string dat_file) {
	ifstream movie_dat;
	movie_dat.open(dat_file);

	// First line : parse title
	getline(movie_dat, title);
	title = title.substr(8); // 8 : "Title : "

	// Second line : parse runtime
	runtime = 0; // Initialize member
	string runtime_string;
	getline(movie_dat, runtime_string);
	runtime_string = runtime_string.substr(11); // 11 : "Run time : "
	istringstream runtime_iss(runtime_string);
	string runtime_word; // Temporarily used for parsing runtime string
	while (runtime_iss >> runtime_word) {
		if (string_ends_with(runtime_word, "h")) {
			runtime += stoi(runtime_word.substr(0, runtime_word.size() - 1)) * 60;
		} else if (string_ends_with(runtime_word, "min")) {
			runtime += stoi(runtime_word.substr(0, runtime_word.size() - 3));
		}
	}

	// Third line : parse screen types
	IMAX = false; // Initialize member
	threeD = false; // Initialize member
	string screens_string;
	getline(movie_dat, screens_string);
	screens_string = screens_string.substr(10); // 10 : "Screens : "
	// Replace ',' with a whitespace for 'screens_iss >> screens_word' to work properly
	replace(screens_string.begin(), screens_string.end(), ',', ' ');
	istringstream screens_iss(screens_string);
	string screens_word; // Temporarily used for parsing screens string
	while (screens_iss >> screens_word) {
		if (screens_word == "IMAX")
			IMAX = true;
		if (screens_word == "3D")
			threeD = true;
	}

	// Clean up
	movie_dat.close();
}

string movie::get_title() const {
	return title;
}
int movie::get_runtime() const {
	return runtime;
}
bool movie::is_IMAX() const {
	return IMAX;
}
bool movie::is_3D() const {
	return threeD;
}

void book_movie() {
	vector<string> movie_dats = listdir("movies", "movie-", ".dat");
	vector<string> movie_titles;
	vector<string>::iterator it_string;

	vector<movie> movie_vec;
	vector<movie>::iterator it_movie;

	for (it_string = movie_dats.begin(); it_string != movie_dats.end(); it_string++)
		movie_vec.push_back(movie("movies/" + *it_string));

	string* movies_array = new string[movie_vec.size() + 1];
	int i = 0;
	for (it_movie = movie_vec.begin(); it_movie != movie_vec.end(); it_movie++) {
		movies_array[i++] = it_movie->get_title();
	}
	movies_array[i++] = "Go back";

	int user_choice = print_menu("Movies now playing", movies_array, i);
	if (user_choice == i)
		return;

	ifstream movie_dat;
	movie_dat.open("movies/" + *((it_string = movie_dats.begin()) + user_choice - 1));

	int lines = count(istreambuf_iterator<char>(movie_dat), istreambuf_iterator<char>(), '\n');
	movie_dat.seekg(0, ios::beg);

	string* strings = new string[lines];
	for (int i = 0; getline(movie_dat >> ws, strings[i]); i++); // ws : absorb whitespace

	// No longer used
	movie_dat.close();

	print_menu("Movie details", strings, lines, false);

	// TODO : Actually book a movie
	exit(0);
}
