#pragma once

#include <stdio.h>
#include <string.h>
#include <opencv2/opencv.hpp>

using namespace cv;

class Decoder
{
private:
	char* pngs_path;
	bool* bin_text;
public:
	Decoder();
	~Decoder();
	int decode(char* _input_video_path, char* _output_text_path);
	int mp4_to_png(char* _video_path);
	int locate_anchors(char* _png_path, Point* points);
	int read_message(Mat* img);
};