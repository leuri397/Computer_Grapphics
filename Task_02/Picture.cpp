#include "Picture.h"
#include <fstream>
#include <cmath>
#include <string>
using namespace std;
#define MAX_VALUE 255

Picture::Picture()
{
	height = 0;
	width = 0;
	image = nullptr;
	correction = -1;
}

Result Picture::OpenFile(string filename)
{
	ifstream input;
	input.open(filename, ios_base::in | ios_base::binary);
	if (!input.is_open())
		return Result::FILE_ERROR;
	string header;
	input >> header;
	if (header != "P5")
	{
			return Result::WRONG_FORMAT;
	}
	input >> width >> height;
	int maxvalue;
	input >> maxvalue;
	if (!input.good())
		return Result::FILE_ERROR;
	if (maxvalue != MAX_VALUE)
		return Result::WRONG_FORMAT;
	char* image_char;
	image = new unsigned char[width * height];
	if (image == NULL)
		return OUT_OF_MEMORY;
	image_char = (char*)image;
	input.read(image_char, 1);
	input.read(image_char,  width * height);
	if (!input.good())
		return FILE_ERROR;
	input.close();
	return Result::OK;
}

Result Picture::SaveToFile(std::string filename)
{
	ofstream output;
	output.open(filename, ios_base::out | ios_base::binary);
	if (!output.is_open())
		return Result::FILE_ERROR;
	output << "P5" << '\n';
	output << width << ' ' << height << '\n' << MAX_VALUE << '\n';
	char* image_char = (char*)image;
	output.write(image_char, width * height);
	if (output.fail())
		return FILE_ERROR;
	output.close();
	return Result::OK;
}

void Picture::setcorrection(float input)
{
	correction = input;
}

void Picture::line(Line NewLine)
{
	int position_x;
	int position_y;
	this->line_intencity = NewLine.brightness;
	if (NewLine.x_start > NewLine.x_finish)
		swap(NewLine.x_start, NewLine.x_finish);
	if (NewLine.y_start > NewLine.y_finish)
		swap(NewLine.y_start, NewLine.y_finish);
	if (NewLine.is_horisontal())
	{
		this->direction = true;

	}
	else if (NewLine.is_vertical())
	{
		this->direction = false;
		swap(NewLine.x_start, NewLine.y_start);
		swap(NewLine.x_finish, NewLine.y_finish);
	}
	if (NewLine.is_horisontal() || NewLine.is_vertical())
	{
	for (position_x = NewLine.x_start; position_x < NewLine.x_finish; position_x++) {
				double y = NewLine.y_start + NewLine.thickness / 2;
				position_y = floor(y);
				double edge = y - position_y;
					if (edge > 0.5)
					{
						edge -= 0.5;
						position_y++;
					}
					else if (edge < 0.5)
						edge += 0.5;
					else
					{
						edge = 1;
					}
					if ((y - edge) > NewLine.y_start - NewLine.thickness/2)
						plot(position_x, position_y, edge);
					else
						plot(position_x, position_y, y - NewLine.y_start + NewLine.thickness/2);
					position_y--;
					y -= edge;
					while (y - (NewLine.y_start - NewLine.thickness / 2) > 1)
					{
						plot(position_x, position_y, 1);
						position_y--;
						y--;
					}
					if (y > NewLine.y_start - NewLine.thickness / 2)
						plot(position_x, position_y, y - (NewLine.y_start - NewLine.thickness / 2));
		}
		circle(NewLine.x_start, NewLine.y_start, NewLine.thickness / 2 - 1);
		circle(NewLine.x_finish, NewLine.y_finish, NewLine.thickness / 2 - 1);
		return;
	}
	if (NewLine.left_to_right())
		direction = true;
	else if (NewLine.top_to_bottom())
	{
		direction = false;
		swap(NewLine.x_start, NewLine.y_start);
		swap(NewLine.x_finish, NewLine.y_finish);
	}
	if (NewLine.left_to_right() || NewLine.top_to_bottom())
	{
		for (position_x = NewLine.x_start; position_x <= NewLine.x_finish; position_x++)
		{
			double current_coeff = ((NewLine.y_finish - NewLine.y_start) / (NewLine.x_finish - NewLine.x_start)) * (position_x - NewLine.x_start) + NewLine.y_start;
			double y =  current_coeff + NewLine.thickness / 2;
			position_y = floor(y);
			double edge = y - position_y;
			if (edge > 0.5)
			{
				edge -= 0.5;
				position_y++;
			}
			else if (edge < 0.5)
				edge += 0.5;
			else
			{
				edge = 1;
			}
			if ((y - edge) > current_coeff - NewLine.thickness / 2)
				plot(position_x, position_y, edge);
			else
				plot(position_x, position_y, y - current_coeff + NewLine.thickness / 2);
			position_y--;
			y -= edge;
			while (y - (current_coeff - NewLine.thickness / 2) > 1)
			{
				plot(position_x, position_y, 1);
				position_y--;
				y--;
			}
			if (y > current_coeff - NewLine.thickness / 2)
				plot(position_x, position_y, y - (current_coeff - NewLine.thickness / 2));
		}
		circle(NewLine.x_start, NewLine.y_start, NewLine.thickness/2 - 1);
		circle(NewLine.x_finish, NewLine.y_finish, NewLine.thickness/2 - 1);
		return;
	}
}

