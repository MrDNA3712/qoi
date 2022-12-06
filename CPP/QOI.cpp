#include "QOI.h"
#include "qoi_imp.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_NO_LINEAR
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


using namespace std;

string img_path = "..\\..\\..\\qoi_test_images";
string out_path = "..\\..\\..\\qoi_test_output";
string images[] = {"dice","kodim10","kodim23","qoi_logo","testcard","testcard_rgba","wikipedia_008"};


void test_image(string name) {
	string png_path = img_path + "\\" + name + ".png";
	string qoi_path = img_path + "\\" + name + ".qoi";
	string png_out_path = out_path + "\\" + name + ".png";
	string qoi_out_path = out_path + "\\" + name + ".qoi";
	
	int png_width = 0, png_height = 0, png_channels = 0;
	uint8_t qoi_colorspace = 0, qoi_channels = 0;
	uint32_t qoi_size = 0, qoi_width = 0, qoi_height = 0;
	
	stbi_info(png_path.c_str(), &png_width, &png_height, &png_channels);
	cout << "Image: " << png_path << endl << "Width: " << png_width << ", Height: " << png_height << ", channels: " << png_channels << endl<<endl;
	void* pixels = (void*)stbi_load(png_path.c_str(), &png_width, &png_height, NULL, png_channels);
	write_qoi(pixels, png_width, png_height, png_channels, 0,qoi_out_path.c_str());

	void* image = read_qoi(qoi_path.c_str(), &qoi_width, &qoi_height, &qoi_channels, &qoi_colorspace, &qoi_size);
	cout << "Image: " << qoi_path << endl;
	cout << "Width: " << qoi_width << ", Height: " << qoi_height << ", Channels: " << (int) qoi_channels << ", Colorspace: " << (int) qoi_colorspace << ", Size: " << qoi_size << endl<<endl;
	stbi_write_png(png_out_path.c_str(), (int)qoi_width, (int)qoi_height, (int)qoi_channels, image, 0);
}

void test_all_images() {
	for (int i = 0; i < (sizeof(images) / sizeof(*images)); i++) {
		test_image(images[i]);
	}
}

int main()
{
	test_all_images();
}
