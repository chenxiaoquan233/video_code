#include "../../include/decode/decoder.h"

Decoder::Decoder(const char* _png_path):png_path(_png_path)
{
}

Decoder::~Decoder()
{
}

int Decoder::decode(char* _input_video_path, char* _output_text_path)
{
	int png_num = mp4_to_png(_input_video_path, 3);
	for (int i = 0; i < png_num; ++i)
	{
		bin_text = new bool[MAX_BIN_PER_IMAGE];
		png_to_bin(i);
		bin_to_text();
		delete bin_text;
	}

	return 0;
}

int Decoder::mp4_to_png(char* _video_path, int fpp)
{
	VideoCapture capture(_video_path);
	int frame_width = (int)capture.get(CAP_PROP_FRAME_WIDTH);
	int frame_height = (int)capture.get(CAP_PROP_FRAME_HEIGHT);
	float frame_fps = capture.get(CAP_PROP_FPS);
	int frame_number = capture.get(CAP_PROP_FRAME_COUNT);//总帧数
	cout << "frame_width is " << frame_width << endl;
	cout << "frame_height is " << frame_height << endl;
	cout << "frame_fps is " << frame_fps << endl;
	cout << "frame_number is " << frame_number << endl;
	int png_num = 0;//统计生成图片的数量
	float frame_space = fpp * frame_fps / 30; //每两张不同图片之间的间隔
	Mat frame;
	for (int i = 0; i < frame_number-1; i++)
	{
		capture.read(frame);
		if (i == (int)(png_num * frame_space))
		{
			char image_name[32];
			sprintf(image_name, "%s%d.png", png_path, png_num);
			imwrite(image_name, frame);
			png_num++;
		}
	}
	return png_num;
}

int Decoder::recog_Qr(Mat& image1)
{
	int count = 0;
	Mat image = Mat::zeros(720, 1280, CV_8UC3);
	resize(image1, image, image.size());
	//Scalar color1 = image.at<uchar>(10, 10);//黑0
	//Scalar color2 = image.at<uchar>(150, 150);//白1

	for (int p = 0; p < ANCHOR_BASE_BLOCKS / BLOCK_SIZE; p++)
	{
		for (int q = 0; q < IMG_Y / BLOCK_WIDTH - 2 * ANCHOR_BASE_BLOCKS / BLOCK_SIZE; q++)
		{
			uchar color = image.at<uchar>(Point(BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * q + BLOCK_WIDTH / 2, 0 + BLOCK_WIDTH * p + BLOCK_WIDTH / 2));
			if (color < 128) bin_text[count++] = false;
			else bin_text[count++] = true;
		}
	}
	for (int p = 0; p < IMG_X / BLOCK_WIDTH - 2 * ANCHOR_BASE_BLOCKS / BLOCK_SIZE; p++)
	{
		for (int q = 0; q < IMG_Y / BLOCK_WIDTH; q++)
		{
			uchar color = image.at<uchar>(Point(0 + BLOCK_WIDTH * q + BLOCK_WIDTH / 2, BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * p + BLOCK_WIDTH / 2));
			if (color < 128) bin_text[count++] = false;
			else bin_text[count++] = true;;
		}
	}
	for (int p = 0; p < ANCHOR_BASE_BLOCKS / BLOCK_SIZE; p++)
	{
		for (int q = 0; q < IMG_Y / BLOCK_WIDTH - ANCHOR_BASE_BLOCKS / BLOCK_SIZE; q++)
		{
			uchar color = image.at<uchar>(Point(BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * q + BLOCK_WIDTH / 2, IMG_X - BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * p + BLOCK_WIDTH / 2));
			if (color < 128) bin_text[count++] = false;
			else bin_text[count++] = true;
		}
	}
	return count;
}

