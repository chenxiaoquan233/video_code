#pragma once
#include "../base.h"

using namespace cv;
using namespace std;

class Decoder :public Code_base
{
private:
	char* pngs_path;
	bool* bin_text;
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
public:
	Decoder();
	~Decoder();
	void png_to_bin();
	bool Qr_rate(float rate);
	int recog_Qr(Mat& image1);
	int mp4_to_png(char* _video_path);
	bool IsQrColorRate(cv::Mat& image, int flag);
	bool Qr_color_rate_X(cv::Mat& image, int flag);
	bool Qr_color_rate_Y(cv::Mat& image, int flag);
	Mat& crop_image(Mat& img, RotatedRect& rotated_rect);
	bool Qr_point(vector<Point>& contour, Mat& img, int i);
	int decode(char* _input_video_path, char* _output_text_path);
	int find_Qr_anchor(Mat& srcImg, vector<vector<Point>>& qrPoint);
};