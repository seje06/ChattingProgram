#pragma once
#include "afxdialogex.h"


// CPageRoom 대화 상자

class CPageRoom : public CDialogEx
{
	DECLARE_DYNAMIC(CPageRoom)

public:
	CPageRoom(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CPageRoom();

	void AddCharLine(const CString& line);
	void ClearChatLines();
	void ClearUsers();
	void AddUser(const CString& name);

	void SetRoomName(CString& name);
	CString GetRoomName();

	CString GetChatText() const { return _chatText; }
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PAGE_ROOM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog() override;

	afx_msg void OnBnClickedSend();
	afx_msg void OnBnClickedLeave();

	DECLARE_MESSAGE_MAP()

private:
	CListBox _chatLog;
	CListBox _users;
	CString _chatText;
	CString _roomName;
};
