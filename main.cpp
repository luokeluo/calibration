
#include<opencv2\opencv.hpp>
#include<cv.h>
#include<time.h>
#include<iostream>

using namespace std;
using namespace cv;

enum {
	LEFT = 0,   //左图 
	RIGHT = 1,   // 右图 
	TEMP = 2   // 全图
};

void help(int number)
{
	// 根据number的值 switch结构输出信息 break之后会运行getchar（） exit（）
	// 不想退出主程序的可以break之前return  可以返回help函数 继续运行主程序
	switch (number)
	{
	case -1:
		cout << "帮助提示如下" << endl;
		cout << "按下1 标定" << endl;
		cout << "按下2 生成深度图" << endl;
		return;
	case 0:
		cout << " 按下m键，截屏绘制角点   按下ESC键，退出程序" << endl;
		break;
	case 1:
		std::cout << "打开摄像头失败，按enter退出" << endl; 
		break;
	case 2:
		std::cout << "手动退出图片拍摄 按enter继续" << endl;
		break;
	case 3:
		std::cout << "图片写入文件出错 按enter继续" << endl; 
		break;
	case 4:
		std::cout << "图片读入出错 按enter退出" << endl; 
		break;
	default:
		break;
	}
	getchar();
	exit(0);
}

String intToString(int v)      // 从一幅大图像中，取出一小块图像。 
{
	char buf[32] = { 0 };
	snprintf(buf, sizeof(buf), "%u", v);

	String str = buf;
	return str;
}



