#include <string>
using namespace std;

class account {
public:
	// Don't allow username to be more than 20 characters
	static const int username_len = 20;
	// Don't allow email to be more than 60 characters
	static const int email_len = 60;
	// Don't allow security question to be more than 100 characters
	static const int sec_q_len = 100;

private:
	char username[username_len + 1];
	// Password is stored in crc32, it cannot exceed 10 characters
	char password[10 + 1];
	char email[email_len + 1];
	char sec_q[sec_q_len + 1];
	// Security answer is stored in crc32, it cannot exceed 10 characters
	char sec_a[10 + 1];

	bool admin;
	// Updated on each re-attempt on login
	// If this exceeds 3, alert the user upon next login
	// If this exceeds 10, wipe the user data
	int retries;

	void set_password(string password);

public:
	// Empty constructor for reinterpret_cast
	account() {}
	// Constructor with unencrypted username, password and email
	account(string username, string password, string email,
		string sec_q, string sec_a,
		bool is_admin);

	string get_username() const;
	bool is_pw_match(string cmp);
	static bool valid_email(string email);
	string get_email() const;
	bool is_admin() const;
	void set_admin(bool admin);

	// Asks the user for the email, if match, set new password
	void pw_recovery();
	// Save to pass.dat file
	void save_to_pass_dat() const;
	// Check if the username exists in pass.dat.
	// If does, return the location for that specific object within pass.dat.
	//     If get, return the account pointer as well.
	// If not,  return 0.
	static int exists_in_pass_dat(string username, bool get = false, account *user = nullptr);
	// Removes the data in pass.dat
	void remove_data() const;
	static void remove_data(string username);
	static void remove_data(int index, string username = "");
	// Garbage collection : remove empty data from pass.dat
	static void gc();
};
