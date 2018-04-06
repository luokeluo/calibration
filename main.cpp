
#include<opencv2\opencv.hpp>
#include<cv.h>
#include<time.h>
#include<iostream>

using namespace std;
using namespace cv;

enum {
	LEFT = 0,   //��ͼ 
	RIGHT = 1,   // ��ͼ 
	TEMP = 2   // ȫͼ
};

void help(int number)
{
	// ����number��ֵ switch�ṹ�����Ϣ break֮�������getchar���� exit����
	// �����˳�������Ŀ���break֮ǰreturn  ���Է���help���� ��������������
	switch (number)
	{
	case -1:
		cout << "������ʾ����" << endl;
		cout << "����1 �궨" << endl;
		cout << "����2 �������ͼ" << endl;
		return;
	case 0:
		cout << " ����m�����������ƽǵ�   ����ESC�����˳�����" << endl;
		break;
	case 1:
		std::cout << "������ͷʧ�ܣ���enter�˳�" << endl; 
		break;
	case 2:
		std::cout << "�ֶ��˳�ͼƬ���� ��enter����" << endl;
		break;
	case 3:
		std::cout << "ͼƬд���ļ����� ��enter����" << endl; 
		break;
	case 4:
		std::cout << "ͼƬ������� ��enter�˳�" << endl; 
		break;
	default:
		break;
	}
	getchar();
	exit(0);
}

String intToString(int v)      // ��һ����ͼ���У�ȡ��һС��ͼ�� 
{
	char buf[32] = { 0 };
	snprintf(buf, sizeof(buf), "%u", v);

	String str = buf;
	return str;
}



void writePicture(Mat &frame, int type, int num)  //  ��ͼƬд���ļ� (type)/num.jpg
{

	string s;
	switch (type)
	{
	case LEFT:
		s = "./left/" + intToString(num) + ".jpg";
		break;
	case RIGHT:
		s = "./right/" + intToString(num) + ".jpg";
		break;
	case TEMP:
		s = "./temp/" + intToString(num) + ".jpg";
		break;
	default:
		help(3);
		break;
	}
	imwrite(s, frame);
}
void readPicture(Mat &frame, int type, int num)// ����ͼƬ��frame
{
	string s;
	switch (type)
	{
	case LEFT:
		s = "./left/" + intToString(num) + ".jpg";
		break;
	case RIGHT:
		s = "./right/" + intToString(num) + ".jpg";
		break;
	case TEMP:
		s = "./temp/" + intToString(num) + ".jpg";
		break;
	default:
		help(3);
		break;
	}
	frame = imread(s);
}
void saveXYZ(const char*filename, const Mat&mat)
{
	const double max_z = 1.0e4;
	FILE* fp = fopen(filename, "wt");
	for (int y = 0; y <mat.rows; y++)
	{
		for (int x = 0; x <mat.cols; x++)
		{
			Vec3f point = mat.at

				<Vec3f>(y, x);
			if (fabs(point[2] - max_z)<FLT_EPSILON || fabs(point[2])>max_z)continue;
			if (point[2]>300)continue;
			fprintf(fp, "%f %f %f \n", point[0], point[1], point[2]);
		}
	}
	fclose(fp);
}
void picture(int num = 20,string name = "",int number = 1 ) // ������Ƭ �����left right�����ļ���
{ 
	if (name == "")
	{

		int num_picture = num;  // ����Ƭ����
		int num_corr_board_w = 9; // ����9��
		int num_corr_board_h = 6;// ����6��
		int length_board = 26;// ʵ�ʳ��� ����
		int num_corr = num_corr_board_w*num_corr_board_h; // �ܽ������
		Size board_size = Size(num_corr_board_w, num_corr_board_h);  // �������ÿ�С��еĽǵ���  
		Size square_size = Size(length_board, length_board); // ʵ�ʲ����õ��Ķ������ÿ�����̸�Ĵ�С  
		vector<Point2f> corners1;                  /****    ����ÿ��ͼ���ϼ�⵽�Ľǵ�       ****/
		vector<Point2f> corners2;
		VideoCapture capture(0);
		//	VideoCapture capture(1);
		//	capture.set(CV_CAP_PROP_FPS, 480);
		if (!capture.isOpened()) {
			help(1);
		}
		namedWindow("���ͼ��");
		namedWindow("�ұ�ͼ��");
		int successes = 0;
		int corner_count = 0;// ���ڼ�¼�ҵ��Ľǵ�����
		help(0);
		while (successes < num_picture) {
			Mat frametemp;
			capture >> frametemp;
			Mat frame_left = frametemp(Rect(frametemp.cols / 2, 0, frametemp.cols / 2, frametemp.rows));
			Mat frame_right = frametemp(Rect(0, 0, frametemp.cols / 2, frametemp.rows));
			Size image_size = frame_left.size();
			imshow("���ͼ��", frame_left);
			imshow("�ұ�ͼ��", frame_right);
			char c = waitKey(30);
			//// ����m�����������ƽǵ�
			//   ����ESC�����˳�����
			if (c == 'm')
			{

				bool ret_left = findChessboardCorners(frame_left,
					Size(num_corr_board_w, num_corr_board_h),
					corners1,
					CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE);
				bool ret_right = findChessboardCorners(frame_right,
					Size(num_corr_board_w, num_corr_board_h),
					corners2,
					CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE);
				corner_count = corners1.size();
				if ((corner_count != num_corr) || ret_left == NULL) continue;
				corner_count = corners2.size();
				if ((corner_count != num_corr) || ret_right == NULL) continue;
				//ָ�������ؼ��������ע  
				TermCriteria criteria = TermCriteria(
					TermCriteria::MAX_ITER | TermCriteria::EPS,
					40,
					0.1
				);
				// תΪ8λ��ͨ���Ҷ�ͼ
				Mat image_gray_left;
				cvtColor(frame_left, image_gray_left, COLOR_BGR2GRAY);
				Mat image_gray_right;
				cvtColor(frame_right, image_gray_right, COLOR_BGR2GRAY);
				//�����ؼ��  
				cornerSubPix(image_gray_left, corners1, Size(5, 5), Size(-1, -1), criteria);
				cornerSubPix(image_gray_right, corners2, Size(5, 5), Size(-1, -1), criteria);
				// ����������� ��ǰ����Ҫ��¼��ͼƬ
				Mat frame_left_temp;
				Mat frame_right_temp;
				frame_left.copyTo(frame_left_temp);
				frame_right.copyTo(frame_right_temp);
				drawChessboardCorners(frame_left,
					board_size,
					corners1, ret_left);
				drawChessboardCorners(frame_right,
					board_size,
					corners2, ret_right);
				imshow("���ͼ��", frame_left);
				imshow("�ұ�ͼ��", frame_right);
				waitKey(500);
				// ����ҵ��Ľǵ����� corner_count ==  board_n
				// Ҳ�����ҵ������ڽǵ㣬��ʾ��ͼ����������ɫ�����ߣ������¼���ݲ���
				if (corner_count == num_corr)
				{

					// ��¼ͼƬ 
					writePicture(frame_left_temp, LEFT, successes + 1);
					writePicture(frame_right_temp, RIGHT, successes + 1);
					// �ı����successse��ֵ  �ı� corner_count
					successes++;
					corner_count = 0;
				}

			}
			else if (c == 27) {
				help(2);
			}

		}
	}
	else {
		cout << "��ûд�˹��� add code here" << endl;
	}
}

