// DIP.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <thread>
#include "FourierTransform.h"
#include "Disortion.h"
#include "EdgeDetector.h"
#include "HistogramEqualization.h"
#include "Projection.h"

using namespace std;

void firstEx() {
	// load color image from file system to Mat variable, this will be loaded using 8 bits (uchar)
	cv::Mat src_8uc3_img = cv::imread("images/lena.png", CV_LOAD_IMAGE_COLOR);

	if (src_8uc3_img.empty()) {
		printf("Unable to read input file (%s, %d).", __FILE__, __LINE__);
	}

	//cv::imshow( "LENA", src_8uc3_img );

	cv::Mat gray_8uc1_img; // declare variable to hold grayscale version of img variable, gray levels wil be represented using 8 bits (uchar)

	// declare variable to hold grayscale version of img variable, gray levels wil be represented using 32 bits (float)
	cv::Mat gray_32fc1_img;

	cv::cvtColor(src_8uc3_img, gray_8uc1_img, CV_BGR2GRAY); // convert input color image to grayscale one, CV_BGR2GRAY specifies direction of conversion
	gray_8uc1_img.convertTo(gray_32fc1_img, CV_32FC1, 1.0 / 255.0); // convert grayscale image from 8 bits to 32 bits, resulting values will be in the interval 0.0 - 1.0

	int x = 10, y = 15; // pixel coordinates

	uchar p1 = gray_8uc1_img.at<uchar>(y, x); // read grayscale value of a pixel, image represented using 8 bits
	float p2 = gray_32fc1_img.at<float>(y, x); // read grayscale value of a pixel, image represented using 32 bits
	cv::Vec3b p3 = src_8uc3_img.at<cv::Vec3b>(y, x); // read color value of a pixel, image represented using 8 bits per color channel
	cv::Vec3b p4 = gray_32fc1_img.at<float>(y, x); // read color value of a pixel, image represented using 8 bits per color channel

	// print values of pixels
	printf("p1 = %d\n", p1);
	printf("p2 = %f\n", p2);
	printf("p3[ 0 ] = %d, p3[ 1 ] = %d, p3[ 2 ] = %d\n", p3[0], p3[1], p3[2]);

	gray_8uc1_img.at<uchar>(y, x) = 0; // set pixel value to 0 (black)

	// draw a rectangle
	cv::rectangle(gray_8uc1_img, cv::Point(65, 84), cv::Point(75, 94),
		cv::Scalar(50), CV_FILLED);

	// declare variable to hold gradient image with dimensions: width= 256 pixels, height= 50 pixels.
	// Gray levels wil be represented using 8 bits (uchar)
	cv::Mat gradient_8uc1_img(50, 256, CV_8UC1);

	// For every pixel in image, assign a brightness value according to the x coordinate.
	// This wil create a horizontal gradient.
	for (int y = 0; y < gradient_8uc1_img.rows; y++) {
		for (int x = 0; x < gradient_8uc1_img.cols; x++) {
			gradient_8uc1_img.at<uchar>(y, x) = x;
		}
	}

	// diplay images
	//cv::imshow("Gradient", gradient_8uc1_img);
	//cv::imshow("Lena gray", gray_8uc1_img);
	//cv::imshow("Lena gray 32f", gray_32fc1_img);

	// nacist obr 
	// zjistit min a max brigt..
	// rozsirit od cerne po svetlou

	//cv::Mat src_8uc3_moon = cv::imread("images/moon.jpg", CV_LOAD_IMAGE_GRAYSCALE);

	/*
	uchar max = src_8uc3_moon.at<uchar>(0,0);
	uchar min = src_8uc3_moon.at<uchar>(0,0);

	for (int y = 0; y < src_8uc3_moon.rows; y++) {
		for (int x = 0; x < src_8uc3_moon.cols; x++) {
			uchar tmp = src_8uc3_moon.at<uchar>(y, x);
			max = (tmp > max ? tmp : max);
			min = (tmp < min ? tmp : min);
		}
	}

	printf("min = %d, max = %d \n", min, max);

	uint oldRange = max - min;
	uint newRange = 255 - 0;
	cv::imshow("Moon2", src_8uc3_moon);
	for (int y = 0; y < src_8uc3_moon.rows; y++) {
		for (int x = 0; x < src_8uc3_moon.cols; x++) {
			uchar oldValue = src_8uc3_moon.at<uchar>(y, x);
			uchar newValue = (((oldValue - min) * newRange) / oldRange) + 0;
			src_8uc3_moon.at<uchar>(y, x) = newValue;
		}
	}

	cv::imshow("Moon", src_8uc3_moon);
	*/

	// sec ex
	

	cv::Mat src_8uc3_moon = cv::imread("images/moon.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	cv::Mat gray_32fc1_moon;
	cv::Mat gray_32fc1_moon2;
	src_8uc3_moon.convertTo(gray_32fc1_moon, CV_32FC1, 1.0 / 255.0);
	src_8uc3_moon.convertTo(gray_32fc1_moon2, CV_32FC1, 1.0 / 255.0);


	cv::Mat src_8uc3_moon2 = src_8uc3_moon;
	float correction = 0.5;
	float correction2 = 2.0;

	for (int y = 0; y < gray_32fc1_moon.rows; y++) {
		for (int x = 0; x < gray_32fc1_moon.cols; x++) {
			float tmp = gray_32fc1_moon.at<float>(y, x);
			tmp = pow((tmp), correction);
			gray_32fc1_moon.at<float>(y, x) = tmp;
		}
	}

	for (int y = 0; y < gray_32fc1_moon2.rows; y++) {
		for (int x = 0; x < gray_32fc1_moon2.cols; x++) {
			float tmp = gray_32fc1_moon2.at<float>(y, x);
			tmp = pow((tmp), correction2);
			gray_32fc1_moon2.at<float>(y, x) = tmp;
		}
	}
	cv::imshow("Moon", gray_32fc1_moon);
	cv::imshow("Moon2", gray_32fc1_moon2);
	
}

void filterEx() {
	cv::Mat src_8uc3_filter = cv::imread("images/moon.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	cv::Mat src_8uc3_filter2 = cv::imread("images/moon.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	
	int **boxBLur = new int*[3];
	for (int i = 0; i < 3; i++) {
		boxBLur[i] = new int[3];
	}

	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 3; x++) {
			boxBLur[y][x] = 1;
		}
	}

	Filter *box = new Filter(3, 3, (1 / 9.0), boxBLur, false);
	cv::Mat filtered = box->getFilteredImage(src_8uc3_filter);
	cv::imshow("Test", filtered);

	int **gaussian = new int*[3];
	for (int i = 0; i < 3; i++) {
		gaussian[i] = new int[3];
	}
	gaussian[0][0] = 1;
	gaussian[0][1] = 2;
	gaussian[0][2] = 1;

	gaussian[1][0] = 2;
	gaussian[1][1] = 4;
	gaussian[1][2] = 2;

	gaussian[2][1] = 1;
	gaussian[2][2] = 2;
	gaussian[2][1] = 1;

	Filter *gbox = new Filter(3, 3, (1 / 16.0), gaussian, false);
	cv::Mat filtered2 = gbox->getFilteredImage(src_8uc3_filter);
	cv::imshow("Tes2", filtered2);
	cv::imshow("Tes3", src_8uc3_filter2);

}

