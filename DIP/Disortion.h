#pragma once
class Disortion
{
public:
	int k1, k2;
	Disortion();
	~Disortion();
	cv::Mat getDisrotedImage(const char * fileName);
};

