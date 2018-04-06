#pragma once
#include"help.h"

void picture(int num  , string name  , int number ) // 拍摄照片 存放在left right两个文件夹
{
	if (name == NULL)
	{
		int num_picture = 20;  // 总照片个数
		int num_corr_board_w = 9; // 宽有9个
		int num_corr_board_h = 6;// 高有6个
		int length_board = 26;// 实际长度 毫米
		int num_corr = num_corr_board_w*num_corr_board_h; // 总交点个数
		Size board_size = Size(num_corr_board_w, num_corr_board_h);  // 定标板上每行、列的角点数  
		Size square_size = Size(length_board, length_board); // 实际测量得到的定标板上每个棋盘格的大小  
		vector<Point2f> corners1;                  /****    缓存每幅图像上检测到的角点       ****/
		vector<Point2f> corners2;
		VideoCapture capture(1);
		capture.set(CV_CAP_PROP_FPS, 480);
		if (!capture.isOpened()) {
			help(1);
		}
		namedWindow("左边图像");
		namedWindow("右边图像");
		int successes = 0;
		int corner_count = 0;// 用于记录找到的角点数量
		help(0);
		while (successes < num) {
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
		cout << ""<<endl;
	}
}

int goodPictureNumber()       // 计数存放在left right文件夹里的有效照片对的个数
{
	int num = 0;
	string name = NULL;
	while (true)
	{
		name = ".//left//" + intToString(num+1) + ".jpg";
		ifstream fin_left(name);
		name = ".//right//" + intToString(num+1) + ".jpg";
		ifstream fin_right(name);
		if (fin_left&&fin_right)
		{
			num++;
		}
		else {
			return num;
		} 
	} 
}







