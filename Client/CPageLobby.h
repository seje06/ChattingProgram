#pragma once
#include "afxdialogex.h"


// CPageLobby 대화 상자

class CPageLobby : public CDialogEx
{
	DECLARE_DYNAMIC(CPageLobby)

public:
	CPageLobby(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CPageLobby();

	void SetMeText(const CString& name);
	CString GetMeText();
	CString GetRoomName() const { return _roomName; }
	int GetSelectedRoomIndex() const;
	CString GetSelectedRoomName() const;

	void InitRoomListColumns();
	void ClearRooms();
	void AddRoomRow(const CString& roomName, int cur, int max);

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PAGE_LOBBY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog() override;

	afx_msg void OnBnClickedCreateRoom();
	afx_msg void OnBnClickedJoinRoom();
	afx_msg void OnBnClickedRefresh();


	DECLARE_MESSAGE_MAP()
	
private:
	CStatic _me;
	CListCtrl _rooms;
	CString _roomName;
	CString _selectedRoomName;
};
