#include "stdafx.h"
#include "FourierTransform.h"
#include <thread>

using namespace std;

FourierTransform::FourierTransform()
{
}


FourierTransform::~FourierTransform()
{
}

cv::Mat FourierTransform::getTransform(const char * fileName)
{
	cv::Mat src_32f_l = cv::imread(fileName, CV_LOAD_IMAGE_GRAYSCALE);
	cv::Mat src_1f_l;
	src_32f_l.convertTo(src_1f_l, CV_64FC1, (double)1.0 / 255);
	//cv::imshow("Source", src_1f_l);
	cv::Mat cv_32Fc2 = this->internalTransform(src_1f_l);
	return cv_32Fc2;
}

cv::Mat FourierTransform::getPowerMatFromMat(cv::Mat mat) {
	cv::Mat powerResult = cv::Mat(mat.rows, mat.cols, CV_64FC1);
	double resultValue, realPart, complexPart;
	cv::Vec2d complexNumber;
#pragma omp parallel for
	for (int j = 0; j < mat.rows; j++) {
		for (int i = 0; i < mat.cols; i++) {
			complexNumber = mat.at<cv::Vec2d>(j, i);
			realPart = complexNumber[0];
			complexPart = complexNumber[1];
			resultValue = log(pow(realPart, 2.0f) + pow(complexPart, 2.0f));
			powerResult.at<double>(j, i) = resultValue;
		}
	}
	//
	powerResult = this->getSwitchQuadrantsMat(powerResult);
	return powerResult;
}

cv::Mat FourierTransform::getPhaseMatFromMat(cv::Mat mat)
{
	cv::Mat phase = cv::Mat(mat.rows, mat.cols, CV_64FC1);
	double resultValue, realPart, complexPart;
	cv::Vec2d complexNumber;
#pragma omp parallel for
	for (int j = 0; j < mat.rows; j++) {
		for (int i = 0; i < mat.cols; i++) {
			complexNumber = mat.at<cv::Vec2d>(j, i);
			realPart = complexNumber[0];
			complexPart = complexNumber[1];
			resultValue = atan(realPart / complexPart);
			phase.at<double>(j, i) = resultValue;
		}
	}
	return phase;
}

cv::Mat FourierTransform::getInverseMat(cv::Mat mat) {
	cv::Mat resultInverse = cv::Mat(mat.rows, mat.cols, CV_64FC1);
	double rows = mat.rows;
	double cols = mat.cols;

	double normalization = 1.0f / sqrt(mat.cols * mat.rows);

	for (int k = 0; k < rows; ++k) {
		for (int l = 0; l < cols; ++l) {
			double realSum = 0.0f;
			for (int m = 0; m < rows; ++m) {
				for (int n = 0; n < cols; ++n) {
					cv::Vec2d complexNumber = mat.at<cv::Vec2d>(m, n);
					double val = 2.0f * CV_PI * (((m * k) / rows) + ((n * l) / cols));
					double realPart = complexNumber[0] * cos(val) * normalization;
					double complexPart = complexNumber[1] * -sin(val) * normalization;
					realSum += realPart + complexPart;
				}
			}
			resultInverse.at<double>(k, l) = realSum;
		}
	}
	return resultInverse;
}

cv::Mat FourierTransform::getFilteredImageByFilter(cv::Mat src_1f_l, cv::Mat &filter_fc1) {
	cv::imshow("filter_fc1", filter_fc1);
	cv::Mat transformed = this->internalTransform(src_1f_l);
	transformed = this->getSwitchQuadrantsMatComplex(transformed);
	
	for (int y = 0; y < transformed.rows; ++y) {
		for (int x = 0; x < transformed.cols; ++x) {
			int barMaskVal = (int)filter_fc1.at<uchar>(y, x);
			if (barMaskVal == 0) {
				cv::Vec2d old = transformed.at<cv::Vec2d>(y, x);
				//old[0] = 0;
				old[1] = 0;
				transformed.at<cv::Vec2d>(y, x) = cv::Vec2d(0,0);
			}
		}
	}
	transformed = this->getSwitchQuadrantsMatComplex(transformed);
	cv::Mat inverseMat = this->getInverseMat(transformed);
	return inverseMat;
}