void anisotropicFilter() {
	int iteration = 1000;
	cv::Mat src_8uc3_filter = cv::imread("images/valve.png", CV_LOAD_IMAGE_GRAYSCALE);
	cv::Mat src_double_filter;
	src_8uc3_filter.convertTo(src_double_filter, CV_64FC1, (double)1.0/255);
	Filter *filter = new Filter();
	cv::Mat src_double_filtered = filter->getAnisotropicFilteredImage(src_double_filter);
	for (int i = 0; i < iteration - 1; i++) {
		if (i % 10 == 0) {
			printf("Done %.2f \% \n", ((float)i/iteration) * 100);
		}
		src_double_filtered = filter->getAnisotropicFilteredImage(src_double_filtered);
	}
	cv::imshow("Test", src_8uc3_filter);
	cv::imshow("Test2", src_double_filtered);
	cv::imwrite("anisotropicD.jpg", src_double_filtered);
	cv::imwrite("anisotropicS.jpg", src_double_filter);
}

void saveImg(cv::Mat src, const char* name) {
	cv::Mat m2;
	src.convertTo(m2, CV_8UC1, (double)255);
	string fileName = "output_images/" + (string)name;
	cv::imwrite(fileName, m2);
}

void fourierTransform() {
	FourierTransform* ft = new FourierTransform();
	cv::Mat transformed = ft->getTransform("images/lena64.png");
	cv::Mat phaseMat = ft->getPhaseMatFromMat(transformed);
	cv::Mat powerMat = ft->getPowerMatFromMat(transformed);

	cv::imshow("Power", powerMat);
	cv::imshow("phaseMat", phaseMat);
	saveImg(powerMat, "Power.png");
	saveImg(phaseMat, "Phase.png");

	cv::Mat inverse = ft->getInverseMat(transformed);
	cv::imshow("inverse", inverse);
	saveImg(inverse, "l_inverse.png");
}

void frequencyFilter() {
	FourierTransform* ft = new FourierTransform();
	cv::Mat mat = ft->getLowPassFilteredImage("images/lena64_noise.png", 25);
	cv::imshow("low_frequencyFilter", mat);
	saveImg(mat, "low_frequencyFilter.png");

	cv::Mat high_frequencyFilter = ft->getHighPassFilteredImage("images/lena64_noise2.png", 25);
	cv::imshow("high_frequencyFilter", high_frequencyFilter);
	saveImg(high_frequencyFilter, "high_frequencyFilter.png");
}

void barFilter() {
	FourierTransform* ft = new FourierTransform();
	for (int i = 0; i < 30; i++) {
		cv::Mat mat = ft->getMatWithRemovedBars("images/lena64_bars.png", i);
		string filename = "lena_bar/bars_lena_";
		filename += std::to_string(i);
		filename += ".png";
		cout << filename << endl;
		saveImg(mat, filename.c_str());
	}
}

void disortedImage() {
	Disortion *dis = new Disortion();
	dis->getDisrotedImage("images/distorted_panorama.jpg");
}

void histogram() {
	HistogramEqualization *he = new HistogramEqualization();
	cv::Mat res = he->getHEImage("images/uneq.jpg");
	saveImg(res, "uneq.png");
}

void projection() {
	Projection *p = new Projection();
	cv::Mat res = p->getProjectedImage("images/flag.png", "images/vsb.jpg");
	cv::imshow("projection", res);
	//saveImg(res, "projection.png");
}

void edgeDetector() {
	EdgeDetector *e = new EdgeDetector();
	//cv::Mat res = e->SoberOperator("images/valve.png");
	cv::Mat res = e->SoberOperator("images/muhehe.jpg");
	cv::imshow("Tst", res);
}

int main()
{
	
	//filterEx();
	//anisotropicFilter();
	
	//show
	//fourierTransform();
	//frequencyFilter();

	//barFilter(); //not done
	
	//added bilinear inter
	//disortedImage();	
	
	//done
	//histogram();
	
	//done
	//projection(); 

	edgeDetector();

	cv::waitKey(0); // wait until keypressed

	return 0;
}
