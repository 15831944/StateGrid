
// SafeDiskManagerDoc.h : CSafeDiskManagerDoc ��Ľӿ�
//


#pragma once


class CSafeDiskManagerDoc : public CDocument
{
protected: // �������л�����
	CSafeDiskManagerDoc();
	DECLARE_DYNCREATE(CSafeDiskManagerDoc)

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ʵ��
public:
	virtual ~CSafeDiskManagerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void SetTitle(LPCTSTR lpszTitle);
};


