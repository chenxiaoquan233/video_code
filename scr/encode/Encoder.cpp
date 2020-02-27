#include "../../include/encode/Encoder.h"

Encoder::Encoder(const char* _png_path):png_path(_png_path)
{
}

Encoder::~Encoder()
{
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
		bin_to_png(bin_text, len * 26);		//图片生成
		png_sum++;
	}
	png_to_mp4(_output_file_name, fps,3, IMG_Y+20, IMG_X+20);
	return 0;
}

int Encoder::text_to_bin(char* _input_file_name)
{
	int bit_message_len = MAX_BIN_PER_IMAGE / 26 * 16;
	int hex_len = bit_message_len / 16;
	int bin_text_len = MAX_BIN_PER_IMAGE / 26 * 26;

	bin_text = new bool[bin_text_len];
	char* text_tmp = new char[bit_message_len / 8];

	int res = fread(text_tmp, 1, hex_len * 2, input_file);

	hex = new unsigned int[res / 2];
	for (int i = 0; i < res; ++i)
	{
		if (i % 2)
		{
			hex[i / 2] += text_tmp[i];
			unsigned int FEC = getFEC(hex[i / 2]);
			hex[i / 2] <<= 10;
			hex[i / 2] += FEC;
			for (int j = 0; j < 26; ++j)
				bin_text[i/2 * 26 + j] = (hex[i / 2] & (1 << j)?1:0);
		}
		else
		{
			hex[i / 2] = text_tmp[i];
			hex[i / 2] <<= 8;
		}
	}
	return res/2;
}

void Encoder::draw_anchors(Mat& image)
{
	//定位点一
	rectangle(image, Point(0 + 10, 0 + 10), Point(160 + 10, 160 + 10), Scalar(255), FILLED, LINE_8);
	rectangle(image, Point(0 + 10, 0 + 10), Point(140 + 10, 140 + 10), Scalar(0), FILLED, LINE_8);
	rectangle(image, Point(20 + 10, 20 + 10), Point(120 + 10, 120 + 10), Scalar(255), FILLED, LINE_8);
	rectangle(image, Point(40 + 10, 40 + 10), Point(100 + 10, 100 + 10), Scalar(0), FILLED, LINE_8);
	//定位点二
	rectangle(image, Point(0 + 10, 560 + 10), Point(160 + 10, 720 + 10), Scalar(255), FILLED, LINE_8);
	rectangle(image, Point(0 + 10, 580 + 10), Point(140 + 10, 720 + 10), Scalar(0), FILLED, LINE_8);
	rectangle(image, Point(20 + 10, 600 + 10), Point(120 + 10, 700 + 10), Scalar(255), FILLED, LINE_8);
	rectangle(image, Point(40 + 10, 620 + 10), Point(100 + 10, 680 + 10), Scalar(0), FILLED, LINE_8);
	//定位点三
	rectangle(image, Point(1120 + 10, 0 + 10), Point(1280 + 10, 160 + 10), Scalar(255), FILLED, LINE_8);
	rectangle(image, Point(1140 + 10, 0 + 10), Point(1280 + 10, 140 + 10), Scalar(0), FILLED, LINE_8);
	rectangle(image, Point(1160 + 10, 20 + 10), Point(1260 + 10, 120 + 10), Scalar(255), FILLED, LINE_8);
	rectangle(image, Point(1180 + 10, 40 + 10), Point(1240 + 10, 100 + 10), Scalar(0), FILLED, LINE_8);
	//定位点四
	rectangle(image, Point(1140 + 10, 580 + 10), Point(1240 + 10, 680 + 10), Scalar(0), FILLED, LINE_8);
	rectangle(image, Point(1160 + 10, 600 + 10), Point(1220 + 10, 660 + 10), Scalar(255), FILLED, LINE_8);
	rectangle(image, Point(1180 + 10, 620 + 10), Point(1200 + 10, 640 + 10), Scalar(0), FILLED, LINE_8);
}

