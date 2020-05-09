#include <fstream>
#include <cmath>
#include <string>
#include <stdexcept>
#include <iostream>
#include "Picture.h"
using namespace std;
#define MAX_VALUE 255

Picture::Picture() : height(0), width(0), image(nullptr), CurrentCS(RGB)
{}

Picture::~Picture()
{
	delete[] image;
}

void Picture::OpenSingleFile(string filename)
{
	ifstream input;
	input.open(filename, ios_base::in | ios_base::binary);
	if (!input.is_open())
		throw std::runtime_error("File error");
	string header;
	input >> header;
	if (header == "P5")
		color = false;
	else if (header == "P6")
		color = true;
	else
	{
		throw std::runtime_error("File has wrong format");
	}
	input >> width >> height;
	int maxvalue;
	input >> maxvalue;
	if (!input.good())
		throw std::runtime_error("Invalid metadata");
	if (maxvalue != MAX_VALUE)
		throw std::runtime_error("Unacceptable format");
	char partPixel;
	char* image_char = &partPixel;
	image = new Pixel[width * height];
	if (image == NULL)
		throw std::runtime_error("Out of memory");
	input.read(image_char, 1);
	for (int i = 0; i < width * height; i++)
	{
		if (color) {
			input.read(image_char, 1);
			image[i].A = *image_char;
			input.read(image_char, 1);
			image[i].B = *image_char;
			input.read(image_char, 1);
			image[i].C = *image_char;
		}
		else
		{
			input.read(image_char, 1);
			image[i].A = *image_char;
			image[i].B = *image_char;
			image[i].C = *image_char;
		}
	}
	if (!input.good())
		throw std::runtime_error("Reading error");
	input.close();
}

void Picture::SaveToSingleFile(std::string filename)
{
	ofstream output;
	output.open(filename, ios_base::out | ios_base::binary);
	if (!output.is_open())
		throw std::runtime_error("File writing error");
	if (color)
		output << "P6" << '\n';
	else
		output << "P5" << '\n';
	output << width << ' ' << height << '\n' << MAX_VALUE << '\n';
	char* image_char = (char*)image;
	if (color)
		output.write(image_char, width * height * 3);
	else
	{
		for (int i = 0; i < width * height; i++)
			output.write(&image_char[i * 3], 1);
	}
	if (output.fail())
		throw std::runtime_error("File writing error");
	output.close();
}

void Picture::SetCS(ColorSpace newCS) noexcept
{
	CurrentCS = newCS;
}

void Picture::changeBrightness(double difference, double multiple) noexcept
{
	if (color)
	{
		if (CurrentCS == RGB)
		{
			for (int i = 0; i < width * height; i++) {
				int result = ((double)image[i].A - difference) * multiple;
				if (result > 255)
					result = 255;
				if (result < 0)
					result = 0;
				image[i].A = result;
				result = ((double)image[i].B - difference) * multiple;
				if (result > 255)
					result = 255;
				if (result < 0)
					result = 0;
				image[i].B = result;
				result = ((double)image[i].C - difference) * multiple;
				if (result > 255)
					result = 255;
				if (result < 0)
					result = 0;
				image[i].C = result;
			}
		} 
		else
		{
			for (int i = 0; i < width * height; i++) 
				{
					int result = ((double)image[i].A - difference) * multiple;
					if (result > 255)
						result = 255;
					if (result < 0)
						result = 0;
					image[i].A = result;
				}
		}
	}
	else
	{
		for (int i = 0; i < width * height; i++)
		{
			int result = ((double)image[i].A - difference) * multiple;
			if (result > 255)
				result = 255;
			if (result < 0)
				result = 0;
			image[i].A = result;
			image[i].B = result;
			image[i].C = result;
		}
	}
}

void Picture::autoBrightness(bool type) noexcept
{
	vector<int> BD;
	BD = getBrightestandDarkest(type);
	double difference = BD[1], multiple = 255.0/((double)BD[0] - BD[1]);
	std::cout << "Difference: " << difference << ".\nMultiple: " << multiple << ".\n";
	changeBrightness(difference, multiple);
}

std::vector<int> Picture::getBrightestandDarkest(bool avoid) noexcept
{
	int pixels[256] = {}, brightest = 0, darkest = 0;
	for (int i = 0; i < width * height; i++)
	{
		if ((color)&&(CurrentCS != YCbCr601))
		{
			pixels[image[i].A]++;
			pixels[image[i].B]++;
			pixels[image[i].C]++;
		}
		else
		{
			pixels[image[i].A]++;
		}
	}
	if (avoid)
	{
		int numberToDel = 0, deleted = 0, Dposition = 0, Bposition = 255;
		if ((color) && (CurrentCS != YCbCr601))
			numberToDel = (double)width * height * 3 * 0.0039;
		else
			numberToDel = (double)width * height * 0.0039;
		while (deleted < numberToDel)
		{
			if (deleted % 2 == 0)
			{
				while (pixels[Dposition] == 0)
					Dposition++;
				pixels[Dposition]--;
				deleted++;
			}
			else
			{
				while (pixels[Bposition] == 0)
					Bposition--;
				pixels[Bposition]--;
				deleted++;
			}
		}
	}
	for (int i = 0; i < 256; i++)
	{
		if (pixels[i] > 0)
		{
			darkest = i;
			break;
		}
	}
	for (int i = 255; i >= 0; i--)
	{
		if (pixels[i] > 0)
		{
			brightest = i;
			break;
		}
	}
	return {brightest, darkest};
}


