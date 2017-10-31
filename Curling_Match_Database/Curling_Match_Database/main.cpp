#include "opencv\cv.h"
#include "opencv\highgui.h"
#include "opencv\cxcore.h"
#include <Windows.h>
#include <time.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\opencv.hpp>
#include "coordinates.h"

using namespace std;
using namespace cv;

ofstream result_text;
//ofstream result_first_thrown;
//ofstream result_total_count;
//ofstream result_tee_point;

coordinates::fpoint tee_point;
coordinates::fpoint tee_point_real;

coordinates::fpoint red_fullSize[8];
coordinates::fpoint yellow_fullSize[8];

coordinates::fpoint red_houseSize[8];
coordinates::fpoint yellow_houseSize[8];

coordinates::fpoint red_houseSize_zero[8];
coordinates::fpoint yellow_houseSize_zero[8];

coordinates::fpoint red_real[8];
coordinates::fpoint yellow_real[8];

void checkCenterLine(IplImage * src);
int FindFirstMove_Yellow(IplImage * src);
int FindFirstMove_Red(IplImage * src);

char *token;
vector<char*> receive;
char message[10240];

void parse(char *in)
{
	receive.clear();
	token = strtok(in, "\t");
	while (token != NULL)
	{
		receive.push_back(token);
		token = strtok(NULL, "\t");
	}
}

