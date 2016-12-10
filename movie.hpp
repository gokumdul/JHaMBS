#include <string>
#include "common.hpp"
using namespace std;

class movie {
private:
	int id; // Unique identifier
	string title;
	int runtime; // In minutes
	bool IMAX;
	bool threeD; // Variable cannot start with a number :(

public:
	movie(string dat_file);
	int get_id() const;
	string get_title() const;
	int get_runtime() const;
	bool is_IMAX() const;
	bool is_3D() const;
};
