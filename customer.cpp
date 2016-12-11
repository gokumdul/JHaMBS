#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdio>
#include "common.hpp"
using namespace std;

customer::customer(string username) {
	username.copy(this->username, username.size());
}
string customer::get_username() const {
	return string(username);
}
