#pragma once
class HistogramEqualization
{
public:
	HistogramEqualization();
	~HistogramEqualization();
	double getResultOfCDFunction(uint* histogram, uchar brightnessValue);
	double getMinimum(uint* histogram);
	uchar * getMap(uint * histogram, int width, int height, uchar * map);
	uint * getHistogram(cv::Mat & src, uint * histogram);
	uchar* getMap(uchar* histogram, int width, int height);
	cv::Mat getHEImage(const char * filename);
	std::vector<uint> lut = std::vector<uint>(256, 0);
	int cdMin;
};

