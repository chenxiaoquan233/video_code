#include "./../include/Encoder.h"

Encoder::Encoder(const char* _png_path):png_path(_png_path),BLOCK_WIDTH(BLOCK_SIZE*10),IMG_X(720),IMG_Y(1280),MAX_BIN_PER_IMAGE((IMG_X * IMG_Y)/100 - ANCHOR_AREA)
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
	set_video_length(_video_length);
	input_file = fopen(_input_file_name, "r");
	while (!feof(input_file))
	{
		int len = text_to_bin(_input_file_name);
		bin_to_png(bin_text, len);		//图片生成
		png_sum++;
	}
	png_to_mp4(_output_file_name, fps, video_length / 1000 * fps / png_sum, IMG_Y, IMG_X);
	return 0;
}

int Encoder::text_to_bin(char* _input_file_name)
{
	bin_text = new bool[MAX_BIN_PER_IMAGE];
	char* text_tmp = new char[MAX_BIN_PER_IMAGE / 8];
	int res = fread(text_tmp, 1, MAX_BIN_PER_IMAGE/8, input_file);
	for (int i = 0; i < res; ++i)
		for (int j = 0; j < 7; ++j)
			bin_text[i * 7 + j] = text_tmp[i] & (1 << j);
	return res;
}

void Encoder::bin_to_png(bool* str, int size)
{
	Mat image(IMG_X, IMG_Y, CV_8UC1);
	//定位点一
	rectangle(image, Point( 0,  0), Point(160, 160), Scalar(255), FILLED, LINE_8);
	rectangle(image, Point( 0,  0), Point(140, 140), Scalar(0), FILLED, LINE_8);
	rectangle(image, Point(20, 20), Point(120, 120), Scalar(255), FILLED, LINE_8);
	rectangle(image, Point(40, 40), Point(100, 100), Scalar(0), FILLED, LINE_8);
	//定位点二
	rectangle(image, Point( 0, 560), Point(160, 720), Scalar(255), FILLED, LINE_8);
	rectangle(image, Point( 0, 580), Point(140, 720), Scalar(0), FILLED, LINE_8);
	rectangle(image, Point(20, 600), Point(120, 700), Scalar(255), FILLED, LINE_8);
	rectangle(image, Point(40, 620), Point(100, 680), Scalar(0), FILLED, LINE_8);
	//定位点三
	rectangle(image, Point(1120,  0), Point(1280, 160), Scalar(255), FILLED, LINE_8);
	rectangle(image, Point(1140,  0), Point(1280, 140), Scalar(0), FILLED, LINE_8);
	rectangle(image, Point(1160, 20), Point(1260, 120), Scalar(255), FILLED, LINE_8);
	rectangle(image, Point(1180, 40), Point(1240, 100), Scalar(0), FILLED, LINE_8);
	int count = 0;//统计已填充数目
	for (int p = 0; p < ANCHOR_BASE_BLOCKS / BLOCK_SIZE; p++)
	{
		for (int q = 0; q < IMG_Y / BLOCK_WIDTH - 2 * ANCHOR_BASE_BLOCKS / BLOCK_SIZE; q++)
		{
			rectangle(image, Point(BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * q, 0 + BLOCK_WIDTH * p), Point(BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * (q+1), 0 + BLOCK_WIDTH * (p+1)), Scalar(255 * (str[count++]?1:0)), FILLED, LINE_8);//黑0白1
			if (count >= size)
			{
				//str[count] = 0;			//溢出部分用0填充
			}
		}
	}
	for (int p = 0; p < IMG_X / BLOCK_WIDTH - 2 * ANCHOR_BASE_BLOCKS / BLOCK_SIZE; p++)
	{
		for (int q = 0; q < IMG_Y / BLOCK_WIDTH; q++)
		{
			rectangle(image, Point(0 + BLOCK_WIDTH * q, BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * p), Point(0 + BLOCK_WIDTH * (q+1), BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * (p+1)), Scalar(255 * (double)str[count++]), FILLED, LINE_8);
			if (count >= size)
			{
				//str[count] = 0;
			}
		}
	}
	for (int p = 0; p < ANCHOR_BASE_BLOCKS / BLOCK_SIZE; p++)
	{
		for (int q = 0; q < IMG_Y / BLOCK_WIDTH - ANCHOR_BASE_BLOCKS / BLOCK_SIZE; q++)
		{
			rectangle(image, Point(BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * q, IMG_X - BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * p), Point(BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * (q+1), IMG_X - BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * (p+1)), Scalar(255 * (double)str[count++]), FILLED, LINE_8);
			if (count >= size)
			{
				//str[count] = 0;
			}
		}
	}
	char image_name[32];
	sprintf(image_name, "%s%d.png", png_path, png_sum);
	imwrite(image_name, image);
}

int Encoder::png_to_mp4(char* video_path, int fps, int fpp, int sizeY, int sizeX)
{
	
	VideoWriter video(video_path, VideoWriter::fourcc('M', 'P', '4', 'V'), fps, Size(sizeY, sizeX));
	printf("%d,%d\n", fps, fpp);
	for (int i = 0; i < png_sum; i++)
	{
		char png_pos[32];
		sprintf(png_pos, "%s%d.png", png_path, i);
		Mat image = imread(png_pos);
		if (!image.empty())
		{
			resize(image, image, Size(sizeY, sizeX));
			for (int fpp_num = 0; fpp_num < fpp; fpp_num++)
				video << image;
		}
	}
	puts("Processed\n");
	return 0;
}
