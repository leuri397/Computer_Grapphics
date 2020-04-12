#include <iostream>
#include <string>
#include "Picture.h"

int main(int argc, char* argv[])
{
    Picture NewPicture;
    ColorSpace convertion;
    bool manyInput, manyOutput;
    std::string inputFile, outputFile, buffer;
    bool correct = false;
    if (argc != 11)
    {
        std::cerr << "Wrong amount of arguments.";
        return 1;
    }
    for (int position = 1; position < argc; position++)
    {
        buffer = argv[position];
        if (buffer == "-f")
        {
            position++;
            buffer = argv[position];
            if (buffer == "RGB")
            {
                NewPicture.SetCS(RGB);
                continue;
            }
            if (buffer =="HSL")
            {
                NewPicture.SetCS(HSL);
                continue;
            }
            if (buffer == "HSV")
            {
                NewPicture.SetCS(HSV);
                continue;
            }
            if (buffer == "YCbCr.601")
            {
                NewPicture.SetCS(YCbCr_601);
                continue;
            }
            if (buffer == "YCbCr.709")
            {
                NewPicture.SetCS(YCbCr_709);
                continue;
            }
            if (buffer == "YCoCg")
            {
                NewPicture.SetCS(YCoCg);
                continue;
            }
            if (buffer == "CMY")
            {
                NewPicture.SetCS(CMY);
                continue;
            }
            std::cerr << "Unknown color space \""<< buffer << "\". Terminating";
            return 1;
        }
        if (buffer =="-t")
        {
            position++;
            buffer = argv[position];
            if (buffer == "RGB")
            {
                convertion = RGB;
                continue;
            }
            if (buffer == "HSL")
            {
                convertion = HSL;
                continue;
            }
            if (buffer == "HSV")
            {
                convertion = HSV;
                continue;
            }
            if (buffer == "YCbCr.601")
            {
                convertion = YCbCr_601;
                continue;
            }
            if (buffer == "YCbCr.709")
            {
                convertion = YCbCr_709;
                continue;
            }
            if (buffer == "YCoCg")
            {
                convertion = YCoCg;
                continue;
            }
            if (buffer == "CMY")
            {
                convertion = CMY;
                continue;
            }
            std::cerr << "Unknown color space \""<< buffer << "\". Terminating";
            return 1;
        }
        if (buffer =="-i")
        {
            position++;
            buffer = argv[position];
            if (buffer =="1")
                manyInput = false;
            else if (buffer =="3")
                manyInput = true;
            else
            {
                std::cerr << "Incorrect inputs count.";
                return 1;
            }
            position++;
            buffer = argv[position];
            inputFile = buffer;
            continue;
        }
        if (buffer =="-o")
        {
            position++;
            buffer = argv[position];
            if (buffer =="1")
                manyOutput = false;
            else if (buffer =="3")
                manyOutput = true;
            else
            {
                std::cerr << "Incorrect outputs count.";
                return 1;
            }
            position++;
            buffer = argv[position];
            outputFile = buffer;
            continue;
        }
    }
    try {
        if (manyInput)
        {
            NewPicture.OpenMultipleFile(inputFile);
        }
        else
        {
            NewPicture.OpenSingleFile(inputFile);
        }
    }
    catch (const std::runtime_error r_e) {
        std::cerr << r_e.what() << '\n';
        return 1;
    }
    NewPicture.convertANY(convertion);
    try {
        if (manyOutput)
        {
            NewPicture.SaveToMultipleFile(outputFile);
        }
        else
        {
            NewPicture.SaveToSingleFile(outputFile);
        }
    }
    catch (const std::runtime_error r_e) {
        std::cerr << r_e.what() << '\n';
        return 1;
    }
    return 0;
}

