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

		// curlit �ؽ�Ʈ ���� �ε�
		ifstream curlit;
		sprintf(filePath_load_text, "SourceText/curlit (%d).txt", match);
		curlit.open(filePath_load_text);
		if (curlit.fail()){
			return 1;
		}

		for (end = 1; end <= 12; end++) {

			// �ҷ��� ��ġ/���� �� �ǳʶ�
			if ((match == 163 && end == 2) || (match == 174 && end == 6) || (match == 175 && end == 4)){
				continue;
			}

			// �̹� ���� �����ϴ� �� ���� ã��
			for (shot = 1; shot <= 17; shot++) {
				sprintf(filePath_check, "SourceImage/%04d %04d %04d.png", match, end, shot);
				IplImage * check = cvLoadImage(filePath_check, CV_LOAD_IMAGE_COLOR);
				if (check == 0) {
					last_shot = shot - 1;  // 15��° ������ �־ 16��° ���̹����� ���ٸ� last_shot ���� 15�� �����
					cvReleaseImage(&check);
					break;
				}
				else{
					cvReleaseImage(&check);
				}
			}

			// ���İ� ���� �ʱ�ȭ
			team_flag = -1;
			// OP, MY ��ǥ ã�� ���� �����ϱ�
			for (shot = 1; shot <= last_shot - 1; shot++){

				// �̹��� �ε�
				sprintf(filePath_load_image, "SourceImage/%04d %04d %04d.png", match, end, shot);
				IplImage * source = cvLoadImage(filePath_load_image, CV_LOAD_IMAGE_COLOR);
				if (source == 0) {
					break;
				}

				// ���� ���� �ʱ�ȭ
				numberOfstone = 0;

				// ��ǥ �迭 �ʱ�ȭ
				for (int i = 0; i < 8; i++) {
					red_fullSize[i].x = red_fullSize[i].y = yellow_fullSize[i].x = yellow_fullSize[i].y = 0;
					red_houseSize[i].x = red_houseSize[i].y = yellow_houseSize[i].x = yellow_houseSize[i].y = 0;
					red_houseSize_zero[i].x = red_houseSize_zero[i].y = yellow_houseSize_zero[i].x = yellow_houseSize_zero[i].y = 0;
				}
				tee_point.x = 0, tee_point.y = 0, tee_point_real.x = 0; tee_point_real.y = 0;

				coordinates::coordinate2_2(source); // ����� ����
				coordinates::coordinate2(source);   // ��ü ����

				for (int i = 0; i < 8; i++){

					// �Ͽ콺 ����
					red_houseSize[i].x = coordinates::red_house[i].x;
					red_houseSize[i].y = coordinates::red_house[i].y;
					yellow_houseSize[i].x = coordinates::yellow_house[i].x;
					yellow_houseSize[i].y = coordinates::yellow_house[i].y;

					// ��ü ����
					red_fullSize[i].x = coordinates::red_total[i].x;
					red_fullSize[i].y = coordinates::red_total[i].y;
					yellow_fullSize[i].x = coordinates::yellow_total[i].x;
					yellow_fullSize[i].y = coordinates::yellow_total[i].y;
				}

				// ����忡 �����ϴ� ���� ���� ����
				for (int i = 0; i < 8; i++){
					if (red_houseSize[i].x != 0 || red_houseSize[i].y != 0){
						numberOfstone++;
					}
					if (yellow_houseSize[i].x != 0 || yellow_houseSize[i].y != 0){
						numberOfstone++;
					}
				}

				// ������ ������ �������� Ȯ��
				if (shot == 1){ // ù���� ���϶�
					if (match <= 853){
						// ���� ������ �����ϴ���
						if (red_fullSize[0].x != 0 && yellow_fullSize[0].x == 0){
							team_flag = 0; // ���� ����
							//result_first_thrown << match << " ��ġ " << end << " ���� : R ����" << endl;
						}
						else if (red_fullSize[0].x == 0 && yellow_fullSize[0].x != 0) {// ��� ������ �����ϴ���
							team_flag = 1; // ���ο� ����
							//result_first_thrown << match << " ��ġ " << end << " ���� : Y ����" << endl;
						}
						else {
							//result_first_thrown << match << " ��ġ " << end << " ���� : ù��° �� ���� ��ã��" << endl;
						}
					}
					else if (match > 853) {
						if (FindFirstMove_Red(source) == 0){
							team_flag = 0;
							//result_first_thrown << match << " ��ġ " << end << " ���� : R ����" << endl;
						}
						else if (FindFirstMove_Yellow(source) == 1){
							team_flag = 1;
							//result_first_thrown << match << " ��ġ " << end << " ���� : Y ����" << endl;
						}
						else{
							team_flag = -1;
							//result_first_thrown << match << " ��ġ " << end << " ���� : ù��° �� ���� ��ã��" << endl;
						}
					}
				}

				// ����� �����ϴ� ���� ���� 3�� �̻��̸�
				if (numberOfstone > 2) {

					// Ƽ ��ǥ ���� 
					checkCenterLine(source);
					/*if (tee_point.x == 0 || tee_point.y == 0){
						result_tee_point << match << " " << end << " " << shot << " Ƽ����Ʈ ������" << endl;
					}*/

					// �ؽ�Ʈ ó��
					sprintf(match_end_shot, "%04d %04d %04d", match, end, shot + 1);
					string label(match_end_shot);

					found = 1;
					while (found){
						curlit.getline(inputString, 100);
						string check(inputString);

						if (check == label){ // match end shot ã����

							found = 0; // while�� �����������
							curlit.getline(strategy_label, 100); // ������ ������ draw	in	4
							parse(strategy_label);
							strcpy_s(strategy, receive[0]);
							/*string notcheck(strategy);
							if (notcheck == test_1 || notcheck == test_2){
							continue;
							}*/
							// char strategy_label[1024] �� �־�� ��


						}
					}
					string notcheck(strategy);
					if (notcheck == test_1 || notcheck == test_2){
						continue;
					}

					total_count++;
					

					tee_point_real.x = (tee_point.x*4.75 / 963);    // -> 2.382    / ������ 2.375
					tee_point_real.y = (tee_point.y*8.23 / 1741.5); // -> 2.429    / ������ 4.880
					x_variance = 2.375 - tee_point_real.x;
					y_variance = 4.88 - tee_point_real.y;

					for (int i = 0; i < 8; i++){
						if (red_houseSize[i].x != 0){
							red_real[i].x = (red_houseSize[i].x * 4.75 / 963) + x_variance;       // �ݿø�  
							red_real[i].y = (red_houseSize[i].y * 8.23 / 1741.5) + y_variance;    // 
						}
						if (yellow_houseSize[i].x != 0){
							yellow_real[i].x = (yellow_houseSize[i].x * 4.75 / 963) + x_variance;    //
							yellow_real[i].y = (yellow_houseSize[i].y * 8.23 / 1741.5) + y_variance; //
						}
					}




					result_text << setfill('0') << setw(6) << total_count << endl;


					if (team_flag == 0){ // ���� �����̸�

						if (shot % 2 == 1){ // 1 3 5 7 9 11 13 15

							result_text << "OP ";
							for (int i = 0; i < 8; i++){ // ���尡 OP
								if (red_houseSize[i].x != 0){
									result_text << fixed << setprecision(3) << red_real[i].x << " " << fixed << setprecision(3) << red_real[i].y << " ";
								}
							}
							result_text << endl;

							result_text << "MY ";
							for (int i = 0; i < 8; i++){ // ���ο찡 MY
								if (yellow_houseSize[i].x != 0){
									result_text << fixed << setprecision(3) << yellow_real[i].x << " " << fixed << setprecision(3) << yellow_real[i].y << " ";
								}
							}
							result_text << endl;

						}
						else if (shot % 2 == 0){ // 2 4 6 8 10 12 14 16

							result_text << "OP ";
							for (int i = 0; i < 8; i++){ // ���ο찡 OP
								if (yellow_houseSize[i].x != 0){
									result_text << fixed << setprecision(3) << yellow_real[i].x << " " << fixed << setprecision(3) << yellow_real[i].y << " ";
								}
							}
							result_text << endl;

							result_text << "MY ";
							for (int i = 0; i < 8; i++){ // ���尡 MY
								if (red_houseSize[i].x != 0){
									result_text << fixed << setprecision(3) << red_real[i].x << " " << fixed << setprecision(3) << red_real[i].y << " ";
								}
							}
							result_text << endl;

						}
					}

					else if (team_flag == 1){ // ���ο� �����̸�

						if (shot % 2 == 1){ // 1 3 5 7 9 11 13 15

							result_text << "OP ";
							for (int i = 0; i < 8; i++){ // ���ο찡 OP
								if (yellow_houseSize[i].x != 0){
									result_text << fixed << setprecision(3) << yellow_real[i].x << " " << fixed << setprecision(3) << yellow_real[i].y << " ";
								}
							}
							result_text << endl;

							result_text << "MY ";
							for (int i = 0; i < 8; i++){ // ���尡 MY
								if (red_houseSize[i].x != 0){
									result_text << fixed << setprecision(3) << red_real[i].x << " " << fixed << setprecision(3) << red_real[i].y << " ";
								}
							}
							result_text << endl;

						}

						else if (shot % 2 == 0){ // 2 4 6 8 10 12 14 16

							result_text << "OP ";
							for (int i = 0; i < 8; i++){ // ���尡 OP
								if (red_houseSize[i].x != 0){
									result_text << fixed << setprecision(3) << red_real[i].x << " " << fixed << setprecision(3) << red_real[i].y << " ";
								}
							}
							result_text << endl;

							result_text << "MY ";
							for (int i = 0; i < 8; i++){ // ���ο찡 MY
								if (yellow_houseSize[i].x != 0){
									result_text << fixed << setprecision(3) << yellow_real[i].x << " " << fixed << setprecision(3) << yellow_real[i].y << " ";
								}
							}
							result_text << endl;

						}
					}

					else {
						result_text << "������ Ȯ���ʿ�" << endl;
					}

					//result_text << "TEE " << fixed << setprecision(3) << tee_point_real.x << " " << fixed << setprecision(3) << tee_point_real.y << endl;
					result_text << strategy;
					result_text << endl;
					result_text << endl;
					//total_count++;

					cout << match << " ��ġ " << end << " ���� " << shot << " �� DB ���� �Ϸ� / ���� ������ �� : " << total_count << " ��" << endl;

					cvReleaseImage(&source);
				}
				else{
					cvReleaseImage(&source);
				}
			}
		}
	}
	//result_total_count << "�� ����� �� ������ ������ : " << total_count;

	//result_tee_point.close();
	//result_total_count.close();
	result_text.close();
	//result_first_thrown.close();
	return 0;
}

