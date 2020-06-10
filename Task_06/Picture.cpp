#include "Picture.h"
#include <fstream>
#include <cmath>
#include <string>
using namespace std;

Picture::Picture() : height(0), width(0), color(true), image(nullptr), B(0), C(0.5)
{}

Errors Picture::OpenFile(string filename)
{
	ifstream input;
	input.open(filename, ios_base::in | ios_base::binary);
	if (!input.is_open())
		return Errors::FILE_ERROR;
	string header;
	input >> header;
	if (header == "P5")
	{
		color = false;
	}
	else
	{
		if (header == "P6")
		{
			color = true;
		}
		else
			return Errors::WRONG_FORMAT;
	}
	input >> width >> height;
	int maxvalue;
	input >> maxvalue;
	if (!input.good())
		return Errors::FILE_ERROR;
	char* image_char;
	if (color)
	{
		image = new Pixel[width * height * 3];
		if (image == NULL)
			return Errors::OUT_OF_MEMORY;
		image_char = (char*)image;
		input.read(image_char, 1);
		for (int i = 0; i < width * height; i++)
		{
			image_char = (char*)&image[i].R;
			input.read(image_char, 1);
			image_char = (char*)&image[i].G;
			input.read(image_char,1);
			image_char = (char*)&image[i].B;
			input.read(image_char, 1);
		}
	}
	else
	{
		image = new Pixel[width * height];
		if (image == NULL)
			return OUT_OF_MEMORY;
		image_char = (char*)image;
		input.read(image_char, 1);
		for (int i = 0; i < width * height; i++)
		{
			image_char = (char*)&image[i].R;
			input.read(image_char, 1);
			image[i].G = image[i].R;
			image[i].B = image[i].R;
		}
	}
	if (!input.good())
		return FILE_ERROR;
	input.close();
	return Errors::OK;
}

double Picture::sinc(double x)
{
			if (x == 0)
				return 1;
			return sin(x) / x;
}

Errors Picture::SaveToFile(std::string filename)
{
	ofstream output;
	output.open(filename, ios_base::out | ios_base::binary);
	if (!output.is_open())
		return Errors::FILE_ERROR;
	if (color)
	{
		output << "P6" << '\n';
	}
	else
	{
		output << "P5" << '\n';
	}
	output << width << ' ' << height << '\n' << 255 << '\n';
	char* image_char = (char*)&image;
	if (color)
	{
		for (int i = 0; i < width * height; i++)
		{
			output.write((char*)&image[i].R, 1);
			output.write((char*)&image[i].G, 1);
			output.write((char*)&image[i].B, 1);
		}
	}
	else
	{
		for (int i = 0; i < width * height; i++)
		{
			output.write((char*)&image[i].R, 1);
		}
	}
	if (output.fail())
		return FILE_ERROR;
	output.close();
	return Errors::OK;
}

