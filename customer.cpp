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
void customer::save_to_user_dat() const {
	fstream user_dat;
	string user_dat_name = "users/user-" + string(username) + ".dat";
	user_dat.open(user_dat_name, ios::in | ios::out | ios::trunc);

	user_dat.write(reinterpret_cast<const char *>(this), sizeof(customer));
	user_dat.close();
}
