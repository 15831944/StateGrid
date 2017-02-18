
// SafeDiskManagerView.cpp : CSafeDiskManagerView ���ʵ��
//

#include "stdafx.h"
#include "SafeDiskManager.h"
#include "SafeDiskManagerDoc.h"
#include "SafeDiskManagerView.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSafeDiskManagerView

IMPLEMENT_DYNCREATE(CSafeDiskManagerView, CListView)

BEGIN_MESSAGE_MAP(CSafeDiskManagerView, CListView)
	ON_WM_KEYDOWN()
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, &CSafeDiskManagerView::OnLvnItemchanged)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CREATE()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

// CSafeDiskManagerView ����/����

CSafeDiskManagerView::CSafeDiskManagerView()
{
	// TODO: �ڴ˴���ӹ������

}

CSafeDiskManagerView::~CSafeDiskManagerView()
{
}


int CSafeDiskManagerView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
//	lpCreateStruct->cy = 0;
//	lpCreateStruct->cx = 0;

	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;

	//
	lpCreateStruct->style = LVS_REPORT;

	//
	m_ImageList.Create(32, 32, ILC_COLOR32|ILC_MASK, 0, 0);
	GetListCtrl().SetImageList(&m_ImageList, LVSIL_NORMAL);

	return 0;
}

BOOL CSafeDiskManagerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CListView::PreCreateWindow(cs);
}

// CSafeDiskManagerView ����

void CSafeDiskManagerView::OnDraw(CDC* /*pDC*/)
{
	CSafeDiskManagerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: �ڴ˴�Ϊ����������ӻ��ƴ���
}

CImageList* CSafeDiskManagerView::GetImageList()
{
	return &m_ImageList;
}

// CSafeDiskManagerView ���

#ifdef _DEBUG
void CSafeDiskManagerView::AssertValid() const
{
	CListView::AssertValid();
}

void CSafeDiskManagerView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CSafeDiskManagerDoc* CSafeDiskManagerView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSafeDiskManagerDoc)));
	return (CSafeDiskManagerDoc*)m_pDocument;
}
#endif //_DEBUG


// CSafeDiskManagerView ��Ϣ�������

void CSafeDiskManagerView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();
}

CChildFrame* CSafeDiskManagerView::GetChildFrame()
{
	CChildFrame* pChild = (CChildFrame*)(CWnd::GetParentFrame());
	ASSERT(pChild != NULL);
	return pChild;
}

void CSafeDiskManagerView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	//
	CListView::OnKeyDown(nChar, nRepCnt, nFlags);

	//
	INT iClickIndex;
	if(nChar == VK_RETURN)
	{
		// Click Index 
		iClickIndex = GetListCtrl().GetNextItem(-1, LVIS_SELECTED);
		GetChildFrame()->OnFileViewDblClick(iClickIndex);
	}
	else if(nChar == VK_BACK)
	{
		/*
		if(m_iRemoteSelLevel == 0)
		{
			return;
		}
		else
		{
			OnDblClickFileView(0);
		}
		*/
	}
	else if(
		nChar == VK_LEFT ||
		nChar == VK_RIGHT || 
		nChar == VK_UP || 
		nChar == VK_DOWN || 
		nChar == VK_HOME || 
		nChar == VK_END )
	{
		// Click Index 
		iClickIndex = GetListCtrl().GetNextItem(-1, LVIS_SELECTED);
		GetChildFrame()->OnFileViewSelChanged(iClickIndex);
	}
}

void CSafeDiskManagerView::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	//
	if((pNMLV->uOldState & LVIS_SELECTED) == 0 && (pNMLV->uNewState & LVIS_SELECTED) == LVIS_SELECTED) 
	{
		GetChildFrame()->OnFileViewSelChanged(pNMLV->iItem);
	}
	*pResult = 0;
}

void CSafeDiskManagerView::OnLButtonDown(UINT nFlags, CPoint point)
{
	//
	CListView::OnLButtonDown(nFlags, point);

	// Click Index 
	INT iClickIndex = GetListCtrl().GetNextItem(-1, LVIS_SELECTED);
	if (-1 == iClickIndex)
	{
		GetChildFrame()->OnFileViewSelChanged(iClickIndex);
	}
}

void CSafeDiskManagerView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	//
	CListView::OnLButtonDblClk(nFlags, point);

	// Click Index 
	INT iClickIndex = GetListCtrl().GetNextItem(-1, LVIS_SELECTED);
	GetChildFrame()->OnFileViewDblClick(iClickIndex);
}

void CSafeDiskManagerView::OnRButtonDown(UINT nFlags, CPoint point)
{
	CListView::OnRButtonDown(nFlags, point);
}
