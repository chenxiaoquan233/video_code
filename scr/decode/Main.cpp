#include "../../include/decode/decoder.h"

using namespace cv;

int main(int argc, char** argv)
{
	Decoder* decoder = new Decoder("../example/decode/pngs/");
	decoder->decode(argv[1],argv[2]);
}


/*#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

Mat transformCorner(Mat src, RotatedRect rect);
bool isCorner(Mat& image);
double Ratete(Mat count);
int main()
{
	Mat src = imread("../example/decode/pngs/0.png");
	if (!src.data)
		return -1;
	Mat srcGray;

	cvtColor(src, srcGray, COLOR_BGR2GRAY);
	blur(srcGray, srcGray, Size(3, 3));
	equalizeHist(srcGray, srcGray);

	threshold(srcGray, srcGray, 0, 255, THRESH_BINARY | THRESH_OTSU);
	imwrite("threshold.png", srcGray);

	vector<vector<Point>> contours, qrPoint;
	vector<Vec4i> hierarchy;
	findContours(srcGray, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
	int ic = 0;
	int parentIdx = -1;

	for (int i = 0; i < contours.size(); i++)
	{
		if (hierarchy[i][2] != -1 && ic == 0)
		{
			parentIdx = i;
			ic++;
		}
		else if (hierarchy[i][2] != -1)
		{
			ic++;
		}
		else if (hierarchy[i][2] == -1)
		{
			parentIdx = -1;
			ic = 0;
		}
		if (ic >= 2)
		{
			qrPoint.push_back(contours[parentIdx]);
			ic = 0;
			parentIdx = -1;
		}
	}

	vector<Point> center_all;
	for (int i = 0; i < qrPoint.size(); i++)
	{
		double area = contourArea(qrPoint[i]);
		if (area < 100)
			continue;
		RotatedRect rect = minAreaRect(Mat(qrPoint[i]));
		double w = rect.size.width;
		double h = rect.size.height;
		double rate = min(w, h) / max(w, h);
		if (rate > 0.85)
		{
			Mat image = transformCorner(src, rect);
			if (isCorner(image))
			{
				Point2f points[4];
				rect.points(points);
				for (int i = 0; i < 4; i++)
					line(src, points[i], points[(i + 1) % 4], Scalar(0, 255, 0), 2);
				center_all.push_back(rect.center);
			}
		}
	}
	vector<vector<Point>> contours3;

	for (int i = 0; i < qrPoint.size(); i++)
	{
		RotatedRect rect = minAreaRect(qrPoint[i]);
		Point2f boxpoint[4];
		rect.points(boxpoint);
		for (int i = 0; i < 4; i++)
			line(src, boxpoint[i], boxpoint[(i + 1) % 4], Scalar(0, 0, 255), 3);
	}

	imwrite("src.png", src);
	waitKey(0);
	return 0;
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
			if (rect.width < mask.cols * 2 / 7)      //2/7是为了防止一些微小的仿射
				continue;
			if (Ratete(dstGray(rect)) > 0.75)       //0.75是我测试几张图片的经验值 可根据情况设置(测试数量并不多)
			{
				rectangle(mask, rect, Scalar(0, 0, 255), 2);
				return true;
			}
		}
	}
	imwrite("dstGray.png", image);
	imwrite("mask.png", dstGray);
	return  false;
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
	cout << (double)number / allpixel << endl;
	return (double)number / allpixel;
}*/