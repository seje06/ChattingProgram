#include "pch.h"
#include "RoomHandler.h"
#include "CPageRoom.h"
#include "Model.h"
#include "ContentsService.h"

void RoomHandler::Execute(UIEvent ev, CDialogEx* page)
{
	CPageRoom* roomPg = dynamic_cast<CPageRoom*>(page);
	if (!roomPg) return;

	switch (ev)
	{
	case UIEvent::CreateRoomCompleted_Room:
		OnCreateRoomCompleted(roomPg);
		break;
	case UIEvent::JoinRoomCompleted_Room:
		OnJoinRoomCompleted(roomPg);
		break;
	case UIEvent::SendChatClicked_Room:
		OnSendChatClicked(roomPg);
		break;
	case UIEvent::SendChatCompleted_Room:
		OnSendChatCompleted(roomPg);
		break;
	case UIEvent::RefreshRoomCompleted:
		OnRefreshCompleted(roomPg);
		break;
	case UIEvent::LeaveRoomClicked_Room:
		OnLeaveRoomClicked(roomPg);
		break;
	case UIEvent::LeaveRoomCompleted_Room:
		OnLeaveRoomCompleted(roomPg);
		break;
	default:
		CRASH("mapping");
		break;
	}

}

void RoomHandler::OnCreateRoomCompleted(CPageRoom* roomPg)
{
	bool isEmpty;
	auto& data = CreateRoomModel::Pop(isEmpty);
	roomPg->SetRoomName(data.roomName);
	roomPg->ShowWindow(SW_SHOW);
	roomPg->ClearUsers();
	roomPg->ClearChatLines();

	auto& authData = AuthModel::Front(isEmpty);
	ASSERT_CRASH(!isEmpty);
	roomPg->AddUser(authData.id);
}

void RoomHandler::OnJoinRoomCompleted(CPageRoom* roomPg)
{
	bool isEmpty;
	auto& data = JoinRoomModel::Pop(isEmpty);
	roomPg->ShowWindow(SW_SHOW);
	roomPg->ClearUsers();
	roomPg->ClearChatLines();
	while (!data.userIds.empty())
	{
		CString& id = data.userIds.front();
		roomPg->AddUser(id);
		data.userIds.pop();
	}
	
}

void RoomHandler::OnSendChatClicked(CPageRoom* roomPg)
{
	Protocol::C_CHAT_LOG pkt;
	bool isEmpty;
	auto& data = AuthModel::Front(isEmpty);
	ASSERT_CRASH(!isEmpty);

	pkt.set_id(CT2A(data.id, CP_UTF8));
	pkt.set_log(CT2A(roomPg->GetChatText(), CP_UTF8));

	function<void(ChatLogModel)> callback = [this](ChatLogModel model)
		{
			_owner->PostMessageW(WMU_UI_EVENT, (WPARAM)UIEvent::SendChatCompleted_Room, 0);
		};

	RequestService<ChatLogModel>(pkt, callback);
}

void RoomHandler::OnSendChatCompleted(CPageRoom* roomPg)
{
	bool isEmpty;
	auto& data = ChatLogModel::Pop(isEmpty);
	ASSERT_CRASH(!isEmpty);
	
	CString bindLog;
	bindLog.Format(L"%s : %s", data.id, data.log);
	
	roomPg->AddCharLine(bindLog);
}

void RoomHandler::OnRefreshCompleted(CPageRoom* roomPg)
{
	bool isEmpty;
	auto& data = RefreshRoomModel::Pop(isEmpty);
	roomPg->ClearUsers();
	while (!data.userIds.empty())
	{
		CString& id = data.userIds.front();
		roomPg->AddUser(id);
		data.userIds.pop();
	}
}

void RoomHandler::OnLeaveRoomClicked(CPageRoom* roomPg)
{
	Protocol::C_ROOM_OUT pkt;
	bool isEmpty;
	auto& data = AuthModel::Front(isEmpty);
	ASSERT_CRASH(!isEmpty);

	pkt.set_id(CT2A(data.id, CP_UTF8));
	pkt.set_roomname(CT2A(roomPg->GetRoomName(), CP_UTF8));

	function<void(RoomOutModel)> callback = [this](RoomOutModel model)
		{
			_owner->PostMessageW(WMU_UI_EVENT, (WPARAM)UIEvent::LeaveRoomCompleted_Room);
		};
	RequestService<RoomOutModel>(pkt, callback);
}

void RoomHandler::OnLeaveRoomCompleted(CPageRoom* roomPg)
{
	roomPg->ClearChatLines();
	roomPg->ClearUsers();

	roomPg->ShowWindow(SW_HIDE);

	_owner->PostMessageW(WMU_UI_EVENT, (WPARAM)UIEvent::LeaveRoomCompleted_Lobby);
}
