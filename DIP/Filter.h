#pragma once
class Filter
{
private:
	int mX;
	int mY;
	int** mat;
	double weigth;
	bool color;
public:
	Filter();
	~Filter();
	Filter(int x, int y, double wei, int ** p);
	Filter(int x, int y, double wei, int ** p, bool color);
	int getX();
	int getY();
	double getWeigth();
	uchar getPixel(cv::Mat image, int currentY, int currentX);
	double getPixelDouble(cv::Mat image, int currentY, int currentX);
	double getAnisotropicPixel(cv::Mat image, int currentY, int currentX);
	double getAnisotropicCordinate(double currentPixel, double neighbourPixel);
	cv::Vec3b getTexel(cv::Mat image, int currentY, int currentX);
	cv::Mat getFilteredImage(cv::Mat image);
	cv::Mat getAnisotropicFilteredImage(cv::Mat image);
};

