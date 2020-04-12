#include <fstream>
#include <cmath>
#include <string>
#include <algorithm>
#include <stdexcept>
#include "Picture.h"
using namespace std;
#define MAX_VALUE 255

Picture::Picture() : height(0), width(0), image(nullptr), currentCS(ColorSpace::RGB)
{}

void Picture::OpenSingleFile(string filename)
{
	ifstream input;
	input.open(filename, ios_base::in | ios_base::binary);
	if (!input.is_open())
		throw std::runtime_error("File error");
	string header;
	input >> header;
	if (header != "P6")
	{
		throw std::runtime_error("File is not P6 format");
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
		input.read(image_char, 1);
		image[i].A = *image_char;
		input.read(image_char, 1);
		image[i].B = *image_char;
		input.read(image_char, 1);
		image[i].C = *image_char;
	}
	if (!input.good())
		throw std::runtime_error("Reading error");
	input.close();
}

void Picture::OpenMultipleFile(std::string filename)
{
	ifstream inputA, inputB, inputC;
	filename.insert(filename.size() - 4, "_1");
	inputA.open(filename, ios_base::in | ios_base::binary);
	if (!inputA.is_open())
		throw std::runtime_error("Problem opening file:" + filename);
	filename.replace(filename.size() - 5, 1, "2");
	inputB.open(filename, ios_base::in | ios_base::binary);
	if (!inputB.is_open())
		throw std::runtime_error("Problem opening file:" + filename);
	filename.replace(filename.size() - 5, 1, "3");
	inputC.open(filename, ios_base::in | ios_base::binary);
	if (!inputC.is_open())
		throw std::runtime_error("Problem opening file:" + filename);

	string headerA, headerB, headerC;
	inputA >> headerA;
	inputB >> headerB;
	inputC >> headerC;
	if ((headerA != headerB) || (headerB != headerC) || (headerC != "P5"))
	{
		throw std::runtime_error("One or many files aren't P5 format");
	}
	inputA >> width >> height;
	int widthB, widthC, heightB, heightC;
	inputB >> widthB >> heightB;
	if ((widthB != width) || (heightB != height))
		throw std::runtime_error("Files have different formats");
	inputC >> widthC >> heightC;
	if ((widthC != width) || (heightC != height))
		throw std::runtime_error("Files have different formats");

	int maxvalue;
	inputA >> maxvalue;
	if (maxvalue != MAX_VALUE)
		throw std::runtime_error("Invalid metadata");
	inputB >> maxvalue;
	if (maxvalue != MAX_VALUE)
		throw std::runtime_error("Invalid metadata");
	inputC >> maxvalue;
	if (maxvalue != MAX_VALUE)
		throw std::runtime_error("Invalid metadata");
	if (!inputA.good()|| !inputB.good()|| !inputC.good())
		throw std::runtime_error("Reading error");
	char partPixel;
	char* image_char = &partPixel;
	image = new Pixel[width * height];
	if (image == NULL)
		throw std::runtime_error("Out of memory");
	inputA.read(image_char, 1);
	inputB.read(image_char, 1);
	inputC.read(image_char, 1);
	for (int i = 0; i < width * height; i++)
	{
		inputA.read(image_char, 1);
		image[i].A = *image_char;
		inputB.read(image_char, 1);
		image[i].B = *image_char;
		inputC.read(image_char, 1);
		image[i].C = *image_char;
	}
	if (!inputA.good() || !inputB.good() || !inputC.good())
		throw std::runtime_error("Reading error");
}

void Picture::SaveToSingleFile(std::string filename)
{
	ofstream output;
	output.open(filename, ios_base::out | ios_base::binary);
	if (!output.is_open())
		throw std::runtime_error("File writing error");
	output << "P6" << '\n';
	output << width << ' ' << height << '\n' << MAX_VALUE << '\n';
	char* image_char = (char*)image;
	output.write(image_char, width * height * 3);
	if (output.fail())
		throw std::runtime_error("File writing error");
	output.close();
}

