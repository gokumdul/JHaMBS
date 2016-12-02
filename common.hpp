#include <string>
#include <fstream>
using namespace std;

// ++ From login.cpp
void login();
void make_new_user_account(bool admin = false);
extern fstream account_file;
// --

// ++ From utils.cpp
unsigned int calc_crc32(const char* contents);
string caesar_cipher(string val, bool encrypt = true);
// --
