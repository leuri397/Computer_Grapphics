#include <string>
#include <vector>
struct Pixel {
	unsigned char A;
	unsigned char B;
	unsigned char C;
};
enum ColorSpace {
	YCbCr601,
	RGB
};
enum Conversion {
	RGB_SET = 0,
	YCBCR_SET = 1,
	RGB_AUTO_SIMPLE = 2,
	YCBCR_AUTO_SIMPLE = 3,
	RGB_AUTO = 4,
	YCBCR_AUTO = 5
};
class Picture {
public:
	Picture();
	~Picture();
	void OpenSingleFile(std::string);
	void SaveToSingleFile(std::string);
	void SetCS(ColorSpace) noexcept;
	void changeBrightness(double difference, double multiple) noexcept;
	void autoBrightness(bool type) noexcept;
private:
	int height;
	int width;
	Pixel* image;
	ColorSpace CurrentCS;
	std::vector<int> getBrightestandDarkest(bool avoid) noexcept;
	bool color;
};