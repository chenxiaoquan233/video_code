#pragma once
#include "../base.h"

using namespace cv;

class Encoder:private Code_base
{
private:
	FILE* input_file = nullptr;
	char* output_file = nullptr;
	const char* png_path = nullptr;
	int png_sum = 0;
	int video_length = 0;
	int fps = 30;
	unsigned int* hex = nullptr;
	bool* bin_text = nullptr;
	bool isfirstpng = true;
public:
	Encoder(const char* _png_path);
	~Encoder();
	void set_video_length(char* _video_length);
	int encode(char* _input_file, char* _output_file, char* _video_length);
	int text_to_bin(char* _input_file_name);
	Mat bin_to_png(bool* str, int size);
	unsigned int getFEC(unsigned int CX);
	void draw_anchors(Mat& img);
	Mat pure_white(int IMG_Y, int IMG_X);
	int filesize(char* _input_file_name);
};