int main(void)
{
	int match, end, shot, last_shot, numberOfstone, team_flag, found;
	float x_variance = 0.0, y_variance = 0.0;
	int p_match = 107, p_end = 2;
	unsigned long int total_count;
	char filePath_load_image[1024], filePath_load_text[1024], filePath_check[1024], filePath_save_image[1024];
	char strategy_label[1024], strategy_label_1[1024], match_end_shot[1024], match_end_shot_1[1024], inputString[1024], inputString_1[1024], inputString_next[1024], strategy[1024], nostatistics[1024], notplayed[1024], NOTstrategy[1024];
	char print_position[128];
	CvFont Font;


	result_text.open("Result/result.txt");
	if (result_text.fail()) {
		return 1;
	}
	//result_first_thrown.open("result_first_thrown.txt");
	//if (result_first_thrown.fail()) {
	//	return 1;
	//}

	//result_total_count.open("result_total_count.txt");
	//if (result_total_count.fail()) {
	//	return 1;
	//}

	//result_tee_point.open("result_tee_point.txt");
	//if (result_total_count.fail()) {
	//	return 1;
	//}

	sprintf(notplayed, "notplayed");
	string test_1(notplayed);

	sprintf(nostatistics, "nostatistics");
	string test_2(nostatistics);


	total_count = 0;
	for (match = 1; match <= 41; match++) {

		// curlit 텍스트 파일 로드
		ifstream curlit;
		sprintf(filePath_load_text, "SourceText/curlit (%d).txt", match);
		curlit.open(filePath_load_text);
		if (curlit.fail()){
			return 1;
		}

		for (end = 1; end <= 12; end++) {

			// 불량한 매치/엔드 는 건너뜀
			if ((match == 163 && end == 2) || (match == 174 && end == 6) || (match == 175 && end == 4)){
				continue;
			}

			// 이번 엔드 존재하는 샷 갯수 찾기
			for (shot = 1; shot <= 17; shot++) {
				sprintf(filePath_check, "SourceImage/%04d %04d %04d.png", match, end, shot);
				IplImage * check = cvLoadImage(filePath_check, CV_LOAD_IMAGE_COLOR);
				if (check == 0) {
					last_shot = shot - 1;  // 15번째 샷까지 있어서 16번째 샷이미지가 없다면 last_shot 에는 15가 저장됨
					cvReleaseImage(&check);
					break;
				}
				else{
					cvReleaseImage(&check);
				}
			}

			// 선후공 변수 초기화
			team_flag = -1;
			// OP, MY 좌표 찾고 전략 맵핑하기
			for (shot = 1; shot <= last_shot - 1; shot++){

				// 이미지 로드
				sprintf(filePath_load_image, "SourceImage/%04d %04d %04d.png", match, end, shot);
				IplImage * source = cvLoadImage(filePath_load_image, CV_LOAD_IMAGE_COLOR);
				if (source == 0) {
					break;
				}

				// 스톤 갯수 초기화
				numberOfstone = 0;

				// 좌표 배열 초기화
				for (int i = 0; i < 8; i++) {
					red_fullSize[i].x = red_fullSize[i].y = yellow_fullSize[i].x = yellow_fullSize[i].y = 0;
					red_houseSize[i].x = red_houseSize[i].y = yellow_houseSize[i].x = yellow_houseSize[i].y = 0;
					red_houseSize_zero[i].x = red_houseSize_zero[i].y = yellow_houseSize_zero[i].x = yellow_houseSize_zero[i].y = 0;
				}
				tee_point.x = 0, tee_point.y = 0, tee_point_real.x = 0; tee_point_real.y = 0;

				coordinates::coordinate2_2(source); // 경기장 영역
				coordinates::coordinate2(source);   // 전체 영역

				for (int i = 0; i < 8; i++){

					// 하우스 영역
					red_houseSize[i].x = coordinates::red_house[i].x;
					red_houseSize[i].y = coordinates::red_house[i].y;
					yellow_houseSize[i].x = coordinates::yellow_house[i].x;
					yellow_houseSize[i].y = coordinates::yellow_house[i].y;

					// 전체 영역
					red_fullSize[i].x = coordinates::red_total[i].x;
					red_fullSize[i].y = coordinates::red_total[i].y;
					yellow_fullSize[i].x = coordinates::yellow_total[i].x;
					yellow_fullSize[i].y = coordinates::yellow_total[i].y;
				}

				// 경기장에 존재하는 스톤 갯수 세기
				for (int i = 0; i < 8; i++){
					if (red_houseSize[i].x != 0 || red_houseSize[i].y != 0){
						numberOfstone++;
					}
					if (yellow_houseSize[i].x != 0 || yellow_houseSize[i].y != 0){
						numberOfstone++;
					}
				}

				// 선공이 무슨색 스톤인지 확인
				if (shot == 1){ // 첫번재 샷일때
					if (match <= 853){
						// 빨간 스톤이 존재하는지
						if (red_fullSize[0].x != 0 && yellow_fullSize[0].x == 0){
							team_flag = 0; // 레드 선공
							//result_first_thrown << match << " 매치 " << end << " 엔드 : R 선공" << endl;
						}
						else if (red_fullSize[0].x == 0 && yellow_fullSize[0].x != 0) {// 노란 스톤이 존재하는지
							team_flag = 1; // 옐로우 선공
							//result_first_thrown << match << " 매치 " << end << " 엔드 : Y 선공" << endl;
						}
						else {
							//result_first_thrown << match << " 매치 " << end << " 엔드 : 첫번째 쏜 스톤 못찾음" << endl;
						}
					}
					else if (match > 853) {
						if (FindFirstMove_Red(source) == 0){
							team_flag = 0;
							//result_first_thrown << match << " 매치 " << end << " 엔드 : R 선공" << endl;
						}
						else if (FindFirstMove_Yellow(source) == 1){
							team_flag = 1;
							//result_first_thrown << match << " 매치 " << end << " 엔드 : Y 선공" << endl;
						}
						else{
							team_flag = -1;
							//result_first_thrown << match << " 매치 " << end << " 엔드 : 첫번째 쏜 스톤 못찾음" << endl;
						}
					}
				}

				// 경기장 존재하는 스톤 갯수 3개 이상이면
				if (numberOfstone > 2) {

					// 티 좌표 측정 
					checkCenterLine(source);
					/*if (tee_point.x == 0 || tee_point.y == 0){
						result_tee_point << match << " " << end << " " << shot << " 티포인트 에러남" << endl;
					}*/

					// 텍스트 처리
					sprintf(match_end_shot, "%04d %04d %04d", match, end, shot + 1);
					string label(match_end_shot);

					found = 1;
					while (found){
						curlit.getline(inputString, 100);
						string check(inputString);

						if (check == label){ // match end shot 찾으면

							found = 0; // while문 빠져나가기용
							curlit.getline(strategy_label, 100); // 다음줄 가져옴 draw	in	4
							parse(strategy_label);
							strcpy_s(strategy, receive[0]);
							/*string notcheck(strategy);
							if (notcheck == test_1 || notcheck == test_2){
							continue;
							}*/
							// char strategy_label[1024] 에 넣어야 함


						}
					}
					string notcheck(strategy);
					if (notcheck == test_1 || notcheck == test_2){
						continue;
					}

					total_count++;
					

					tee_point_real.x = (tee_point.x*4.75 / 963);    // -> 2.382    / 원래는 2.375
					tee_point_real.y = (tee_point.y*8.23 / 1741.5); // -> 2.429    / 원래는 4.880
					x_variance = 2.375 - tee_point_real.x;
					y_variance = 4.88 - tee_point_real.y;

					for (int i = 0; i < 8; i++){
						if (red_houseSize[i].x != 0){
							red_real[i].x = (red_houseSize[i].x * 4.75 / 963) + x_variance;       // 반올림  
							red_real[i].y = (red_houseSize[i].y * 8.23 / 1741.5) + y_variance;    // 
						}
						if (yellow_houseSize[i].x != 0){
							yellow_real[i].x = (yellow_houseSize[i].x * 4.75 / 963) + x_variance;    //
							yellow_real[i].y = (yellow_houseSize[i].y * 8.23 / 1741.5) + y_variance; //
						}
					}




					result_text << setfill('0') << setw(6) << total_count << endl;


					if (team_flag == 0){ // 레드 선공이면

						if (shot % 2 == 1){ // 1 3 5 7 9 11 13 15

							result_text << "OP ";
							for (int i = 0; i < 8; i++){ // 레드가 OP
								if (red_houseSize[i].x != 0){
									result_text << fixed << setprecision(3) << red_real[i].x << " " << fixed << setprecision(3) << red_real[i].y << " ";
								}
							}
							result_text << endl;

							result_text << "MY ";
							for (int i = 0; i < 8; i++){ // 옐로우가 MY
								if (yellow_houseSize[i].x != 0){
									result_text << fixed << setprecision(3) << yellow_real[i].x << " " << fixed << setprecision(3) << yellow_real[i].y << " ";
								}
							}
							result_text << endl;

						}
						else if (shot % 2 == 0){ // 2 4 6 8 10 12 14 16

							result_text << "OP ";
							for (int i = 0; i < 8; i++){ // 옐로우가 OP
								if (yellow_houseSize[i].x != 0){
									result_text << fixed << setprecision(3) << yellow_real[i].x << " " << fixed << setprecision(3) << yellow_real[i].y << " ";
								}
							}
							result_text << endl;

							result_text << "MY ";
							for (int i = 0; i < 8; i++){ // 레드가 MY
								if (red_houseSize[i].x != 0){
									result_text << fixed << setprecision(3) << red_real[i].x << " " << fixed << setprecision(3) << red_real[i].y << " ";
								}
							}
							result_text << endl;

						}
					}

					else if (team_flag == 1){ // 옐로우 선공이면

						if (shot % 2 == 1){ // 1 3 5 7 9 11 13 15

							result_text << "OP ";
							for (int i = 0; i < 8; i++){ // 옐로우가 OP
								if (yellow_houseSize[i].x != 0){
									result_text << fixed << setprecision(3) << yellow_real[i].x << " " << fixed << setprecision(3) << yellow_real[i].y << " ";
								}
							}
							result_text << endl;

							result_text << "MY ";
							for (int i = 0; i < 8; i++){ // 레드가 MY
								if (red_houseSize[i].x != 0){
									result_text << fixed << setprecision(3) << red_real[i].x << " " << fixed << setprecision(3) << red_real[i].y << " ";
								}
							}
							result_text << endl;

						}

						else if (shot % 2 == 0){ // 2 4 6 8 10 12 14 16

							result_text << "OP ";
							for (int i = 0; i < 8; i++){ // 레드가 OP
								if (red_houseSize[i].x != 0){
									result_text << fixed << setprecision(3) << red_real[i].x << " " << fixed << setprecision(3) << red_real[i].y << " ";
								}
							}
							result_text << endl;

							result_text << "MY ";
							for (int i = 0; i < 8; i++){ // 옐로우가 MY
								if (yellow_houseSize[i].x != 0){
									result_text << fixed << setprecision(3) << yellow_real[i].x << " " << fixed << setprecision(3) << yellow_real[i].y << " ";
								}
							}
							result_text << endl;

						}
					}

					else {
						result_text << "에러남 확인필요" << endl;
					}

					//result_text << "TEE " << fixed << setprecision(3) << tee_point_real.x << " " << fixed << setprecision(3) << tee_point_real.y << endl;
					result_text << strategy;
					result_text << endl;
					result_text << endl;
					//total_count++;

					cout << match << " 매치 " << end << " 엔드 " << shot << " 샷 DB 생성 완료 / 누적 데이터 수 : " << total_count << " 개" << endl;

					cvReleaseImage(&source);
				}
				else{
					cvReleaseImage(&source);
				}
			}
		}
	}
	//result_total_count << "총 구축된 샷 데이터 갯수는 : " << total_count;

	//result_tee_point.close();
	//result_total_count.close();
	result_text.close();
	//result_first_thrown.close();
	return 0;
}

