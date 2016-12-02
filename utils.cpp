#include <string>
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

#define CAESAR_SWIFT 50
string caesar_cipher(string val, bool encrypt) {
	// ASCII 032~126
	string tmp(val);

	for (int i = 0; i < tmp.size(); i++) {
		if (32 <= tmp[i] && tmp[i] <= 126) {
			tmp[i] = (char)
				(
					(
						(int)tmp[i] +
							// if cipher  : +CAESAR_SWIFT
							// if !cipher : -CAESAR_SWIFT, 95 is added to prevent underflow
							(encrypt ? (CAESAR_SWIFT) : (95 - CAESAR_SWIFT))
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
			// cerr << tmp[i] << " : character not representable with ASCII!" << endl;
		}
	}

	return tmp;
}
