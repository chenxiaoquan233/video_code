#include "../../include/encode/Encoder.h"

Encoder::Encoder()
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
	Parametercheck(_input_file_name, _output_file_name, _video_length);
	set_video_length(_video_length);
	input_file = fopen(_input_file_name, "r");
	VideoWriter video(_output_file_name, 0x00000021, fps, Size(IMG_Y + 20, IMG_X + 20));
	Mat image_white = pure_white(IMG_Y + 20, IMG_X + 20);
	video << image_white;
	int frame_number=1;
	while (!feof(input_file)&&frame_number<= video_length*30/ANCHOR_WIDTH - 3 * BLOCK_SIZE - 4)
	{
		Mat frame;
		int len = text_to_bin(_input_file_name);
		frame=bin_to_png(bin_text, len * 26);		//图片生成
		png_sum++;
		/*imshow("1.png", frame);
		imwrite("1.png", frame);
		waitKey(0);*/
		resize(frame, frame, Size(IMG_Y + 20, IMG_X + 20));
		for (int frame_i = 0;frame_i < 3;frame_i++)
			video << frame;
		frame_number += 3;
	}
	video << image_white;
	return 0;
}

int Encoder::text_to_bin(char* _input_file_name)
{
	int bit_message_len = MAX_BIN_PER_IMAGE / 26 * 16;
	int hex_len = bit_message_len / 16;

	bin_text = new bool[MAX_BIN_PER_IMAGE];
	char* text_tmp = new char[bit_message_len / 8];
	memset(text_tmp, 0, bit_message_len / 8 * sizeof(char));
	int res;

	if (isfirstpng == true)
	{
		int len_of_text = filesize(_input_file_name);		//获取长度
		char start = 64;									//符号@
		char* len_text = new char[bit_message_len / 8];
		sprintf(len_text, "%d%c", len_of_text, start);		//将“长度@”加入数组
		res = fread(text_tmp, 1, hex_len * 2 - strlen(len_text), input_file);
		res += strlen(len_text);
		text_tmp = strcat(len_text, text_tmp);				//将“长度@”添加到正文前面
		isfirstpng = false;
	}
	else
	{
		res = fread(text_tmp, 1, hex_len * 2, input_file);
	}
	
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
	rectangle(image, Point(0 + 10, 0 + 10), Point(ANCHOR_WIDTH + 10, ANCHOR_WIDTH + 10), Scalar(255), FILLED, LINE_8);
	rectangle(image, Point(0 + 10, 0 + 10), Point(ANCHOR_WIDTH - 2 * BASE_BLOCK_WIDTH + 10, ANCHOR_WIDTH - 2 * BASE_BLOCK_WIDTH + 10), Scalar(0), FILLED, LINE_8);
	rectangle(image, Point(2 * BASE_BLOCK_WIDTH + 10, 2 * BASE_BLOCK_WIDTH + 10), Point(ANCHOR_WIDTH - 4 * BASE_BLOCK_WIDTH + 10, ANCHOR_WIDTH - 4 * BASE_BLOCK_WIDTH + 10), Scalar(255), FILLED, LINE_8);
	rectangle(image, Point(4 * BASE_BLOCK_WIDTH + 10, 4 * BASE_BLOCK_WIDTH + 10), Point(ANCHOR_WIDTH - 6 * BASE_BLOCK_WIDTH + 10, ANCHOR_WIDTH - 6 * BASE_BLOCK_WIDTH + 10), Scalar(0), FILLED, LINE_8);
	//定位点二
	rectangle(image, Point(0 + 10, IMG_X - ANCHOR_WIDTH + 10), Point(ANCHOR_WIDTH + 10, IMG_X + 10), Scalar(255), FILLED, LINE_8);
	rectangle(image, Point(0 + 10, IMG_X - ANCHOR_WIDTH + 2 * BASE_BLOCK_WIDTH + 10), Point(ANCHOR_WIDTH - 2 * BASE_BLOCK_WIDTH + 10, IMG_X + 10), Scalar(0), FILLED, LINE_8);
	rectangle(image, Point(2 * BASE_BLOCK_WIDTH + 10, IMG_X - ANCHOR_WIDTH + 4 * BASE_BLOCK_WIDTH + 10), Point(ANCHOR_WIDTH - 4 * BASE_BLOCK_WIDTH + 10, IMG_X - 2 * BASE_BLOCK_WIDTH + 10), Scalar(255), FILLED, LINE_8);
	rectangle(image, Point(4 * BASE_BLOCK_WIDTH + 10, IMG_X - ANCHOR_WIDTH + 6 * BASE_BLOCK_WIDTH + 10), Point(ANCHOR_WIDTH - 6 * BASE_BLOCK_WIDTH + 10, IMG_X - 4 * BASE_BLOCK_WIDTH + 10), Scalar(0), FILLED, LINE_8);
	//定位点三
	rectangle(image, Point(IMG_Y - ANCHOR_WIDTH + 10, 0 + 10), Point(IMG_Y + 10, ANCHOR_WIDTH + 10), Scalar(255), FILLED, LINE_8);
	rectangle(image, Point(IMG_Y - ANCHOR_WIDTH + 2 * BASE_BLOCK_WIDTH + 10, 0 + 10), Point(IMG_Y + 10, ANCHOR_WIDTH - 2 * BASE_BLOCK_WIDTH + 10), Scalar(0), FILLED, LINE_8);
	rectangle(image, Point(IMG_Y - ANCHOR_WIDTH + 4 * BASE_BLOCK_WIDTH + 10, 2 * BASE_BLOCK_WIDTH + 10), Point(IMG_Y - 2 * BASE_BLOCK_WIDTH + 10, ANCHOR_WIDTH - 4 * BASE_BLOCK_WIDTH + 10), Scalar(255), FILLED, LINE_8);
	rectangle(image, Point(IMG_Y - ANCHOR_WIDTH + 6 * BASE_BLOCK_WIDTH + 10, 4 * BASE_BLOCK_WIDTH + 10), Point(IMG_Y - 4 * BASE_BLOCK_WIDTH + 10, ANCHOR_WIDTH - 6 * BASE_BLOCK_WIDTH + 10), Scalar(0), FILLED, LINE_8);
	//定位点四
	rectangle(image, Point(IMG_Y - ANCHOR_WIDTH + 10, IMG_X - ANCHOR_WIDTH + 10), Point(IMG_Y + 10, IMG_X + 10), Scalar(255), FILLED, LINE_8);
	rectangle(image, Point(IMG_Y - ANCHOR_WIDTH + 2 * BASE_BLOCK_WIDTH + 10, IMG_X - ANCHOR_WIDTH + 2 * BASE_BLOCK_WIDTH + 10), Point(IMG_Y + 10, IMG_X + 10), Scalar(0), FILLED, LINE_8);
	rectangle(image, Point(IMG_Y - ANCHOR_WIDTH + 4 * BASE_BLOCK_WIDTH + 10, IMG_X - ANCHOR_WIDTH + 4 * BASE_BLOCK_WIDTH + 10), Point(IMG_Y - 2 * BASE_BLOCK_WIDTH + 10, IMG_X - 2 * BASE_BLOCK_WIDTH + 10), Scalar(255), FILLED, LINE_8);
	rectangle(image, Point(IMG_Y - ANCHOR_WIDTH + 6 * BASE_BLOCK_WIDTH + 10, IMG_X - ANCHOR_WIDTH + 6 * BASE_BLOCK_WIDTH + 10), Point(IMG_Y - 4 * BASE_BLOCK_WIDTH + 10, IMG_X - 4 * BASE_BLOCK_WIDTH + 10), Scalar(0), FILLED, LINE_8);
}