void checkCenterLine(IplImage * src)
{
	// Ƽ��ǥ �ʱ�ȭ

	tee_point.x = tee_point.y = 0;
	int black_pixel_count;

	// y�� �����˻� -> x�� �߽� ��ǥ ã�� - �����ȼ� 1000�� �̻��̸�
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

	// x�� �����˻� - �����ȼ� 450�� �̻��̸�
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
	IplImage* roi = (IplImage*)cvClone(src); // �ҽ� �̹��� roi�� ī��
	IplImage* cropped_red = cvCreateImage(cvSize(55, 55)/**/, 8, 3); // ���ͻ��� 55 X 55 

	thrown_index = -1;
	for (int red_index = 0; red_index < 8; red_index++) {

		if (red_fullSize[red_index].x == 0 && red_fullSize[red_index].y == 0) continue;

		// ������ �»�� ��ǥ ����
		upper_x = red_fullSize[red_index].x - 27/**/;
		upper_y = red_fullSize[red_index].y - 27/**/;

		// roi ��ü �������� 55 X 55 ũ���� �κ� �߶�
		cvSetImageROI(roi, cvRect(upper_x, upper_y, 55/**/, 55/**/)); // cvRect(x,y,width,height)
		// �߸� ������ cropped �� �Ѿ
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

		// ���� �����̳�?
		if (thrown_black_pixel_count > 700) {
			cvReleaseImage(&roi);
			cvReleaseImage(&cropped_red);
			thrown_index = 0;
			return thrown_index;
		}
	}

	// �������� ���ο����� �ƹ� �䵵 ���ٸ�
	if (thrown_index == -1){
		cvReleaseImage(&roi);
		cvReleaseImage(&cropped_red);
		return -1;
	}
}

