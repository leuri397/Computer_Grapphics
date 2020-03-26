#include "Picture.h"
#include <fstream>
#include <cmath>
#include <string>
using namespace std;
#define MAX_VALUE 255

const double ordered_matrix[8][8] ={
	{0.0 / 64.0, 48.0 / 64.0, 12.0 / 64.0, 60.0 / 64.0, 3.0 / 64.0, 51.0 / 64.0, 15.0 / 64.0, 63.0 / 64.0},
	{32.0 / 64.0, 16.0 / 64.0, 44.0 / 64.0, 28.0 / 64.0, 35.0 / 64.0, 19.0 / 64.0, 47.0 / 64.0, 31.0 / 64.0},
	{8.0 / 64.0, 56.0 / 64.0, 4.0 / 64.0, 52.0 / 64.0, 11.0 / 64.0, 59.0 / 64.0, 7.0 / 64.0, 55.0 / 64.0},
	{40.0 / 64.0, 24.0 / 64.0, 36.0 / 64.0, 20.0 / 64.0, 43.0 / 64.0, 27.0 / 64.0, 39.0 / 64.0, 23.0 / 64.0},
	{2.0 / 64.0, 50.0 / 64.0, 14.0 / 64.0, 62.0 / 64.0, 1.0 / 64.0, 49.0 / 64.0, 13.0 / 64.0, 61.0 / 64.0},
	{34.0 / 64.0, 18.0 / 64.0, 46.0 / 64.0, 30.0 / 64.0, 33.0 / 64.0, 17.0 / 64.0, 45.0 / 64.0, 29.0 / 64.0},
	{10.0 / 64.0, 58.0 / 64.0, 6.0 / 64.0, 54.0 / 64.0, 9.0 / 64.0, 57.0 / 64.0, 5.0 / 64.0, 53.0 / 64.0},
	{42.0 / 64.0, 26.0 / 64.0, 38.0 / 64.0, 22.0 / 64.0, 41.0 / 64.0, 25.0 / 64.0, 37.0 / 64.0, 21.0 / 64.0},
};
const double halftone_matrix[4][4] = {
	{13.0 / 16.0, 11.0 / 16.0, 4.0 / 16.0, 8.0 / 16.0},
	{6.0 / 16.0, 0, 3.0 / 16.0, 15.0 / 16.0},
	{14.0 / 16.0, 1.0 / 16.0, 2.0 / 16.0, 7.0 / 16.0},
	{9.0 / 16.0, 5.0 / 16.0, 10.0 / 16.0, 12.0 / 16.0},
};
const double JJN_matrix[3][5] = { {0, 0, 0, 7, 5}, {3, 5, 7, 5, 3}, {1, 3, 5, 3, 1} };
const double SIERRA3_matrix[3][5] = { {0, 0, 0, 5, 3}, {2, 4, 5, 4, 2}, {0, 2, 3, 2, 0} };
const double ATKINSON_matrix[3][5] = { {0, 0, 0, 1, 1},{0, 1, 1, 1, 0},{0, 0, 1, 0, 0} };

Picture::Picture() : height(0), width(0), image(nullptr), correction(0)
{}

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
	input.read(image_char, width * height);
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

