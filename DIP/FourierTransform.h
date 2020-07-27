#pragma once
class FourierTransform
{
public:

	FourierTransform();
	~FourierTransform();
	cv::Mat getTransform(const char* fileName);
	cv::Mat getPowerMatFromMat(cv::Mat mat);
	cv::Mat Normalization(cv::Mat mat¨, int min, int max);
	cv::Mat getPhaseMatFromMat(cv::Mat mat);
	cv::Mat getInverseMat(cv::Mat mat);
	cv::Mat getFilteredImageByFilter(cv::Mat src_1f_l, cv::Mat & filter_fc1);
	cv::Mat getLowPassFilteredImage(const char * fileName, int radius);
	cv::Mat getHighPassFilteredImage(const char * fileName, int radius);
	cv::Mat getMatWithRemovedBars(const char * fileName, int offset);
	cv::Mat getMatWithRemovedBars(const char * fileName);
	cv::Mat getElipsefilter(uint width, uint height, uint radius, bool inverted);
	cv::Mat getSwitchQuadrantsMatComplex(cv::Mat mat);
	cv::Mat getSwitchQuadrantsMat(cv::Mat mat);
	cv::Mat internalTransform(cv::Mat cv_32Fc2);
};

