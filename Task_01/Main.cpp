#include <iostream>
#include "Picture.h"
#include <string>

int errorHandler(Errors error) {
    switch (error)
    {
    case Errors::OK:
        return 0;
    case Errors::FILE_ERROR:
        std::cout << "Error occured while reading/writing image file\n";
        return -1;
    case Errors::OUT_OF_MEMORY:
        std::cout << "Not enough memory to store or make any conversion with picture\n";
        return -1;
    case Errors::WRONG_FORMAT:
        std::cout << "Wrong format of file\n";
        return -1;
    }
}

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        std::cout << "Wrong amount of arguments\n";
        return -1;
    }
    std::string inputFile, outputFile;
    int conversion;
    Errors Current_Error;
    inputFile = argv[1];
    outputFile = argv[2];
    conversion = atoi(argv[3]);

    Picture NewPic;
    Current_Error = NewPic.OpenFile(inputFile);
    if (errorHandler(Current_Error) == -1)
        return -1;
    switch (conversion)
    {
    case 0:
        NewPic.Invert();
        break;
    case 1:
        NewPic.Flip(false);
        break;
    case 2:
        NewPic.Flip(true);
        break;
    case 3:
       Current_Error = NewPic.Rotate(true);
        break;
    case 4:
       Current_Error = NewPic.Rotate(false);
        break;
    default:
        std::cout << "Unknown type of conversion!\n";
        return -1;
    }
    if (errorHandler(Current_Error) != 0)
        return -1;
    Current_Error = NewPic.SaveToFile(outputFile);
    if (errorHandler(Current_Error) != 0)
        return -1;
    return 0;
}