void Picture::Dither(DitheringType Dither, unsigned int bits)
{
	int value = pow(2, bits);
	double rate_matrix[3][5];
	double buffer;
	double compare, current_error;
	vector<double> error;
	switch (Dither) {
	case NO_DITHERING:
		for (int i = 0; i < width * height; i++)
		{
			buffer = back_correct(image[i]) / (double)255;
			buffer *= value - 1;
			buffer = round(buffer);
			image[i] = round(correct(buffer * (255 / (value - 1))));
		}
		break;
	case ORDERED_8x8:
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				buffer = (back_correct(image[i * width + j]) + (255/(bits))*(ordered_matrix[i % 8][j % 8] - 0.75))/255;
				if (buffer < 0)
					buffer = 0;
				buffer *= value;
				buffer = round(buffer);
				image[i * width + j] = round(correct(buffer * (255 / (value - 1))));
			}
		}
		break;
	case RANDOM:
		srand(666);
		for (int i = 0; i < width * height; i++)
		{
			buffer = (back_correct(image[i]) + (255 / (bits))*((double)rand()/32767.0 - 0.75))/ (double)255;
			if (buffer < 0)
				buffer = 0;
			buffer *= value;
			buffer = round(buffer);
			image[i] = round(correct(buffer * (255 / (value - 1))));
		}
		break;
	case FLOYD_STEINBERG:

		error.resize(width*height, 0);
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				buffer = (back_correct(image[i * width + j] ) + error[i * width + j]) / (double)255;
				buffer *= (value - 1);
				buffer = round(buffer);
				buffer *= 255 / (value - 1);
				current_error = image[i * width + j] + error[i * width + j] - buffer;
				image[i * width + j] = buffer;
				if (j + 1 < width)
					error[i * width + j + 1] += current_error*(7.0/16.0);
				if (i + 1 < height)
				{
					if (j + 1 < width) {
						error[(i + 1) * width + j + 1] += current_error * (1.0 / 16.0);
					}
					error[(i + 1) * width + j] += current_error * (5.0 / 16.0);
					if ((i - 1 > 0)&&(j - 1 > 0))
						error[(i - 1) * width + j - 1] += current_error * (3.0 / 16.0);
				}
			}
		}
		break;
	case JJN:

		error.resize(width * height, 0);
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				buffer = (back_correct(image[i * width + j]) + error[i * width + j]) / (double)255;
				buffer *= (value - 1);
				buffer = round(buffer);
				buffer *= 255 / (value - 1);
				current_error = image[i * width + j] + error[i * width + j] - buffer;
				image[i * width + j] = buffer;
				for (int k = 0; k <= 2; k++)
					{
						for (int v = -2; v <= 2; v++)
						{
							if (i + k < height) {
								if ((k == 0) && (v > 0))
								{
									if (j + v < width)
									error[(i + k) * width + j + v] += current_error * JJN_matrix[k][2 + v] / 48.0;
								}
								else
								{
									if ((j + v < width) && (j + v > 0))
									{
										error[(i + k) * width + j + v] += current_error * JJN_matrix[k][2 + v] / 48.0;
									}
								}
							}
						}
					}
			}
		}
		break;
	case SIERRA3:

		error.resize(width * height, 0);
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				buffer = (back_correct(image[i * width + j]) + error[i * width + j]) / (double)255;
				buffer *= (value - 1);
				buffer = round(buffer);
				buffer *= 255 / (value - 1);
				current_error = image[i * width + j] + error[i * width + j] - buffer;
				image[i * width + j] = buffer;
				for (int k = 0; k <= 2; k++)
				{
					for (int v = -2; v <= 2; v++)
					{
						if (i + k < height) {
							if ((k == 0) && (v > 0))
							{
								if (j + v < width)
									error[(i + k) * width + j + v] += current_error * SIERRA3_matrix[k][2 + v] / 32.0;
							}
							else
							{
								if ((j + v < width) && (j + v > 0))
								{
									error[(i + k) * width + j + v] += current_error * SIERRA3_matrix[k][2 + v] / 32.0;
								}
							}
						}
					}
				}
			}
		}
		break;
	case ATKINSON:

		error.resize(width * height, 0);
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				buffer = (back_correct(image[i * width + j]) + error[i * width + j]) / (double)255;
				buffer *= (value - 1);
				buffer = round(buffer);
				buffer *= 255 / (value - 1);
				current_error = image[i * width + j] + error[i * width + j] - buffer;
				image[i * width + j] = buffer;
				for (int k = 0; k <= 2; k++)
				{
					for (int v = -2; v <= 2; v++)
					{
						if (i + k < height) {
							if ((k == 0) && (v > 0))
							{
								if (j + v < width)
									error[(i + k) * width + j + v] += current_error * ATKINSON_matrix[k][2 + v] / 8.0;
							}
							else
							{
								if ((j + v < width) && (j + v > 0))
								{
									error[(i + k) * width + j + v] += current_error * ATKINSON_matrix[k][2 + v] / 8.0;
								}
							}
						}
					}
				}
			}
		}
		break;
	case HALFTONE_4x4:
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				buffer = (back_correct(image[i * width + j]) + (255 / (bits)) * (halftone_matrix[i % 4][j % 4] - 0.75)) / 255;
				if (buffer < 0)
					buffer = 0;
				buffer *= value;
				buffer = round(buffer);
				image[i * width + j] = round(correct(buffer * (255 / (value - 1))));
			}
		}
		break;
	}
}

void Picture::setcorrection(float input)
{
	correction = input;
}

void Picture::Fill()
{
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			image[i * width + j] = correct(((double)j / width) * 255.0);
		}
	}
}

double Picture::correct(double value)
{
	value = value / 255;
	if (value > 1)
		value = 1;
	if (correction == 0)
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
	if (correction == 0)
	{
		if (value < 0.04045)
			return  (255 * value) / 12.92;
		else
			return 255 * (pow((200.0 * value + 11.0) / 211.0, 2.4));
	}
	else {
		return 255 * pow(value, 1 / correction);
	}
}


Picture::~Picture()
{
	delete[] image;
}