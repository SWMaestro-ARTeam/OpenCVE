#include "Chess_recognition_GH.h"


Chess_recognition_GH::Chess_recognition_GH(){
	Linefindcount_x1 = 0, Linefindcount_y1 = 0, Linefindcount_x2 = 0, Linefindcount_y2 = 0;
}


Chess_recognition_GH::~Chess_recognition_GH(){
}

//<<<<<<< HEAD:CVES/ChessRecognition.cpp

void Chess_recognition_GH::GetLinegrayScale(IplImage *gray_image, int linefindcount_x1, int linefindcount_y1, int linefindcount_x2, int linefindcount_y2) {
	// �߰� ���� �˻�

	int image_y = gray_image->height, image_x = gray_image->width;
	int x1, x2, y1, y2;

	// x���� grayscale�� ������� y���� Ž����ġ�� ����

	y1 = ((image_y / 7) * 3) - linefindcount_x1;
	y2 = ((image_y / 7) * 4) + linefindcount_x2;

	// ó�� vector �迭�� �߽��� �Ǵ� ���� �־��ְ�
	// �� ���Ŀ� ¦���� ������, Ȧ���� ������ ��ġ�� �־��ش�

	line_x1.push_back(setMyGrayPoint(Getgrayscale(gray_image, image_x / 2, y1), image_x / 2, y1));
	line_x2.push_back(setMyGrayPoint(Getgrayscale(gray_image, image_x / 2, y2), image_x / 2, y2));

	for (register int x = 1; x <= image_x / 2; x++) {
		line_x1.push_back(setMyGrayPoint(Getgrayscale(gray_image, (image_x / 2) + x, y1), (image_x / 2) + x, y1));
		line_x1.push_back(setMyGrayPoint(Getgrayscale(gray_image, (image_x / 2) - x, y1), (image_x / 2) - x, y1));

		line_x2.push_back(setMyGrayPoint(Getgrayscale(gray_image, (image_x / 2) + x, y2) ,(image_x / 2) + x, y2));
		line_x2.push_back(setMyGrayPoint(Getgrayscale(gray_image, (image_x / 2) - x, y2) ,(image_x / 2) - x, y2));
	}

	// y���� grayscale�� ������� x���� Ž����ġ�� ����

	x1 = ((image_x / 5) * 2) - linefindcount_y1;
	x2 = ((image_x / 5) * 3) + linefindcount_y2;

	// ó�� vector �迭�� �߽��� �Ǵ� ���� �־��ְ�
	// �� ���Ŀ� ¦���� �Ʒ���, Ȧ���� ������ ��ġ�� �־��ش�

	line_y1.push_back(setMyGrayPoint(Getgrayscale(gray_image, x1, image_y / 2), x1, image_y / 2));
	line_y2.push_back(setMyGrayPoint(Getgrayscale(gray_image, x2, image_y / 2), x2, image_y / 2));

	for (register int y = 1; y <= image_y / 2; y++) {
		line_y1.push_back(setMyGrayPoint(Getgrayscale(gray_image, x1, (image_y / 2) + y), x1, (image_y / 2) + y));
		line_y1.push_back(setMyGrayPoint(Getgrayscale(gray_image, x1, (image_y / 2) - y), x1, (image_y / 2) - y));

		line_y2.push_back(setMyGrayPoint(Getgrayscale(gray_image, x2, (image_y / 2) + y), x2, (image_y / 2) + y));
		line_y2.push_back(setMyGrayPoint(Getgrayscale(gray_image, x2, (image_y / 2) - y), x2, (image_y / 2) - y));
	}
}

void Chess_recognition_GH::GetgraySidelinesPoint(IplImage *chess_image) {

	// �� grayscale�� ����Ǿ� �ִ� vector �迭���� �ش� ������ �������� ���Ѵ�

	GetgraySidelines(chess_image, &line_x1, &line_point_x1, &in_line_point_x1, true);
	GetgraySidelines(chess_image, &line_x2, &line_point_x2, &in_line_point_x2, true);
	GetgraySidelines(chess_image, &line_y1, &line_point_y1, &in_line_point_y1, false);
	GetgraySidelines(chess_image, &line_y2, &line_point_y2, &in_line_point_y2, false);
}

