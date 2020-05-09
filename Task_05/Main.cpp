#include <iostream>
#include "Picture.h"

int main(int argc, char* argv[])
{
    std::string input, output;
    Conversion CurrentConversion;
    double difference, multiple;
    if (argc < 4)
    {
        std::cerr << "Incorrect amount of arguments\n";
        return 1;
    }
    input = argv[1];
    output = argv[2];
    if ((atoi(argv[3]) < 0)|| (atoi(argv[3]) > 6))
       {
           std::cerr << "Incorrect conversion\n";
            return 1;
       }
    CurrentConversion = static_cast<Conversion>(atoi(argv[3]));
    if ((CurrentConversion == RGB_SET) || (CurrentConversion == YCBCR_SET))
    {
        if (argc != 6)
        {
            std::cerr << "Not enough paramets for conversion\n";
            return 1;
        }
        difference = atoi(argv[4]);
        multiple = atof(argv[5]);
    }
    Picture NewPic;
    if ((CurrentConversion == YCBCR_SET) || (CurrentConversion == YCBCR_AUTO) || (CurrentConversion == YCBCR_AUTO_SIMPLE))
        NewPic.SetCS(YCbCr601);
    try
    {
        NewPic.OpenSingleFile(input);
        if ((CurrentConversion == RGB_SET) || (CurrentConversion == YCBCR_SET))
            NewPic.changeBrightness(difference, multiple);
        else
            NewPic.autoBrightness((CurrentConversion == RGB_AUTO) || (CurrentConversion == YCBCR_AUTO));
        NewPic.SaveToSingleFile(output);
    }
    catch (const std::runtime_error r_e) {
        std::cerr << r_e.what() << '\n';
        return 1;
    }
    return 0;
}

