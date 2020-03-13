#include "../../include/decode/decoder.h"

Decoder::Decoder(const char* _png_path):png_path(_png_path)
{
	is_text = false;
	real_text_len = 0;
}

Decoder::~Decoder()
{
}

int Decoder::decode(char* _input_video_path, char* _output_text_path)
{
	VideoCapture capture(_input_video_path);
	Mat frame;
	int fpp = 1;
	bool start = false;
	int i = 0;
	while (mp4_to_png(capture, fpp, frame))
	{
		bin_text = new bool[MAX_BIN_PER_IMAGE];
		if (!png_to_bin(frame)) { delete bin_text;continue; }
		else if (!start) { mp4_to_png(capture, fpp, frame);fpp = 3; start = true; }  //在第一检测到定位块以后，再往后调一帧，开始读取图片
		bin_to_text(_output_text_path);
		delete bin_text;
	}
	return 0;
}

bool Decoder::mp4_to_png(VideoCapture& capture, int fpp, Mat& frame)
{
	int frame_width = (int)capture.get(CAP_PROP_FRAME_WIDTH);
	int frame_height = (int)capture.get(CAP_PROP_FRAME_HEIGHT);
	float frame_fps = capture.get(CAP_PROP_FPS);
	int frame_number = capture.get(CAP_PROP_FRAME_COUNT);//总帧数
#ifdef DEBUG
	cout << "frame_width is " << frame_width << endl;
	cout << "frame_height is " << frame_height << endl;
	cout << "frame_fps is " << frame_fps << endl;
	cout << "frame_number is " << frame_number << endl;
#endif
	int frame_space = fpp * frame_fps / 30 + 0.5; //每两张不同图片之间的间隔 
	if (fpp == 1)
		if (capture.read(frame)) return true;
		else return false;
	for (int i = 0;i < frame_space;i++)
	{
		if (!capture.read(frame)) return false;
	}
	return true;
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
	/*第二版3-6区块，留查
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
	/*第一版第三区块，留查
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
			/******************由图可知最里面的矩形宽度占总宽的3/7***********************/
			if (rect.width < mask.cols * 2 / 7)      //2/7是为了防止一些微小的仿射
				continue;
			if (Ratete(dstGray(rect)) > 0.75)       //0.75是我测试几张图片的经验值 可根据情况设置(测试数量并不多)
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

//找出二维码框进行截取解码
int Decoder::find_Qr_anchor(Mat& srcImg, vector<vector<Point>>& qrPoint)
{
	if (!srcImg.data)
		return -1;
	Mat srcGray;
	int cols = srcImg.cols;
	int rows = srcImg.rows;

	vector<vector<Point>> contour2;

	cvtColor(srcImg, srcGray, COLOR_BGR2GRAY);
	//blur(srcGray, srcGray, Size(5, 5));

	//equalizeHist(srcGray, srcGray);

	threshold(srcGray, srcGray, 188, 255, THRESH_BINARY | THRESH_OTSU);

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
		//有两个子轮廓才是二维码的顶点
		if (ic >= 2)
		{
			bool isQr = Qr_point(contours[parentIdx], srcGray, parentIdx);
			//保存找到的四个黑色定位角
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
			if (isCorner(image))
			{
				Point2f points[4];
				rect.points(points);
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
		}
	}
	Mat output;

	if (qr_center.size() == 4)
	{
		anchor_sequence(qr_center);
		for (int i = 0; i < qr_center.size(); ++i)
			src_center.push_back(qr_center[i]);
		vector<Point2f> origin_center;
		origin_center.push_back(Point2f(cols * 73. / 1280, rows * 647. / 720));
		origin_center.push_back(Point2f(cols * 73. / 1280, rows * 73. / 720));
		origin_center.push_back(Point2f(cols * 1207. / 1280, rows * 73. / 720));
		origin_center.push_back(Point2f(cols * 1207. / 1280, rows * 647. / 720));
		Mat warp_mat = getPerspectiveTransform(src_center, origin_center);
		warpPerspective(srcGray, output, warp_mat, srcImg.size());
		recog_Qr(output);
		return 0;
	}
	else return -1;
}

bool Decoder::png_to_bin(Mat frame)
{
	Mat image;
	vector<vector<Point>> QrPoint;
	char png_name[64];
	image = frame;
	
	if (find_Qr_anchor(image, QrPoint) == -1)
		return false;
	return true;
	//find_Qr_anchor(image, QrPoint);
}

int Decoder::bin_to_text(char* _output_file_path)
{
	FILE* output_file = fopen(_output_file_path, "a");
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

		num_or_text(text[2 * i], output_file);
		num_or_text(text[2 * i + 1], output_file);
#ifdef DEBUG
		putchar(text[2 * i]);
		putchar(text[2 * i + 1]);
#endif
	}
	fclose(output_file);
#ifdef DEBUG
	putchar('\n');
	putchar('\n');
#endif
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

void Decoder::enter_text()
{
	is_text = true;
}

bool Decoder::check_text_state()
{
	return is_text;
}

void Decoder::num_or_text(char ch, FILE* output_file)
{
	if (!check_text_state())
	{
		if (ch != '@')
		{
			real_text_len *= 10;
			real_text_len += (ch - '0');
		}
		else
		{
			enter_text();
		}
	}
	else
	{
		
		if (real_text_len > 0)
		{
			fwrite(&ch, 1, 1, output_file);
			real_text_len--;
		}
	}
}
char* Decoder::getoutfile()
{
	char* output = new char[100];
	cout << "Please enter the output file name(with path):";
	cin >> output;
	FILE* fp;
	fopen_s(&fp, output, "rb");
	if ((fp == NULL))
	{
		cout << "error on open file!" << endl;
	}
	else
	{
		fclose(fp);
		return output;
	}
}
char* Decoder::getinfile()
{
	char* input = new char[100];
	char* decode = new char[100];
	cout << "输入指令:(decode)";
	cin >> decode;
	if (strcmp(decode, "decode"))
	{
		cout << "Wrong" << endl;
	}
	else
	{
		cout << "Please enter the input file name(with path):";
		cin >> input;
		if (input[strlen(input) - 1] == '4' && (input[strlen(input) - 2] == 'p' || input[strlen(input) - 2] == 'P') && (input[strlen(input) - 3] == 'm' || input[strlen(input) - 3] == 'M') && input[strlen(input) - 4] == '.')
		{
			return input;
		}
		else
		{
			cout << "error file name" << endl;
		}
	}
}