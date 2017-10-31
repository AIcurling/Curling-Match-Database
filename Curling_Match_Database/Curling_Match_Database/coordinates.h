#pragma once
#include "opencv\cv.h"
#include "opencv\highgui.h"
#include "opencv\cxcore.h"
#include <Windows.h>
#include <time.h>
#include <stdio.h>
#include <iostream>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\opencv.hpp>

using namespace std;
using namespace cv;

namespace coordinates {

	struct fpoint {
		float x;
		float y;
	};

	fpoint red_total[8];
	fpoint yellow_total[8];

	fpoint red_house[8];
	fpoint yellow_house[8];

	const unsigned int filter = 1000/*���� 375*/, filter2 = 280, files = 160000, width = 964, height = 1931;
	const unsigned short ori_step = width & 3, ori_WidthStep = width * 3 + ori_step, out_WidthStep = width + (ori_step == 0 ? 0 : 4 - ori_step);

	char filePath[1024];
	IplImage * source = 0;  // �÷�


	vector<CvPoint> tour;

	// ��ǥ ����� �迭
	int center_index;


	// ���� �׷��� 
	unsigned long ori_Index;
	unsigned int file, scale;
	unsigned short x, minX, maxX, y, minY, maxY;

	char to;
	char min = ((char)(uchar)0);
	char max = ((char)(uchar)255);
	char half = ((char)(uchar)127);
	char house = ((char)(uchar)200);

	IplImage *ori = cvCreateImage(cvSize(964, 1931), 8, 3), *out, *ori_next = cvCreateImage(cvSize(964, 1934), 8, 1);

	void growRegion_1();
	void growRegion_1_2();
	void growRegion_2();
	void growRegion_2_2();

	// x, y ��ǥ ��ȸ ����ȭ loop �� ���� �Ǻ� ��ũ��
#define ForAll ori_Index = 0; for ( y = 0; y < height; y++ , ori_Index += ori_step ) for ( x = 0; x < width; x++ , ori_Index += 3 )
#define red ori->imageData [ ori_Index ] == coordinates::min && ori->imageData [ ori_Index + 2 ] == coordinates::max && ori->imageData [ ori_Index + 1 ] == coordinates::min
#define yellow ori->imageData [ ori_Index ] == coordinates::min && ori->imageData [ ori_Index + 1 ] == coordinates::max && ori->imageData [ ori_Index + 2 ] == coordinates::max
#define bb ori->imageData [ ori_Index ] == coordinates::min && ori->imageData [ ori_Index + 1 ] == coordinates::min && ori->imageData [ ori_Index + 2 ] == coordinates::min
#define ww ori->imageData [ ori_Index ] == coordinates::max && ori->imageData [ ori_Index + 1 ] == coordinates::max && ori->imageData [ ori_Index + 2 ] == coordinates::max
#define house1 ori->imageData [ ori_Index ] == coordinates::max && ori->imageData [ ori_Index + 1 ] == coordinates::house && ori->imageData [ ori_Index + 2 ] == coordinates::house
#define house2 ori->imageData [ ori_Index ] == coordinates::house && ori->imageData [ ori_Index + 1 ] == coordinates::house && ori->imageData [ ori_Index + 2 ] == coordinates::max
#define null ori->imageData [ ori_Index ] = ori->imageData [ ori_Index + 1 ] = ori->imageData [ ori_Index + 2 ] =coordinates::max

	bool redAt(unsigned long index)
	{
		return ori->imageData[index] == coordinates::min && ori->imageData[index + 2] == coordinates::max && ori->imageData[index + 1] == coordinates::min;
	}

	bool yellowAt(unsigned long index)
	{
		return ori->imageData[index] == coordinates::min && ori->imageData[index + 1] == coordinates::max && ori->imageData[index + 2] == coordinates::max;
	}

