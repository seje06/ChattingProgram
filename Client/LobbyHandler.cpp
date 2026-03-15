#include "pch.h"
#include "LobbyHandler.h"
#include "CPageLobby.h"
#include "Model.h"
#include "ContentsService.h"

void LobbyHandler::Execute(UIEvent ev, CDialogEx* page)
{
	CPageLobby* lobbyPg = dynamic_cast<CPageLobby*>(page);
	if (!lobbyPg) return;

	switch (ev)
	{
	case UIEvent::LoginCompleted_Lobby:
		OnLoginCompleted(lobbyPg);
		break;
	case UIEvent::RefreshClicked_Lobby:
		OnRefreshClicked(lobbyPg);
		break;
	/*case UIEvent::RegisterCompleted_Lobby:
		page->ShowWindow(SW_SHOW);
		lobbyPg->AddRoomRow(L"회원가입 완료 후 로비", 1, 1);
		break;*/
	case UIEvent::RefreshCompleted_Lobby:
		OnRefreshCompleted(lobbyPg);
		break;
	case UIEvent::CreateRoomClicked_Lobby:
		OnCreateRoomClicked(lobbyPg);
		break;
	case UIEvent::CreateRoomCompleted_Lobby:
		OnCreateRoomCompleted(lobbyPg);
		break;
	case UIEvent::JoinRoomClicked_Lobby:
		OnJoinRoomClicked(lobbyPg);
		break;
	case UIEvent::JoinRoomCompleted_Lobby:
		OnJoinRoomCompleted(lobbyPg);
		break;
	case UIEvent::LeaveRoomCompleted_Lobby:
		OnLeaveRoomCompleted(lobbyPg);
		break;
	default:
		CRASH("mapping");
		break;
	}
}

void LobbyHandler::OnLoginCompleted(CPageLobby* lobbyPg)
{
	lobbyPg->ShowWindow(SW_SHOW);
	bool isEmpty;
	auto data = AuthModel::Front(OUT isEmpty);

	Protocol::C_REFRESH_LOBBY pkt;
	if (!isEmpty)
	{
		lobbyPg->SetMeText(data.id);
		lobbyPg->ClearRooms();
	}
	function<void(RefreshLobbyModel)> callback = [this](RefreshLobbyModel model)
		{
			_owner->PostMessageW(WMU_UI_EVENT, (WPARAM)UIEvent::RefreshCompleted_Lobby);
		};
	RequestService<RefreshLobbyModel>::RequestService(pkt, callback);
	
	
}

void LobbyHandler::OnRefreshClicked(CPageLobby* lobbyPg)
{
	Protocol::C_REFRESH_LOBBY pkt;

	function<void(RefreshLobbyModel)> callback = [this](RefreshLobbyModel model)
		{
			_owner->PostMessageW(WMU_UI_EVENT, (WPARAM)UIEvent::RefreshCompleted_Lobby);
		};
	RequestService<RefreshLobbyModel>::RequestService(pkt, callback);
}

void LobbyHandler::OnRefreshCompleted(CPageLobby* lobbyPg)
{
	bool isEmpty = false;
	auto data = RefreshLobbyModel::Pop(isEmpty);
	ASSERT_CRASH(!isEmpty);
	
	if (data.isSuccess)
	{
		lobbyPg->ClearRooms();
		while (!data.roomNames.empty())
		{
			lobbyPg->AddRoomRow(data.roomNames.front(), data.userCounts.front(), 100);
			data.roomNames.pop();
			data.userCounts.pop();
		}
	}
}

void LobbyHandler::OnCreateRoomClicked(CPageLobby* lobbyPg)
{
	Protocol::C_CREATE_ROOM pkt;
	pkt.set_roomname(CT2A(lobbyPg->GetRoomName(), CP_UTF8));
	pkt.set_id(CT2A(lobbyPg->GetMeText(), CP_UTF8));
	
	function<void(CreateRoomModel)> callback = [this](CreateRoomModel model)
		{
			_owner->PostMessageW(WMU_UI_EVENT, (WPARAM)UIEvent::CreateRoomCompleted_Lobby);
		};
	RequestService<CreateRoomModel>::RequestService(pkt, callback);
}

void LobbyHandler::OnCreateRoomCompleted(CPageLobby* lobbyPg)
{
	bool isEmpty;
	auto data = CreateRoomModel::Front(isEmpty);
	ASSERT_CRASH(!isEmpty);

	if (!data.isSuccess)
	{
		AfxMessageBox(L"생성 실패!.", MB_OK | MB_ICONERROR);
		bool isEmpty;
		auto data = CreateRoomModel::Pop(isEmpty);
		return;
	}
	lobbyPg->SetRoomName(data.roomName);
	lobbyPg->ShowWindow(SW_HIDE);
	_owner->PostMessageW(WMU_UI_EVENT, (WPARAM)UIEvent::CreateRoomCompleted_Room);
}

void LobbyHandler::OnJoinRoomClicked(CPageLobby* lobbyPg)
{
	Protocol::C_JOIN_ROOM pkt;
	pkt.set_roomname(CT2A(lobbyPg->GetSelectedRoomName(), CP_UTF8));
	pkt.set_id(CT2A(lobbyPg->GetMeText(), CP_UTF8));

	function<void(JoinRoomModel)> callback = [this](JoinRoomModel model)
		{
			_owner->PostMessageW(WMU_UI_EVENT, (WPARAM)UIEvent::JoinRoomCompleted_Lobby);
		};
	RequestService<JoinRoomModel>::RequestService(pkt, callback);
}

void LobbyHandler::OnJoinRoomCompleted(CPageLobby* lobbyPg)
{
	bool isEmpty;
	auto data = JoinRoomModel::Front(isEmpty);
	ASSERT_CRASH(!isEmpty);
	if (!data.isSuccess)
	{
		AfxMessageBox(L"참가 실패!.", MB_OK | MB_ICONERROR);;
		auto data = JoinRoomModel::Pop(isEmpty);
		return;
	}
	lobbyPg->ShowWindow(SW_HIDE);
	_owner->PostMessageW(WMU_UI_EVENT, (WPARAM)UIEvent::JoinRoomCompleted_Room);
}


void LobbyHandler::OnLeaveRoomCompleted(CPageLobby* lobbyPg)
{
	lobbyPg->ShowWindow(SW_SHOW);
	lobbyPg->ClearRooms();
	bool isEmpty;
	auto data = AuthModel::Front(OUT isEmpty);
	ASSERT_CRASH(!isEmpty);
	
	Protocol::C_REFRESH_LOBBY pkt;
	lobbyPg->SetMeText(data.id);
	function<void(RefreshLobbyModel)> callback = [this](RefreshLobbyModel model)
		{
			_owner->PostMessageW(WMU_UI_EVENT, (WPARAM)UIEvent::RefreshCompleted_Lobby);
		};
	RequestService<RefreshLobbyModel>::RequestService(pkt, callback);
	
}
