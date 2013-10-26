#include "Chess_recognition_GH.h"


Chess_recognition_GH::Chess_recognition_GH(){
	Linefindcount_x1 = 0, Linefindcount_y1 = 0, Linefindcount_x2 = 0, Linefindcount_y2 = 0;
}


Chess_recognition_GH::~Chess_recognition_GH(){
}

//<<<<<<< HEAD:CVES/ChessRecognition.cpp
void Chess_recognition_GH::Chess_recognition_process(IplImage *src, vector<Chess_point> *point) {

	//=======
	//void Chess_recognition::Chess_recognition_process(IplImage *src, vector<Chess_point> *point) {

	// 영상 이진화
	//>>>>>>> CVES_HandRecognition:CVES/Chess_recognition.cpp
	GrayImageBinarization(src);

	while(1){
		cvShowImage("ImageBin",src);
		if(cvWaitKey(33))
			break;
	}
	// 해당 영상에서의 좌표 x,y 와 grayscale을 추출하여 vector<MyGrayPoint> 형의 line에 저장
	GetLinegrayScale(src, Linefindcount_x1, Linefindcount_y1, Linefindcount_x2, Linefindcount_y2);

	// 체스판의 경계를 구하여 in_line_point 변수들에 저장
	GetgraySidelinesPoint(src);

	while(1){
		cvShowImage("findCrossPoint",src);
		if(cvWaitKey(33))
			break;
	}

	// 해당 라인에서 9곳의 체스판 경계를 찾지 못 하였으면
	// 탐색라인을 이동시켜 적절한 탐색라인을 찾는다
	// flag의 값에 따라 Linefindcount의 값을 변경한다
	// true  : +
	// false : -

	// 만약 9곳의 경계를 모두 찾게 되면 해당 라인으로 고정시킨다

	if (Linefindcount_x1 >= (src->width / 5) * 2 - 10)
		flag_x1 = false;

	if (Linefindcount_y1 >= (src->height / 5) * 2 - 10)
		flag_y1 = false;

	if (Linefindcount_x2 >= (src->width / 5) * 2 - 10)
		flag_x2 = false;

	if (Linefindcount_y2 >= (src->height / 5) * 2 - 10)
		flag_y2 = false;

	if (Linefindcount_x1 <= 1)
		flag_x1 = true;

	if (Linefindcount_y1 <= 1)
		flag_y1 = true;	

	if (Linefindcount_x2 <= 1)
		flag_x2 = true;

	if (Linefindcount_y2 <= 1)
		flag_y2 = true;

	// 각 라인이 모든 체스판의 경계를 찾았다면 다음 과정으로 넘어가고
	// 찾지 못하였으면 x축 또는 y축을 이동시켜 경계를 찾을 수 있는 라인을 찾는다

	if (in_line_point_x1.size() == 9 && in_line_point_x2.size() ==  9 && in_line_point_y1.size() == 9 && in_line_point_y2.size() == 9) {

		// 각 찾은 경계점들의 수직이 되는 점 모두의 교차점을 찾는다
		GetInCrossPoint(src, point);
	}
	else if (in_line_point_x1.size() != 9 || in_line_point_x2.size() != 9) {
		if (flag_x1 && (in_line_point_x1.size() != 9))
			Linefindcount_x1 += 3;
		else if (!flag_x1 && (in_line_point_x1.size() != 9))
			Linefindcount_x1 -= 3;
		if (flag_x2 && (in_line_point_x2.size() != 9))
			Linefindcount_x2 += 3;
		else if (!flag_x2 && (in_line_point_x2.size() != 9))
			Linefindcount_x2 -= 3;
	}
	else if(in_line_point_y1.size() != 9 || in_line_point_y2.size() != 9) {
		if (flag_y1 && (in_line_point_y1.size() != 9))
			Linefindcount_y1 += 3;
		else if (!flag_y1 && (in_line_point_y1.size() != 9))
			Linefindcount_y1 -= 3;
		if (flag_y2 && (in_line_point_y2.size() != 9))
			Linefindcount_y2 += 3;
		else if (!flag_y2 && (in_line_point_y2.size() != 9))
			Linefindcount_y2 -= 3;
	}

	// 메모리 초기화

	MemoryClear();
}


