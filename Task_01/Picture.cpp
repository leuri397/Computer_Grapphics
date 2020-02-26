#include "Picture.h"
#include <fstream>
#include <cmath>
#include <string>
using namespace std; 

void Picture::swap(unsigned char& a, unsigned char& b)
{
	char buffer;
	buffer = a;
	a = b;
	b = buffer;
}

Picture::Picture()
{
	height = 0;
	width = 0;
	color = false;
	image = nullptr;
	bytes = 0;
	maxvalue = 0;
}

Errors Picture::OpenFile(string filename)
{
	if (!((filename.substr(filename.length() - 4, 4) == ".pnm") || (filename.substr(filename.length() - 4, 4) == ".pgm")))
		return Errors::WRONG_FORMAT;
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
	input >> maxvalue;
	if (!input.good())
		return Errors::FILE_ERROR;
	bytes = ceil(log2(maxvalue))/8;
	char* image_char;
	if (color)
	{
		image = new unsigned char[bytes * width * height * 3];
		if (image == NULL)
			return Errors::OUT_OF_MEMORY;
		image_char = (char*)image;
		input.read(image_char, 1);
		input.read(image_char, bytes * width * height * 3);
	}
	else
	{
		image = new unsigned char[bytes * width * height];
		if (image == NULL)
			return OUT_OF_MEMORY;
		image_char = (char*)image;
		input.read(image_char, 1);
		input.read(image_char, bytes * width * height);
	}
	if (!input.good())
		return FILE_ERROR;
	input.close();
	return Errors::OK;
}
 
Errors Picture::SaveToFile(std::string filename)
{
	if (!((filename.substr(filename.length() - 4, 4) == ".pnm") || (filename.substr(filename.length() - 4, 4) == ".pgm")))
		return Errors::WRONG_FORMAT;
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
	output << width << ' ' << height << '\n' << maxvalue << '\n';
	char* image_char = (char*)image;
	if (color)
	{
		output.write(image_char, bytes * width * height * 3);
	}
	else
	{
		output.write(image_char, bytes * width * height);
	}
	if (output.fail())
		return FILE_ERROR;
	output.close();
	return Errors::OK;
}

void Picture::Invert()
{
	if (color)
	{
		for (int i = 0; i < width * height * bytes * 3; i += bytes)
		{
			image[i] = maxvalue - image[i];
		}
	}
	else
	{
		for (int i = 0; i < width * height * bytes; i += bytes)
		{
				image[i] = maxvalue - image[i];
		}
	}
}

void Picture::Flip(bool upright)
{
	if (upright)
	{
		if (color)
		{
			for (int i = 0; i < height / 2; i++)
			{
				for (int j = 0; j < width * 3; j++)
				{
					swap(image[i * width * 3+j], image[(height - i - 1) * width * 3 + j]);
				}
			}
		}
		else
		{
			for (int i = 0; i < height / 2; i++)
			{
				for (int j = 0; j < width; j++)
				{
					swap(image[i * width + j], image[(height - i - 1) * width + j]);
				}
			}
		}
	}
	else
		if (color)
		{
			for (int i = 0; i < height; i++)
			{
				for (int j = 0; j < width / 2; j++)
				{
					for (int k = 0; k < 3; k++)
					{
						swap(image[i * width * 3 + j * 3 + k], image[i * width * 3 + (width - j - 1) * 3 + k]);
					}
				}
			}
		}
		else
		{
			for (int i = 0; i < height; i++)
			{
				for (int j = 0; j < width / 2; j++)
				{
					swap(image[i * width + j], image[i * width + width - j - 1]);
				}
			}
		}
}

Errors Picture::Rotate(bool clockwise)
{
	unsigned char* new_image;
	if (color)
	{
		new_image = new unsigned char[width * height * bytes * 3];
	}
	else
	{
		new_image = new unsigned char[width * height * bytes];
	}
	if (new_image == NULL)
		return Errors::OUT_OF_MEMORY;
	if (!color)
	{
		if (clockwise)
		{
			for (int i = 0; i < height; i++)
			{
				for (int j = 0; j < width; j++)
				{
					new_image[j * height + i] = image[(height - 1 - i) * width + j];
				}
			}
		}
		else
		{
			for (int i = 0; i < height; i++)
			{
				for (int j = 0; j < width; j++)
				{
					new_image[j * height + i] = image[i * width + width - j - 1];
				}
			}
		}
	}
	else
	{
		if (clockwise)
		{
			for (int i = 0; i < height; i++)
			{
				for (int j = 0; j < width; j++)
				{
					for (int k = 0; k < 3; k++)
					{
						new_image[j * height * 3 + i * 3 + k] = image[(height - 1 - i) * width * 3 + j * 3 + k];
					}
				}
			}

		}
		else
			for (int i = 0; i < height; i++)
			{
				for (int j = 0; j < width; j++)
				{
					for (int k = 0; k < 3; k++)
					{
						new_image[j * height * 3 + i * 3 + k] = image[i * width * 3 + (width - j - 1) * 3 + k];
					}
				}
			}
	}
	int buffer = height;
	height = width;
	width = buffer;
	delete[] image;
	image = new_image;
	return Errors::OK;
}

Picture::~Picture()
{
	delete[] image;
}


