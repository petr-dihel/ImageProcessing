#pragma once
class EdgeDetector
{
public:
	
	EdgeDetector() {
	};

	~EdgeDetector();
	double getXGradient(cv::Mat src, int x, int y);
	double getYGradient(cv::Mat src, int x, int y);
	cv::Mat SoberOperator(const char* filename);
	cv::Mat NonMaxima(const char * filename);
};

