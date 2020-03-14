#pragma once
#include "../base.h"

using namespace cv;
using namespace std;

class Decoder :public Code_base
{
private:
	bool* bin_text = nullptr;
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	const char* png_path = nullptr;
	char* text = nullptr;
	unsigned int CheckMatrix[26][2];
	bool is_text;
	int real_text_len;
public:
	Decoder(const char* _png_path);
	~Decoder();
	bool png_to_bin(Mat frame);
	bool Qr_rate(float rate);
	int recog_Qr(Mat& image1);
	int bin_to_text(char* _output_text_path);
	bool mp4_to_png(VideoCapture& capture, int fpp, Mat& frame);
	bool IsQrColorRate(cv::Mat& image, int flag);
	bool Qr_color_rate_X(cv::Mat& image, int flag);
	bool Qr_color_rate_Y(cv::Mat& image, int flag);
	Mat crop_image(Mat& img, RotatedRect& rotated_rect);
	bool Qr_point(vector<Point>& contour, Mat& img, int i);
	int decode(char* _input_video_path, char* _output_text_path);
	int find_Qr_anchor(Mat& srcImg, vector<vector<Point>>& qrPoint);
	unsigned int GetFEC(unsigned int CX);
	unsigned int CorrectError(unsigned int code);
	void CreateCheckMatrix();
	void enter_text();
	bool check_text_state();
	void num_or_text(char ch, FILE* output_file);
	char* Parameterjudgement(char* _input_file_name, char* _output_file_name);
};
