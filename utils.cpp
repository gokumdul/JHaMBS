#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
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

// Regular Caesar's cipher encryption
string caesar_cipher(string val, bool encrypt, unsigned int swift) {
	string tmp(val);

	for (int i = 0; i < tmp.size(); i++)
		tmp[i] = caesar_cipher(tmp[i], encrypt, swift);

	return tmp;
}

// 2-stage Caesar's cipher encryption with the offset for the 2nd round
string caesar_cipher(string val, string offset, bool encrypt, unsigned int swift) {
	string tmp(val);

	for (int i = 0; i < tmp.size(); i++)
		tmp[i] = caesar_cipher(tmp[i], encrypt, (unsigned int)offset[i % offset.size()] + swift);

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
