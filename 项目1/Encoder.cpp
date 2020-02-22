#include "Encoder.h"

Encoder::Encoder()
{

}

Encoder::~Encoder()
{

}

void Encoder::set_output_file(char* _output_file)
{
	this->output_file = _output_file;
}

void Encoder::set_video_length(char* _video_length)
{
	int num = 0;
	int arg_len = strlen(_video_length);
	for (int i = 0; i < arg_len; ++i)
		num *= 10, num += _video_length[i] - '0';
	this->video_length = num;
}

int Encoder::encode(char* _input_file_name, char* _output_file_name, char* _video_length)
{
	input_file = fopen(_input_file_name, "r");
	while (!feof(input_file))
	{
		int len = text_to_bin(_input_file_name);
		png_sum++;
	}
	//png_to_mp4(_output_file_name, fps, _video_length * fps / png_sum, sizeX, sizeY) 参数未定
	return 0;
}

int Encoder::text_to_bin(char* _input_file_name)
{
	bin_text = new bool[MAX_BIN_PER_IMAGE];
	return fread(bin_text, 1, MAX_BIN_PER_IMAGE/8, input_file);
}

int Encoder::png_to_mp4(char* video_path, int fps, int fpp, int sizeX, int sizeY)
{
	
	VideoWriter video(video_path, VideoWriter::fourcc('M', 'P', '4', 'V'), fps, Size(sizeX, sizeY));

	for (size_t i = 0; i < png_sum; i++)
	{
		char png_pos[32];
		sprintf(png_pos, "%s/%d.png", png_path, i);
		Mat image = imread(png_pos);
		if (!image.empty())
		{
			resize(image, image, Size(sizeX, sizeY));
			for (int fpp_num = 0; fpp_num < fpp; fpp_num++)
				video << image;
		}
	}
	puts("处理完毕\n");
	return 0;
}