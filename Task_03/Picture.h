#pragma once
#include <string>
#include <vector>
enum Result {
	OK,
	FILE_ERROR,
	OUT_OF_MEMORY,
	WRONG_FORMAT
};
enum DitheringType {
	NO_DITHERING = 0,
	ORDERED_8x8 = 1,
	RANDOM = 2,
	FLOYD_STEINBERG = 3,
	JJN = 4,
	SIERRA3 = 5,
	ATKINSON = 6,
	HALFTONE_4x4 = 7
};
class Picture {
public:
	Picture();
	~Picture();
	Result OpenFile(std::string);
	Result SaveToFile(std::string);
	void Dither(DitheringType, unsigned int bits);
	void setcorrection(float);
	void Fill();
private:
	int height;
	int width;
	unsigned char* image;


	float correction;

	double correct(double value);
	double back_correct(double value);
};