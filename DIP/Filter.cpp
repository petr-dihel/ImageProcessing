#include "stdafx.h"
#include "Filter.h"
#include <iostream>

using namespace std;

Filter::Filter()
{
}


Filter::~Filter()
{
}

Filter::Filter(int x, int y, double wei, int** p) {
	this->mX = x;
	this->mY = y;
	this->weigth = wei;
	this->mat = new int*[mY];
	color = false;
	for (int i = 0; i < mY; ++i) {
		this->mat[i] = new int[mX];
	}

	for (int y = 0; y < mY; ++y) {
		for (int x = 0; x < mY; ++x) {
			this->mat[y][x] = p[y][x];
		}
	}
}

Filter::Filter(int x, int y, double wei, int ** p, bool color)
{
	this->mX = x;
	this->mY = y;
	this->weigth = wei;
	this->mat = new int*[mY];
	color = false;
	for (int i = 0; i < mY; ++i) {
		this->mat[i] = new int[mX];
	}

	for (int y = 0; y < mY; ++y) {
		for (int x = 0; x < mY; ++x) {
			this->mat[y][x] = p[y][x];
		}
	}
	this->color = color;
}

int Filter::getX() {
	return this->mX;
}

int Filter::getY() {
	return this->mY;
}

double Filter::getWeigth() {
	return this->weigth;
}

double Filter::getPixelDouble(cv::Mat image, int currentY, int currentX) {
	int minBorder = this->mX / 2;
	double pixel = image.at<double>(currentY, currentX);
	if (
		currentX < minBorder
		|| currentY < minBorder
		|| currentX >(image.cols - 1 - minBorder)
		|| currentY >(image.rows - 1 - minBorder)
		) {
		return pixel;
	}

	double sum = 0;
	for (int y = currentY - minBorder; y <= currentY + minBorder; y++) {
		for (int x = currentX - minBorder; x <= currentX + minBorder; x++) {
			int a = (currentY + minBorder) - y;
			int b = (currentX + minBorder) - x;
			double tmp = this->mat[a][b];
			double tmpPixel = image.at<double>(y, x);
			sum += tmpPixel + tmp;
		}
	}
	sum *= this->weigth;
	return (double)sum;
}

uchar Filter::getPixel(cv::Mat image, int currentY, int currentX) {
	int minBorder = this->mX / 2;
	uchar pixel = image.at<uchar>(currentY, currentX);
	if (
		currentX < minBorder
		|| currentY < minBorder
		|| currentX >(image.cols - 1 - minBorder)
		|| currentY >(image.rows  - 1 - minBorder)
		) {
		return pixel;
	}

	double sum = 0;
	for (int y = currentY - minBorder; y <= currentY + minBorder; y++) {
		for (int x = currentX - minBorder; x <= currentX + minBorder; x++) {
			int a = (currentY + minBorder) - y;
			int b = (currentX + minBorder) - x;
			uchar tmp = this->mat[a][b];
			uchar tmpPixel = image.at<uchar>(y, x);
			sum += tmpPixel + tmp;
		}
	}
	sum *= this->weigth;
	return (uchar)sum;
}


double Filter::getAnisotropicPixel(cv::Mat image, int currentY, int currentX)
{
	int minBorder = 1;
	double currentPixel = image.at<double>(currentY, currentX);
	if (
		currentX < minBorder
		|| currentY < minBorder
		|| currentX > (image.cols - 1 - minBorder)
		|| currentY > (image.rows - 1 - minBorder)
		) {
		return currentPixel;
	}

	double I0 = (this->getAnisotropicCordinate(
		currentPixel,
		image.at<double>(currentY - 1, currentX)
	));

	double I1 = (this->getAnisotropicCordinate(
		currentPixel,
		image.at<double>(currentY + 1, currentX)
	));

	double I2 = (this->getAnisotropicCordinate(
		currentPixel,
		image.at<double>(currentY, currentX - 1)
	));

	double I3 = (this->getAnisotropicCordinate(
		currentPixel,
		image.at<double>(currentY, currentX + 1)
	));

	double lambda = 0.1;
	double newPixel = currentPixel * (1 - lambda * (I0 + I1 + I2 + I3))
		+ lambda * (
			I0 * image.at<double>(currentY - 1, currentX)
			+ I1 * image.at<double>(currentY + 1, currentX)
			+ I2 * image.at<double>(currentY, currentX - 1)
			+ I3 * image.at<double>(currentY, currentX + 1)
		);
	return newPixel;
}

double Filter::getAnisotropicCordinate(double currentPixel, double neighbourPixel)
{
	double currentI = neighbourPixel - currentPixel;
	double exponent = -((currentI * currentI) / ((double)0.015 * (double)0.015));
	return exp(exponent);
}

cv::Vec3b Filter::getTexel(cv::Mat image, int currentY, int currentX)
{
	int minBorder = this->mX / 2;
	if (
		currentX < minBorder
		|| currentY < minBorder
		|| currentX >(image.cols - 1 - minBorder)
		|| currentY >(image.rows - 1 - minBorder)
		) {
		cv::Vec3b  pixel = image.at<cv::Vec3b >(currentY, currentX);
		return pixel;
	}

	cv::Vec3f sum = cv::Vec3b{0,0,0};
	for (int y = currentY - minBorder; y <= currentY + minBorder; y++) {
		for (int x = currentX - minBorder; x <= currentX + minBorder; x++) {
			int a = (currentY + minBorder) - y;
			int b = (currentX + minBorder) - x;		
			uchar tmp = this->mat[a][b];
			cv::Vec3b tmpPixel = image.at<cv::Vec3b>(y, x);
			tmpPixel[0] += tmp;
			tmpPixel[1] += tmp;
			tmpPixel[2] += tmp;
			sum += tmpPixel;
		}
	}
	sum *= this->weigth;
	return sum;
}

cv::Mat Filter::getFilteredImage(cv::Mat image) {
	for (int y = 0; y < image.rows; y++) {
		for (int x = 0; x < image.cols; x++) {
			if (this->color) {
				cv::Vec3b newPixel = this->getTexel(image, y, x);
				cv::Vec3b old = image.at<cv::Vec3b>(y, x);			
				image.at<cv::Vec3b>(y, x) = newPixel;
			}
			else {
				uchar newPixel = this->getPixel(image, y, x);
				uchar old = image.at<uchar>(y, x);
				image.at<uchar>(y, x) = newPixel;
			}
			
		}
	}
	return image;
}

cv::Mat Filter::getAnisotropicFilteredImage(cv::Mat image)
{
	cv::Mat newImage;
	image.copyTo(newImage);
	for (int y = 0; y < image.rows; y++) {
		for (int x = 0; x < image.cols; x++) {
			double newPixel = this->getAnisotropicPixel(image, y, x);
			newImage.at<double>(y, x) = newPixel;
		}
	}
	return newImage;
}