void Encoder::bin_to_png(bool* str, int size)
{
	Mat image(IMG_X + 20, IMG_Y + 20, CV_8UC1);
	rectangle(image, Point(0, 0), Point(IMG_Y + 20, IMG_X + 20), Scalar(255), FILLED, LINE_8);

	draw_anchors(image);
	
	int count = 0;//统计已填充数目
	for (int p = 0; p < ANCHOR_BASE_BLOCKS / BLOCK_SIZE; p++)
	{
		for (int q = 0; q < IMG_Y / BLOCK_WIDTH - 2 * ANCHOR_BASE_BLOCKS / BLOCK_SIZE; q++)
		{
			rectangle(image, Point(BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * q + 10, 0 + BLOCK_WIDTH * p + 10), Point(BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * (q+1) + 10, 0 + BLOCK_WIDTH * (p+1) + 10), Scalar(255 * str[count++]), FILLED, LINE_8);//黑0白1
			if (count >= size)
			{
				str[count] = 0;			//溢出部分用0填充
			}
		}
	}
	for (int p = 0; p < IMG_X / BLOCK_WIDTH - 2 * ANCHOR_BASE_BLOCKS / BLOCK_SIZE; p++)
	{
		for (int q = 0; q < IMG_Y / BLOCK_WIDTH; q++)
		{
			rectangle(image, Point(0 + BLOCK_WIDTH * q + 10, BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * p + 10), Point(0 + BLOCK_WIDTH * (q+1) + 10, BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * (p+1) + 10), Scalar(255 * str[count++]), FILLED, LINE_8);
			if (count >= size)
			{
				str[count] = 0;
			}
		}
	}
	for (int q = 0; q < IMG_Y / BLOCK_WIDTH - ANCHOR_BASE_BLOCKS / BLOCK_SIZE; q++)
	{
		rectangle(image, Point(BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * q + 10, IMG_X - BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + 10), Point(BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * (q + 1) + 10, IMG_X - BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH + 10), Scalar(255 * str[count++]), FILLED, LINE_8);
		if (count >= size)
		{
			str[count] = 0;
		}
	}
	for (int p = 0; p < MINI_ANCHOR_BASE_BLOCKS / BLOCK_SIZE; p++)
	{
		for (int q = 0; q < IMG_Y / BLOCK_WIDTH - 2 * ANCHOR_BASE_BLOCKS / BLOCK_SIZE + 1; q++)
		{
			rectangle(image, Point(BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * q + 10, IMG_X - BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * (p+1) + 10), Point(BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * (q + 1) + 10, IMG_X - BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * (p + 2) + 10), Scalar(255 * str[count++]), FILLED, LINE_8);
			if (count >= size)
			{
				str[count] = 0;
			}
		}
	}
	for (int p = 0; p < MINI_ANCHOR_BASE_BLOCKS / BLOCK_SIZE; p++)
	{
		for (int q = 0; q < 2; q++)
		{
			rectangle(image, Point(IMG_Y - ANCHOR_BASE_BLOCKS * BASE_BLOCK_WIDTH + MINI_ANCHOR_BASE_BLOCKS * BASE_BLOCK_WIDTH + BLOCK_WIDTH * q + 10 + 20, IMG_X - BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * (p + 1) + 10), Point(IMG_Y - ANCHOR_BASE_BLOCKS * BASE_BLOCK_WIDTH + MINI_ANCHOR_BASE_BLOCKS * BASE_BLOCK_WIDTH + BLOCK_WIDTH * (q + 1) + 10 + 20, IMG_X - BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * (p + 2) + 10), Scalar(255 * str[count++]), FILLED, LINE_8);
			if (count >= size)
			{
				str[count] = 0;
			}
		}
	}
	for (int p = 0; p < 2; p++)
	{
		for (int q = 0; q < IMG_Y / BLOCK_WIDTH - ANCHOR_BASE_BLOCKS / BLOCK_SIZE; q++)
		{
			rectangle(image, Point(BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * q + 10, IMG_X - BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BASE_BLOCK_WIDTH * MINI_ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * p + 20 + 10), Point(BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * (q+1) + 10, IMG_X - BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BASE_BLOCK_WIDTH * MINI_ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * (p+1) + 20 + 10), Scalar(255 * str[count++]), FILLED, LINE_8);
			if (count >= size)
			{
				str[count] = 0;
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

unsigned int Encoder::getFEC(unsigned int CX)
{
	unsigned int RX;
	CX = CX << 10;
	unsigned int gx = 0x05B9 << (26 - 11);
	for (int j = 0; j < 16; j++)
	{
		if ((CX & 0x2000000) != 0)
			CX ^= gx;
		CX = CX << 1;
	}
	RX = CX >> 16;
	return RX;
}