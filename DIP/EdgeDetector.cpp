#include "stdafx.h"
#include "EdgeDetector.h"
#include "Filter.h"

EdgeDetector::~EdgeDetector()
{
}

double EdgeDetector::getXGradient(cv::Mat src, int x, int y) {
	double gx =
		src.at<double>(y - 1, x - 1) +
		2 * src.at<double>(y, x - 1) +
		src.at<double>(y + 1, x - 1) -
		src.at<double>(y - 1, x + 1) -
		2 * src.at<double>(y, x + 1) -
		src.at<double>(y + 1, x + 1);
	return gx;
}

double EdgeDetector::getYGradient(cv::Mat src, int x, int y) {
	double gY = 
		src.at<double>(y - 1, x - 1) +
		2 * src.at<double>(y - 1, x) +
		src.at<double>(y - 1, x + 1) -
		src.at<double>(y + 1, x - 1) -
		2 * src.at<double>(y + 1, x) -
		src.at<double>(y + 1, x + 1);
	return gY;
}

cv::Mat EdgeDetector::SoberOperator(const char * filename)
{
	cv::Mat src_c3 = cv::imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
	cv::imshow("Original", src_c3);
	cv::Mat src_fc = cv::Mat(src_c3.size(), CV_8UC1);

	src_c3.convertTo(src_fc, CV_64FC1, (double)1.0 / 255);

	cv::Mat res_f = cv::Mat(src_c3.size(), CV_64FC1);
	for (int j = 1; j < src_c3.rows - 1; j++) {
		for (int i = 1; i < src_c3.cols - 1; i++) {
			double gX = this->getXGradient(src_fc, i, j);
			double gY = this->getYGradient(src_fc, i, j);
			double res = sqrtf(gX * gX + gY * gY);
			res = std::max<double>(0, std::min<double>(255, res));
			res_f.at<double>(j, i) = res;
		}
	}
	return res_f;
}

cv::Mat EdgeDetector::NonMaxima(const char * filename) {
	cv::Mat src_c3 = cv::imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
	cv::imshow("Original", src_c3);
	cv::Mat src_fc3 = cv::Mat(src_c3.size(), src_c3.type());

	for (int y = 1; y < src_c3.rows - 1; y++) {
		for (int x = 1; x < src_c3.cols - 1; x++) {
			double e = src_fc3.at<double>(y + 1, x +1);
			double e1 = src_fc3.at<double>(y, x + 1);

			double e2 = src_fc3.at<double>(y, x - 1);
			double e3 = src_fc3.at<double>(y - 1, x - 1);

			double pixel = src_fc3.at<double>(y, x);
		}
	}
	return cv::Mat();
}