void Picture::SaveToMultipleFile(std::string filename)
{
	ofstream outputA, outputB, outputC;
	filename.insert(filename.size() - 4, "_1");
	outputA.open(filename, ios_base::out | ios_base::binary);
	if (!outputA.is_open())
		throw std::runtime_error("Problem opening file:" + filename);
	filename.replace(filename.size() - 5, 1, "2");
	outputB.open(filename, ios_base::out | ios_base::binary);
	if (!outputB.is_open())
		throw std::runtime_error("Problem opening file:" + filename);
	filename.replace(filename.size() - 5, 1, "3");
	outputC.open(filename, ios_base::out | ios_base::binary);
	if (!outputC.is_open())
		throw std::runtime_error("Problem opening file:" + filename);

	outputA << "P5" << '\n';
	outputA << width << ' ' << height << '\n' << MAX_VALUE << '\n';
	outputB << "P5" << '\n';
	outputB << width << ' ' << height << '\n' << MAX_VALUE << '\n';
	outputC << "P5" << '\n';
	outputC << width << ' ' << height << '\n' << MAX_VALUE << '\n';
	
	if (!outputA.good() || !outputB.good() || !outputC.good())
		throw std::runtime_error("File writing error");
	char* part_pixel;
	for (int i = 0; i < width * height; i++)
	{
		outputA.write((char*)&image[i].A, 1);
		outputB.write((char*)&image[i].B, 1);
		outputC.write((char*)&image[i].C, 1);
	}
	if (!outputA.good() || !outputB.good() || !outputC.good())
		throw std::runtime_error("File writing error");
}

void Picture::convertANY(ColorSpace convert)
{
	if (currentCS == convert)
		return;
	convertRGB();
	if (convert == RGB)
		return;
	double Max, Min, H, S, L, C, X, V, M;
	double Y, Cb, Cr, Co, Cg, Kr, Kg, Kb, R, G, B;
	char Ho;
	switch (convert)
	{
	case HSL:
	case HSV:
		for (int i = 0; i < width * height; i++) {
			R = image[i].A / 255.0;
			G = image[i].B / 255.0;
			B = image[i].C / 255.0;
			Max = max(R, max(G, B));
			Min = min(R, min(G, B));
			V = Max;
			C = Max - Min;
			L = V - C / 2.0;
			if (C == 0)
				H = 0;
			else 
			{
				if (V == R)
					H = (60.0) * ((G - B) / C);
				else if (V == G)
					H = (60.0) * (2 + (B - R) / C);
				else if (V == B)
					H = (60.0) * (4 + (R - G) / C);
				else
					H = 0;
			}
			if (convert == HSV)
			{
				S = (V == 0) ? 0 : C / V;
				image[i].C = V * 255.0;
			}
			if (convert == HSL) {
				S = ((L == 0) || (L == 1)) ? 0 : ((V - L) / min(L, 1 - L));
				image[i].C = L * 255.0;
			}
			image[i].B = S * 255.0;
			image[i].A = (H/360.0)*255.0;
			
		}
		currentCS = convert;
		break;
	case YCbCr_601:
	case YCbCr_709:
		if (convert == YCbCr_601) {
			Kr = 0.299;
			Kg = 0.587;
			Kb = 0.114;
			currentCS = YCbCr_601;
		}
		else
		{
			Kr = 0.0722;
			Kg = 0.2126;
			Kb = 0.7152;
			currentCS = YCbCr_709;
		}
		for (int i = 0; i < width * height; i++)
		{
			R = image[i].A / 255.0;
			G = image[i].B / 255.0;
			B = image[i].C / 255.0;
			Y = Kr * R + Kg * G + Kb * B;
			Cb = 0.5 * ((B - Y) / (1.0 - Kb));
			Cr = 0.5 * ((R - Y) / (1.0 - Kr));
			image[i].A = Y * 255.0;
			image[i].B = (Cb + 0.5)*255.0;
			image[i].C = (Cr + 0.5)*255.0;
		}
		break;
	case YCoCg:
		for (int i = 0; i < width * height; i++) {
			R = image[i].A / 255.0;
			G = image[i].B / 255.0;
			B = image[i].C / 255.0;
			Y = R / 4 + G / 2 + B / 4;
			Co = R / 2 - B / 2;
			Cg = -R / 4 + G / 2 - B / 4;
			image[i].A = Y * 255.0;
			image[i].B = (Co + 0.5) * 255.0;
			image[i].C = (Cg + 0.5) * 255.0;
		}
		currentCS = YCoCg;
		break;
	case CMY:
		for (int i = 0; i < width * height; i++) {
			R = image[i].A / 255.0;
			G = image[i].B / 255.0;
			B = image[i].C / 255.0;
			C = 1 - R;
			M = 1 - G;
			Y = 1 - B;
			image[i].A = C * 255.0;
			image[i].B = M  * 255.0;
			image[i].C = Y * 255.0;
		}
		currentCS = CMY;
		break;
	}
}

