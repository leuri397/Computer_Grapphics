#pragma once
#include <string>
enum Errors {
	OK,
	FILE_ERROR,
	OUT_OF_MEMORY,
	WRONG_FORMAT
};

enum Type {
	CLOSEST = 0,
	BILINEAR = 1,
	LANCZOS3 = 2,
	BC_SPLINE = 3
};
struct Pixel {
	unsigned char R;
	unsigned char G;
	unsigned char B;
};

class Picture {
public:
	Picture();
	~Picture();
	Errors OpenFile(std::string);
	Errors SaveToFile(std::string);
	Errors Scale(int width, int height, float dx, float dy, Type scale);
	void SetBC(double B, double C);
private:
	int height;
	int width;
	bool color;
	Pixel* image;
	double B;
	double C;
	double interpolate(double a, double b, double c, double d, double width, double height);
	double L(double x);
	double k(double x);
	double sinc(double x);
};