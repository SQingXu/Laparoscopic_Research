#include "aruco_detection.h"

using namespace cv;
using namespace std;

int LoadImage(Mat* loadImage) {
	*loadImage = imread("RSCapture_1.bmp", CV_LOAD_IMAGE_COLOR);
	if (!loadImage->data) {
		cout << "Could not find or open the image\n";
		return -1;
	}
	return 0;
}
void readCamCalibFile(Mat* cameraMatrix, Mat* distorCoeff) {
	FileStorage fs("out_camera_data.xml", FileStorage::READ);
	fs["camera_matrix"] >> *cameraMatrix;
	fs["distortion_coefficients"] >> *distorCoeff;
	fs.release();
	return;
}
int aruco_detect() {
	Mat* img = new Mat();
	if (LoadImage(img) == -1) {
		return -1;
	}

	vector<int> markerIds;
	vector<vector<Point2f>> markerCorners, rejectedCandidates;
	Ptr<aruco::DetectorParameters> parameters;
	Ptr<aruco::Dictionary> dictionary = aruco::getPredefinedDictionary(aruco::DICT_6X6_250);
	Mat flipped;
	cv::flip(*img, flipped, 1);
	aruco::detectMarkers(flipped, dictionary, markerCorners, markerIds);

	Mat* outputImage = new Mat();
	if(markerIds.size() <= 0) {
		//cout << "No marker detected\n";
		return -1;
	}
	aruco::drawDetectedMarkers(flipped, markerCorners, markerIds);
	Mat camera_matrix, distort_coeff;
	readCamCalibFile(&camera_matrix, &distort_coeff);
	if (!camera_matrix.data || !distort_coeff.data) {
		//cout << "matrix not valid\n";
		return -1;
	}
	vector<Vec3d> rot_vecs, tranl_vecs;
	aruco::estimatePoseSingleMarkers(markerCorners, 0.034, camera_matrix, distort_coeff, rot_vecs, tranl_vecs);
	Mat rot_matrix;
	Mat translate_vec, rotation_vec;
	cv::Rodrigues(rot_vecs[0], rot_matrix);
	rot_matrix = rot_matrix.t();//inverse the rotation matrix (same as transpose)
	cv::Rodrigues(rot_matrix, rotation_vec);
	translate_vec = -rot_matrix * Mat(tranl_vecs[0]);

	//cout << "Rotation Vector: " << rot_matrix << endl;
	//cout << "Translation Vector: " << translate_vec << endl;
	
	//rot[0] = rot_matrix.at<double>(0, 0);
	//rot[1] = rot_matrix.at<double>(1, 0);
	FileStorage fs("cam_aruco_calib.json", FileStorage::WRITE);

	fs << "up_column" << "{" << "x" << rot_matrix.at<double>(0, 1) << "y" << rot_matrix.at<double>(1, 1) << "z" << rot_matrix.at<double>(2, 1) << "}";
	fs << "forward_column" << "{" << "x" << rot_matrix.at<double>(0, 2) << "y" << rot_matrix.at<double>(1, 2) << "z" << rot_matrix.at<double>(2, 2) << "}";
	fs << "translation_vector" << "{" << "x" << translate_vec.at<double>(0, 0) << "y" << translate_vec.at<double>(0, 1) << "z" << translate_vec.at<double>(0, 2) << "}";
	fs.release();

	cv::aruco::drawAxis(flipped, camera_matrix, distort_coeff, rot_vecs, tranl_vecs, 0.1);
	namedWindow("Display window", WINDOW_AUTOSIZE);// Create a window for display.
	imshow("Display window", flipped);                   // Show our image inside it.
	waitKey(0);
	return 2;
}