int goodPictureNumber()       // ���������left right�ļ��������Ч��Ƭ�Եĸ���
{
	int num = 0;
	string name = "";
	while (true)
	{
		cout << "try goodPictureNumber 1" << endl;
		name = "./left/" + intToString(num + 1) + ".jpg";
		IplImage* image1 = cvLoadImage(name.c_str(),0);
		cout << "try goodPictureNumber 2" << endl;
		name = "./right/" + intToString(num + 1) + ".jpg";
		IplImage* image2 = cvLoadImage(name.c_str(), 0);
		if (image1&&image2)
		{
			num++;
		}
		else {
			return num;
		}
	}
	return num;
}

void calib()
{
	int num_picture = 20;  // ����Ƭ����
	int num_corr_board_w = 9; // ����9��
	int num_corr_board_h = 6;// ����6��
	int length_board = 26;// ʵ�ʳ��� ����
	int num_corr = num_corr_board_w*num_corr_board_h; // �ܽ������ 


	Mat frame_left, frame_right;

	Size board_size = Size(num_corr_board_w, num_corr_board_h);  // �������ÿ�С��еĽǵ���  
	Size square_size = Size(length_board, length_board); // ʵ�ʲ����õ��Ķ������ÿ�����̸�Ĵ�С  
	vector<Point2f> corners1;                  // ����ÿ��ͼ���ϼ�⵽�Ľǵ�   
	vector<Point2f> corners2;
	vector<vector<Point2f>>  corners_Seq1;   // �����⵽�����нǵ� 
	vector<vector<Point2f>>  corners_Seq2;
	Mat mat_left_point(num_corr*num_picture, 2, CV_32FC1);// ������F����
	Mat mat_right_point(num_corr*num_picture, 2, CV_32FC1);// ������F���� 
	cout << "FileStorage fs;" << endl;
	FileStorage fs;
	fs.open("calib_result.xml", FileStorage::WRITE);
	time_t rawtime;
	time(&rawtime);
	fs << "calibrationDate" << asctime(localtime(&rawtime));
	/*
	��һ�� �ж��Ƿ����㹻����Ƭ
	*/
	cout << "��һ�� �ж��Ƿ����㹻����Ƭ" << endl;
	num_picture = goodPictureNumber();
	cout << "try calib 1" << endl;
	if (20 > num_picture)
	{
		cout << "��Ƭ�������㣬���ǽ�Ϊ���������� 20p" << endl;
		picture(20 );
	}
	/*
	�ڶ��� ����ͼƬ��Ϣ
	*/
	int successes = 0;
	int corner_count = 0;
	while (successes < num_picture)
	{
		readPicture(frame_left, LEFT, successes + 1);
		readPicture(frame_right, RIGHT, successes + 1);
		bool ret_left = findChessboardCorners(frame_left,
			Size(num_corr_board_w, num_corr_board_h),
			corners1,
			CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE);
		bool ret_right = findChessboardCorners(frame_right,
			Size(num_corr_board_w, num_corr_board_h),
			corners2,
			CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE);
		corner_count = corners1.size();
		if ((corner_count != num_corr) || ret_left == NULL) continue;
		corner_count = corners2.size();
		if ((corner_count != num_corr) || ret_right == NULL) continue;
		//ָ�������ؼ��������ע  
		TermCriteria criteria = TermCriteria(
			TermCriteria::MAX_ITER | TermCriteria::EPS,
			40,
			0.1
		);
		// תΪ8λ��ͨ���Ҷ�ͼ
		Mat image_gray_left;
		cvtColor(frame_left, image_gray_left, COLOR_BGR2GRAY);
		Mat image_gray_right;
		cvtColor(frame_right, image_gray_right, COLOR_BGR2GRAY);
		//�����ؼ��  
		cornerSubPix(image_gray_left, corners1, Size(5, 5), Size(-1, -1), criteria);
		cornerSubPix(image_gray_right, corners2, Size(5, 5), Size(-1, -1), criteria);
		corners_Seq1.push_back(corners1);
		corners_Seq2.push_back(corners2);
		for (int i = 0; i < num_corr; i++)
		{//// ��¼��ߵ�
			mat_left_point.at<float>(successes*i, 0) = corners1[i].x;
			mat_left_point.at<float>(successes*i, 1) = corners1[i].y;
			///// ��¼�ұߵ�
			mat_right_point.at<float>(successes*i, 0) = corners2[i].x;
			mat_right_point.at<float>(successes*i, 1) = corners2[i].y;
		}
	}
	std::cout << "�ǵ���ȡ��ɣ�\n";
	/*
	������ �궨�ڲξ��� ����ϵ��
	*/
	std::cout << "��ʼ���ꡭ����������" << endl;
	Size image_size = frame_left.size();                        ///ͼ��ĳߴ�      ****/
	vector<vector<Point3f>>  object_Points_left;   /****  ���涨����Ͻǵ����ά����   ****/
	vector<vector<Point3f>>  object_Points_right;   /****  ���涨����Ͻǵ����ά����   ****/
	vector<int>  point_counts;                       /*****    ÿ��ͼ���нǵ������    ****/
	Mat intrinsic_matrix_left = Mat(3, 3, CV_32FC1, Scalar::all(0));                /*****    ������ڲ�������    ****/
	Mat distortion_coeffs_left = Mat(1, 5, CV_32FC1, Scalar::all(0));
	Mat intrinsic_matrix_right = Mat(3, 3, CV_32FC1, Scalar::all(0));     /*****    ������ڲ�������    ****/
	Mat distortion_coeffs_right = Mat(1, 5, CV_32FC1, Scalar::all(0));    /* �������5������ϵ����k1,k2,p1,p2,k3 */

																		  /* ��ʼ��������Ͻǵ����ά���� */
	for (int t = 0; t<num_picture; t++)
	{
		vector<Point3f> tempPointSet1;
		vector<Point3f> tempPointSet2;
		for (int i = 0; i<board_size.height; i++)
		{
			for (int j = 0; j<board_size.width; j++)
			{
				/* ���趨��������������ϵ��z=0��ƽ���� */
				Point3f tempPoint1;
				tempPoint1.x = i*square_size.width;
				tempPoint1.y = j*square_size.height;
				tempPoint1.z = 0;
				tempPointSet1.push_back(tempPoint1);
				/* ���趨��������������ϵ��z=0��ƽ���� */
				Point3f tempPoint2;
				tempPoint2.x = i*square_size.width;
				tempPoint2.y = j*square_size.height;
				tempPoint2.z = 0;
				tempPointSet2.push_back(tempPoint2);
			}
		}
		object_Points_left.push_back(tempPointSet1);//left
		object_Points_right.push_back(tempPointSet2);//right
	}

	/* ��ʼ��ÿ��ͼ���еĽǵ������������Ǽ���ÿ��ͼ���ж����Կ��������Ķ���� */
	for (int i = 0; i< num_picture; i++)
	{
		point_counts.push_back(board_size.width*board_size.height);
	}

	/* ��ʼ���� */
	vector<Mat> rotation_vectors;           /* ÿ��ͼ�����ת���� */
	vector<Mat> translation_vectors;        /* ÿ��ͼ���ƽ������ */
	calibrateCamera(object_Points_left, corners_Seq1, image_size,
		intrinsic_matrix_left, distortion_coeffs_left,
		rotation_vectors, translation_vectors);
	calibrateCamera(object_Points_right, corners_Seq2, image_size,
		intrinsic_matrix_right, distortion_coeffs_right,
		rotation_vectors, translation_vectors);
	/*  ���涨���� *****/
	std::cout << "��ʼ���涨����������������" << endl;

	cout << "left����ڲ�������" << endl;
	fs << "M1" << intrinsic_matrix_left;
	cout << "left����ϵ����\n";
	fs << "D1" << distortion_coeffs_left;
	cout << "right����ڲ�������" << endl;
	fs << "M2" << intrinsic_matrix_right;
	cout << "right����ϵ����\n";
	fs << "D2" << distortion_coeffs_right;
	/*
	����������� R T E F
	����������д���ļ�
	*/
	vector<uchar> m_RANSACStatus;
	// �����洢F����
	Mat m_Fundamental = findFundamentalMat(mat_left_point, mat_right_point, m_RANSACStatus, FM_RANSAC);
	cout << "F��������\n";
	fs << "F1" << m_Fundamental;
	/* ������ */
	// Mat corpond_line_left(num_corr*num_picture, 3, CV_32FC1);
	// computeCorrespondEpilines(mat_left_point, 1, m_Fundamental, corpond_line_left);
	// Mat corpond_line_right(num_corr*num_picture, 3, CV_32FC1);
	// computeCorrespondEpilines(mat_right_point, 2, m_Fundamental, corpond_line_right);
	// fout << "left�����ߣ�\n";
	// fout << corpond_line_left << endl << endl << endl;
	// fout << "right�����ߣ�\n";
	// fout << corpond_line_right << endl << endl << endl;
	/////////////////////////////////////���Ĳ� ����궨    ��/////////////////////////////////////////////////
	Mat mat_R, mat_T, mat_E, mat_F;
	double ret_sterCali = stereoCalibrate(object_Points_left,
		corners_Seq1, corners_Seq2, intrinsic_matrix_left, distortion_coeffs_left,
		intrinsic_matrix_right, distortion_coeffs_right, image_size, mat_R, mat_T, mat_E, mat_F,
		CV_CALIB_USE_INTRINSIC_GUESS,
		TermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 100, 1e-5));
	cout << "stereoCalibrate��\n";
	fs << "R" << mat_R;
	fs << "T" << mat_T;
	fs << "E" << mat_E;
	fs << "F2" << mat_F;

	/*
	����У�� һ������� R1 R2 P1 P2 Q �����д���ļ�
	*/
	Mat R1(3, 3, CV_32F); Mat R2(3, 3, CV_32F);
	Mat P1(3, 4, CV_32F); Mat P2(3, 4, CV_32F);
	Mat Q(4, 4, CV_32F);
	stereoRectify(intrinsic_matrix_left, distortion_coeffs_left,
		intrinsic_matrix_right, distortion_coeffs_right,
		image_size,
		mat_R, mat_T,
		R1, R2, P1, P2, Q);
	cout << "R1, R2, P1, P2, Q" << endl;
	fs << "R1" << R1 << "R2" << R2;
	fs << "P1" << P1 << "P2" << P2 << "Q" << Q;
	/*
	У��ӳ��  �������ͼ���map д���ļ�map1_left   map2_left  map1_right   map2_right
	*/
	Mat map1_left(image_size.height, image_size.width, CV_32F);
	Mat map2_left(image_size.height, image_size.width, CV_32F);
	Mat map1_right(image_size.height, image_size.width, CV_32F);
	Mat map2_right(image_size.height, image_size.width, CV_32F);
	initUndistortRectifyMap(intrinsic_matrix_left, distortion_coeffs_left,
		R1, intrinsic_matrix_left, image_size, CV_32FC1,
		map1_left, map2_left);
	initUndistortRectifyMap(intrinsic_matrix_right, distortion_coeffs_right,
		R2, intrinsic_matrix_right, image_size, CV_32FC1,
		map1_right, map2_right);
	cout << "map1_left   map2_left  map1_right   map2_right" << endl;
	fs << "map1_left" << map1_left << "map2_left" << map2_left;
	fs << "map1_right" << map1_right << "map2_right" << map2_right;
	fs.release();
	cout << "calib ������������б궨 " << endl;

}

void getDispInit(){}
int main()
{   
	calib();
	return 0;
}