void Chess_recognition_GH::GetInCrossPoint(IplImage *chess_image, vector<Chess_point> *point) {
	point->clear();

	// in_line_point �������� ����
	// ã�� ��������� x�Ǵ� y�� �߽����� �������Ѵ�

	for (register int i = 0; i < in_line_point_x1.size(); i++) {
		for (register int j = i + 1; j < in_line_point_x1.size(); j++) {
			if (in_line_point_x1[i].x > in_line_point_x1[j].x) {
				MyPoint t_point = in_line_point_x1[i];

				in_line_point_x1[i] = in_line_point_x1[j];
				in_line_point_x1[j] = t_point;
			}
			if (in_line_point_x2[i].x > in_line_point_x2[j].x) {
				MyPoint t_point = in_line_point_x2[i];

				in_line_point_x2[i] = in_line_point_x2[j];
				in_line_point_x2[j] = t_point;
			}
			if (in_line_point_y1[i].y > in_line_point_y1[j].y) {
				MyPoint t_point = in_line_point_y1[i];

				in_line_point_y1[i] = in_line_point_y1[j];
				in_line_point_y1[j] = t_point;
			}
			if (in_line_point_y2[i].y > in_line_point_y2[j].y) {
				MyPoint t_point = in_line_point_y2[i];

				in_line_point_y2[i] = in_line_point_y2[j];
				in_line_point_y2[j] = t_point;
			}
		}
	}

	// ã�� ��� ��������� ������ �Ǵ� ������ ���������� ��� �������� ã�´�
	// 9 * 9 = 81
	// ���� ��� �������� point ������ �ִ´� 

	MyLinePoint t_in_line_point_x, t_in_line_point_y;
	MyPoint t_in_point;

	for (register int i = 0; i < in_line_point_x1.size(); i++) {
		for (register int j = 0; j < in_line_point_x1.size(); j++) {
			SetMyLinePoint(in_line_point_x1[i].x, in_line_point_x1[i].y, in_line_point_x2[i].x, in_line_point_x2[i].y, &t_in_line_point_x);
			SetMyLinePoint(in_line_point_y1[j].x, in_line_point_y1[j].y, in_line_point_y2[j].x, in_line_point_y2[j].y, &t_in_line_point_y);

			GetCrossPoint(t_in_line_point_x, t_in_line_point_y, &t_in_point);

			Chess_point temp;
			temp.Cordinate = cvPoint(t_in_point.x, t_in_point.y);
			temp.index = cvPoint(i, j);
			point->push_back(temp);
		}
	}
}

void Chess_recognition_GH::SetMyLinePoint(int x1, int y1, int x2, int y2, MyLinePoint *setLinePoint){
	setLinePoint->x1 = x1;
	setLinePoint->x2 = x2;
	setLinePoint->y1 = y1;
	setLinePoint->y2 = y2;
}

int Chess_recognition_GH::Getgrayscale(IplImage *gray_image, int x, int y){
	int index = x + y*gray_image->widthStep ;
	unsigned char value = gray_image->imageData[index];

	return (int)value;
}

Chess_recognition_GH::MyGrayPoint Chess_recognition_GH::setMyGrayPoint(int grayscale, int x, int y){
	MyGrayPoint t_graypoint;

	t_graypoint.grayscale = grayscale;
	t_graypoint.x = x;
	t_graypoint.y = y;

	return t_graypoint;
}

Chess_recognition_GH::MyPoint Chess_recognition_GH::setMyPoint(int x, int y) {
	MyPoint t_point;
	t_point.x = x;
	t_point.y = y;

	return t_point;
}

