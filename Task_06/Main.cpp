#include "Picture.h"
#include <iostream>

int errorHandler(Errors error) {
    switch (error)
    {
    case Errors::OK:
        return 0;
    case Errors::FILE_ERROR:
        std::cerr << "Error occured while reading/writing image file\n";
        return 1;
    case Errors::OUT_OF_MEMORY:
        std::cerr << "Not enough memory to store or make any conversion with picture\n";
        return 1;
    case Errors::WRONG_FORMAT:
        std::cerr << "Wrong format of file\n";
        return 1;
    }
}
int main(int argc, char* argv[])
{
    Picture NewPic;
    Errors current_error;
    if (argc < 8)
    {
        std::cerr << "Wrong amount of arguments" << '\n';
    }
    if ((argc == 10) || (argc == 9))
    {
        double B = atof(argv[8]), C = 0.5;
        if (argc == 10)
            C = atof(argv[9]);
        NewPic.SetBC(B, C);
    }

    current_error = NewPic.OpenFile(argv[1]);
    if (errorHandler(current_error) != 0)
        return 1;
    std::string output = argv[2];
   current_error = NewPic.Scale(atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), static_cast<Type>(atoi(argv[7])));
    if (errorHandler(current_error) != 0)
        return 1;
    current_error = NewPic.SaveToFile(output);
    if (errorHandler(current_error) != 0)
        return 1;
    return 0;
}