	class Point
	{
	public:
		// yellow �˻�
		Point(unsigned short x, unsigned short y, unsigned long index)
		{
			// min max ��ǥ ����
			if (x > maxX) maxX = x;
			if (y > maxY) maxY = y;
			if (x < minX) minX = x;
			if (y < minY) minY = y;

			// ���� Ȯ�� �� 'Ž���Ϸ�' ǥ��
			scale++;
			ori->imageData[index] = half;
			CvPoint abc;
			abc.x = x;
			abc.y = y;
			tour.push_back(abc);

			// ���� �߰� with yellow ���͸� �� ��ǥ ���� ó��
			if (x > 0 && yellowAt(index - 3)) Point(x - 1, y, index - 3);
			if (x < width - 1 && yellowAt(index + 3)) Point(x + 1, y, index + 3);
			if (y > 0 && yellowAt(index - ori_WidthStep)) Point(x, y - 1, index - ori_WidthStep);
			if (y < height - 1 && yellowAt(index + ori_WidthStep)) Point(x, y + 1, index + ori_WidthStep);
		}

		// red �˻�
		Point(unsigned long index, unsigned short x, unsigned short y)
		{
			// min max ��ǥ ����
			if (x > maxX) maxX = x;
			if (y > maxY) maxY = y;
			if (x < minX) minX = x;
			if (y < minY) minY = y;

			// ���� Ȯ�� �� 'Ž���Ϸ�' ǥ��
			scale++;
			ori->imageData[index] = half;
			CvPoint abc;
			abc.x = x;
			abc.y = y;
			tour.push_back(abc);

			// ���� �߰� with red ���͸� �� ��ǥ ���� ó��
			if (x > 0 && redAt(index - 3)) Point(index - 3, x - 1, y);
			if (x < width - 1 && redAt(index + 3)) Point(index + 3, x + 1, y);
			if (y > 0 && redAt(index - ori_WidthStep)) Point(index - ori_WidthStep, x, y - 1);
			if (y < height - 1 && redAt(index + ori_WidthStep)) Point(index + ori_WidthStep, x, y + 1);
		}
	};

	int window_width = 60, window_height = 60;
	int black_pixel_count = 0;


	/*
	
	int coordinate1(IplImage *imageInput)     - ��ü����
		void growRegion_1()    - �ִ� �ּ� ���. total

	int coordinate1_2(IplImage *imageInput)   - �Ͽ콺����
		void growRegion_1_2()  - �ִ� �ּ� ���. house

	int coordinate2(IplImage *imageInput)     - ��ü����
		void growRegion_2()    - ��ü ���. total

	int coordinate2_2(IplImage *imageInput)   - �Ͽ콺����
		void growRegion_2_2()  - ��ü ���. house


		// x ->   (4.75 / 966.0)
		// y ->   (8.23 / 1739.0)


	*/



	// ��ǥ ���� �ڵ�

	// ���1. ��ü ����
	int coordinate1(IplImage *imageInput)
	{
		cvCopyImage(imageInput, ori);
		// ��ǥ ���� ����

		ForAll
			if ((house1) || (house2)) {
				null;
			}

		ForAll
			if (!(bb) && !(ww) && !(red) && !(yellow)) {
				ori->imageData[ori_Index] = coordinates::min;
				ori->imageData[ori_Index + 1] = coordinates::max;
				ori->imageData[ori_Index + 2] = coordinates::max;
			}
		// �߽���ǥ �ε��� �ʱ�ȭ
		center_index = 0;

		// �߽���ǥ �迭 �ʱ�ȭ
		for (int i = 0; i < 8; i++) {
			yellow_total[i].x = yellow_total[i].y = red_total[i].x = red_total[i].y = 0;
		}

		// ���� ������ ��ǥ ã��
		to = coordinates::min;
		// ��� ��ǥ�� ����
		ForAll
			// yellow��
			if (yellow)
				// ���� Ȯ��
				growRegion_1();

		center_index = 0;
		to = coordinates::max;
		// ��� ��ǥ�� ����
		ForAll
			// red��
			if (red)
				// ���� Ȯ��
				growRegion_1();

		return 0;
	}

