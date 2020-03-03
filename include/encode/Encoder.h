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
public:
	Encoder(const char* _png_path);
	~Encoder();
	void set_video_length(char* _video_length);
	int encode(char* _input_file, char* _output_file, char* _video_length);
	int text_to_bin(char* _input_file_name);
	void bin_to_png(bool* str, int size);
	int png_to_mp4(char* video_path, int fps, int fpp, int sizeX, int sizeY);
	unsigned int getFEC(unsigned int CX);
	void draw_anchors(Mat& img);
	Mat pure_white(int IMG_X, int IMG_Y);
};