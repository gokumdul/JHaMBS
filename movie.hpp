#include <string>
using namespace std;

class movie {
private:
	string title;
	int runtime; // In minutes
	bool IMAX;
	bool threeD; // Variable cannot start with a number :(

public:
	movie(string dat_file);
	string get_title() const;
	int get_runtime() const;
	bool is_IMAX() const;
	bool is_3D() const;
};
