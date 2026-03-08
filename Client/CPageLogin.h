#pragma once
#include "afxdialogex.h"


// CPageLogin 대화 상자

class CPageLogin : public CDialogEx
{
	DECLARE_DYNAMIC(CPageLogin)

public:
	CPageLogin(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CPageLogin();

	CString GetId() const { return _id; }
	CString GetPw() const { return _pw; }

	void SetStatusText(const CString& text);

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PAGE_LOGIN };
#endif

protected:
	// 컨트롤의값을 변수에 넣거나 또는 반대로 하게해줄수 있음, 또는 유효성 검사
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog() override;

	//afx_msg란 메시지에 함수를 연결시키려면 붙여줘야하는 매크로
	afx_msg void OnBnClickedLogin();
	afx_msg void OnBnClickedLegister();

	DECLARE_MESSAGE_MAP()

private:
	CString _id;
	CString _pw;
	CStatic _status;
};
