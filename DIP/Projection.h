#pragma once
class Projection
{
public:
	Projection();
	~Projection();
	cv::Mat getTransformationMatrix();
	cv::Mat getProjectedImage(const char * src_filename, const char * dest_filename);
};

