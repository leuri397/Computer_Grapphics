#pragma once
#include <string>
enum Errors {
	OK,
	FILE_ERROR,
	OUT_OF_MEMORY,
	WRONG_FORMAT
};
class Picture {
public:
	Picture();
	~Picture();
	Errors OpenFile(std::string);
	Errors SaveToFile(std::string);
	void Invert();
	void Flip(bool);
	Errors Rotate(bool);
private:
	int height;
	int width;
	bool color;
	unsigned char* image;
	int bytes;
	int maxvalue;
	void swap(unsigned char&, unsigned char&);
};