#pragma once
#include"help.h"

void getDisp(Mat &left, Mat &right, Mat &disp8)    // ��ȡ�Ӳ�ͼ �����������ͼ 3D
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
	remap(img1proc, img1remap, map1_left, map2_left, cv::INTER_LINEAR);       // �������Ӳ����Ļ������У��
	remap(img2proc, img2remap, map1_right, map2_right, cv::INTER_LINEAR);
	Ptr<StereoBM> bm = StereoBM::create(16, 5);
	bm->setBlockSize(9);
	//bm->setROI1(validROIL);
	//	bm->setROI2(validROIR);
	bm->setPreFilterCap(31); //Ԥ�����˲����Ľض�ֵ��Ԥ��������ֵ������[-preFilterCap, preFilterCap]��Χ�ڵ�ֵ��
							 //������Χ��1 - 31���ĵ�����31������������ 63��, int

	bm->setMinDisparity(0);	//��С�ӲĬ��ֵΪ0, �����Ǹ�ֵ��int��

	bm->setNumDisparities(2 * 16);//�Ӳ�ڣ�������Ӳ�ֵ����С�Ӳ�ֵ֮��,���ڴ�С������16����������int��

	bm->setTextureThreshold(500);//������������ж���ֵ�������ǰSAD�����������ھ����ص��x��������ֵ֮��С��ָ����ֵ��
								 //��ô��ڶ�Ӧ�����ص���Ӳ�ֵΪ 0

	bm->setUniquenessRatio(1);//uniquenessRatio��Ҫ���Է�ֹ��ƥ��

	bm->setSpeckleWindowSize(100);//����Ӳ���ͨ����仯�ȵĴ��ڴ�С, ֵΪ 0 ʱȡ�� speckle ��飬int ��

	bm->setSpeckleRange(32);//�Ӳ�仯��ֵ�����������Ӳ�仯������ֵʱ���ô����ڵ��Ӳ����㣬int ��

	bm->setDisp12MaxDiff(-1);//���Ӳ�ͼ��ֱ�Ӽ���ó��������Ӳ�ͼ��ͨ��cvValidateDisparity����ó���֮������������졣
							 //��������ֵ���Ӳ�ֵ�������㡣�ò���Ĭ��Ϊ -1������ִ�������Ӳ��顣int �͡�
							 //ע���ڳ�����Խ׶���ñ��ָ�ֵΪ -1���Ա�鿴��ͬ�Ӳ�����ɵ��Ӳ�Ч����
	bm->setPreFilterType(CV_STEREO_BM_NORMALIZED_RESPONSE);//Ԥ�����˲��������ͣ���Ҫ�����ڽ�������ʧ�棨photometric distortions����������������ǿ�����,
														   //�����ֿ�ѡ���ͣ�CV_STEREO_BM_NORMALIZED_RESPONSE����һ����Ӧ�� 
														   //���� CV_STEREO_BM_XSOBEL��ˮƽ����Sobel���ӣ�Ĭ�����ͣ�, �ò���Ϊ int �ͣ�
	Mat disp;
	bm->compute(img1remap, img2remap, disp);
	disp.convertTo(disp8, CV_32F);
	Mat xyz;
	reprojectImageTo3D(disp8, xyz, Q, true, -1);
	saveXYZ("picture.txt", xyz);
	imshow("disp8", disp8);

	waitKey(0);
}


void getDispInit()  // �����õĻ�ȡ���ͼ�ĺ���
{
	Mat left = imread(".//left//1.jpg");
	Mat right = imread(".//right//1.jpg");
	Mat disp8;
	getDisp(left,right,disp8);
}



