#pragma once
#include <stdio.h>
#include <string.h>
#include <opencv2/opencv.hpp>

using namespace cv;

class Encoder
{
private:
	FILE* input_file = nullptr;
	char* output_file = nullptr;
	char* png_path = nullptr;
	int png_sum = 0;
	int video_length = 0;
	int fps = 60;
	bool* bin_text = nullptr;
	const int MAX_BIN_PER_IMAGE = 528;
public:
	Encoder();
	~Encoder();
	void set_output_file(char* _output_file);
	void set_video_length(char* _video_length);
	int encode(char* _input_file, char* _output_file, char* _video_length);
	int text_to_bin(char* _input_file_name);
	void bin_to_png(bool* str);
	int png_to_mp4(char* video_path, int fps, int fpp, int sizeX, int sizeY);
};