bool Decoder::Qr_rate(float rate)
{
	//大概比例 不能太严格
	return rate > 0.3 && rate < 1.9;
}
//横向黑白比例判断
bool Decoder::Qr_color_rate_X(cv::Mat& image, int flag)
{
	int nr = image.rows / 2;
	int nc = image.cols * image.channels();

	vector<int> vValueCount;
	vector<uchar> vColor;
	int count = 0;
	uchar lastColor = 0;

	uchar* data = image.ptr<uchar>(nr);
	for (int i = 0; i < nc; i++)
	{
		vColor.push_back(data[i]);
		uchar color = data[i];
		if (color > 0)
			color = 255;

		if (i == 0)
		{
			lastColor = color;
			count++;
		}
		else
		{
			if (lastColor != color)
			{
				vValueCount.push_back(count);
				count = 0;
			}
			count++;
			lastColor = color;
		}
	}

	if (count != 0)
		vValueCount.push_back(count);

	if (vValueCount.size() < 5 || vValueCount.size() > 7)
		return false;

	//横向黑白比例1:1:3:1:1
	int index = -1;
	int maxCount = -1;
	for (int i = 0; i < vValueCount.size(); i++)
	{
		if (i == 0)
		{
			index = i;
			maxCount = vValueCount[i];
		}
		else
		{
			if (vValueCount[i] > maxCount)
			{
				index = i;
				maxCount = vValueCount[i];
			}
		}
	}

	//左边 右边 都有两个值，才行
	if (index < 2)
		return false;
	if ((vValueCount.size() - index) < 3)
		return false;

	//黑白比例1:1:3:1:1
	float rate = ((float)maxCount) / 3.00;

	if (!Qr_rate(vValueCount[index - 2] / rate))
		return false;
	if (!Qr_rate(vValueCount[index - 1] / rate))
		return false;
	if (!Qr_rate(vValueCount[index + 1] / rate))
		return false;
	if (!Qr_rate(vValueCount[index + 2] / rate))
		return false;

	return true;
}
//纵向黑白比例判断
bool Decoder::Qr_color_rate_Y(cv::Mat& image, int flag) {
	int nc = image.cols / 2;
	int nr = image.rows;

	vector<int> vValueCount;
	int count = 0;
	uchar lastColor = 0;

	for (int i = 0; i < nr; i++)
	{
		uchar* data = image.ptr<uchar>(i, nc);
		uchar color;
		if (data[0] > 0 || data[1] > 0 || data[2] > 0)
			color = 255;
		else
			color = 0;

		if (i == 0)
		{
			lastColor = color;
			count++;
		}
		else
		{
			if (lastColor != color)
			{
				vValueCount.push_back(count);
				count = 0;
			}
			count++;
			lastColor = color;
		}
	}

	if (count != 0)
		vValueCount.push_back(count);

	if (vValueCount.size() < 5 || vValueCount.size() > 7)
		return false;

	//横向黑白比例1:1:3:1:1
	int index = -1;
	int maxCount = -1;
	for (int i = 0; i < vValueCount.size(); i++)
	{
		if (i == 0)
		{
			index = i;
			maxCount = vValueCount[i];
		}
		else
		{
			if (vValueCount[i] > maxCount)
			{
				index = i;
				maxCount = vValueCount[i];
			}
		}
	}

	//左边 右边 都有两个值，才行
	if (index < 2)
		return false;
	if ((vValueCount.size() - index) < 3)
		return false;

	//黑白比例1:1:3:1:1
	float rate = ((float)maxCount) / 3.00;

	if (!Qr_rate(vValueCount[index - 2] / rate))
		return false;
	if (!Qr_rate(vValueCount[index - 1] / rate))
		return false;
	if (!Qr_rate(vValueCount[index + 1] / rate))
		return false;
	if (!Qr_rate(vValueCount[index + 2] / rate))
		return false;

	return true;
}
//黑白比例判断
bool Decoder::IsQrColorRate(cv::Mat& image, int flag)
{
	if (!Qr_color_rate_X(image, flag))
		return false;
	return Qr_color_rate_Y(image, flag);;
}
//二维码定位角区域切割
Mat Decoder::crop_image(Mat& img, RotatedRect& rotatedRect) {
	Point2f points[4];
	rotatedRect.points(points);
	int topLeftIndex = 0;
	double topLeftR = points[0].x * points[0].x + points[0].y * points[0].y;
	for (int i = 1; i < 4; i++) {
		double r = points[i].x * points[i].x + points[i].y * points[i].y;
		if (r < topLeftR) {
			topLeftIndex = i;
			topLeftR = r;
		}
	}
	double x1 = points[(topLeftIndex + 1) % 4].x - points[topLeftIndex].x, y1 = points[(topLeftIndex + 1) % 4].y - points[topLeftIndex].y;
	double x2 = points[(topLeftIndex + 3) % 4].x - points[topLeftIndex].x, y2 = points[(topLeftIndex + 3) % 4].y - points[topLeftIndex].y;
	double vX1 = x1, vY1 = y1, vX2 = x2, vY2 = y2;
	int width = (int)sqrt(vX1 * vX1 + vY1 * vY1), height = (int)sqrt(vX2 * vX2 + vY2 * vY2);
	if (img.dims == 3) {
		Mat ret(Size(width, height), CV_8UC3);
		for (int j = 0; j < ret.rows; j++) {
			for (int i = 0; i < ret.cols; i++) {
				double kx = (double)i / width, ky = (double)j / height;
				int x = (int)(points[topLeftIndex].x + kx * vX1 + ky * vX2), y = (int)(points[topLeftIndex].y + kx * vY1 + ky * vY2);
				if (x < 0)x = 0;
				else if (x >= img.cols)x = img.cols - 1;
				if (y < 0)y = 0;
				else if (y >= img.rows)y = img.rows - 1;
				ret.at<Vec3b>(j, i) = img.at<Vec3b>(y, x);
			}
		}
		return ret;
	}
	else {
		Mat ret(Size(width, height), CV_8UC1);
		for (int j = 0; j < ret.rows; j++) {
			for (int i = 0; i < ret.cols; i++) {
				double kx = (double)i / width, ky = (double)j / height;
				int x = (int)(points[topLeftIndex].x + kx * vX1 + ky * vX2), y = (int)(points[topLeftIndex].y + kx * vY1 + ky * vY2);
				if (x < 0)x = 0;
				else if (x >= img.cols)x = img.cols - 1;
				if (y < 0)y = 0;
				else if (y >= img.rows)y = img.rows - 1;
				ret.at<uchar>(j, i) = img.at<uchar>(y, x);
			}
		}
		return ret;
	}
}
//定位角
bool Decoder::Qr_point(vector<Point>& contour, Mat& img, int i)
{
	//最小大小限定
	RotatedRect rotated_rect = minAreaRect(contour);
	if (rotated_rect.size.height < 40  || rotated_rect.size.width < 40)
		return false;
	//将二维码从整个图上抠出来
	cv::Mat cropImg = crop_image(img, rotated_rect);
	int flag = i++;
	//横向黑白比例1:1:3:1:1
	bool result = IsQrColorRate(cropImg, flag);
	return result;
}
//找出二维码框进行截取解码
int Decoder::find_Qr_anchor(Mat& srcImg, vector<vector<Point>>& qrPoint)
{
	//彩色图转灰度图
	Mat src_gray;
	cvtColor(srcImg, src_gray, COLOR_BGR2GRAY);

	//二值化
	Mat threshold_output;
	threshold(src_gray, threshold_output, 0, 255, THRESH_BINARY | THRESH_OTSU);
	Mat threshold_output_copy = threshold_output.clone();
	//调用查找轮廓函数
	findContours(threshold_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE, Point(0, 0));
	Mat srcImg_copy = srcImg.clone();
	for (int i = 0; i < contours.size(); i++)
	{
		Rect rect = boundingRect((Mat)contours[i]);
		float c = (float)rect.width / rect.height;
		rectangle(srcImg_copy, rect, Scalar(255), 2);
	}
	//通过黑色定位角作为父轮廓，有两个子轮廓的特点，筛选出三个定位角
	int parentIdx = -1;

	for (int i = 0; i < contours.size(); i++)
	{
		int k = i;
		int ic = 0;
		while (hierarchy[k][2] != -1)
		{
			if (ic == 0)
				parentIdx = i;
			k = hierarchy[k][2];
			ic++;
		}
		//有两个子轮廓才是二维码的顶点
		if (ic >= 2)
		{
			bool isQr = Qr_point(contours[parentIdx], threshold_output, parentIdx);
			//保存找到的三个黑色定位角
			if (isQr)
				qrPoint.push_back(contours[parentIdx]);
			parentIdx = -1;
		}
	}
	cout << "已识别定位点数目：" << qrPoint.size() << endl;
	//绘制二维码定位角
	for (int i = 0; i < qrPoint.size(); i++)
	{
		Rect rect = boundingRect((Mat)qrPoint[i]);
		rectangle(srcImg, rect, Scalar(255), 2);
	}
	vector<Point> qr_center;					//各个定位角的中心
	vector<int> state(qrPoint.size(), 0);	//状态变量，记录该定位角是否属于某个完整二维码（只有三个定位角同时出现才认为是一个完整的二维码）
	vector<vector<Point>> qrBox;			//保存所有二维码整体位置框
	for (int i = 0; i < qrPoint.size(); i++)
	{
		Point tmp = qrPoint[i][0];
		for (int m = 1; m < qrPoint[i].size(); m++)
			tmp += qrPoint[i][m];
		tmp = tmp / (int)qrPoint[i].size();
		qr_center.push_back(tmp);
	}
	for (int i = 0; i < qrPoint.size(); i++) {
		if (state[i] == 1) continue;
		for (int j = 0; j < qrPoint.size(); j++) {
			if (j == i || state[j] == 1) continue;
			for (int k = 0; k < qrPoint.size(); k++) {
				if (k == j || k == i || state[k] == 1)continue;

				float Dij, Dik, Djk;
				Dij = (qr_center[i].x - qr_center[j].x) * (qr_center[i].x - qr_center[j].x) + (qr_center[i].y - qr_center[j].y) * (qr_center[i].y - qr_center[j].y);
				Dik = (qr_center[i].x - qr_center[k].x) * (qr_center[i].x - qr_center[k].x) + (qr_center[i].y - qr_center[k].y) * (qr_center[i].y - qr_center[k].y);
				Djk = (qr_center[k].x - qr_center[j].x) * (qr_center[k].x - qr_center[j].x) + (qr_center[k].y - qr_center[j].y) * (qr_center[k].y - qr_center[j].y);
				float ratio = Dij / Dik;
				float ratio1 = (Dij + Dik) / Djk;
				if (ratio > 0.2 && ratio < 1 && ratio1>0.85 && ratio1 < 1.15)
				{
					state[i] = 1;
					state[j] = 1;
					state[k] = 1;
					vector<Point> contour;

					contour.insert(contour.end(), qrPoint[i].begin(), qrPoint[i].end());
					contour.insert(contour.end(), qrPoint[j].begin(), qrPoint[j].end());
					contour.insert(contour.end(), qrPoint[k].begin(), qrPoint[k].end());
					RotatedRect rotated_rect = minAreaRect(contour);
					Point2f points[4];
					rotated_rect.points(points);
					vector<Point> box;
					for (int m = 0; m < 4; m++)
						box.push_back(points[m]);
					qrBox.push_back(box);//二维码定位框
					Mat img_tmp;
					img_tmp = crop_image(src_gray, rotated_rect);
					recog_Qr(img_tmp);
					return 0;
				}
			}
		}
	}
}

