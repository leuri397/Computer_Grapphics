#pragma once
#pragma once
#include <string>
enum Result {
	OK,
	FILE_ERROR,
	OUT_OF_MEMORY,
	WRONG_FORMAT
};
class Line {
public:
	Line(float _x_start, float _y_start, float _x_finish, float _y_finish, float _thickness, int brightness);
	float x_start;
	float x_finish;
	float y_start;
	float y_finish;
	bool is_vertical();
	bool is_horisontal();
	bool top_to_bottom();
	bool left_to_right();
	float thickness;
	int brightness;
};
class Picture {
public:
	Picture();
	~Picture();
	Result OpenFile(std::string);
	Result SaveToFile(std::string);
	void setcorrection(float);
	void line(Line);
	void circle(int x, int y, double radius);
private:
	int height;
	int width;
	unsigned char* image;
	float correction;
	bool direction;
	unsigned char line_intencity;
	void plot(int x, int y, double brightness);
	double correct(double value);
	double back_correct(double value);
};