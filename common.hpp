#ifndef __COMMON_HPP
#define __COMMON_HPP

#include <string>
#include <fstream>
#include <vector>
#include "account.hpp"
#include "customer.hpp"
#include "movie.hpp"
#include "hall.hpp"
using namespace std;

enum LOGIN_STATUS { LOGIN_FAILED, LOGIN_ADMIN, LOGIN_USER };

// ++ From account.cpp
LOGIN_STATUS login();
void make_new_user_account(bool admin = false);
void reset_password(string username = "");
void reset_email(string username = "");
extern customer logged_in_session;
// --

// ++ From admin.cpp
void admin_menu();
// --

// ++ From user.cpp
void user_menu();
// --

// ++ From movie.cpp
void book_movie();
int select_movie();
// --

// ++ From hall.cpp
void create_new_hall();
void hall_add_movie_timetable(hall &hall_obj);
void hall_remove_movie_timetable(hall &hall_obj, bool entirely = false);
// --

// ++ From utils.cpp
unsigned int calc_crc32(const char* contents);
string caesar_cipher(string val, string offset = "", bool encrypt = true, unsigned int swift = 50);
extern inline char caesar_cipher(char val, bool encrypt = true, unsigned int offset = 0);
bool string_starts_with(string orig, string cmp);
bool string_ends_with(string orig, string cmp);
vector<string> listdir(string path, string starts_with, string ends_with);
int count_digits(int val);
int print_menu(const string title, const string strings[], const int size,
	const bool prompt = true, const vector<bool> to_prompt = vector<bool>(), const int width = 60);
// template functions cannot be linked together, so put it in header
// T vector to T array
template<typename T>T* vtoa(vector<T> vec, int reserve = 0) {
	typename vector<T>::iterator it;
	T* array = new T[vec.size() + reserve];

	int i = 0;
	for (it = vec.begin(); it != vec.end(); it++)
		array[i++] = *it;

	return array;
}
extern bool should_clear;
void cls();
// --

#endif // __COMMON_HPP