Mat Encoder::bin_to_png(bool* str, int size)
{
	Mat image(IMG_X+20,IMG_Y+20,CV_8UC1);
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
	for (int p = 0; p < ANCHOR_BASE_BLOCKS / BLOCK_SIZE; p++)
	{
		for (int q = 0; q < IMG_Y / BLOCK_WIDTH - 2 * ANCHOR_BASE_BLOCKS / BLOCK_SIZE; q++)
		{
			rectangle(image, Point(ANCHOR_BASE_BLOCKS * BASE_BLOCK_WIDTH + BLOCK_WIDTH * q + 10, IMG_X - ANCHOR_BASE_BLOCKS * BASE_BLOCK_WIDTH + BLOCK_WIDTH * p + 10), Point(ANCHOR_BASE_BLOCKS * BASE_BLOCK_WIDTH + BLOCK_WIDTH * (q + 1) + 10, IMG_X - ANCHOR_BASE_BLOCKS * BASE_BLOCK_WIDTH + BLOCK_WIDTH * (p + 1) + 10), Scalar(255 * str[count++]), FILLED, LINE_8);
			if (count >= size)
			{
				str[count] = 0;
			}
		}
		
	}
	return image;
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
Mat Encoder::pure_white(int IMG_Y, int IMG_X)
{
	Mat image(IMG_X, IMG_Y, CV_8UC3, Scalar(255, 255, 255));
	rectangle(image, Point(0, 0), Point(IMG_X, IMG_Y), Scalar(255, 255, 255), FILLED, LINE_8);
	return image;
}
int Encoder::filesize(char* _input_file_name)
{
	int length = 0;
	FILE* fp;
	fopen_s(&fp, _input_file_name, "rb");
	if (fp == NULL)
	{
		return -1;
	}
	fseek(fp, 0L, SEEK_END);
	length = ftell(fp);
	fclose(fp);
	return length;

}
void Encoder::Parametercheck(char* _input_file_name, char* _output_file_name, char* _video_length)
{
	FILE* fp;
	fp = fopen(_input_file_name, "rb");
	if ((fp == NULL))
	{
		std::cout << "error on open file!" << std::endl;
		exit(0);
	}
	else
	{
		//std::cout << "input_file_name is ok" << std::endl;
		fclose(fp);
	}
	if (_output_file_name[strlen(_output_file_name) - 4] == '.' && (_output_file_name[strlen(_output_file_name) - 3] == 'm' || _output_file_name[strlen(_output_file_name) - 3] == 'M') && (_output_file_name[strlen(_output_file_name) - 2] == 'p' || _output_file_name[strlen(_output_file_name) - 3] == 'P') && _output_file_name[strlen(_output_file_name) - 1] == '4')
	{
		//std::cout << "output_file_name is ok" << std::endl;
	}
	else
	{
		std::cout << "Wrong file format!" << std::endl;
		exit(0);
	}
	int num = 0;
	int arg_len = strlen(_video_length);
	for (int i = 0; i < arg_len; ++i)
	{
		num *= 10, num += _video_length[i] - '0';
	}
	if (num > 180)
	{
		//std::cout << "video_length is ok" << std::endl;
	}
	else
	{
		std::cout << "Video length too short! Please try again!" << std::endl;
		exit(0);
	}
}
