
// ChildView.h: CChildView 클래스의 인터페이스
//


#pragma once
#include "CCardData.h"

enum RATING
{
	NOMAL,
	RARE,
	SUPER_RARE,
	ULTRA_RARE,
	END,
};

// CChildView 창

class CChildView : public CWnd
{
// 생성입니다.
public:
	CChildView();

// 특성입니다.
public:
	CRect windowRect; //전체 윈도우 크기
	CRect btnRect; //버튼 구역
	CRect cardRect; //카드 위치 구역

	int cntInven; //전체 갯수
	int cardMax;
	CArray<CCardData, CCardData&> cardDatas;
	bool isDrawing;
	CUIntArray drawsCard;

// 작업입니다.
public:
	void PickUp(int cnt);
	void DrawButton(CPaintDC * dc);
	void DrawInfoPercentage(CPaintDC * dc);
	void DrawResult(CPaintDC * dc);
	void DrawCardInfo(CPaintDC * dc);
	void InitData(); //결과 값 초기화
	void OnColorYellow();


// 재정의입니다.
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 구현입니다.
public:
	virtual ~CChildView();

	// 생성된 메시지 맵 함수
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnInitInven();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};

