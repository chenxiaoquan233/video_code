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
	const char* png_path = nullptr;
	int png_sum = 0;
	int video_length = 0;
	int fps = 30;
	const int BLOCK_SIZE = 2;//side block number of every single block
	int BLOCK_WIDTH;
	const int BASE_BLOCK_WIDTH = 10;
	const int IMG_X = 720;
	const int IMG_Y = 1280;
	unsigned int* hex = nullptr;
	int ANCHOR_AREA; //blocks coverd by anchor
	bool* bin_text = nullptr;
	int MAX_BIN_PER_IMAGE;
	const int ANCHOR_BASE_BLOCKS = 16;
public:
	Encoder(const char* _png_path);
	~Encoder();
	void set_output_file(char* _output_file);
	void set_video_length(char* _video_length);
	int encode(char* _input_file, char* _output_file, char* _video_length);
	int text_to_bin(char* _input_file_name);
	void bin_to_png(bool* str, int size);
	int png_to_mp4(char* video_path, int fps, int fpp, int sizeX, int sizeY);
	unsigned int getFEC(unsigned int CX);
};