int FindFirstMove_Yellow(IplImage * src)
{
	int  upper_x, upper_y, thrown_black_pixel_count, thrown_index;
	IplImage* roi = (IplImage*)cvClone(src); // �ҽ� �̹��� roi�� ī��
	IplImage* cropped_yellow = cvCreateImage(cvSize(55, 55)/**/, 8, 3); // ���ͻ��� 55 X 55 

	thrown_index = -1;
	for (int yellow_index = 0; yellow_index < 8; yellow_index++) {

		if (yellow_fullSize[yellow_index].x == 0 && yellow_fullSize[yellow_index].y == 0) continue;

		// ������ �»�� ��ǥ ����
		upper_x = yellow_fullSize[yellow_index].x - 27/**/;
		upper_y = yellow_fullSize[yellow_index].y - 27/**/;

		// roi ��ü �������� 55 X 55 ũ���� �κ� �߶�
		cvSetImageROI(roi, cvRect(upper_x, upper_y, 55/**/, 55/**/)); // cvRect(x,y,width,height)
		// �߸� ������ cropped �� �Ѿ
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

		// ���� �����̳�?
		if (thrown_black_pixel_count > 700) {
			cvReleaseImage(&roi);
			cvReleaseImage(&cropped_yellow);
			thrown_index = 1;
			return thrown_index;
		}
	}

	// �������� ���ο����� �ƹ� �䵵 ���ٸ�
	if (thrown_index == -1){
		cvReleaseImage(&roi);
		cvReleaseImage(&cropped_yellow);
		return -1;
	}

}