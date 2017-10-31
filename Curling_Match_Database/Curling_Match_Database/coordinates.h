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

	const unsigned int filter = 1000/*원래 375*/, filter2 = 280, files = 160000, width = 964, height = 1931;
	const unsigned short ori_step = width & 3, ori_WidthStep = width * 3 + ori_step, out_WidthStep = width + (ori_step == 0 ? 0 : 4 - ori_step);

	char filePath[1024];
	IplImage * source = 0;  // 컬러


	vector<CvPoint> tour;

	// 좌표 저장용 배열
	int center_index;


	// 리전 그로잉 
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

	// x, y 좌표 순회 최적화 loop 및 색상 판별 매크로
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
		// yellow 검사
		Point(unsigned short x, unsigned short y, unsigned long index)
		{
			// min max 좌표 조정
			if (x > maxX) maxX = x;
			if (y > maxY) maxY = y;
			if (x < minX) minX = x;
			if (y < minY) minY = y;

			// 영역 확장 및 '탐색완료' 표시
			scale++;
			ori->imageData[index] = half;
			CvPoint abc;
			abc.x = x;
			abc.y = y;
			tour.push_back(abc);

			// 영역 추가 with yellow 필터링 및 좌표 예외 처리
			if (x > 0 && yellowAt(index - 3)) Point(x - 1, y, index - 3);
			if (x < width - 1 && yellowAt(index + 3)) Point(x + 1, y, index + 3);
			if (y > 0 && yellowAt(index - ori_WidthStep)) Point(x, y - 1, index - ori_WidthStep);
			if (y < height - 1 && yellowAt(index + ori_WidthStep)) Point(x, y + 1, index + ori_WidthStep);
		}

		// red 검사
		Point(unsigned long index, unsigned short x, unsigned short y)
		{
			// min max 좌표 조정
			if (x > maxX) maxX = x;
			if (y > maxY) maxY = y;
			if (x < minX) minX = x;
			if (y < minY) minY = y;

			// 영역 확장 및 '탐색완료' 표시
			scale++;
			ori->imageData[index] = half;
			CvPoint abc;
			abc.x = x;
			abc.y = y;
			tour.push_back(abc);

			// 영역 추가 with red 필터링 및 좌표 예외 처리
			if (x > 0 && redAt(index - 3)) Point(index - 3, x - 1, y);
			if (x < width - 1 && redAt(index + 3)) Point(index + 3, x + 1, y);
			if (y > 0 && redAt(index - ori_WidthStep)) Point(index - ori_WidthStep, x, y - 1);
			if (y < height - 1 && redAt(index + ori_WidthStep)) Point(index + ori_WidthStep, x, y + 1);
		}
	};

	int window_width = 60, window_height = 60;
	int black_pixel_count = 0;


	/*
	
	int coordinate1(IplImage *imageInput)     - 전체영역
		void growRegion_1()    - 최대 최소 평균. total

	int coordinate1_2(IplImage *imageInput)   - 하우스영역
		void growRegion_1_2()  - 최대 최소 평균. house

	int coordinate2(IplImage *imageInput)     - 전체영역
		void growRegion_2()    - 전체 평균. total

	int coordinate2_2(IplImage *imageInput)   - 하우스영역
		void growRegion_2_2()  - 전체 평균. house


		// x ->   (4.75 / 966.0)
		// y ->   (8.23 / 1739.0)


	*/



	// 좌표 추출 코드

	// 방식1. 전체 영역
	int coordinate1(IplImage *imageInput)
	{
		cvCopyImage(imageInput, ori);
		// 좌표 추출 연산

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
		// 중심좌표 인덱스 초기화
		center_index = 0;

		// 중심좌표 배열 초기화
		for (int i = 0; i < 8; i++) {
			yellow_total[i].x = yellow_total[i].y = red_total[i].x = red_total[i].y = 0;
		}

		// 현재 샷에서 좌표 찾기
		to = coordinates::min;
		// 모든 좌표에 대해
		ForAll
			// yellow면
			if (yellow)
				// 영역 확장
				growRegion_1();

		center_index = 0;
		to = coordinates::max;
		// 모든 좌표에 대해
		ForAll
			// red면
			if (red)
				// 영역 확장
				growRegion_1();

		return 0;
	}

	// 방식1. 하우스 영역
	int coordinate1_2(IplImage *imageInput)
	{
		cvCopyImage(imageInput, ori);
		// 좌표 추출, 영상에 출력, 텍스트 출력
		// 좌표 추출 연산

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
		// 중심좌표 인덱스 초기화
		center_index = 0;

		// 중심좌표 배열 초기화
		for (int i = 0; i < 8; i++) {
			yellow_house[i].x = yellow_house[i].y = red_house[i].x = red_house[i].y = 0;
		}

		// 현재 샷에서 좌표 찾기
		to = coordinates::min;
		// 모든 좌표에 대해
		ForAll
			// yellow면
			if (yellow)
				// 영역 확장
				growRegion_1_2();

		center_index = 0;
		to = coordinates::max;
		// 모든 좌표에 대해
		ForAll
			// red면
			if (red)
				// 영역 확장
				growRegion_1_2();
		return 0;
	}

	// 방식2. 전체 영역
	int coordinate2(IplImage *imageInput)
	{
		cvCopyImage(imageInput, ori);
		// 좌표 추출, 영상에 출력, 텍스트 출력
		// 좌표 추출 연산

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
		// 중심좌표 인덱스 초기화
		center_index = 0;

		// 중심좌표 배열 초기화
		for (int i = 0; i < 8; i++) {
			yellow_total[i].x = yellow_total[i].y = red_total[i].x = red_total[i].y = 0;
		}

		// 현재 샷에서 좌표 찾기
		to = coordinates::min;
		// 모든 좌표에 대해
		ForAll
			// yellow면
			if (yellow)
				// 영역 확장
				growRegion_2();

		center_index = 0;
		to = coordinates::max;
		// 모든 좌표에 대해
		ForAll
			// red면
			if (red)
				// 영역 확장
				growRegion_2();
		return 0;
	}

	// 방식2. 하우스 영역
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

		// 좌표 추출, 영상에 출력, 텍스트 출력
		// 좌표 추출 연산

		// 중심좌표 인덱스 초기화
		center_index = 0;

		// 중심좌표 배열 초기화
		for (int i = 0; i < 8; i++) {
			yellow_house[i].x = yellow_house[i].y = red_house[i].x = red_house[i].y = 0;
		}

		// 현재 샷에서 좌표 찾기
		to = coordinates::min;
		// 모든 좌표에 대해
		ForAll
			// yellow면
			if (yellow)
				// 영역 확장
				growRegion_2_2();

		center_index = 0;
		to = coordinates::max;
		// 모든 좌표에 대해
		ForAll
			// red면
			if (red)
				// 영역 확장
				growRegion_2_2();
		return 0;
	}

	// 방식1 - 최대 최소 평균. total
	void growRegion_1()
	{
		// 영역 크기 및 min, max 초기화
		float extra_x, extra_y;
		scale = 0;
		minX = maxX = x;
		minY = maxY = y;
		tour.clear();
		// 재귀적 영역 탐색 // (KIA~ 코딩 기법 하나 늘었네 여기서) -> (if문으로 red yellow 검사하지 않고 재귀와 생성자로 자동으로 red yellow 분류)
		if (to == coordinates::min) coordinates::Point(x, y, ori_Index);
		else coordinates::Point(ori_Index, x, y);

		// 영역 표시 with 크기 필터링
		if (scale > filter)
		{
			float y = (minY + maxY) / 2.0, x = (minX + maxX) / 2.0;


			// ★★ 반올림 관련 ★★
			/*
			extra_x, extra_y = 0;
			// y 관련
			if ((y - (int)y) >= 0 && (y - (int)y) < 0.25) { extra_y = 0; }
			else if ((y - (int)y) >= 0.25 && (y - (int)y) < 0.75) { extra_y = 0.5; }
			else { extra_y = 1; }
			y = (int)y + extra_y;
			// x관련
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

	// 방식1 - 최대 최소 평균. house
	void growRegion_1_2()
	{
		// 영역 크기 및 min, max 초기화
		float extra_x, extra_y;
		scale = 0;
		minX = maxX = x;
		minY = maxY = y;
		tour.clear();
		// 재귀적 영역 탐색 // (KIA~ 코딩 기법 하나 늘었네 여기서) -> (if문으로 red yellow 검사하지 않고 재귀와 생성자로 자동으로 red yellow 분류)
		if (to == coordinates::min) coordinates::Point(x, y, ori_Index);
		else coordinates::Point(ori_Index, x, y);

		// 영역 표시 with 크기 필터링
		if (scale > filter)
		{
			float y = (minY + maxY) / 2.0, x = (minX + maxX) / 2.0;


			// ★★ 반올림 관련 ★★

			/*
			extra_x, extra_y = 0;
			// y 관련
			if ((y - (int)y) >= 0 && (y - (int)y) < 0.25) { extra_y = 0; }
			else if ((y - (int)y) >= 0.25 && (y - (int)y) < 0.75) { extra_y = 0.5; }
			else { extra_y = 1; }
			y = (int)y + extra_y;
			// x관련
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

	// 방식2 - 전체 평균. total
	void growRegion_2()
	{
		// 영역 크기 및 min, max 초기화
		float extra_x, extra_y;
		scale = 0;
		minX = maxX = x;
		minY = maxY = y;
		tour.clear();
		// 재귀적 영역 탐색 // (KIA~ 코딩 기법 하나 늘었네 여기서) -> (if문으로 red yellow 검사하지 않고 재귀와 생성자로 자동으로 red yellow 분류)
		if (to == coordinates::min) coordinates::Point(x, y, ori_Index);
		else coordinates::Point(ori_Index, x, y);

		// 영역 표시 with 크기 필터링
		if (scale > filter)
		{
			float sumY = 0, sumX = 0;
			for (int tours = 0; tours < tour.size(); tours++) {
				sumY += tour[tours].y;
				sumX += tour[tours].x;
			}
			float y = sumY / tour.size(), x = sumX / tour.size();


			// ★★ 반올림 관련 ★★

			/*
			extra_x, extra_y = 0;
			// y 관련
			if ((y - (int)y) >= 0 && (y - (int)y) < 0.25) { extra_y = 0; }
			else if ((y - (int)y) >= 0.25 && (y - (int)y) < 0.75) { extra_y = 0.5; }
			else { extra_y = 1; }
			y = (int)y + extra_y;
			// x관련
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

	// 방식2 - 전체 평균. house
	void growRegion_2_2()
	{
		// 영역 크기 및 min, max 초기화
		float extra_x, extra_y;
		scale = 0;
		minX = maxX = x;
		minY = maxY = y;
		tour.clear();
		// 재귀적 영역 탐색 // (KIA~ 코딩 기법 하나 늘었네 여기서) -> (if문으로 red yellow 검사하지 않고 재귀와 생성자로 자동으로 red yellow 분류)
		if (to == coordinates::min) coordinates::Point(x, y, ori_Index);
		else coordinates::Point(ori_Index, x, y);

		// 영역 표시 with 크기 필터링
		if (scale > filter)
		{
			float sumY = 0, sumX = 0;
			for (int tours = 0; tours < tour.size(); tours++) {
				sumY += tour[tours].y;
				sumX += tour[tours].x;
			}
			float y = sumY / tour.size(), x = sumX / tour.size();


			// ★★ 반올림 관련 ★★

			/*
			extra_x, extra_y = 0;
			// y 관련
			if ((y - (int)y) >= 0 && (y - (int)y) < 0.25) { extra_y = 0; }
			else if ((y - (int)y) >= 0.25 && (y - (int)y) < 0.75) { extra_y = 0.5; }
			else { extra_y = 1; }
			y = (int)y + extra_y;
			// x관련
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
