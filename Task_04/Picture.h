#pragma once
#include <string>
#include <vector>
struct Pixel {
	unsigned char A;
	unsigned char B;
	unsigned char C;
};
enum ColorSpace {
	RGB,
	HSL,
	HSV,
	YCbCr_601,
	YCbCr_709,
	YCoCg,
	CMY
};
class Picture {
public:
	Picture();
	~Picture();
	void SetCS(ColorSpace);
	void OpenSingleFile(std::string);
	void OpenMultipleFile(std::string);
	void SaveToSingleFile(std::string);
	void SaveToMultipleFile(std::string);
	void convertANY(ColorSpace);
private:
	int height;
	int width;
	Pixel* image;
	ColorSpace currentCS;
	void convertRGB();
};
