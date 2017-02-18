
// SafeDiskManagerView.h : CSafeDiskManagerView ��Ľӿ�
//


#pragma once

class CSafeDiskManagerDoc;
class CChildFrame;
class CSafeDiskManagerView : public CListView
{
protected: // �������л�����
	CSafeDiskManagerView();
	DECLARE_DYNCREATE(CSafeDiskManagerView)

// ����
public:
	CSafeDiskManagerDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// ʵ��
public:
	virtual ~CSafeDiskManagerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnFilePrintPreview();
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	CChildFrame* GetChildFrame();
	CImageList m_ImageList;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	CImageList* GetImageList();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // SafeDiskManagerView.cpp �еĵ��԰汾
inline CSafeDiskManagerDoc* CSafeDiskManagerView::GetDocument() const
   { return reinterpret_cast<CSafeDiskManagerDoc*>(m_pDocument); }
#endif