	// ���1. �Ͽ콺 ����
	int coordinate1_2(IplImage *imageInput)
	{
		cvCopyImage(imageInput, ori);
		// ��ǥ ����, ���� ���, �ؽ�Ʈ ���
		// ��ǥ ���� ����

		ForAll
			if ((house1) || (house2)) {
				null;
			}

		ForAll
			if (!(bb) && !(ww) && !(red) && !(yellow)) {
				ori->imageData[ori_Index] = coordinates::min;
				ori->imageData[ori_Index + 1] = coordinates::max;
				ori->imageData[ori_Index + 2] = coordinates::max;
			}
		// �߽���ǥ �ε��� �ʱ�ȭ
		center_index = 0;

		// �߽���ǥ �迭 �ʱ�ȭ
		for (int i = 0; i < 8; i++) {
			yellow_house[i].x = yellow_house[i].y = red_house[i].x = red_house[i].y = 0;
		}

		// ���� ������ ��ǥ ã��
		to = coordinates::min;
		// ��� ��ǥ�� ����
		ForAll
			// yellow��
			if (yellow)
				// ���� Ȯ��
				growRegion_1_2();

		center_index = 0;
		to = coordinates::max;
		// ��� ��ǥ�� ����
		ForAll
			// red��
			if (red)
				// ���� Ȯ��
				growRegion_1_2();
		return 0;
	}

	// ���2. ��ü ����
	int coordinate2(IplImage *imageInput)
	{
		cvCopyImage(imageInput, ori);
		// ��ǥ ����, ���� ���, �ؽ�Ʈ ���
		// ��ǥ ���� ����

		ForAll
			if ((house1) || (house2)) {
				null;
			}

		ForAll
			if (!(bb) && !(ww) && !(red) && !(yellow)) {
				ori->imageData[ori_Index] = coordinates::min;
				ori->imageData[ori_Index + 1] = coordinates::max;
				ori->imageData[ori_Index + 2] = coordinates::max;
			}
		// �߽���ǥ �ε��� �ʱ�ȭ
		center_index = 0;

		// �߽���ǥ �迭 �ʱ�ȭ
		for (int i = 0; i < 8; i++) {
			yellow_total[i].x = yellow_total[i].y = red_total[i].x = red_total[i].y = 0;
		}

		// ���� ������ ��ǥ ã��
		to = coordinates::min;
		// ��� ��ǥ�� ����
		ForAll
			// yellow��
			if (yellow)
				// ���� Ȯ��
				growRegion_2();

		center_index = 0;
		to = coordinates::max;
		// ��� ��ǥ�� ����
		ForAll
			// red��
			if (red)
				// ���� Ȯ��
				growRegion_2();
		return 0;
	}

	// ���2. �Ͽ콺 ����
	int coordinate2_2(IplImage *imageInput)
	{
		cvCopyImage(imageInput, ori);

		ForAll
			if ((house1) || (house2)) {
				null;
			}

		ForAll
			if (!(bb) && !(ww) && !(red) && !(yellow)) {
				ori->imageData[ori_Index] = coordinates::min;
				ori->imageData[ori_Index + 1] = coordinates::max;
				ori->imageData[ori_Index + 2] = coordinates::max;
			}

		// ��ǥ ����, ���� ���, �ؽ�Ʈ ���
		// ��ǥ ���� ����

		// �߽���ǥ �ε��� �ʱ�ȭ
		center_index = 0;

		// �߽���ǥ �迭 �ʱ�ȭ
		for (int i = 0; i < 8; i++) {
			yellow_house[i].x = yellow_house[i].y = red_house[i].x = red_house[i].y = 0;
		}

		// ���� ������ ��ǥ ã��
		to = coordinates::min;
		// ��� ��ǥ�� ����
		ForAll
			// yellow��
			if (yellow)
				// ���� Ȯ��
				growRegion_2_2();

		center_index = 0;
		to = coordinates::max;
		// ��� ��ǥ�� ����
		ForAll
			// red��
			if (red)
				// ���� Ȯ��
				growRegion_2_2();
		return 0;
	}