void Decoder::png_to_bin(int num)
{
	Mat image;
	vector<vector<Point>> QrPoint;
	char png_name[32];
	sprintf(png_name, "../example/decode/pngs/%d.png", num);
	image = imread(png_name);
	find_Qr_anchor(image, QrPoint);
}
int Decoder::bin_to_text()
{
	int char_sum = MAX_BIN_PER_IMAGE / 26 * 2;
	text = new char[char_sum];
	unsigned int tmp_code = 0;
	for (int i = 0; i < char_sum / 2; ++i)
	{
		tmp_code = 0;
		for (int j = 0; j < 26; ++j)
		{
			tmp_code += bin_text[26 * i + j]<<j;
		}
		tmp_code = CorrectError(tmp_code);
		text[2 * i] = 0;
		text[2 * i + 1] = 0;
		for (int j = 8; j < 16; j++)
			text[2 * i] += (tmp_code & (1 << j)) >> 8;
		for (int j = 0; j < 8; j++)
			text[2 * i + 1] += tmp_code & (1 << j);
		putchar(text[2 * i]);
		putchar(text[2 * i + 1]);
	}
	putchar('\n');
	return char_sum;
}
unsigned int Decoder::GetFEC(unsigned int CX) {
	unsigned int RX;
	unsigned int tmp;
	unsigned int gx = 0x05B9 << (26 - 11);

	CX = CX << 10;
	tmp = CX;
	for (int i = 0; i < 16; i++) 
	{
		if ((CX & 0x2000000) != 0) 
		{
			CX ^= gx;
		}
		CX = CX << 1;
	}
	RX = CX >> 16;
	return RX;
}
void Decoder::CreateCheckMatrix() 
{
	unsigned int RX, CX;
	CX = 1 << 15;
	for (int i = 0; i < 16; i++) {
		if (i < 16) {
			RX = GetFEC(CX);
			CheckMatrix[i][0] = RX;
			CheckMatrix[i][1] = CX << 10;
		}
		CX = CX >> 1;
	}
	CX = 1 << 9;
	for (int i = 0; i < 10; i++) {
		CheckMatrix[i + 16][0] = CX;
		CheckMatrix[i + 16][1] = CX;
		CX = CX >> 1;
	}
}
unsigned int Decoder::CorrectError(unsigned int code) {
	unsigned int encode = 0;
	unsigned int decode = 0;
	unsigned int gx = 0x05B9 << (26 - 11);
	unsigned int res;
	decode = code;
	for (int i = 0; i < 16; i++)
	{
		if ((code & 0x2000000) != 0) {
			code ^= gx;
		}
		code = code << 1;
	}
	res = code >> (26 - 10);
	if (res == 0) {
		return decode >> 10;
	}
	//2.2 correct one bit error
	for (int i = 0; i < 26; i++) {
		if (res == CheckMatrix[i][0]) {
			decode = decode ^ CheckMatrix[i][1];
			return decode >> 10;
		}
	}
	//2.3 correct two bit error
	for (int i = 0; i < 26; i++) {
		for (int j = i + 1; j < 26; j++) {
			if (res == (CheckMatrix[i][0] ^ CheckMatrix[j][0])) {
				decode = decode ^ CheckMatrix[i][1] ^ CheckMatrix[j][1];
				return decode >> 10;
			}
		}
	}
	return decode >> 10;
}