
// SafeDiskManagerDoc.cpp : CSafeDiskManagerDoc ���ʵ��
//

#include "stdafx.h"
#include "SafeDiskManager.h"

#include "SafeDiskManagerDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSafeDiskManagerDoc

IMPLEMENT_DYNCREATE(CSafeDiskManagerDoc, CDocument)

BEGIN_MESSAGE_MAP(CSafeDiskManagerDoc, CDocument)
END_MESSAGE_MAP()


// CSafeDiskManagerDoc ����/����

CSafeDiskManagerDoc::CSafeDiskManagerDoc()
{
	// TODO: �ڴ����һ���Թ������

}

CSafeDiskManagerDoc::~CSafeDiskManagerDoc()
{
}

BOOL CSafeDiskManagerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}




// CSafeDiskManagerDoc ���л�

void CSafeDiskManagerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}


// CSafeDiskManagerDoc ���

#ifdef _DEBUG
void CSafeDiskManagerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSafeDiskManagerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CSafeDiskManagerDoc ����

void CSafeDiskManagerDoc::SetTitle(LPCTSTR lpszTitle)
{
//	CDocument::SetTitle(lpszTitle);
}