	// ���1 - �ִ� �ּ� ���. total
	void growRegion_1()
	{
		// ���� ũ�� �� min, max �ʱ�ȭ
		float extra_x, extra_y;
		scale = 0;
		minX = maxX = x;
		minY = maxY = y;
		tour.clear();
		// ����� ���� Ž�� // (KIA~ �ڵ� ��� �ϳ� �þ��� ���⼭) -> (if������ red yellow �˻����� �ʰ� ��Ϳ� �����ڷ� �ڵ����� red yellow �з�)
		if (to == coordinates::min) coordinates::Point(x, y, ori_Index);
		else coordinates::Point(ori_Index, x, y);

		// ���� ǥ�� with ũ�� ���͸�
		if (scale > filter)
		{
			float y = (minY + maxY) / 2.0, x = (minX + maxX) / 2.0;


			// �ڡ� �ݿø� ���� �ڡ�
			/*
			extra_x, extra_y = 0;
			// y ����
			if ((y - (int)y) >= 0 && (y - (int)y) < 0.25) { extra_y = 0; }
			else if ((y - (int)y) >= 0.25 && (y - (int)y) < 0.75) { extra_y = 0.5; }
			else { extra_y = 1; }
			y = (int)y + extra_y;
			// x����
			if ((x - (int)x) >= 0 && (x - (int)x) < 0.25) { extra_x = 0; }
			else if ((x - (int)x) >= 0.25 && (x - (int)x) < 0.75) { extra_x = 0.5; }
			else { extra_x = 1; }
			x = (int)x + extra_x;
			*/

			// x ->   (4.75 / 966.0)
			// y ->   (8.23 / 1739.0)

			if (to == coordinates::min) {
				yellow_total[center_index].x = x;
				yellow_total[center_index].y = y;
			}
			else if (to == coordinates::max) {
				red_total[center_index].x = x;
				red_total[center_index].y = y;
			}
			center_index++;
		}
	}

	// ���1 - �ִ� �ּ� ���. house
	void growRegion_1_2()
	{
		// ���� ũ�� �� min, max �ʱ�ȭ
		float extra_x, extra_y;
		scale = 0;
		minX = maxX = x;
		minY = maxY = y;
		tour.clear();
		// ����� ���� Ž�� // (KIA~ �ڵ� ��� �ϳ� �þ��� ���⼭) -> (if������ red yellow �˻����� �ʰ� ��Ϳ� �����ڷ� �ڵ����� red yellow �з�)
		if (to == coordinates::min) coordinates::Point(x, y, ori_Index);
		else coordinates::Point(ori_Index, x, y);

		// ���� ǥ�� with ũ�� ���͸�
		if (scale > filter)
		{
			float y = (minY + maxY) / 2.0, x = (minX + maxX) / 2.0;


			// �ڡ� �ݿø� ���� �ڡ�

			/*
			extra_x, extra_y = 0;
			// y ����
			if ((y - (int)y) >= 0 && (y - (int)y) < 0.25) { extra_y = 0; }
			else if ((y - (int)y) >= 0.25 && (y - (int)y) < 0.75) { extra_y = 0.5; }
			else { extra_y = 1; }
			y = (int)y + extra_y;
			// x����
			if ((x - (int)x) >= 0 && (x - (int)x) < 0.25) { extra_x = 0; }
			else if ((x - (int)x) >= 0.25 && (x - (int)x) < 0.75) { extra_x = 0.5; }
			else { extra_x = 1; }
			x = (int)x + extra_x;
			*/

			// x ->   (4.75 / 966.0)
			// y ->   (8.23 / 1739.0)

			if (to == coordinates::min) {
				if (y > 60) {
					yellow_house[center_index].x = x;
					yellow_house[center_index].y = y;
				}
			}
			else if (to == coordinates::max) {
				if (y > 60) {
					red_house[center_index].x = x;
					red_house[center_index].y = y;
				}
			}
			center_index++;
		}
	}

