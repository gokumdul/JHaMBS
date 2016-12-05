#include <string>
#include <fstream>
#include <vector>
using namespace std;

enum LOGIN_STATUS { LOGIN_FAILED, LOGIN_ADMIN, LOGIN_USER };

// ++ From login.cpp
LOGIN_STATUS login();
void make_new_user_account(bool admin = false);
extern fstream account_file;
// --

// ++ From admin.cpp
void admin_menu();
// --

// ++ From movie.cpp
void book_movie();
// --

// ++ From utils.cpp
unsigned int calc_crc32(const char* contents);
string caesar_cipher(string val, bool encrypt = true, unsigned int swift = 50);
string caesar_cipher(string val, string offset, bool encrypt = true, unsigned int swift = 50);
extern inline char caesar_cipher(char val, bool encrypt = true, unsigned int offset = 0);
bool string_starts_with(string orig, string cmp);
bool string_ends_with(string orig, string cmp);
vector<string> listdir(string path, string starts_with, string ends_with);
int count_digits(int val);
int print_menu(const string title, const string strings[], const int size, const bool prompt = true, const int width = 60);
// template functions cannot be linked together, so put it in header
// T vector to T array
template<typename T>T* vtoa(vector<T> vec) {
	typename vector<T>::iterator it;
	T* array = new T[vec.size()];

	int i = 0;
	for (it = vec.begin(); it != vec.end(); it++)
		array[i++] = *it;

	return array;
}
// --