void Chess_recognition_GH::GetLinegrayScale(IplImage *gray_image, int linefindcount_x1, int linefindcount_y1, int linefindcount_x2, int linefindcount_y2) {
	// 중간 부터 검사

	int image_y = gray_image->height, image_x = gray_image->width;
	int x1, x2, y1, y2;

	// x축의 grayscale을 얻기위해 y축의 탐색위치를 결정

	y1 = ((image_y / 7) * 3) - linefindcount_x1;
	y2 = ((image_y / 7) * 4) + linefindcount_x2;

	// 처음 vector 배열에 중심이 되는 값을 넣어주고
	// 그 이후에 짝수는 오른쪽, 홀수는 왼쪽의 수치를 넣어준다

	line_x1.push_back(setMyGrayPoint(Getgrayscale(gray_image, image_x / 2, y1), image_x / 2, y1));
	line_x2.push_back(setMyGrayPoint(Getgrayscale(gray_image, image_x / 2, y2), image_x / 2, y2));

	for (register int x = 1; x <= image_x / 2; x++) {
		line_x1.push_back(setMyGrayPoint(Getgrayscale(gray_image, (image_x / 2) + x, y1), (image_x / 2) + x, y1));
		line_x1.push_back(setMyGrayPoint(Getgrayscale(gray_image, (image_x / 2) - x, y1), (image_x / 2) - x, y1));

		line_x2.push_back(setMyGrayPoint(Getgrayscale(gray_image, (image_x / 2) + x, y2) ,(image_x / 2) + x, y2));
		line_x2.push_back(setMyGrayPoint(Getgrayscale(gray_image, (image_x / 2) - x, y2) ,(image_x / 2) - x, y2));
	}

	// y축의 grayscale을 얻기위해 x축의 탐색위치를 결정

	x1 = ((image_x / 5) * 2) - linefindcount_y1;
	x2 = ((image_x / 5) * 3) + linefindcount_y2;

	// 처음 vector 배열에 중심이 되는 값을 넣어주고
	// 그 이후에 짝수는 아래쪽, 홀수는 위쪽의 수치를 넣어준다

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

	// 각 grayscale이 저장되어 있는 vector 배열에서 해당 라인의 교차점을 구한다

	GetgraySidelines(chess_image, &line_x1, &line_point_x1, &in_line_point_x1, true);
	GetgraySidelines(chess_image, &line_x2, &line_point_x2, &in_line_point_x2, true);
	GetgraySidelines(chess_image, &line_y1, &line_point_y1, &in_line_point_y1, false);
	GetgraySidelines(chess_image, &line_y2, &line_point_y2, &in_line_point_y2, false);
}

void Chess_recognition_GH::GetInCrossPoint(IplImage *chess_image, vector<Chess_point> *point) {
	point->clear();

	// in_line_point 오름차순 정렬
	// 찾은 경계점들을 x또는 y를 중심으로 재정렬한다

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

	// 찾은 모든 경계점들을 수직이 되는 라인의 경계점들과의 모든 교차점을 찾는다
	// 9 * 9 = 81
	// 그후 모든 교차점을 point 변수에 넣는다 

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

	// 이미지의 grayscale을 저장할 변수

	float hist[256]={0,};
	int temp[256];

	memset(temp, 0, sizeof(int) * 256);

	bool flag = true;

	// 영상의 grayscale을 저장한다

	for(register int i = 0; i < gray_image->width; i++) {
		for(register int j = 0; j < gray_image->height; j++) {
			temp[Getgrayscale(gray_image, i, j)]++;

			// 맨처음 영상이 단일색으로 나올 경우가 있기 때문에 예외처리를 해준다

			if (Getgrayscale(gray_image, i, j) != 0)
				flag = false;
		}
	}

	if (flag)
		return;

	float area = (float)gray_image->width * gray_image->height;

	// grayscale의 평균값

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

			// 해당 위치의 grayscale을 T값을 기준으로 이진화를 결정한다 

			gray_image->imageData[index] = Getgrayscale(gray_image, i, j) > T ? 255 : 0;
		}
	}
}

void Chess_recognition_GH::GetgraySidelines(IplImage *image, vector<MyGrayPoint> *line, MyLinePoint *line_point, vector<MyPoint> *in_line_point, bool XYFlag){

	// 경계를 찾은 후 어느 정도의 경계에는 계산을 하지 않는다

	int line_count = 0, jump_count_p = 0, jump_count_m = 0, jump_count = 0 ;

	if(XYFlag){
		jump_count = image->width / 12;
		line_point->x1 = image->width / 2;
	}
	else{
		jump_count = image->height / 12;
		line_point->y1 = image->height / 2;
	}

	// 교차되는 체스판의 경계를 검출 할 때 체스 말이 판과 대비가 될 경우
	// 경계선으로 인식 되는 경우를 막는다

	bool change_flag_line_t, change_flag_line_t1, change_flag_line_t2;

	// line vector함수 차수 변환

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

				// 해당 위치에서 그다음 픽셀이 대비가 된다면 경계선으로 인식

				// 이 부분에서 대각선 방향을 처리해 준다 XYFlag가 ture 면 x축, false이면 y축
				// 해당 방향으로 뻗어있는 두 대각선 방향의 색을 비교하여 차이가나면 경계선으로 인식한다

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

				// 확실히 하기위해 최소 3픽셀 까지 대비가 되면 경계선으로  인식한다

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

		// 경계선을 9개 다 찾으면 더 이상 찾을 필요가 없으므로 캔슬한다

		if(line_count == 9){
			break;
		}
	}
}

void Chess_recognition_GH::MemoryClear() {
	line_x1.clear(), line_x2.clear(), line_x_mid.clear(), line_y1.clear(), line_y2.clear(), line_y_mid.clear();

	in_line_point_x1.clear(), in_line_point_x2.clear(), in_line_point_y1.clear(), in_line_point_y2.clear();
}