void checkCenterLine(IplImage * src)
{
	// 티좌표 초기화

	tee_point.x = tee_point.y = 0;
	int black_pixel_count;

	// y축 직선검사 -> x축 중심 좌표 찾기 - 검정픽셀 1000개 이상이면
	for (int x = 470; x < 490; x++){
		black_pixel_count = 0;
		for (int y = 0; y < 1931; y++){
			CvScalar s;
			s = cvGet2D(src, y, x);
			if (s.val[0] == 0 && s.val[1] == 0 && s.val[2] == 0)
				black_pixel_count++;
		}
		if (black_pixel_count > 1000){
			tee_point.x = x + 1;
			break;
		}
	}

	// x축 직선검사 - 검정픽셀 450개 이상이면
	for (int y = 505; y < 522; y++){
		black_pixel_count = 0;
		for (int x = 0; x < 964; x++){
			CvScalar s;
			s = cvGet2D(src, y, x);
			if (s.val[0] == 0 && s.val[1] == 0 && s.val[2] == 0)
				black_pixel_count++;
		}
		if (black_pixel_count > 450){
			tee_point.y = y + 1;
			break;
		}
	}

	return;
}

int FindFirstMove_Red(IplImage * src)
{
	int  upper_x, upper_y, thrown_black_pixel_count, thrown_index;
	IplImage* roi = (IplImage*)cvClone(src); // 소스 이미지 roi로 카피
	IplImage* cropped_red = cvCreateImage(cvSize(55, 55)/**/, 8, 3); // 필터생성 55 X 55 

	thrown_index = -1;
	for (int red_index = 0; red_index < 8; red_index++) {

		if (red_fullSize[red_index].x == 0 && red_fullSize[red_index].y == 0) continue;

		// 필터의 좌상단 좌표 셋팅
		upper_x = red_fullSize[red_index].x - 27/**/;
		upper_y = red_fullSize[red_index].y - 27/**/;

		// roi 전체 영역에서 55 X 55 크기의 부분 잘라냄
		cvSetImageROI(roi, cvRect(upper_x, upper_y, 55/**/, 55/**/)); // cvRect(x,y,width,height)
		// 잘린 영역이 cropped 로 넘어감
		cvCopy(roi, cropped_red);

		thrown_black_pixel_count = 0;

		for (int j = 0; j < 55/**/; j++) {
			for (int k = 0; k < 55/**/; k++) {
				CvScalar s;
				s = cvGet2D(cropped_red, j, k);
				if (s.val[0] == 0 && s.val[1] == 0 && s.val[2] == 0)
					thrown_black_pixel_count++;
			}
		}

		// 레드 선공이냐?
		if (thrown_black_pixel_count > 700) {
			cvReleaseImage(&roi);
			cvReleaseImage(&cropped_red);
			thrown_index = 0;
			return thrown_index;
		}
	}

	// 레드인지 옐로우인지 아무 답도 없다면
	if (thrown_index == -1){
		cvReleaseImage(&roi);
		cvReleaseImage(&cropped_red);
		return -1;
	}
}

