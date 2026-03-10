// CPageRoom.cpp: 구현 파일
//

#include "pch.h"
#include "Client.h"
#include "afxdialogex.h"
#include "CPageRoom.h"


// CPageRoom 대화 상자

IMPLEMENT_DYNAMIC(CPageRoom, CDialogEx)

CPageRoom::CPageRoom(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PAGE_ROOM, pParent)
{

}

CPageRoom::~CPageRoom()
{
}

void CPageRoom::AddCharLine(const CString& line)
{
	// 채팅 로그 리스트박스에 한 줄 추가
	_chatLog.AddString(line);
	
	// 방금 추가한 마지막 항목이 보이도록 선택 위치를 맨 아래로 이동
	const int count = _chatLog.GetCount();
	if (count > 0)
	{
		//_chatLog.SetCurSel(count - 1);
	}
}

void CPageRoom::ClearChatLines()
{
	_chatLog.ResetContent();
}

void CPageRoom::ClearUsers()
{
	// 유저 목록 전체 삭제
	_users.ResetContent();
}

void CPageRoom::AddUser(const CString& name)
{
	// 유저 목록에 한 명 추가
	_users.AddString(name);
}

void CPageRoom::SetRoomName(CString& name)
{
	_roomName = name;
}

CString CPageRoom::GetRoomName()
{
	return _roomName;
}

void CPageRoom::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_CHATLOG, _chatLog);
	DDX_Control(pDX, IDC_LIST_USERS, _users);
	DDX_Text(pDX, IDC_EDIT_CHAT, _chatText);
}

BOOL CPageRoom::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	return TRUE;
}

void CPageRoom::OnBnClickedSend()
{
	UpdateData(TRUE);
	if (_chatText.IsEmpty()) return;

	if (GetParent())
	{
		GetParent()->PostMessageW(WMU_UI_EVENT, (WPARAM)UIEvent::SendChatClicked_Room, 0);
	}
}

void CPageRoom::OnBnClickedLeave()
{
	if (GetParent())
	{
		GetParent()->PostMessageW(WMU_UI_EVENT, (WPARAM)UIEvent::LeaveRoomClicked_Room, 0);
	}
}


BEGIN_MESSAGE_MAP(CPageRoom, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_SEND, &CPageRoom::OnBnClickedSend)
	ON_BN_CLICKED(IDC_BTN_LEAVE, &CPageRoom::OnBnClickedLeave)
END_MESSAGE_MAP()


// CPageRoom 메시지 처리기