cv::Mat FourierTransform::getLowPassFilteredImage(const char * fileName, int radius) {
	cv::Mat src_32f_l = cv::imread(fileName, CV_LOAD_IMAGE_GRAYSCALE);
	cv::Mat src_1f_l;
	src_32f_l.convertTo(src_1f_l, CV_64FC1, (double)1.0 / 255);
	cv::Mat filter = this->getElipsefilter(src_1f_l.cols, src_1f_l.rows, radius, true);
	src_1f_l = this->getFilteredImageByFilter(src_1f_l, filter);
	return src_1f_l;
}

cv::Mat FourierTransform::getHighPassFilteredImage(const char * fileName, int radius) {
	cv::Mat src_32f_l = cv::imread(fileName, CV_LOAD_IMAGE_GRAYSCALE);
	cv::Mat src_1f_l;
	src_32f_l.convertTo(src_1f_l, CV_64FC1, (double)1.0 / 255);
	cv::Mat filter = this->getElipsefilter(src_1f_l.cols, src_1f_l.rows, radius, false);
	src_1f_l = this->getFilteredImageByFilter(src_1f_l, filter);
	return src_1f_l;
}

cv::Mat FourierTransform::getMatWithRemovedBars(const char * fileName, int offset) {
	cv::Mat src_32f_l = cv::imread(fileName, CV_LOAD_IMAGE_GRAYSCALE);
	cv::Mat src_1f_l;
	src_32f_l.convertTo(src_1f_l, CV_64FC1, (double)1.0 / 255);
	cv::Mat filter = cv::Mat::zeros(src_1f_l.size(), CV_8UC1);

	const uint lineWidth = 25;
	int radius = 2;
	cv::rectangle(filter, cv::Rect(0, 0, filter.cols, filter.rows), cv::Scalar(255), -1);
	cv::rectangle(filter, 
		cv::Rect(
			offset,
			(filter.rows / 2) - (lineWidth / 2), 
			1,
			lineWidth
		),
		cv::Scalar(0), -1
	);
	cv::rectangle(filter,
		cv::Rect(
			(filter.cols) - offset,
			(filter.rows / 2) - (lineWidth / 2),
			1,
			lineWidth
		),
		cv::Scalar(0), -1
	);
	
	return this->getFilteredImageByFilter(src_1f_l, filter);
}

cv::Mat FourierTransform::getElipsefilter(uint width, uint height, uint radius, bool inverted)
{
	cv::Mat filter;
	filter = cv::Mat::zeros(height, width, CV_8UC1);
	if (inverted) {
		cv::rectangle(filter, cv::Rect(0, 0, filter.cols, filter.rows), cv::Scalar(255), -1);
		cv::circle(filter, cv::Point(height / 2, width / 2), radius, cv::Scalar(0.0f), -1);
	} else {
		cv::circle(filter, cv::Point(height / 2, width / 2), radius, cv::Scalar(255.0f), -1);
	}
	//cv::Mat filter_fc1(height, width, CV_64FC1);
	//filter.convertTo(filter_fc1, CV_64FC1, (double)1.0 / 255);
	return filter;
}