void Picture::convertRGB()
{
	double H, S, L, C, H_D, X, m, R, G, B, Y, Cb, Cr, Co, Cg, M, Kr, Kg, Kb;
	if (currentCS == RGB)
		return;
	switch (currentCS)
	{
	case HSL:
	case HSV:
		for (int i = 0; i < height * width; i++)
		{
			H = (image[i].A / 255.0) * 360.0;
			S = image[i].B / 255.0;
			L = image[i].C / 255.0;
			H_D = H/ 60;
			if (currentCS == HSL) {
				C = (1 - abs(2 * L - 1)) * S;
				X = C * (1 - abs(fmod(H_D, 2) - 1));
				m = L - C / 2.0;
			}
			else
			{
				C = S * L;
				X = C * (1.0 - abs(fmod(H_D, 2) - 1.0));
				m = L - C;
			}
			
			m *= 255.0;
			if ((H_D >= 0) && (H_D <= 1))
			{
				image[i].A = C * 255.0 + m;
				image[i].B = X * 255.0 + m;
				image[i].C = 0 + m;
			}
			if ((H_D > 1) && (H_D <= 2))
			{
				image[i].A = X * 255.0 + m;
				image[i].B = C * 255.0 + m;
				image[i].C = 0 + m;
			}
			if ((H_D > 2) && (H_D <= 3))
			{
				image[i].A = 0 + m;
				image[i].B = C * 255.0 + m;
				image[i].C = X * 255.0 + m;
			}
			if ((H_D > 3) && (H_D <= 4))
			{
				image[i].A = 0 + m;
				image[i].B = X * 255.0 + m;
				image[i].C = C * 255.0 + m;
			}
			if ((H_D > 4) && (H_D <= 5))
			{
				image[i].A = X * 255.0 + m;
				image[i].B = 0 + m;
				image[i].C = C * 255.0 + m;
			}
			if ((H_D > 5) && (H_D <= 6))
			{
				image[i].A = C * 255.0 + m;
				image[i].B = 0 + m;
				image[i].C = X * 255.0 + m;
			}
			
		}
		break;
	case YCbCr_601:
	case YCbCr_709:
		if (currentCS == YCbCr_601) {
			Kr = 0.299;
			Kg = 0.587;
			Kb = 0.114;
		}
		else
		{
			Kr = 0.0722;
			Kg = 0.2126;
			Kb = 0.7152;
		}
		for (int i = 0; i < width * height; i++) {
			Y = image[i].A / 255.0;
			Cb = (image[i].B / 255.0) - 0.5;
			Cr = (image[i].C / 255.0) - 0.5;
			R = (Y + Cr * (2.0 - 2.0 * Kr));
			G = (Y - (Kb / Kg) * (2.0 - 2.0 * Kb) * Cb - (Kr / Kg) * (2.0 - 2.0 * Kr) * Cr);
 			B = (Y + (2.0 - 2.0 * Kb) * Cb);
			if (R < 0)
				R = 0;
			if (G < 0)
				G = 0;
			if (B < 0)
				B = 0;
			if (R > 1)
				R = 1;
			if (G > 1)
				G = 1;
			if (B > 1)
				B = 1;
			image[i].A = R * 255.0;
			image[i].B = G * 255.0;
			image[i].C = B * 255.0;
		}
		break;
	case YCoCg:
		for (int i = 0; i < width * height; i++) {
			Y = image[i].A / 255.0;
			Co = (image[i].B / 255.0) - 0.5;
			Cg = (image[i].C / 255.0) - 0.5;
			R = Y + Co - Cg;
			G = Y + Cg;
			B = Y - Co - Cg;
			if (R < 0)
				R = 0;
			if (G < 0)
				G = 0;
			if (B < 0)
				B = 0;
			if (R > 1)
				R = 1;
			if (G > 1)
				G = 1;
			if (B > 1)
				B = 1;
			image[i].A = R * 255.0;
			image[i].B = G * 255.0;
			image[i].C = B * 255.0;
		}
		break;
	case CMY:
		for (int i = 0; i < width * height; i++) {
			C = image[i].A / 255.0;
			M = image[i].B / 255.0;
			Y = image[i].C / 255.0;
			R = 1 - C;
			G = 1 - M;
			B = 1 - Y;
			image[i].A = R * 255.0;
			image[i].B = G * 255.0;
			image[i].C = B * 255.0;
		}
		break;
	}
	currentCS = RGB;
}

Picture::~Picture()
{
	delete[] image;
}

void Picture::SetCS(ColorSpace current)
{
	currentCS = current;
}
