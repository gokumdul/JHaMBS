#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <iterator>
#include <dirent.h>
#include "common.hpp"
using namespace std;

// From http://www.hackersdelight.org/hdcodetxt/crc.c.txt
unsigned int calc_crc32(const char* contents) {
	int i = 0, j;
	unsigned int byte, crc, mask;

	crc = 0xFFFFFFFF;
	for (i = 0; contents[i] != '\0'; i++) {
		byte = contents[i];
		crc = crc ^ byte;
		for (j = 7; j >= 0; j--) {
			mask = -(crc & 1);
			crc = (crc >> 1) ^ (0xEDB88320 & mask);
		}
	}
	crc = ~crc;

	return crc;
}

// Caesar's cipher encryption
string caesar_cipher(string val, string offset, bool encrypt, unsigned int swift) {
	bool offset_empty = offset.empty(); // Cache offset.empty()
	string tmp(val);

	for (int i = 0; i < tmp.size(); i++)
		tmp[i] = caesar_cipher(tmp[i], encrypt,
			offset_empty ? 0 : (unsigned int)offset[i % offset.size()] + swift);

	return tmp;
}

// Single character Caesar's cipher encryption
// Use inlining for faster execution when used on a loop
inline char caesar_cipher(char val, bool encrypt, unsigned int offset) {
	// Reset offset within the valid range
	offset %= 95;

	// ASCII 032~126
	if (32 <= val && val <= 126) {
		val = (char)
			(
				(
					(int)val +
						// if encrypt  : +offset
						// if !encrypt : -offset, 95 is added to prevent underflow
						(encrypt ? offset : (95 - offset))
					// Subtract 32 now to calculate the remainder correctly
					- 32)
				// Remainder(%) is used to limit the character within the 0~94 range
				% 95
				// Add 32 now as we subtracted it earlier
				// (Change the range from 0~94 to 32~126)
				+ 32
			);
	} else {
		// Skip de/encrypting character not representable with ASCII
		// cerr << val << " : character not representable with ASCII!" << endl;
	}

	return val;
}

bool string_starts_with(string orig, string cmp) {
	if (orig.length() >= cmp.length()) {
		return (0 == orig.compare(0, cmp.length(), cmp));
	} else {
		return false;
	}
}

bool string_ends_with(string orig, string cmp) {
	if (orig.length() >= cmp.length()) {
		return (0 == orig.compare(orig.length() - cmp.length(), cmp.length(), cmp));
	} else {
		return false;
	}
}

// From http://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
vector<string> listdir(string path, string starts_with, string ends_with) {
	DIR *dir;
	struct dirent *ent;
	vector<string> lists;

	if ((dir = opendir(path.c_str())) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			string name(ent->d_name);

			if (string_starts_with(name, starts_with) &&
				string_ends_with(name, ends_with)) {
				lists.push_back(name);
			}
		}
		closedir(dir);
		free(ent);
	} else {
		// Error
	}

	return lists;
}

int count_digits(int val) {
	int ret = 0;

	if (val < 0)
		val *= -1;

	do {
		val /= 10;
		ret++;
	} while (val);

	return ret;
}

/*
 * Usage :
 * string menu[] = { "Login", "New account", "Find password", "Quit" };
 * print_menu("Start", menu, sizeof(menu) / sizeof(string));
 *
 * The 4th argument width is optional, default is 60.
 * The return value is the user's choice.
 */
int print_menu(const string title, const string strings[], const int size, const bool prompt, const int width) {
	// Calculate space for center-aligning title
	bool title_is_even = (title.size() % 2 == 0);
	int title_left_margin  = width / 2 - title.size() / 2 - 1 - !title_is_even;
	int title_right_margin = width / 2 - title.size() / 2 - 1;

	// Print title
	cout << "┌";
	for (int i = 1; i < title_left_margin; i++)
		cout << "─";
	cout << ' ';
	cout << title;
	cout << ' ';
	for (int i = 1; i < title_right_margin; i++)
		cout << "─";
	cout << "┐";
	cout << endl;

	// Print the middle space
	cout << "│";
	for (int i = 1; i < width - 1; i++)
		cout << ' ';
	cout << "│";
	cout << endl;

	// Calculate space for center-aligning all items
	int largest = 0;
	for (int i = 0; i < size; i++) {
		if (strings[i].size() > largest)
			largest = strings[i].size();
	}
	if (prompt)
		largest += count_digits(size) + 2; // 2 : ". "

	// Print items
	int number = 0;
	for (int i = 0; i < size; i++) {
		int j, k;
		cout << "│";
		for (j = 0; j < width / 2 - largest / 2 - 1; j++)
			cout << ' ';
		if (prompt)
			cout << ++number << ". ";
		cout << strings[i];
		for (k = 0; k < width - 2 - j - (prompt ? 3 : 0) - strings[i].size(); k++)
			cout << ' ';
		cout << "│";
		cout << endl;
	}

	// Print the middle space
	cout << "│";
	for (int i = 1; i < width - 1; i++)
		cout << ' ';
	cout << "│";
	cout << endl;

	// Print the last line
	cout << "└";
	for (int i = 1; i < width - 1; i++)
		cout << "─";
	cout << "┘";
	cout << endl;

	int choice = 0;
	if (prompt) {
		while (1) {
			// Now ask the user
			cout << "Please choose : ";
			cin >> choice;

			// Clear the buffer for next getline()
			cin.ignore();

			if (1 <= choice && choice <= number)
				return choice;
			else
				cerr << "Please enter a valid value!" << endl;
		}
	}
	return 0;
}
