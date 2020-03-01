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
	int frame_number = capture.get(CAP_PROP_FRAME_COUNT);//��֡��
	cout << "frame_width is " << frame_width << endl;
	cout << "frame_height is " << frame_height << endl;
	cout << "frame_fps is " << frame_fps << endl;
	cout << "frame_number is " << frame_number << endl;
	int png_num = 0;//ͳ������ͼƬ������
	float frame_space = fpp * frame_fps / 30; //ÿ���Ų�ͬͼƬ֮��ļ��
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

	for (int p = 0; p < ANCHOR_BASE_BLOCKS / BLOCK_SIZE; p++)
	{
		for (int q = 0; q < IMG_Y / BLOCK_WIDTH - 2 * ANCHOR_BASE_BLOCKS / BLOCK_SIZE; q++)
		{
			uchar color = image.at<uchar>(Point(BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * q + BLOCK_WIDTH / 2, 0 + BLOCK_WIDTH * p + BLOCK_WIDTH / 2));
			if (color < 128) bin_text[count++] = false;// putchar('0');
			else bin_text[count++] = true;// putchar('1');
		}
	}
	for (int p = 0; p < IMG_X / BLOCK_WIDTH - 2 * ANCHOR_BASE_BLOCKS / BLOCK_SIZE; p++)
	{
		for (int q = 0; q < IMG_Y / BLOCK_WIDTH; q++)
		{
			uchar color = image.at<uchar>(Point(0 + BLOCK_WIDTH * q + BLOCK_WIDTH / 2, BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * p + BLOCK_WIDTH / 2));
			if (color < 128) bin_text[count++] = false;// , putchar('0');
			else bin_text[count++] = true;// , putchar('1');
		}
	}
	for (int p = 0; p < ANCHOR_BASE_BLOCKS / BLOCK_SIZE; p++)
	{
		for (int q = 0; q < IMG_Y / BLOCK_WIDTH -2 * ANCHOR_BASE_BLOCKS / BLOCK_SIZE; q++)
		{
			uchar color = image.at<uchar>(Point(BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * q + BLOCK_WIDTH / 2, IMG_X - BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * p + BLOCK_WIDTH / 2));
			if (color < 128) bin_text[count++] = false;// , putchar('0');
			else bin_text[count++] = true;// , putchar('1');
		}
	}
	/*�ڶ���3-6���飬����
	for (int p = 0; p < ANCHOR_BASE_BLOCKS / BLOCK_SIZE-MINI_ANCHOR_BASE_BLOCKS/BLOCK_SIZE-BOTTOM_BASE_BLOCKS/BLOCK_SIZE; p++)
	{
		for (int q = 0; q < IMG_Y / BLOCK_WIDTH - ANCHOR_BASE_BLOCKS / BLOCK_SIZE; q++)
		{
			uchar color = image.at<uchar>(Point(BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * q + BLOCK_WIDTH / 2, IMG_X - BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * p + BLOCK_WIDTH / 2));
			if (color < 128) bin_text[count++] = false;// , putchar('0');
			else bin_text[count++] = true;// , putchar('1');
		}
	}
	for (int p = 0; p < MINI_ANCHOR_BASE_BLOCKS / BLOCK_SIZE; p++)
	{
		for (int q = 0; q < IMG_Y / BLOCK_WIDTH - ANCHOR_BASE_BLOCKS / BLOCK_SIZE - MINI_ANCHOR_BASE_BLOCKS/BLOCK_SIZE - BOTTOM_BASE_BLOCKS/BLOCK_SIZE; q++)
		{
			uchar color = image.at<uchar>(Point(BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * q + BLOCK_WIDTH / 2, IMG_X - BASE_BLOCK_WIDTH * BOTTOM_BASE_BLOCKS-BASE_BLOCK_WIDTH * MINI_ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * p + BLOCK_WIDTH / 2));
			if (color < 128) bin_text[count++] = false;// , putchar('0');
			else bin_text[count++] = true;// , putchar('1');
		}
	}
	for (int p = 0; p < MINI_ANCHOR_BASE_BLOCKS / BLOCK_SIZE; p++)
	{
		for (int q = 0; q < BOTTOM_BASE_BLOCKS / BLOCK_SIZE; q++)
		{
			uchar color = image.at<uchar>(Point(IMG_Y - BASE_BLOCK_WIDTH * BOTTOM_BASE_BLOCKS + BLOCK_WIDTH * q + BLOCK_WIDTH / 2, IMG_X - BASE_BLOCK_WIDTH * BOTTOM_BASE_BLOCKS - BASE_BLOCK_WIDTH * MINI_ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * p + BLOCK_WIDTH / 2));
			if (color < 128) bin_text[count++] = false;// , putchar('0');
			else bin_text[count++] = true;// , putchar('1');
		}
	}
	for (int p = 0; p < BOTTOM_BASE_BLOCKS / BLOCK_SIZE; p++)
	{
		for (int q = 0; q < IMG_Y / BLOCK_WIDTH - ANCHOR_BASE_BLOCKS / BLOCK_SIZE; q++)
		{
			uchar color = image.at<uchar>(Point(BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * q + BLOCK_WIDTH / 2, IMG_X - BASE_BLOCK_WIDTH * BOTTOM_BASE_BLOCKS + BLOCK_WIDTH * p + BLOCK_WIDTH / 2));
			if (color < 128) bin_text[count++] = false;// , putchar('0');
			else bin_text[count++] = true;// , putchar('1');
		}
	}
	*/
	/*��һ��������飬����
	for (int p = 0; p < ANCHOR_BASE_BLOCKS / BLOCK_SIZE; p++)
	{
		for (int q = 0; q < IMG_Y / BLOCK_WIDTH - ANCHOR_BASE_BLOCKS / BLOCK_SIZE; q++)
		{
			uchar color = image.at<uchar>(Point(BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * q + BLOCK_WIDTH / 2, IMG_X - BASE_BLOCK_WIDTH * ANCHOR_BASE_BLOCKS + BLOCK_WIDTH * p + BLOCK_WIDTH / 2));
			if (color < 128) bin_text[count++] = false;// , putchar('0');
			else bin_text[count++] = true;// , putchar('1');
		}
	}
	*/
	return count;
}
bool Decoder::Qr_rate(float rate)
{
	//��ű��� ����̫�ϸ�
	return rate > 0.3 && rate < 1.9;
}
//����ڰױ����ж�
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

	//����ڰױ���1:1:3:1:1
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

	//��� �ұ� ��������ֵ������
	if (index < 2)
		return false;
	if ((vValueCount.size() - index) < 3)
		return false;

	//�ڰױ���1:1:3:1:1
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
//����ڰױ����ж�
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

	//����ڰױ���1:1:3:1:1
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

	//��� �ұ� ��������ֵ������
	if (index < 2)
		return false;
	if ((vValueCount.size() - index) < 3)
		return false;

	//�ڰױ���1:1:3:1:1
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
//�ڰױ����ж�
bool Decoder::IsQrColorRate(cv::Mat& image, int flag)
{
	if (!Qr_color_rate_X(image, flag))
		return false;
	return Qr_color_rate_Y(image, flag);;
}
//��ά�붨λ�������и�
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
//��λ��
bool Decoder::Qr_point(vector<Point>& contour, Mat& img, int i)
{
	//��С��С�޶�
	RotatedRect rotated_rect = minAreaRect(contour);
	if (rotated_rect.size.height < 40  || rotated_rect.size.width < 40)
		return false;
	//����ά�������ͼ�Ͽٳ���
	cv::Mat cropImg = crop_image(img, rotated_rect);
	int flag = i++;
	//����ڰױ���1:1:3:1:1
	bool result = IsQrColorRate(cropImg, flag);
	return result;
}

