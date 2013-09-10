////////////////////////////////////////////////////
//
//  Ŭ���� : CBlobLabeling
//
//                    by ��ƾ(http://martinblog.net)
#pragma once

#include <cv.h>
#include <highgui.h>

typedef struct 
{
	bool	bVisitedFlag;
	CvPoint ptReturnPoint;
} Visited;

class  CBlobLabeling
{
public:
	CBlobLabeling(void);
public:
	~CBlobLabeling(void);

public:
	IplImage*	m_Image;				// ���̺��� ���� �̹���
	int			m_nThreshold;			// ���̺� ������Ȧ�� ��
	Visited*	m_vPoint;				// ���̺��� �湮����
	int			m_nBlobs;				// ���̺��� ����
	CvRect*		m_recBlobs;				// �� ���̺� ����


public:
	// ���̺� �̹��� ����
	void		SetParam(IplImage* image, int nThreshold);

	// ���̺�(����)
	void		DoLabeling();

	// ���̺� �׸���
	void DrawLabel(IplImage *img, CvScalar RGB);
	//���� ���� �̹��� �ܿ� ������
	void GetSideBlob(IplImage *img, std::vector<int> *piece_idx);

private:
	// ���̺�(����)
	int		 Labeling(IplImage* image, int nThreshold);

	// ����Ʈ �ʱ�ȭ
	void	 InitvPoint(int nWidth, int nHeight);
	void	 DeletevPoint();

	// ���̺� ��� ���
	void	 DetectLabelingRegion(int nLabelNumber, unsigned char *DataBuf, int nWidth, int nHeight);

	// ���̺�(���� �˰���)
	int		_Labeling(unsigned char *DataBuf, int nWidth, int nHeight, int nThreshold);
	
	// _Labling ���� ��� �Լ�
	int		__NRFIndNeighbor(unsigned char *DataBuf, int nWidth, int nHeight, int nPosX, int nPosY, int *StartX, int *StartY, int *EndX, int *EndY );
	int		__Area(unsigned char *DataBuf, int StartX, int StartY, int EndX, int EndY, int nWidth, int nLevel);
};