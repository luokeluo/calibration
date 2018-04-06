#pragma once
#include"help.h"

void picture(int num  , string name  , int number ) // ������Ƭ �����left right�����ļ���
{
	if (name == NULL)
	{
		int num_picture = 20;  // ����Ƭ����
		int num_corr_board_w = 9; // ����9��
		int num_corr_board_h = 6;// ����6��
		int length_board = 26;// ʵ�ʳ��� ����
		int num_corr = num_corr_board_w*num_corr_board_h; // �ܽ������
		Size board_size = Size(num_corr_board_w, num_corr_board_h);  // �������ÿ�С��еĽǵ���  
		Size square_size = Size(length_board, length_board); // ʵ�ʲ����õ��Ķ������ÿ�����̸�Ĵ�С  
		vector<Point2f> corners1;                  /****    ����ÿ��ͼ���ϼ�⵽�Ľǵ�       ****/
		vector<Point2f> corners2;
		VideoCapture capture(1);
		capture.set(CV_CAP_PROP_FPS, 480);
		if (!capture.isOpened()) {
			help(1);
		}
		namedWindow("���ͼ��");
		namedWindow("�ұ�ͼ��");
		int successes = 0;
		int corner_count = 0;// ���ڼ�¼�ҵ��Ľǵ�����
		help(0);
		while (successes < num) {
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
		cout << ""<<endl;
	}
}

int goodPictureNumber()       // ���������left right�ļ��������Ч��Ƭ�Եĸ���
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