Mat transformCorner(Mat src, RotatedRect rect)
{
	Point center = rect.center;
	Point TopLeft = Point(cvRound(center.x), cvRound(center.y)) - Point(rect.size.height / 2, rect.size.width / 2);
	TopLeft.x = TopLeft.x > src.cols ? src.cols : TopLeft.x;
	TopLeft.x = TopLeft.x < 0 ? 0 : TopLeft.x;
	TopLeft.y = TopLeft.y > src.rows ? src.rows : TopLeft.y;
	TopLeft.y = TopLeft.y < 0 ? 0 : TopLeft.y;

	Rect RoiRect = Rect(TopLeft.x, TopLeft.y, rect.size.width, rect.size.height);
	double angle = rect.angle;
	Mat mask, roi, dst;
	Mat image;
	Size sz = src.size();
	mask = Mat::zeros(src.size(), CV_8U);
	vector<Point> contour;
	Point2f points[4];
	rect.points(points);
	for (int i = 0; i < 4; i++)
		contour.push_back(points[i]);
	vector<vector<Point>> contours;
	contours.push_back(contour);
	drawContours(mask, contours, 0, Scalar(1), -1);

	src.copyTo(dst, mask);
	//roi = dst(RoiRect);
	Mat M = getRotationMatrix2D(center, angle, 1);
	warpAffine(dst, image, M, sz);
	roi = image(RoiRect);

	return roi;
}
double Ratete(Mat count)
{
	int number = 0;
	int allpixel = 0;
	for (int row = 0; row < count.rows; row++)
	{
		for (int col = 0; col < count.cols; col++)
		{
			if (count.at<uchar>(row, col) == 255)
			{
				number++;
			}
			allpixel++;
		}
	}
	return (double)number / allpixel;
}
bool isCorner(Mat& image)
{
	Mat mask, dstGopy;
	Mat dstGray;
	mask = image.clone();
	cvtColor(image, dstGray, COLOR_BGR2GRAY);
	threshold(dstGray, dstGray, 100, 255, THRESH_BINARY_INV);
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(dstGray, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
	for (int i = 0; i < contours.size(); i++)
	{
		if (hierarchy[i][2] == -1 && hierarchy[i][3])
		{
			Rect rect = boundingRect(Mat(contours[i]));
			rectangle(image, rect, Scalar(0, 0, 255), 2);
			/******************��ͼ��֪������ľ��ο���ռ�ܿ���3/7***********************/
			if (rect.width < mask.cols * 2 / 7)      //2/7��Ϊ�˷�ֹһЩ΢С�ķ���
				continue;
			if (Ratete(dstGray(rect)) > 0.75)       //0.75���Ҳ��Լ���ͼƬ�ľ���ֵ �ɸ����������(��������������)
			{
				rectangle(mask, rect, Scalar(0, 0, 255), 2);
				return true;
			}
		}
	}
	return  false;
}

void anchor_sequence(vector<Point2f>& anchor_center)
{
	Point2f tmp_center[4],tmp;
	for (int i = 0;i < 4;i++)
		tmp_center[i] = anchor_center[i];
	for(int j=0;j<4;j++)
		for (int i = 0;i < 3;i++)
		{
			if ((tmp_center[i].x + tmp_center[i].y) > (tmp_center[i + 1].x + tmp_center[i + 1].y))
			{
				tmp = tmp_center[i]; tmp_center[i] = tmp_center[i + 1]; tmp_center[i + 1] = tmp;
			}
		}
	if (tmp_center[1].y < tmp_center[2].y) {
		tmp = tmp_center[1];tmp_center[1] = tmp_center[2];tmp_center[2] = tmp;
	}
	for (int i = 0;i < 4;i++)
		anchor_center.pop_back();
	anchor_center.push_back(tmp_center[1]);
	anchor_center.push_back(tmp_center[0]);
	anchor_center.push_back(tmp_center[2]);
	anchor_center.push_back(tmp_center[3]);

/*	Point2f tmp_center[3];
	double a0, a1, a2;
	a0 = abs((anchor_center[1].x - anchor_center[0].x) * (anchor_center[2].x - anchor_center[0].x) + (anchor_center[1].y - anchor_center[0].y) * (anchor_center[2].y - anchor_center[0].y));
	a1 = abs((anchor_center[0].x - anchor_center[1].x) * (anchor_center[2].x - anchor_center[1].x) + (anchor_center[0].y - anchor_center[1].y) * (anchor_center[2].y - anchor_center[1].y));
	a2 = abs((anchor_center[1].x - anchor_center[2].x) * (anchor_center[0].x - anchor_center[2].x) + (anchor_center[1].y - anchor_center[2].y) * (anchor_center[0].y - anchor_center[2].y));
	if (a0 <= a1 && a0 <= a2) tmp_center[1] = anchor_center[0], anchor_center.erase(anchor_center.begin() + 0);
	if (a1 <= a0 && a1 <= a2) tmp_center[1] = anchor_center[1], anchor_center.erase(anchor_center.begin() + 1);
	if (a2 <= a1 && a2 <= a0) tmp_center[1] = anchor_center[2], anchor_center.erase(anchor_center.begin() + 2);
	double d0, d2;
	d0 = abs((anchor_center[0].x - tmp_center[1].x) * (anchor_center[0].x - tmp_center[1].x) + (anchor_center[0].y - tmp_center[1].y) * (anchor_center[0].y - tmp_center[1].y));
	d2 = abs((anchor_center[1].x - tmp_center[1].x) * (anchor_center[1].x - tmp_center[1].x) + (anchor_center[1].y - tmp_center[1].y) * (anchor_center[1].y - tmp_center[1].y));
	if (d0 > d2) tmp_center[2] = anchor_center[0], tmp_center[0] = anchor_center[1];
	else tmp_center[0] = anchor_center[0], tmp_center[2] = anchor_center[1];
	anchor_center.pop_back();
	anchor_center.pop_back();
	anchor_center.push_back(tmp_center[0]);
	anchor_center.push_back(tmp_center[1]);
	anchor_center.push_back(tmp_center[2]); */
}

//�ҳ���ά�����н�ȡ����
int Decoder::find_Qr_anchor(Mat& srcImg, vector<vector<Point>>& qrPoint)
{
	/*//��ɫͼת�Ҷ�ͼ
	Mat srcGray;
	cvtColor(srcImg, srcGray, COLOR_BGR2GRAY);

	//��ֵ��
	Mat threshold_output;
	threshold(srcGray, threshold_output, 0, 255, THRESH_BINARY | THRESH_OTSU);
	Mat threshold_output_copy = threshold_output.clone();
	imwrite("threshold.png", threshold_output_copy);
	//���ò�����������
	findContours(threshold_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE, Point(0, 0));
	Mat srcImg_copy = srcImg.clone();
	for (int i = 0; i < contours.size(); i++)
	{
		Rect rect = boundingRect((Mat)contours[i]);
		float c = (float)rect.width / rect.height;
		rectangle(srcImg_copy, rect, Scalar(255), 2);
	}
	//ͨ����ɫ��λ����Ϊ�����������������������ص㣬ɸѡ��������λ��
	int parentIdx = -1;
	imwrite("contours.png", srcImg_copy);
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
		//���������������Ƕ�ά��Ķ���
		if (ic >= 2)
		{
			bool isQr = Qr_point(contours[parentIdx], threshold_output, parentIdx);
			//�����ҵ���������ɫ��λ��
			if (isQr)
				qrPoint.push_back(contours[parentIdx]);
			parentIdx = -1;
		}
	}
	cout << "��ʶ��λ����Ŀ��" << qrPoint.size() << endl;
	//���ƶ�ά�붨λ��
	for (int i = 0; i < qrPoint.size(); i++)
	{
		Rect rect = boundingRect((Mat)qrPoint[i]);
		rectangle(srcImg, rect, Scalar(255), 2);
	}
	imwrite("anchors.png", srcImg);*/

	if (!srcImg.data)
		return -1;
	Mat srcGray;
	resize(srcImg, srcImg, Size(1280,720));

	vector<vector<Point>> contour2;

	cvtColor(srcImg, srcGray, COLOR_BGR2GRAY);
	blur(srcGray, srcGray, Size(3, 3));
	equalizeHist(srcGray, srcGray);

	threshold(srcGray, srcGray, 20, 255, THRESH_BINARY | THRESH_OTSU);

	findContours(srcGray, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
	
	int ic = 0;
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
		//���������������Ƕ�ά��Ķ���
		if (ic >= 2)
		{
			bool isQr = Qr_point(contours[parentIdx], srcGray, parentIdx);
			//�����ҵ����ĸ���ɫ��λ��
			if (isQr)
				contour2.push_back(contours[parentIdx]);
			parentIdx = -1;
		}
	}

	vector<Point2f> qr_center,src_center;
	for (int i = 0; i < contour2.size(); i++)
	{
		
		double area = contourArea(contour2[i]);
		if (area < 100)
			continue;
		RotatedRect rect = minAreaRect(Mat(contour2[i]));
		double w = rect.size.width;
		double h = rect.size.height;
		double rate = min(w, h) / max(w, h);
		if (rate > 0.85)
		{
			Mat image = transformCorner(srcImg, rect);
			char tmp_name[20];
			sprintf(tmp_name, "cor%d.png", i);
			imwrite(tmp_name, image);
			if (isCorner(image))
			{
				Point2f points[4];
				rect.points(points);
				/*for (int i = 0; i < 4; i++)
					line(srcImg, points[i], points[(i + 1) % 4], Scalar(0, 255, 255), 5);*/
				qr_center.push_back(rect.center);
				qrPoint.push_back(contour2[i]);
			}
		}
	}

	for (int i = 0; i < qrPoint.size(); i++)
	{
		RotatedRect rect = minAreaRect(qrPoint[i]);
		Point2f boxpoint[4];
		rect.points(boxpoint);
		qrPoint[i].clear();
		for (int j = 0; j < 4; j++)
		{
			qrPoint[i].push_back(boxpoint[j]);
			line(srcImg, boxpoint[i], boxpoint[(i + 1) % 4], Scalar(0, 0, 255), 3);
		}
	}
	imwrite("box.png", srcImg);
	Mat output;
	//warpAffine(srcImg, output, M, Size(720,1280));
	
	/*imshow("srcImg", srcImg);
	waitKey(0);*/


	/*vector<Point2f> qr_center,src_center;					//������λ�ǵ�����
	vector<int> state(qrPoint.size(), 0);	//״̬��������¼�ö�λ���Ƿ�����ĳ��������ά�루ֻ��������λ��ͬʱ���ֲ���Ϊ��һ�������Ķ�ά�룩
	vector<vector<Point>> qrBox;			//�������ж�ά������λ�ÿ�
	for (int i = 0; i < qrPoint.size(); i++)
	{
		Point2f tmp = qrPoint[i][0];
		printf("%d,%d ",qrPoint[i][0].x,qrPoint[i][0].y);
		for (int m = 1; m < qrPoint[i].size(); m++)
			tmp += Point2f(qrPoint[i][m]), printf("%d,%d ", qrPoint[i][m].x, qrPoint[i][m].y);
		tmp = tmp / (int)qrPoint[i].size();
		qr_center.push_back(tmp);
		printf("%f,%f\n", tmp.x, tmp.y);
	}*/
	if (qr_center.size() == 4)
	{
		anchor_sequence(qr_center);
		for (int i = 0; i < qr_center.size(); ++i)
			src_center.push_back(qr_center[i]);
		//src_center.push_back(Point2f(qr_center[2].x + (qr_center[0].x - qr_center[1].x), qr_center[0].y + (qr_center[2].y - qr_center[1].y)));
		for (int i = 0; i < src_center.size(); ++i)
			line(srcImg, src_center[i], src_center[(i + 1) % 4], Scalar(0, 255, 255), 5);
		imwrite("centers.png", srcImg);
		vector<Point2f> origin_center;
		origin_center.push_back(Point2f(73, 647));
		origin_center.push_back(Point2f(73, 73));
		origin_center.push_back(Point2f(1207, 73));
		origin_center.push_back(Point2f(1207, 647));
		Mat warp_mat = getPerspectiveTransform(src_center, origin_center);
		warpPerspective(srcGray, output, warp_mat, srcImg.size());
		imwrite("test.png", output);
		recog_Qr(output);
	}
	return 0;

	/*for (int i = 0; i < qrPoint.size(); i++) {
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
					qrBox.push_back(box);//��ά�붨λ��
					Mat img_tmp;
					img_tmp = crop_image(srcGray, rotated_rect);
					recog_Qr(img_tmp);
					return 0;
				}
			}
		}
	}*/
}
void Decoder::png_to_bin(int num)
{
	Mat image;
	vector<vector<Point>> QrPoint;
	char png_name[64];
	sprintf(png_name, "../example/pngs/encode/%d.png",num);
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