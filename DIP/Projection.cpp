#include "stdafx.h"
#include "Projection.h"


Projection::Projection()
{
}


Projection::~Projection()
{
}

cv::Mat Projection::getTransformationMatrix() {
	double Building[4][2] = { {0,0}, {323,0}, {323,215}, {0,215} };
	double FlagPoints[4][2] = { {69,107}, {227,76}, {228,122}, {66,134} };

	cv::Mat matrixEq = cv::Mat(8, 8, CV_64FC1);
	cv::Mat matrixEq2 = cv::Mat(8, 1, CV_64FC1);
	for (int i = 0; i < 4; i++) {
		matrixEq.at<double>(i * 2, 0) = (double)Building[i][1];
		matrixEq.at<double>(i * 2, 1) = 1.0f;
		matrixEq.at<double>(i * 2, 2) = 0.0f;
		matrixEq.at<double>(i * 2, 3) = 0.0f;
		matrixEq.at<double>(i * 2, 4) = 0.0f;
		matrixEq.at<double>(i * 2, 5) = (double)(-FlagPoints[i][0] * Building[i][0]);
		matrixEq.at<double>(i * 2, 6) = (double)(-FlagPoints[i][0] * Building[i][1]);
		matrixEq.at<double>(i * 2, 7) = (double)(-FlagPoints[i][0]);
		matrixEq2.at<double>(i * 2, 0) = (double)-Building[i][0];
		
		matrixEq.at<double>((i * 2) + 1, 0) = 0.0f;
		matrixEq.at<double>((i * 2) + 1, 1) = 0.0f;
		matrixEq.at<double>((i * 2) + 1, 2) = (double)Building[i][0];
		matrixEq.at<double>((i * 2) + 1, 3) = (double)Building[i][1];
		matrixEq.at<double>((i * 2) + 1, 4) = 1.0f;
		matrixEq.at<double>((i * 2) + 1, 5) = (double)(-FlagPoints[i][1] * Building[i][0]);
		matrixEq.at<double>((i * 2) + 1, 6) = (double)(-FlagPoints[i][1] * Building[i][1]);
		matrixEq.at<double>((i * 2) + 1, 7) = (double)(-FlagPoints[i][1]);
		matrixEq2.at<double>((i * 2) + 1, 0) = 0.0f;
	}

	cv::Mat matResult = cv::Mat(8, 1, CV_64FC1);
	cv::solve(matrixEq, matrixEq2, matResult);

	cv::Mat transformationMatrix = cv::Mat(3, 3, CV_64FC1);
	transformationMatrix.at<double>(0, 0) = 1.0f;

	unsigned int index = 0;
	for (unsigned int row = 0; row < transformationMatrix.rows; row++)
	{
		for (unsigned int col = 0; col < transformationMatrix.cols; col++)
		{
			if (!(row == 0 && col == 0))
				transformationMatrix.at<double>(row, col) = matResult.at<double>(index++, 0);
		}
	}
	return transformationMatrix;
}

cv::Mat Projection::getProjectedImage(const char* transformed, const char* background) {
	cv::Mat src_c3 = cv::imread(transformed, CV_LOAD_IMAGE_COLOR);
	cv::Mat dest_c3 = cv::imread(background, CV_LOAD_IMAGE_COLOR);

	cv::Mat transformedMat;
	src_c3.convertTo(transformedMat, CV_32FC3, 1.0 / 255.0);

	cv::Mat backgroundMat;
	dest_c3.convertTo(backgroundMat, CV_32FC3, 1.0 / 255.0);

	cv::imshow("Original", transformedMat);
	cv::imshow("Original_2", backgroundMat);
	cv::Mat result = cv::Mat(backgroundMat.size(), CV_32FC3);
	backgroundMat.copyTo(result);
	cv::Mat transformationMatrix = this->getTransformationMatrix();
	
	cv::Mat originalCordinates = cv::Mat(3, 1, CV_64FC1);
	cv::Mat homoCordinates = cv::Mat(3, 1, CV_64FC1);
	originalCordinates.at<double>(2, 0) = 1.0f;

	uchar x_t, y_t;
	double phi, psi, omega;
	for (unsigned int y = 0; y < transformedMat.rows; y++)
	{
		for (unsigned int x = 0; x < transformedMat.cols; x++)
		{
			cv::Vec3f imageValue = transformedMat.at<cv::Vec3f>(y, x);
			
		originalCordinates.at<double>(1, 0) = (double)y;
			originalCordinates.at<double>(0, 0) = (double)x;			
			homoCordinates = transformationMatrix * originalCordinates;
		
			phi = homoCordinates.at<double>(0, 0);
			psi = homoCordinates.at<double>(1, 0);
			omega = homoCordinates.at<double>(2, 0);

			x_t = (uchar)(phi / omega);
			y_t = (uchar)(psi / omega);

			double d_x = (phi / omega);
			double d_y = (psi / omega);

			double highX = (int)ceil(d_x);
			double lowX = (int)floor(d_x);

			double highY = (int)ceil(d_y);
			double lowY = (int)floor(d_y);

			if (
				false &&
				lowX > 0 && highX < transformedMat.cols
				&& lowY > 0 && highY < transformedMat.rows
				) {
				cv::Vec3f topLeft = transformedMat.at<cv::Vec3f>(cv::Point(lowX, lowY));
				cv::Vec3f topRight = transformedMat.at<cv::Vec3f>(cv::Point(highX, lowY));
				cv::Vec3f bottomLeft = transformedMat.at<cv::Vec3f>(cv::Point(lowX, highY));
				cv::Vec3f bottomRight = transformedMat.at<cv::Vec3f>(cv::Point(highX, highY));

				float highX_lowX = ((highX - lowX) > 0 ? highX - lowX : 1);
				float highY_lowY = ((highY - lowY) > 0 ? highY - lowY : 1);

				float highX_d_x = ((highX - d_x) > 0 ? highX - d_x : 0.5);

				float highY_d_y = ((highY - d_y) > 0 ? highY - d_y : 0.5);
				float d_y_lowY = ((d_y - lowY) > 0 ? (d_y - lowY) : 0.5);

				cv::Vec3f p1 = highX_d_x / highX_lowX * topLeft + (d_x - lowX) / highX_lowX * topRight;
				cv::Vec3f p2 = highX_d_x / highX_lowX * bottomLeft + (d_x - lowX) / highX_lowX * bottomRight;

				imageValue = (highY_d_y / highY_lowY * p1 + d_y_lowY / highY_lowY * p2);
			}
		
			result.at<cv::Vec3f>(y_t, x_t) = imageValue;
		}
	}

	return result;
}