	// ���2 - ��ü ���. total
	void growRegion_2()
	{
		// ���� ũ�� �� min, max �ʱ�ȭ
		float extra_x, extra_y;
		scale = 0;
		minX = maxX = x;
		minY = maxY = y;
		tour.clear();
		// ����� ���� Ž�� // (KIA~ �ڵ� ��� �ϳ� �þ��� ���⼭) -> (if������ red yellow �˻����� �ʰ� ��Ϳ� �����ڷ� �ڵ����� red yellow �з�)
		if (to == coordinates::min) coordinates::Point(x, y, ori_Index);
		else coordinates::Point(ori_Index, x, y);

		// ���� ǥ�� with ũ�� ���͸�
		if (scale > filter)
		{
			float sumY = 0, sumX = 0;
			for (int tours = 0; tours < tour.size(); tours++) {
				sumY += tour[tours].y;
				sumX += tour[tours].x;
			}
			float y = sumY / tour.size(), x = sumX / tour.size();


			// �ڡ� �ݿø� ���� �ڡ�

			/*
			extra_x, extra_y = 0;
			// y ����
			if ((y - (int)y) >= 0 && (y - (int)y) < 0.25) { extra_y = 0; }
			else if ((y - (int)y) >= 0.25 && (y - (int)y) < 0.75) { extra_y = 0.5; }
			else { extra_y = 1; }
			y = (int)y + extra_y;
			// x����
			if ((x - (int)x) >= 0 && (x - (int)x) < 0.25) { extra_x = 0; }
			else if ((x - (int)x) >= 0.25 && (x - (int)x) < 0.75) { extra_x = 0.5; }
			else { extra_x = 1; }
			x = (int)x + extra_x;
			*/

			// x ->   (4.75 / 966.0)
			// y ->   (8.23 / 1739.0)

			if (to == coordinates::min) {
				yellow_total[center_index].x = x;
				yellow_total[center_index].y = y;
			}
			else if (to == coordinates::max) {
				red_total[center_index].x = x;
				red_total[center_index].y = y;
			}
			center_index++;
		}
	}

	// ���2 - ��ü ���. house
	void growRegion_2_2()
	{
		// ���� ũ�� �� min, max �ʱ�ȭ
		float extra_x, extra_y;
		scale = 0;
		minX = maxX = x;
		minY = maxY = y;
		tour.clear();
		// ����� ���� Ž�� // (KIA~ �ڵ� ��� �ϳ� �þ��� ���⼭) -> (if������ red yellow �˻����� �ʰ� ��Ϳ� �����ڷ� �ڵ����� red yellow �з�)
		if (to == coordinates::min) coordinates::Point(x, y, ori_Index);
		else coordinates::Point(ori_Index, x, y);

		// ���� ǥ�� with ũ�� ���͸�
		if (scale > filter)
		{
			float sumY = 0, sumX = 0;
			for (int tours = 0; tours < tour.size(); tours++) {
				sumY += tour[tours].y;
				sumX += tour[tours].x;
			}
			float y = sumY / tour.size(), x = sumX / tour.size();


			// �ڡ� �ݿø� ���� �ڡ�

			/*
			extra_x, extra_y = 0;
			// y ����
			if ((y - (int)y) >= 0 && (y - (int)y) < 0.25) { extra_y = 0; }
			else if ((y - (int)y) >= 0.25 && (y - (int)y) < 0.75) { extra_y = 0.5; }
			else { extra_y = 1; }
			y = (int)y + extra_y;
			// x����
			if ((x - (int)x) >= 0 && (x - (int)x) < 0.25) { extra_x = 0; }
			else if ((x - (int)x) >= 0.25 && (x - (int)x) < 0.75) { extra_x = 0.5; }
			else { extra_x = 1; }
			x = (int)x + extra_x;
			*/

			// x ->   (4.75 / 966.0)
			// y ->   (8.23 / 1739.0)

			if (to == coordinates::min) {
				if (y > 60) {
					yellow_house[center_index].x = x;
					yellow_house[center_index].y = y;
				}
			}
			else if (to == coordinates::max) {
				if (y > 60) {
					red_house[center_index].x = x;
					red_house[center_index].y = y;
				}
			}
			center_index++;
		}
	}
}
