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
    if (argc != 7)
    {
        cerr << "Wrong amount of arguments\n";
        return 1;
    }
    string input_name = argv[1], output_name = argv[2];
    bool gradient = false;
    if (*argv[3] == *"1")
        gradient = true;
    else if (*argv[3] == *"0")
        gradient = false;
    else
    {
        cerr << "Invalid arguments\n";
        return 1;
    }
    DitheringType NewDither = (DitheringType)atoi(argv[4]);
    Picture NewPic;
    int bits = atoi(argv[5]);
    NewPic.setcorrection(atof(argv[6]));
    Result current_result;
    current_result = NewPic.OpenFile(input_name);
    if (errorHandler(current_result) == -1)
        return 1;
   // NewPic.line(NewLine);
    if (gradient)
        NewPic.Fill();
    NewPic.Dither(NewDither, bits);
    current_result = NewPic.SaveToFile(output_name);
    if (errorHandler(current_result) == -1)
        return 1;
    return 0;
}
