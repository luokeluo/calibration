#pragma once
#include"help.h"

void getDisp(Mat &left, Mat &right, Mat &disp8)    // 获取视差图 并且生成深度图 3D
{
	FileStorage fs;
	fs.open("calib_result.xml", FileStorage::READ);
	Mat  map1_left, map2_left, map1_right, map2_right, Q;
	fs["map1_left"] >> map1_left;
	fs["map2_left"] >> map2_left;
	fs["map1_right"] >> map1_right;
	fs["map2_right"] >> map2_right;
	fs["Q"] >> Q; 
	Mat img1proc, img2proc, img1remap, img2remap;
	cvtColor(left, img1proc, CV_BGR2GRAY);
	cvtColor(right, img2proc, CV_BGR2GRAY);
	remap(img1proc, img1remap, map1_left, map2_left, cv::INTER_LINEAR);       // 对用于视差计算的画面进行校正
	remap(img2proc, img2remap, map1_right, map2_right, cv::INTER_LINEAR);
	Ptr<StereoBM> bm = StereoBM::create(16, 5);
	bm->setBlockSize(9);
	//bm->setROI1(validROIL);
	//	bm->setROI2(validROIR);
	bm->setPreFilterCap(31); //预处理滤波器的截断值，预处理的输出值仅保留[-preFilterCap, preFilterCap]范围内的值，
							 //参数范围：1 - 31（文档中是31，但代码中是 63）, int

	bm->setMinDisparity(0);	//最小视差，默认值为0, 可以是负值，int型

	bm->setNumDisparities(2 * 16);//视差窗口，即最大视差值与最小视差值之差,窗口大小必须是16的整数倍，int型

	bm->setTextureThreshold(500);//低纹理区域的判断阈值。如果当前SAD窗口内所有邻居像素点的x导数绝对值之和小于指定阈值，
								 //则该窗口对应的像素点的视差值为 0

	bm->setUniquenessRatio(1);//uniquenessRatio主要可以防止误匹配

	bm->setSpeckleWindowSize(100);//检查视差连通区域变化度的窗口大小, 值为 0 时取消 speckle 检查，int 型

	bm->setSpeckleRange(32);//视差变化阈值，当窗口内视差变化大于阈值时，该窗口内的视差清零，int 型

	bm->setDisp12MaxDiff(-1);//左视差图（直接计算得出）和右视差图（通过cvValidateDisparity计算得出）之间的最大容许差异。
							 //超过该阈值的视差值将被清零。该参数默认为 -1，即不执行左右视差检查。int 型。
							 //注意在程序调试阶段最好保持该值为 -1，以便查看不同视差窗口生成的视差效果。
	bm->setPreFilterType(CV_STEREO_BM_NORMALIZED_RESPONSE);//预处理滤波器的类型，主要是用于降低亮度失真（photometric distortions）、消除噪声和增强纹理等,
														   //有两种可选类型：CV_STEREO_BM_NORMALIZED_RESPONSE（归一化响应） 
														   //或者 CV_STEREO_BM_XSOBEL（水平方向Sobel算子，默认类型）, 该参数为 int 型；
	Mat disp;
	bm->compute(img1remap, img2remap, disp);
	disp.convertTo(disp8, CV_32F);
	Mat xyz;
	reprojectImageTo3D(disp8, xyz, Q, true, -1);
	saveXYZ("picture.txt", xyz);
	imshow("disp8", disp8);

	waitKey(0);
}


void getDispInit()  // 测试用的获取深度图的函数
{
	Mat left = imread(".//left//1.jpg");
	Mat right = imread(".//right//1.jpg");
	Mat disp8;
	getDisp(left,right,disp8);
}



