// CPageLobby.cpp: 구현 파일
//

#include "pch.h"
#include "Client.h"
#include "afxdialogex.h"
#include "CPageLobby.h"


// CPageLobby 대화 상자

IMPLEMENT_DYNAMIC(CPageLobby, CDialogEx)

CPageLobby::CPageLobby(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PAGE_LOBBY, pParent)
{

}

CPageLobby::~CPageLobby()
{
}

void CPageLobby::SetMeText(const CString& name)
{
	if (::IsWindow(_me.GetSafeHwnd())) _me.SetWindowTextW(name);
}

CString CPageLobby::GetMeText()
{
	CString text;
	if (!::IsWindow(_me.GetSafeHwnd())) return text;
	
	_me.GetWindowTextW(text);
	if (::IsWindow(_me.GetSafeHwnd())) return text;
}

int CPageLobby::GetSelectedRoomIndex() const
{
	POSITION pos = _rooms.GetFirstSelectedItemPosition();
	if (!pos) return -1;
	return _rooms.GetNextSelectedItem(pos);
}

CString CPageLobby::GetSelectedRoomName() const
{
	return _selectedRoomName;
}

void CPageLobby::InitRoomListColumns()
{
	_rooms.SetExtendedStyle(_rooms.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	_rooms.InsertColumn(0, L"방 이름", LVCFMT_LEFT, 200);
	_rooms.InsertColumn(1, L"인원", LVCFMT_LEFT, 80);

}

void CPageLobby::ClearRooms()
{
	_rooms.DeleteAllItems();
}

void CPageLobby::AddRoomRow(const CString& roomName, int cur, int max)
{
	int idx = _rooms.InsertItem(_rooms.GetItemCount(), roomName);
	CString people;
	people.Format(L"%d / %d", cur, max);
	_rooms.SetItemText(idx, 1, people);

}

void CPageLobby::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_STATIC_ME, _me);
	DDX_Control(pDX, IDC_LIST_ROOMS, _rooms);
	DDX_Text(pDX, IDC_EDIT_ROOMNAME, _roomName);
}

BOOL CPageLobby::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//_rooms.SubclassDlgItem(IDC_LIST_ROOMS, this);

	InitRoomListColumns();
	SetMeText(L"Me : (none)");

	return TRUE;
}

void CPageLobby::OnBnClickedCreateRoom()
{
	UpdateData(TRUE);
	if (_roomName.IsEmpty()) return;

	if (GetParent())
	{
		GetParent()->PostMessageW(WMU_UI_EVENT, (WPARAM)UIEvent::CreateRoomClicked_Lobby, 0);
	}
}

void CPageLobby::OnBnClickedJoinRoom()
{
	int index = GetSelectedRoomIndex();
	if (index < 0) return;
	_selectedRoomName = _rooms.GetItemText(index, 0);
	if (GetParent())
	{
		GetParent()->PostMessageW(WMU_UI_EVENT, (WPARAM)UIEvent::JoinRoomClicked_Lobby, 0);
	}
}

void CPageLobby::OnBnClickedRefresh()
{
	if (GetParent())
	{
		GetParent()->PostMessageW(WMU_UI_EVENT, (WPARAM)UIEvent::RefreshClicked_Lobby, 0);
	}
}


BEGIN_MESSAGE_MAP(CPageLobby, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_CREATE_ROOM, &CPageLobby::OnBnClickedCreateRoom)
	ON_BN_CLICKED(IDC_BTN_JOIN_ROOM, &CPageLobby::OnBnClickedJoinRoom)
	ON_BN_CLICKED(IDC_BTN_REFRESH, &CPageLobby::OnBnClickedRefresh)
END_MESSAGE_MAP()


// CPageLobby 메시지 처리기