void writePicture(Mat &frame, int type, int num)  //  将图片写入文件 (type)/num.jpg
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
void readPicture(Mat &frame, int type, int num)// 读入图片到frame
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
void picture(int num = 20,string name = "",int number = 1 ) // 拍摄照片 存放在left right两个文件夹
{ 
	if (name == "")
	{

		int num_picture = num;  // 总照片个数
		int num_corr_board_w = 9; // 宽有9个
		int num_corr_board_h = 6;// 高有6个
		int length_board = 26;// 实际长度 毫米
		int num_corr = num_corr_board_w*num_corr_board_h; // 总交点个数
		Size board_size = Size(num_corr_board_w, num_corr_board_h);  // 定标板上每行、列的角点数  
		Size square_size = Size(length_board, length_board); // 实际测量得到的定标板上每个棋盘格的大小  
		vector<Point2f> corners1;                  /****    缓存每幅图像上检测到的角点       ****/
		vector<Point2f> corners2;
		VideoCapture capture(0);
		//	VideoCapture capture(1);
		//	capture.set(CV_CAP_PROP_FPS, 480);
		if (!capture.isOpened()) {
			help(1);
		}
		namedWindow("左边图像");
		namedWindow("右边图像");
		int successes = 0;
		int corner_count = 0;// 用于记录找到的角点数量
		help(0);
		while (successes < num_picture) {
			Mat frametemp;
			capture >> frametemp;
			Mat frame_left = frametemp(Rect(frametemp.cols / 2, 0, frametemp.cols / 2, frametemp.rows));
			Mat frame_right = frametemp(Rect(0, 0, frametemp.cols / 2, frametemp.rows));
			Size image_size = frame_left.size();
			imshow("左边图像", frame_left);
			imshow("右边图像", frame_right);
			char c = waitKey(30);
			//// 按下m键，截屏绘制角点
			//   按下ESC键，退出程序
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
				//指定亚像素计算迭代标注  
				TermCriteria criteria = TermCriteria(
					TermCriteria::MAX_ITER | TermCriteria::EPS,
					40,
					0.1
				);
				// 转为8位单通道灰度图
				Mat image_gray_left;
				cvtColor(frame_left, image_gray_left, COLOR_BGR2GRAY);
				Mat image_gray_right;
				cvtColor(frame_right, image_gray_right, COLOR_BGR2GRAY);
				//亚像素检测  
				cornerSubPix(image_gray_left, corners1, Size(5, 5), Size(-1, -1), criteria);
				cornerSubPix(image_gray_right, corners2, Size(5, 5), Size(-1, -1), criteria);
				// 画出五彩连线 提前备份要记录的图片
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
				imshow("左边图像", frame_left);
				imshow("右边图像", frame_right);
				waitKey(500);
				// 如果找到的角点数量 corner_count ==  board_n
				// 也就是找到所有内角点，显示的图像有五颜六色的连线，进项记录数据操作
				if (corner_count == num_corr)
				{

					// 记录图片 
					writePicture(frame_left_temp, LEFT, successes + 1);
					writePicture(frame_right_temp, RIGHT, successes + 1);
					// 改变计数successse的值  改变 corner_count
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
		cout << "还没写此功能 add code here" << endl;
	}
}

int goodPictureNumber()       // 计数存放在left right文件夹里的有效照片对的个数
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
	int num_picture = 20;  // 总照片个数
	int num_corr_board_w = 9; // 宽有9个
	int num_corr_board_h = 6;// 高有6个
	int length_board = 26;// 实际长度 毫米
	int num_corr = num_corr_board_w*num_corr_board_h; // 总交点个数 


	Mat frame_left, frame_right;

	Size board_size = Size(num_corr_board_w, num_corr_board_h);  // 定标板上每行、列的角点数  
	Size square_size = Size(length_board, length_board); // 实际测量得到的定标板上每个棋盘格的大小  
	vector<Point2f> corners1;                  // 缓存每幅图像上检测到的角点   
	vector<Point2f> corners2;
	vector<vector<Point2f>>  corners_Seq1;   // 保存检测到的所有角点 
	vector<vector<Point2f>>  corners_Seq2;
	Mat mat_left_point(num_corr*num_picture, 2, CV_32FC1);// 用于找F矩阵
	Mat mat_right_point(num_corr*num_picture, 2, CV_32FC1);// 用于找F矩阵 
	cout << "FileStorage fs;" << endl;
	FileStorage fs;
	fs.open("calib_result.xml", FileStorage::WRITE);
	time_t rawtime;
	time(&rawtime);
	fs << "calibrationDate" << asctime(localtime(&rawtime));
	/*
	第一步 判断是否有足够的照片
	*/
	cout << "第一步 判断是否有足够的照片" << endl;
	num_picture = goodPictureNumber();
	cout << "try calib 1" << endl;
	if (20 > num_picture)
	{
		cout << "照片数量不足，我们将为您重新拍摄 20p" << endl;
		picture(20 );
	}
	/*
	第二步 载入图片信息
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
		//指定亚像素计算迭代标注  
		TermCriteria criteria = TermCriteria(
			TermCriteria::MAX_ITER | TermCriteria::EPS,
			40,
			0.1
		);
		// 转为8位单通道灰度图
		Mat image_gray_left;
		cvtColor(frame_left, image_gray_left, COLOR_BGR2GRAY);
		Mat image_gray_right;
		cvtColor(frame_right, image_gray_right, COLOR_BGR2GRAY);
		//亚像素检测  
		cornerSubPix(image_gray_left, corners1, Size(5, 5), Size(-1, -1), criteria);
		cornerSubPix(image_gray_right, corners2, Size(5, 5), Size(-1, -1), criteria);
		corners_Seq1.push_back(corners1);
		corners_Seq2.push_back(corners2);
		for (int i = 0; i < num_corr; i++)
		{//// 记录左边点
			mat_left_point.at<float>(successes*i, 0) = corners1[i].x;
			mat_left_point.at<float>(successes*i, 1) = corners1[i].y;
			///// 记录右边点
			mat_right_point.at<float>(successes*i, 0) = corners2[i].x;
			mat_right_point.at<float>(successes*i, 1) = corners2[i].y;
		}
	}
	std::cout << "角点提取完成！\n";
	/*
	第三步 标定内参矩阵 畸变系数
	*/
	std::cout << "开始定标………………" << endl;
	Size image_size = frame_left.size();                        ///图像的尺寸      ****/
	vector<vector<Point3f>>  object_Points_left;   /****  保存定标板上角点的三维坐标   ****/
	vector<vector<Point3f>>  object_Points_right;   /****  保存定标板上角点的三维坐标   ****/
	vector<int>  point_counts;                       /*****    每幅图像中角点的数量    ****/
	Mat intrinsic_matrix_left = Mat(3, 3, CV_32FC1, Scalar::all(0));                /*****    摄像机内参数矩阵    ****/
	Mat distortion_coeffs_left = Mat(1, 5, CV_32FC1, Scalar::all(0));
	Mat intrinsic_matrix_right = Mat(3, 3, CV_32FC1, Scalar::all(0));     /*****    摄像机内参数矩阵    ****/
	Mat distortion_coeffs_right = Mat(1, 5, CV_32FC1, Scalar::all(0));    /* 摄像机的5个畸变系数：k1,k2,p1,p2,k3 */

																		  /* 初始化定标板上角点的三维坐标 */
	for (int t = 0; t<num_picture; t++)
	{
		vector<Point3f> tempPointSet1;
		vector<Point3f> tempPointSet2;
		for (int i = 0; i<board_size.height; i++)
		{
			for (int j = 0; j<board_size.width; j++)
			{
				/* 假设定标板放在世界坐标系中z=0的平面上 */
				Point3f tempPoint1;
				tempPoint1.x = i*square_size.width;
				tempPoint1.y = j*square_size.height;
				tempPoint1.z = 0;
				tempPointSet1.push_back(tempPoint1);
				/* 假设定标板放在世界坐标系中z=0的平面上 */
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

	/* 初始化每幅图像中的角点数，这里我们假设每幅图像中都可以看到完整的定标板 */
	for (int i = 0; i< num_picture; i++)
	{
		point_counts.push_back(board_size.width*board_size.height);
	}

	/* 开始定标 */
	vector<Mat> rotation_vectors;           /* 每幅图像的旋转向量 */
	vector<Mat> translation_vectors;        /* 每幅图像的平移向量 */
	calibrateCamera(object_Points_left, corners_Seq1, image_size,
		intrinsic_matrix_left, distortion_coeffs_left,
		rotation_vectors, translation_vectors);
	calibrateCamera(object_Points_right, corners_Seq2, image_size,
		intrinsic_matrix_right, distortion_coeffs_right,
		rotation_vectors, translation_vectors);
	/*  保存定标结果 *****/
	std::cout << "开始保存定标结果………………" << endl;

	cout << "left相机内参数矩阵：" << endl;
	fs << "M1" << intrinsic_matrix_left;
	cout << "left畸变系数：\n";
	fs << "D1" << distortion_coeffs_left;
	cout << "right相机内参数矩阵：" << endl;
	fs << "M2" << intrinsic_matrix_right;
	cout << "right畸变系数：\n";
	fs << "D2" << distortion_coeffs_right;
	/*
	求解其他矩阵 R T E F
	将基础矩阵写入文件
	*/
	vector<uchar> m_RANSACStatus;
	// 用来存储F矩阵
	Mat m_Fundamental = findFundamentalMat(mat_left_point, mat_right_point, m_RANSACStatus, FM_RANSAC);
	cout << "F基础矩阵：\n";
	fs << "F1" << m_Fundamental;
	/* 画极线 */
	// Mat corpond_line_left(num_corr*num_picture, 3, CV_32FC1);
	// computeCorrespondEpilines(mat_left_point, 1, m_Fundamental, corpond_line_left);
	// Mat corpond_line_right(num_corr*num_picture, 3, CV_32FC1);
	// computeCorrespondEpilines(mat_right_point, 2, m_Fundamental, corpond_line_right);
	// fout << "left画极线：\n";
	// fout << corpond_line_left << endl << endl << endl;
	// fout << "right画极线：\n";
	// fout << corpond_line_right << endl << endl << endl;
	/////////////////////////////////////第四步 立体标定    、/////////////////////////////////////////////////
	Mat mat_R, mat_T, mat_E, mat_F;
	double ret_sterCali = stereoCalibrate(object_Points_left,
		corners_Seq1, corners_Seq2, intrinsic_matrix_left, distortion_coeffs_left,
		intrinsic_matrix_right, distortion_coeffs_right, image_size, mat_R, mat_T, mat_E, mat_F,
		CV_CALIB_USE_INTRINSIC_GUESS,
		TermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 100, 1e-5));
	cout << "stereoCalibrate：\n";
	fs << "R" << mat_R;
	fs << "T" << mat_T;
	fs << "E" << mat_E;
	fs << "F2" << mat_F;

	/*
	立体校正 一并解出来 R1 R2 P1 P2 Q 将结果写入文件
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
	校正映射  求解两幅图像的map 写入文件map1_left   map2_left  map1_right   map2_right
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
	cout << "calib 函数完成了所有标定 " << endl;

}

void getDispInit(){}
int main()
{   
	calib();
	return 0;
}