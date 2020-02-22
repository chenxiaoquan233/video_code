#include "./../include/Encoder.h"

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
		bin_to_png(bin_text);		//图片生成
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

void Encoder::bin_to_png(bool* str)
{
	Mat image(720, 1280, CV_8UC1);
	//定位点一
	rectangle(image, Point(0, 0), Point(160, 160), Scalar(255), FILLED, LINE_8);
	rectangle(image, Point(20, 20), Point(140, 140), Scalar(0), FILLED, LINE_8);
	rectangle(image, Point(40, 40), Point(120, 120), Scalar(255), FILLED, LINE_8);
	rectangle(image, Point(60, 60), Point(100, 100), Scalar(0), FILLED, LINE_8);
	//定位点二
	rectangle(image, Point(0, 560), Point(160, 720), Scalar(255), FILLED, LINE_8);
	rectangle(image, Point(20, 580), Point(140, 700), Scalar(0), FILLED, LINE_8);
	rectangle(image, Point(40, 600), Point(120, 680), Scalar(255), FILLED, LINE_8);
	rectangle(image, Point(60, 620), Point(100, 660), Scalar(0), FILLED, LINE_8);
	//定位点三
	rectangle(image, Point(1120, 0), Point(1280, 160), Scalar(255), FILLED, LINE_8);
	rectangle(image, Point(1140, 20), Point(1260, 140), Scalar(0), FILLED, LINE_8);
	rectangle(image, Point(1160, 40), Point(1240, 120), Scalar(255), FILLED, LINE_8);
	rectangle(image, Point(1180, 60), Point(1220, 100), Scalar(0), FILLED, LINE_8);
	int size = sizeof(str);
	int count = 0;//统计已填充数目
	for (int p = 0; p < 4; p++)
	{
		for (int q = 0; q < 24; q++)
		{
			rectangle(image, Point(160 + 40 * q, 0 + 40 * p), Point(200 + 40 * q, 40 + 40 * p), Scalar(255 * (double)str[count++]), FILLED, LINE_8);//黑0白1
			if (count >= size)
			{
				str[count] = 0;			//溢出部分用0填充
			}
		}
	}
	for (int p = 0; p < 10; p++)
	{
		for (int q = 0; q < 32; q++)
		{
			rectangle(image, Point(0 + 40 * q, 160 + 40 * p), Point(40 + 40 * q, 200 + 40 * p), Scalar(255 * (double)str[count++]), FILLED, LINE_8);
			if (count >= size)
			{
				str[count] = 0;
			}
		}
	}
	for (int p = 0; p < 4; p++)
	{
		for (int q = 0; q < 28; q++)
		{
			rectangle(image, Point(160 + 40 * q, 560 + 40 * p), Point(200 + 40 * q, 600 + 40 * p), Scalar(255 * (double)str[count++]), FILLED, LINE_8);
			if (count >= size)
			{
				str[count] = 0;
			}
		}
	}
	std::string image_name = std::to_string(png_sum) + ".png";
	imwrite(image_name, image);
	waitKey(0);
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
	puts("Processed\n");
	return 0;
}