Errors Picture::Scale(int width_, int height_, float dx, float dy, Type scale)
{
	double x_coeff, y_coeff, xDiff, yDiff;
	int old_x, old_y;
	int R, G, B;
	Pixel* buffer;
	x_coeff = (double)(width)/ (width_);
	y_coeff = (double)(height)/ (height_);
	buffer = new Pixel[width_ * height_];
	if (buffer == NULL)
		return Errors::OUT_OF_MEMORY;
	Pixel buffer_single;
	fill((char*)&buffer[0], (char*)&buffer[width_ * height_], 0);
	switch (scale)
	{
	case CLOSEST:
		for (int x = 0; x < width_; x++)
			for (int y = 0; y < height_; y++)
			{
				if ((x + dx >= width_)||(x + dx < 0))
					continue;
				if ((y + dy >= height_)||(y + dy < 0))
					continue;
					buffer[(int)round((y + dy) * width_ + x + dx)].R = image[(int)round(y * y_coeff) * width + (int)round(x * x_coeff)].R;
					buffer[(int)round((y + dy) * width_ + x + dx)].G = image[(int)round(y * y_coeff) * width + (int)round(x * x_coeff)].G;
					buffer[(int)round((y + dy) * width_ + x + dx)].B = image[(int)round(y * y_coeff) * width + (int)round(x * x_coeff)].B;
			}
		break;

	case BILINEAR:
		for (int x = 0; x < width_; x++)
			for (int y = 0; y < height_; y++)
			{
				old_x = floor(x * x_coeff );
				old_y = floor(y * y_coeff );
				xDiff = x * x_coeff - old_x;
				yDiff = y * y_coeff - old_y;
				if ((old_x >= 0)&&(old_y >= 0)&&(old_x < width)&&(old_y < height))
				{
					if ((y + dy >= height_) || (y + dy < 0) || (x + dx >= width_) || (x + dx < 0))
						continue;
					buffer[(int)round((y + dy)) * width_ + x + (int)round(dx)].R 
						= interpolate(image[old_y*width + old_x].R,
							 old_x + 1 >= width ? 0 : image[old_y * width + old_x + 1].R,
							old_y + 1 >= height ? 0 : image[(old_y + 1) * width + old_x].R,
							old_y + 1 >= height || old_x + 1 >= width? 0 : image[(old_y + 1) * width + old_x + 1].R,
							xDiff, yDiff);
					buffer[(int)round((y + dy)) * width_ + x + (int)round(dx)].G
						= interpolate(image[old_y * width + old_x].G,
							old_x + 1 >= width ? 0 : image[old_y * width + old_x + 1].G,
							old_y + 1 >= height ? 0 : image[(old_y + 1) * width + old_x].G,
							old_y + 1 >= height || old_x + 1 >= width ? 0: image[(old_y + 1) * width + old_x + 1].G,
							xDiff, yDiff);
					buffer[(int)round((y + dy)) * width_ + x + (int)round(dx)].B
						= interpolate(image[old_y * width + old_x].B,
							old_x + 1 >= width ? 0 : image[old_y * width + old_x + 1].B,
							old_y + 1 >= height ? 0 : image[(old_y + 1) * width + old_x].B,
							old_y + 1 >= height || old_x + 1 >= width ? 0 : image[(old_y + 1) * width + old_x + 1].B,
							xDiff, yDiff);
				}
			}
		break;
	case LANCZOS3:
		for (int x = 0; x < width_; x++) {
			for (int y = 0; y < height_; y++)
			{
				int a = 3;
				double R = 0, G = 0, B = 0, Ry = 0, Gy = 0, By = 0;
				for (int i = y*y_coeff - a + 1; i < y*y_coeff + a; i++)
				{
					for (int j = x*x_coeff - a + 1; j < x*x_coeff + a; j++)
					{
						double bx = j, by = i;
						if (bx < 0)
							continue;
						if (by < 0)
							continue;
						if (bx >= width)
							continue;
						if (by >= height)
							continue;
						int index = (by)* width + bx;
						double Lx = L(x * x_coeff - bx);
						double Ly = L(y * y_coeff - by);
							Ry += image[index].R * Lx * Ly;
							Gy += image[index].G * Lx * Ly;
							By += image[index].B * Lx * Ly;
					}
				}
				if ((x + dx >= width_) || (x + dx < 0))
					continue;
				if ((y + dy >= height_) || (y + dy < 0))
					continue;
				if (Ry < 0)
					Ry = 0;
				if (Gy < 0)
					Gy = 0;
				if (By < 0)
					By = 0;
				if (Ry > 255)
					Ry = 255;
				if (Gy > 255)
					Gy = 255;
				if (By > 255)
					By = 255;
				buffer[(int)round((y + dy) * width_ + x + dx)].R = Ry;
				buffer[(int)round((y + dy) * width_ + x + dx)].G = Gy;
				buffer[(int)round((y + dy) * width_ + x + dx)].B = By;
			}
		}
		break;
	case BC_SPLINE:
		for (double y = 0; y < height_; y+=1/y_coeff) {
			for (int x = 0; x < width_; x++) {
				double R = 0, G = 0, B = 0, Ry = 0, Gy = 0, By = 0;
				for (int i = 0; i < width; i++)
				{
					double kx = k(x * x_coeff - i);
					if (kx == 0)
						continue;
					Ry += image[(int)round(y * y_coeff) * width + i].R * kx;
					Gy += image[(int)round(y * y_coeff) * width + i].G * kx;
					By += image[(int)round(y * y_coeff) * width + i].B * kx;
				}
				if ((x + dx >= width_) || (x + dx < 0))
					continue;
				if ((y + dy >= height_) || (y + dy < 0))
					continue;
				if (Ry < 0)
					Ry = 0;
				if (Gy < 0)
					Gy = 0;
				if (By < 0)
					By = 0;
				if (Ry > 255)
					Ry = 255;
				if (Gy > 255)
					Gy = 255;
				if (By > 255)
					By = 255;
				buffer[(int)round((floor(y) + dy) * width_ + x + dx)].R = Ry;
				buffer[(int)round((floor(y) + dy) * width_ + x + dx)].G = Gy;
				buffer[(int)round((floor(y) + dy) * width_ + x + dx)].B = By;
			}
		}

		Pixel* second_buffer;
		second_buffer = new Pixel[ height_ * width_ ];
		if (second_buffer == NULL)
			return Errors::OUT_OF_MEMORY;
		fill((char*)&second_buffer[0], (char*)&second_buffer[width_ * height_], 0);

		for (int x = 0; x < width_; x++) {
			for (int y = 0; y < height_; y++)
			{
				double R = 0, G = 0, B = 0, Ry = 0, Gy = 0, By = 0;
				for (double i = 0; i < height_; i+=1/y_coeff)
				{
					double ky = k((y - i)*y_coeff);
					if (ky == 0)
						continue;
					if ((x + dx >= width_) || (x + dx < 0))
						continue;
					if ((i + dy >= height_) || (i + dy < 0))
						continue;
					Ry += buffer[(int)round((floor(i) + dy) * width_) + (int)round(x + dx)].R * ky;
					Gy += buffer[(int)round((floor(i) + dy) * width_) + (int)round(x + dx)].G * ky;
					By += buffer[(int)round((floor(i) + dy) * width_) + (int)round(x + dx)].B * ky;
				}
				if ((x + dx >= width_) || (x + dx < 0))
					continue;
				if ((y + dy >= height_) || (y + dy < 0))
					continue;
				if (Ry < 0)
					Ry = 0;
				if (Gy < 0)
					Gy = 0;
				if (By < 0)
					By = 0;
				if (Ry > 255)
					Ry = 255;
				if (Gy > 255)
					Gy = 255;
				if (By > 255)
					By = 255;
				second_buffer[(int)round((y + dy) * width_) + (int)round(x + dx)].R = Ry;
				second_buffer[(int)round((y + dy) * width_) + (int)round(x + dx)].G = Gy;
				second_buffer[(int)round((y + dy) * width_) + (int)round(x + dx)].B = By;
			}
		}
		delete[] buffer;
		buffer = second_buffer;
		break;
	}
	delete[] image;
	image = buffer;
	width = width_;
	height = height_;
	return Errors::OK;
}