bool Chess_recognition_GH::GetCrossPoint(MyLinePoint line1, MyLinePoint line2, MyPoint *out) {
	float x12 = line1.x1 - line1.x2;
	float x34 = line2.x1 - line2.x2;
	float y12 = line1.y1 - line1.y2;
	float y34 = line2.y1 - line2.y2;

	float c = (x12 * y34) - (y12 * x34);

	if (fabs(c) < 0.01)	{
		// No intersection
		return false;
	}
	else {
		// Intersection
		float a = (line1.x1 * line1.y2) - (line1.y1 * line1.x2);
		float b = (line2.x1 * line2.y2) - (line2.y1 * line2.x2);

		float x = ((a * x34) - (b * x12)) / c;
		float y = ((a * y34) - (b * y12)) / c;

		out->x = (int)x;
		out->y = (int)y;

		return true;
	}
}

void Chess_recognition_GH::GrayImageBinarization(IplImage *gray_image) {

	// �̹����� grayscale�� ������ ����

	float hist[256]={0,};
	int temp[256];

	memset(temp, 0, sizeof(int) * 256);

	bool flag = true;

	// ������ grayscale�� �����Ѵ�

	for(register int i = 0; i < gray_image->width; i++) {
		for(register int j = 0; j < gray_image->height; j++) {
			temp[Getgrayscale(gray_image, i, j)]++;

			// ��ó�� ������ ���ϻ����� ���� ��찡 �ֱ� ������ ����ó���� ���ش�

			if (Getgrayscale(gray_image, i, j) != 0)
				flag = false;
		}
	}

	if (flag)
		return;

	float area = (float)gray_image->width * gray_image->height;

	// grayscale�� ��հ�

	for (register int i = 1; i < 256; i++)
		hist[i] = temp[i] / area;

	int T, Told;

	float sum = 0.f;
	for (register int i=1; i < 256; i++)
		sum += (i * hist[i]);

	T = (int)sum;

	do {
		Told = T;
		int a1, a2, b1, b2, u1, u2;

		a1 = b1 = 0;

		for (register int i = 0; i < Told; i++) {
			a1 += (i * temp[i]);
			b1 += temp[i];
		}

		u1 = a1 / b1;
		a2 = b2 = 0;

		for (register int i = Told + 1; i < 256; i++) {
			a2 += (i * temp[i]);
			b2 += (temp[i]);
		}
		u2 = a2 / b2;

		if (b1 == 0) b1 = 1.f;
		if (b2 == 0) b2 = 1.f;

		T = (int)((u1 + u2) / 2);
	} while(T != Told);

	for(register int i=0; i < gray_image->width; i++) {
		for(register int j=0; j < gray_image->height; j++) {
			int index = i + (j * gray_image->widthStep);

			// �ش� ��ġ�� grayscale�� T���� �������� ����ȭ�� �����Ѵ� 

			gray_image->imageData[index] = Getgrayscale(gray_image, i, j) > T ? 255 : 0;
		}
	}
}

