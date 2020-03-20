#include <iostream>
#include <string>
#include "Picture.h"
using namespace std;

int errorHandler(Result error) {
    switch (error)
    {
    case Result::OK:
        return 0;
    case Result::FILE_ERROR:
        cerr << "Error occured while reading/writing image file\n";
        return -1;
    case Result::OUT_OF_MEMORY:
        cerr << "Not enough memory to store or make any conversion with picture\n";
        return -1;
    case Result::WRONG_FORMAT:
        cerr << "Wrong format of file\n";
        return -1;
    }
}

int main(int argc, char* argv[])
{
    if ((argc < 9) || (argc > 10))
    {
        cerr << "Wrong amount of arguments\n";
        return -1;
    }
    string input_name = argv[1], output_name = argv[2];
    Line NewLine(atof(argv[5]), atof(argv[6]), atof(argv[7]), atof(argv[8]), atof(argv[4]), atoi(argv[3]));
    Picture NewPic;
    if (argc == 10)
        NewPic.setcorrection(atof(argv[9]));
    Result current_result;
    current_result = NewPic.OpenFile(input_name);
    if (errorHandler(current_result) == -1)
        return 1;
    NewPic.line(NewLine);
    current_result = NewPic.SaveToFile(output_name);
    if (errorHandler(current_result) == -1)
        return 1;
    return 0;
}