void Picture::SetBC(double B_, double C_)
{
	B = B_;
	C = C_;
}

double Picture::interpolate(double a, double b, double c, double d, double width_, double height_)
{
	double r = a * (1.0 - width_)* (1.0 - height_)
		+ b * width_ * (1.0 - height_)
		+ c * (1.0 - width_) * height_
		+ d * width_ * height_;
	if (r < 0)
		return 0;
	if (r > 255)
		return 255;
	return r;
}

double Picture::L(double x)
{
	double answer;
	if (x == 0)
		return 1;
	if ((x > 3)||(x < -3))
		return 0;
	answer = (3*sin(3.14*x)*sin(3.14*x/3))/(3.14*3.14*x*x);
	return answer;
}

double Picture::k(double x)
{
	x = abs(x);
	if (x < 1)
		return (1.0 / 6.0) * ((12 - 9*B - 6*C)*pow(x,3) + (-18 + 12*B + 6*C)*x*x + (6 - 2*B));
	if ((x >= 1) && (x < 2))
		return (1.0 / 6.0) * ((-B - 6*C)*pow(x, 3) + (6*B + 30*C)*x*x + (-12*B - 48*C)*x + (8*B + 24*C));
	return 0.0;
}




Picture::~Picture()
{
	delete[] image;
}