void Chess_recognition_GH::GetgraySidelines(IplImage *image, vector<MyGrayPoint> *line, MyLinePoint *line_point, vector<MyPoint> *in_line_point, bool XYFlag){

	// ��踦 ã�� �� ��� ������ ��迡�� ����� ���� �ʴ´�

	int line_count = 0, jump_count_p = 0, jump_count_m = 0, jump_count = 0 ;

	if(XYFlag){
		jump_count = image->width / 12;
		line_point->x1 = image->width / 2;
	}
	else{
		jump_count = image->height / 12;
		line_point->y1 = image->height / 2;
	}

	// �����Ǵ� ü������ ��踦 ���� �� �� ü�� ���� �ǰ� ��� �� ���
	// ��輱���� �ν� �Ǵ� ��츦 ���´�

	bool change_flag_line_t, change_flag_line_t1, change_flag_line_t2;

	// line vector�Լ� ���� ��ȯ

	vector<MyGrayPoint> _TT = *((vector<MyGrayPoint> *)line);

	_TT[0].grayscale;
	_TT[1].grayscale;

	change_flag_line_t1 = 255 >= _TT[0].grayscale ? true : false;
	change_flag_line_t2 = 255 >= _TT[1].grayscale ? true : false;

	for (register int i = 2; i < line->size() - 10; i++) {
		if ((i % 2 == 1) && (jump_count_p > 0)) {
			jump_count_p--;
		}
		else if ((i % 2 == 0) && (jump_count_m > 0)) {
			jump_count_m--;
		}
		else {
			bool change_flag_t;

			change_flag_t = 255 >= _TT[i].grayscale ? true : false;

			if (i % 2 == 0)
				change_flag_line_t = change_flag_line_t1;
			else
				change_flag_line_t = change_flag_line_t2;

			if (_TT[i].grayscale != _TT[i + 2].grayscale) {
				int flag = true;

				// �ش� ��ġ���� �״��� �ȼ��� ��� �ȴٸ� ��輱���� �ν�

				// �� �κп��� �밢�� ������ ó���� �ش� XYFlag�� ture �� x��, false�̸� y��
				// �ش� �������� �����ִ� �� �밢�� ������ ���� ���Ͽ� ���̰����� ��輱���� �ν��Ѵ�

				if(XYFlag){
					if(i%2==1 && (Getgrayscale(image, _TT[i].x+2, _TT[i].y-2) != Getgrayscale(image, _TT[i].x+2, _TT[i].y+2)))
						return;
					else if(i%2==0 && (Getgrayscale(image, _TT[i].x-2, _TT[i].y-2) != Getgrayscale(image, _TT[i].x-2, _TT[i].y+2)))
						return;
				}
				else{
					if(i%2==1 && (Getgrayscale(image, _TT[i].x+2, _TT[i].y+2) != Getgrayscale(image, _TT[i].x-2, _TT[i].y+2))) 
						return;
					else if(i%2==0 && (Getgrayscale(image, _TT[i].x-2, _TT[i].y-2) != Getgrayscale(image, _TT[i].x+2, _TT[i].y-2)))
						return;
				}

				// Ȯ���� �ϱ����� �ּ� 3�ȼ� ���� ��� �Ǹ� ��輱����  �ν��Ѵ�

				for (register int j = 1; j <= 3; j++) {
					if(i + (j * 2) > _TT.size())
						continue;

					if (_TT[i].grayscale == _TT[i + (j * 2)].grayscale && change_flag_t == change_flag_line_t) {
						flag = false;
						break;
					}
				}
				if (flag) {
					if (line_count < 9){
						if (line_point->x1 > _TT[i].x) {
							line_point->x1 = _TT[i].x;
							line_point->y1 = _TT[i].y;
						}

						if (line_point->x2 < _TT[i].x) {
							line_point->x2 = _TT[i].x;
							line_point->y2 = _TT[i].y;
						}

						in_line_point->push_back(setMyPoint(_TT[i].x, _TT[i].y));

						cvCircle(image, cvPoint(_TT[i].x, _TT[i].y), 5, cvScalar(0, 0, 0));

						line_count++;

						if (i % 2 == 0)
							change_flag_line_t1 = !change_flag_t;
						else
							change_flag_line_t2 = !change_flag_t;
					}
					if (i % 2 == 1)
						jump_count_p = jump_count;
					else
						jump_count_m = jump_count;	
				}
			}
		}

		// ��輱�� 9�� �� ã���� �� �̻� ã�� �ʿ䰡 �����Ƿ� ĵ���Ѵ�

		if(line_count == 9){
			break;
		}
	}
}

void Chess_recognition_GH::MemoryClear() {
	line_x1.clear(), line_x2.clear(), line_x_mid.clear(), line_y1.clear(), line_y2.clear(), line_y_mid.clear();

	in_line_point_x1.clear(), in_line_point_x2.clear(), in_line_point_y1.clear(), in_line_point_y2.clear();
}