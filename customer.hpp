#include <string>
#include "common.hpp"
using namespace std;

class customer {
private:
	// Store just the user name here, credentials are stored in pass.dat
	// Don't allow username to be more than 20 characters
	static const int username_len = 20;
	char username[username_len + 1];
public:
	// Default constructor for empty initialization
	customer() {};
	customer(string username);
	string get_username() const;
};