cv::Mat FourierTransform::getSwitchQuadrantsMatComplex(cv::Mat mat) {
	cv::Mat newMat = cv::Mat(mat.rows, mat.cols, CV_64FC2);
	int heigthCenter = mat.rows / 2;
	int widthCenter = mat.cols / 2;
	int newMatY = 0, newMatX = 0;
	for (int j = 0; j < mat.rows; j++) {

		for (int i = 0; i < mat.cols; i++) {

			//second to fourth
			if (j < heigthCenter && i < widthCenter) {
				newMatY = j + heigthCenter;
				newMatX = i + widthCenter;
				newMat.at<cv::Vec2d>(newMatY, newMatX) = mat.at<cv::Vec2d>(j, i);
			}
			// first to third

			if (j < heigthCenter && i >= widthCenter) {
				newMatY = j + heigthCenter;
				newMatX = i - widthCenter;
				newMat.at<cv::Vec2d>(newMatY, newMatX) = mat.at<cv::Vec2d>(j, i);
			}

			// third to first
			if (j >= heigthCenter && i < widthCenter) {
				newMatY = j - heigthCenter;
				newMatX = i + widthCenter;
				newMat.at<cv::Vec2d>(newMatY, newMatX) = mat.at<cv::Vec2d>(j, i);
			}

			// fourth to second
			if (j >= heigthCenter && i >= widthCenter) {
				newMatY = j - heigthCenter;
				newMatX = i - widthCenter;
				newMat.at<cv::Vec2d>(newMatY, newMatX) = mat.at<cv::Vec2d>(j, i);
			}

		}
	}
	return newMat;
}

cv::Mat FourierTransform::getSwitchQuadrantsMat(cv::Mat mat) {
	cv::Mat newMat = cv::Mat(mat.rows, mat.cols, CV_64FC1);
	int heigthCenter = mat.rows / 2;
	int widthCenter = mat.cols / 2;
	int newMatY = 0, newMatX = 0;
	for (int j = 0; j < mat.rows; j++) {
#pragma omp parallel for
		for (int i = 0; i < mat.cols; i++) {

			//second to fourth
			if (j < heigthCenter && i < widthCenter) {
				newMatY = j + heigthCenter;
				newMatX = i + widthCenter;
				newMat.at<double>(newMatY, newMatX) = mat.at<double>(j, i);
			}
			// first to third
			
			if (j < heigthCenter && i >= widthCenter) {
				newMatY = j + heigthCenter;
				newMatX = i - widthCenter;
				newMat.at<double>(newMatY, newMatX) = mat.at<double>(j, i);
			}
			
			// third to first
			if (j >= heigthCenter && i < widthCenter) {
				newMatY = j - heigthCenter;
				newMatX = i + widthCenter;
				newMat.at<double>(newMatY, newMatX) = mat.at<double>(j, i);
			}
			
			// fourth to second
			if (j >= heigthCenter && i >= widthCenter) {
				newMatY = j - heigthCenter;
				newMatX = i - widthCenter;
				newMat.at<double>(newMatY, newMatX) = mat.at<double>(j, i);
			}
			
		}
	}
	return newMat;
}

cv::Mat FourierTransform::internalTransform(cv::Mat src_1f_l)
{
	double value, eValue, realPart, complexPart;
	
	double rows = src_1f_l.rows;
	double cols = src_1f_l.cols;
	double normalization = 1.0f / sqrt(rows * (double)cols);

	//cv::normalize(src_1f_l, src_1f_l, 0.0f, 1.0f, cv::NORM_MINMAX);
	//src_1f_l = src_1f_l / sqrtf(rows * (double)cols);

	cv::Mat cv_32Fc2 = cv::Mat(rows, cols, CV_64FC2);

	for (int k = 0; k < rows; k++) {
		for (int l = 0; l < cols; l++) {
			cv::Vec2d vec;
			vec[0] = 0.0f;
			vec[1] = 0.0f;
			
			for (int m = 0; m < rows; m++) {
				for (int n = 0; n < cols; n++) {
					value = src_1f_l.at<double>(m, n);
					eValue = 2.0f * CV_PI * (double)(
						( (m * (double)k) / (double)rows)
							+ ((n * (double)l) / (double)cols)
					);
					realPart = value * cos(eValue) * normalization;
					complexPart = (-value * sin(eValue)) * normalization;
					vec[0] += (realPart);
					vec[1] += (complexPart);
				}
			}
			cv_32Fc2.at<cv::Vec2d>(k, l) = vec;
		}

		double percent = ((float)k / cv_32Fc2.rows) * 100;
		//printf("Percent %f \n", percent);
	}
	//cv::normalize(cv_32Fc2, cv_32Fc2, 0.0f, 1.0f, cv::NORM_MINMAX);
	return cv_32Fc2;
}