void Picture::plot(int x, int y, double brightness)
{
	if ((((y < height) && direction) || ((x < height) && !direction)) && (((x < height) && direction) || ((y < height) && !direction)) && (x >= 0) && (y >= 0))
	{
			if (brightness > 1.0 - 1 / 255)
				brightness = 1;
			if (this->direction) {
				image[y * width + x] = correct(back_correct(image[y * width + x]) * (1 - brightness) + brightness * line_intencity);
			}
			else
			{
					image[x * width + y] = correct(back_correct(image[x * width + y]) * (1 - brightness) + brightness * line_intencity);
			}
	}
}

double Picture::correct(double value)
{
	value = value / 255;
	if (correction == -1)
	{
		if (value < 0.0031308)
			return  value * 12.92 * 255;
		else
			return 255 * ((211.0 * pow(value, 0.4166) - 11.0) / 200.0);
	}
	else {
		return 255 * pow(value, correction);
	}
}

double Picture::back_correct(double value)
{
	value = value / 255;
	if (correction == -1)
	{
		if (value < 0.04045)
			return  255 * value * 12.92;
		else
			return 255 * (pow((200.0 * value + 11.0) / 211.0, 2.4));
	}
	else {
		return 255 * pow(value, 1/correction);
	}
}


void Picture::circle(int x, int y, double radius){
	int x_position, y_position = y + radius;
	for (x_position = x; x_position < x + radius; x_position++)
	{
		y_position = y - sqrt(pow(radius, 2) - pow(x - x_position ,2));
		for (int posy = y_position; posy < 2 * y - y_position; posy++)
		{
			plot(x_position, posy, 1);
			plot(2 * x - x_position, posy, 1);
		}
	}
}

Picture::~Picture()
{
	delete[] image;
}

Line::Line(float _x_start, float _y_start, float _x_finish, float _y_finish, float _thickness, int _brightness)
{
	x_start = _x_start;
	y_start = _y_start;
	x_finish = _x_finish;
	y_finish = _y_finish;
	thickness = _thickness;
	brightness = _brightness;
}

bool Line::is_vertical()
{
	if (this->x_start == this->x_finish)
		return true;
	else
		return false;
}

bool Line::is_horisontal()
{
	if (this->y_start == this->y_finish)
		return true;
	else
		return false;
}

bool Line::left_to_right()
{
	if (abs(x_start - x_finish) >= abs(y_start - y_finish))
		return true;
	else
		return false;
}

bool Line::top_to_bottom()
{
	if (abs(x_start - x_finish) < abs(y_start - y_finish))
		return true;
	else
		return false;
}