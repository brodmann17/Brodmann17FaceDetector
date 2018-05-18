/*
 * example.cpp
 *
 *  Created on: May 16, 2018
 *      Author: Brodmann17
 */

#include "libbrodmann17.h"

#include <string>
#include <unistd.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

static const int MAX_DETECTIONS = 20;

using namespace bd17;
using namespace std;
using namespace cv;

int imageExample(string image_filename) {

	// Prepare Detector
	Init();
	void* detector = CreateDetector();

	unsigned int detections_num = 0;
	float* detections = new float [PARAMS_PER_DETECTION*MAX_DETECTIONS];

	Mat im = cv::imread(image_filename, cv::IMREAD_COLOR);

	if (!im.size().area()) return EXIT_FAILURE;


	// Run detection
	if (!Detect(detector, detections, &detections_num, MAX_DETECTIONS,
			(void*)im.data, im.cols, im.rows, bd17_image_format_t::bd17_bgr_interleaved_byte,
			NULL, NULL))
	{
		fprintf(stderr, "Error: Detection error");
		DestroyDetector(detector);
		delete [] detections;
		return EXIT_FAILURE;
	}

	// Draw results
	for (unsigned int i = 0; i < detections_num; i++)
	{
		// Upper left corner
		Point pt1(detections[i*PARAMS_PER_DETECTION], detections[i*PARAMS_PER_DETECTION+1]);

		// Bottom right corner
		Point pt2(detections[i*PARAMS_PER_DETECTION] + detections[i*PARAMS_PER_DETECTION+2] - 1, detections[i*PARAMS_PER_DETECTION+1] + detections[i*PARAMS_PER_DETECTION+3] - 1);

		// Draw rectangle
		rectangle(im, pt1, pt2, Scalar(0, 0, 255), 2);
	}
	imshow("Output", im);
	int key = waitKey(0);

	// Clean Up
	DestroyDetector(detector);
	delete [] detections;

	return EXIT_SUCCESS;
}

static inline bool is_file_exists (const std::string& name) {
    return ( access( name.c_str(), F_OK ) != -1 );
}

int main(int argc, char ** argv) {
	std::string image = "./example.jpg";
	if (argc != 2) {
		fprintf(stderr, "Input file is not given, uses example.jpg\r\n");
	} else {
		image = std::string(argv[1]);
	}
	if (!is_file_exists(image)) {
		fprintf(stderr, "File %s does not exists\r\n", image.c_str());
		return -1;
	}
	return imageExample(image);
}


