#include "stdafx.h"
#include "Disortion.h"

struct DisortionUserData
{
	cv::Mat src_c3;
	int k1, k2;
};


Disortion::Disortion()
{
}


Disortion::~Disortion()
{
}

float BilinearInterpolation(float q11, float q12, float q21, float q22, float x1, float x2, float y1, float y2, float x, float y)
{
	float x2x1, y2y1, x2x, y2y, yy1, xx1;
	x2x1 = x2 - x1;
	y2y1 = y2 - y1;
	x2x = x2 - x;
	y2y = y2 - y;
	yy1 = y - y1;
	xx1 = x - x1;
	return 1.0 / (x2x1 * y2y1) * (
		q11 * x2x * y2y +
		q21 * xx1 * y2y +
		q12 * x2x * yy1 +
		q22 * xx1 * yy1
		);
}

void internalDisortedImage(int id, void * userdata) {
	DisortionUserData *disortionUserData = (DisortionUserData *)userdata;
	cv::Mat src_c3 = disortionUserData->src_c3;
	cv::Mat out_c3 = cv::Mat(src_c3.size(), CV_32FC3);
	double heigth = src_c3.rows;
	double width = src_c3.cols;

	double centerImageW = width / 2.0f;
	double centerImageH = heigth / 2.0f;
	double r = sqrtf(centerImageH * centerImageH + centerImageW * centerImageW);

	for (int y = 0; y < heigth; y++) {
		double y_ = (y - centerImageH) / r;
		for (int x = 0; x < width; x++) {
			double x_ = (x - centerImageW) / r;
			double r_2 = x_ * x_ + y_ * y_;
			double phi_1 = 1 / (1 + r_2 * (disortionUserData->k1/10.f) + r_2 * r_2 * (disortionUserData->k2/10.f));
			
			double d_x = (x - centerImageW) * phi_1 + centerImageW;
			double d_y = (y - centerImageH) * phi_1 + centerImageH;
			
			double highX = (int)ceil(d_x);
			double lowX = (int)floor(d_x);

			double highY = (int)ceil(d_y);
			double lowY = (int)floor(d_y);

			cv::Vec3f topLeft = src_c3.at<cv::Vec3f>(cv::Point(lowX, lowY));
			cv::Vec3f topRight = src_c3.at<cv::Vec3f>(cv::Point(highX, lowY));
			cv::Vec3f bottomLeft = src_c3.at<cv::Vec3f>(cv::Point(lowX, highY));
			cv::Vec3f bottomRight = src_c3.at<cv::Vec3f>(cv::Point(highX, highY));

			float highX_lowX = ((highX - lowX) > 0 ? highX - lowX : 1);
			float highY_lowY = ((highY - lowY) > 0 ? highY - lowY : 1);

			float highX_d_x = ((highX - d_x) > 0 ? highX - d_x : 0.5);

			float highY_d_y = ((highY - d_y) > 0 ? highY - d_y : 0.5);
			float d_y_lowY = ((d_y - lowY) > 0 ? (d_y - lowY) : 0.5);

			cv::Vec3f p1 = highX_d_x / highX_lowX * topLeft + (d_x - lowX)/ highX_lowX * topRight;
			cv::Vec3f p2 = highX_d_x / highX_lowX * bottomLeft + (d_x - lowX) / highX_lowX * bottomRight;
		
			cv::Vec3f imageValue = highY_d_y / highY_lowY * p1 + d_y_lowY / highY_lowY * p2;
	
			out_c3.at<cv::Vec3f>(y, x) = imageValue;
		}
	}
	cv::imshow("Image", out_c3);
}

cv::Mat Disortion::getDisrotedImage(const char* fileName) {
	DisortionUserData userData = DisortionUserData();
	userData.k1 = 3;
	userData.k2 = 1;
	
	cv::namedWindow("Image");
	cv::Mat src_c3 = cv::imread(fileName, CV_LOAD_IMAGE_COLOR);
	cv::Mat src_fc3;
	src_c3.convertTo(src_fc3, CV_32FC3, 1.0 / 255.0);
	userData.src_c3 = src_fc3;
	internalDisortedImage(0, &userData);
	cv::createTrackbar("K1", "Image", &userData.k1, 100, internalDisortedImage, &userData);
	cv::createTrackbar("K2", "Image", &userData.k2, 100, internalDisortedImage, &userData);
	//cv::imshow("test", src_c3);
	//cv::imshow("test2", out_c3);
	cv::waitKey(0); // wait until keypressed

	return cv::Mat();
}