int FindFirstMove_Yellow(IplImage * src)
{
	int  upper_x, upper_y, thrown_black_pixel_count, thrown_index;
	IplImage* roi = (IplImage*)cvClone(src); // 소스 이미지 roi로 카피
	IplImage* cropped_yellow = cvCreateImage(cvSize(55, 55)/**/, 8, 3); // 필터생성 55 X 55 

	thrown_index = -1;
	for (int yellow_index = 0; yellow_index < 8; yellow_index++) {

		if (yellow_fullSize[yellow_index].x == 0 && yellow_fullSize[yellow_index].y == 0) continue;

		// 필터의 좌상단 좌표 셋팅
		upper_x = yellow_fullSize[yellow_index].x - 27/**/;
		upper_y = yellow_fullSize[yellow_index].y - 27/**/;

		// roi 전체 영역에서 55 X 55 크기의 부분 잘라냄
		cvSetImageROI(roi, cvRect(upper_x, upper_y, 55/**/, 55/**/)); // cvRect(x,y,width,height)
		// 잘린 영역이 cropped 로 넘어감
		cvCopy(roi, cropped_yellow);

		thrown_black_pixel_count = 0;

		for (int j = 0; j < 55/**/; j++) {
			for (int k = 0; k < 55/**/; k++) {
				CvScalar s;
				s = cvGet2D(cropped_yellow, j, k);
				if (s.val[0] == 0 && s.val[1] == 0 && s.val[2] == 0)
					thrown_black_pixel_count++;
			}
		}

		// 레드 선공이냐?
		if (thrown_black_pixel_count > 700) {
			cvReleaseImage(&roi);
			cvReleaseImage(&cropped_yellow);
			thrown_index = 1;
			return thrown_index;
		}
	}

	// 레드인지 옐로우인지 아무 답도 없다면
	if (thrown_index == -1){
		cvReleaseImage(&roi);
		cvReleaseImage(&cropped_yellow);
		return -1;
	}

}