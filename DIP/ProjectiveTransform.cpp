#include "stdafx.h"
#include "ProjectiveTransform.h"


ProjectiveTransform::ProjectiveTransform()
{
}


ProjectiveTransform::~ProjectiveTransform()
{
}

void ProjectiveTransform::Transform(const char * filename)
{
	cv::Mat src_c3_transformed = cv::imread(filename, CV_LOAD_IMAGE_COLOR);
	cv::Mat src_c3_into = cv::imread(filename, CV_LOAD_IMAGE_COLOR);
	int heigth = src_c3_transformed.rows;
	int width = src_c3_transformed.cols;
	int startIntoX = 69;
	int startIntoY = 107;

	for (int y = 0; y < heigth; y++) {
		for (int x = 0; x < width; x++) {
			cv::Vec3b v_3 = src_c3_transformed.at<cv::Vec3b>(y, x);

			cv::Vec3f input = cv::Vec3f{ (float)y, 1, 0, 0, 0 , /* -x_*x_1 */ };
			cv::InputArray eq_1 = cv::InputArray{y, 1, 0, 0, 0};
			//cv::solve()
		}
	}
}

