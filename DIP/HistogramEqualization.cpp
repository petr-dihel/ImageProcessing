#include "stdafx.h"
#include "HistogramEqualization.h"


HistogramEqualization::HistogramEqualization()
{
}

HistogramEqualization::~HistogramEqualization()
{
}

double HistogramEqualization::getResultOfCDFunction(uint* histogram, uchar brightnessValue) {
	double result = 0;
	for (unsigned int i = 0; i <= brightnessValue; i++)
	{
		result += histogram[i];
	}
	return result;
}

double HistogramEqualization::getMinimum(uint* histogram)
{
	double minVal = 1;
	double cdf;
	for (unsigned int i = 0; i < 256; i++)
	{
		cdf = this->getResultOfCDFunction(histogram, i);
		if (cdf < minVal && cdf > 0)
		{
			minVal = cdf;
		}
	}
	return minVal;
}

uchar* HistogramEqualization::getMap(uint* histogram, int width, int height, uchar* map) {
	for (unsigned int i = 0; i < 256; i++)
	{
		map[i] = 0;
	}
	double cdf, value;
	double minimum = this->getMinimum(histogram);
	for (unsigned int i = 0; i < 256; i++)
	{
		cdf = this->getResultOfCDFunction(histogram, i);
		if (cdf > 0) {
			int x = 0;
		}
		value = (((double)cdf - minimum) / (((double)width*height) - (double)minimum)) * ((double)256 - 1);
		double a = (double)cdf - (double)minimum;
		double b = (double)a / (((double)width*height) - (double)minimum);
		double c = (double)b * 255;
		//value = std::max<uint>(0, value);
		value = std::max(value, 0.0);	
		map[i] = (uchar)value;
	}
	return map;
}

uint* HistogramEqualization::getHistogram(cv::Mat& src, uint* histogram) {

	for (unsigned int i = 0; i < 256; i++)
	{
		histogram[i] = 0;
	}
	uchar pixelValue;

	for (unsigned int y = 0; y < src.rows; y++)
	{
		for (unsigned int x = 0; x < src.cols; x++)
		{
			pixelValue = src.at<uchar>(y, x);
			histogram[pixelValue] += 1;
		}
	}
	return histogram;
}

cv::Mat HistogramEqualization::getHEImage(const char* filename) {
	cv::Mat src_c3 = cv::imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
	cv::imshow("Original", src_c3);

	cv::Mat res_f = cv::Mat(src_c3.size(), src_c3.type());
	uint histogram[256];
	this->getHistogram(src_c3, histogram);

	uchar map[256];
	this->getMap(histogram, src_c3.cols, src_c3.rows, map);

	uchar oldPixelValue;
	uint factor = src_c3.rows * src_c3.cols - this->cdMin;
	uchar newPixelValue;
	for (unsigned int y = 0; y < src_c3.rows; y++)
	{
		for (unsigned int x = 0; x < src_c3.cols; x++)
		{
			oldPixelValue = src_c3.at<uchar>(y, x);
			uchar a = map[oldPixelValue];
			res_f.at<uchar>(y, x) = map[oldPixelValue];
		}
	}

	cv::imshow("Res-2", res_f);
	return res_f;
}