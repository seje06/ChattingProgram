#pragma once
#include "afxdialogex.h"
#include "CommandMapper.h"

// CMainDialog 대화 상자

class CMainDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CMainDialog)

public:
	CMainDialog(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CMainDialog();
	;
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAIN_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog() override;
	afx_msg LRESULT OnUIEvent(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

private:
	std::map<UIEvent, std::pair<CDialogEx*, IEventHandler*>> _eventMap;
	EventMapper _cmdMapper;
public:
	afx_msg void OnStnClickedStaticPageHost();
};
