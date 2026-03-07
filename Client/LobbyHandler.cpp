#include "pch.h"
#include "LobbyHandler.h"
#include "CPageLobby.h"

void LobbyHandler::Execute(UIEvent ev, CDialogEx* page)
{
	CPageLobby* lobbyPg = dynamic_cast<CPageLobby*>(page);
	if (!lobbyPg) return;

	switch (ev)
	{
	case UIEvent::LoginCompleted_Lobby:
		page->ShowWindow(SW_SHOW);
		lobbyPg->AddRoomRow(L"로그인완료후 로비", 1 , 1);
		
		break;
	case UIEvent::CreateRoomCompleted_Lobby:
		page->ShowWindow(SW_HIDE);
		_owner->PostMessageW(WMU_UI_EVENT, (WPARAM)UIEvent::CreateRoomCompleted_Room, 0);
		break;
	case UIEvent::JoinRoomCompleted_Lobby:
		page->ShowWindow(SW_HIDE);
		_owner->PostMessageW(WMU_UI_EVENT, (WPARAM)UIEvent::JoinRoomCompleted_Room, 0);
		break;
	case UIEvent::LeaveRoomCompleted_Lobby:
		page->ShowWindow(SW_SHOW);
		lobbyPg->AddRoomRow(L"룸떠난후 로비", 1, 1);
		break;
	case UIEvent::RefreshClicked_Lobby:
		_owner->PostMessageW(WMU_UI_EVENT, (WPARAM)UIEvent::RefreshCompleted_Lobby, 0);
		break;
	case UIEvent::RefreshCompleted_Lobby:
		TRACE(L"갱신완료");
		break;
	case UIEvent::JoinRoomClicked_Lobby:
		_owner->PostMessageW(WMU_UI_EVENT, (WPARAM)UIEvent::JoinRoomCompleted_Lobby, 0);
		break;
	case UIEvent::CreateRoomClicked_Lobby:
		_owner->PostMessageW(WMU_UI_EVENT, (WPARAM)UIEvent::CreateRoomCompleted_Lobby, 0);
		break;
	}
}
