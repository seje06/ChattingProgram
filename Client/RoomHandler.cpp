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
		page->ShowWindow(SW_SHOW);
		break;
	case UIEvent::JoinRoomCompleted_Room:
		page->ShowWindow(SW_SHOW);
		break;
	case UIEvent::SendChatClicked_Room:
		_owner->PostMessageW(WMU_UI_EVENT, WPARAM(UIEvent::SendChatCompleted_Room), 0);
		break;
	case UIEvent::SendChatCompleted_Room:
		TRACE(roomPg->GetChatText());
		roomPg->AddCharLine(roomPg->GetChatText());
		break;
	case UIEvent::RefreshRoomCompleted:
		break;
	case UIEvent::LeaveRoomClicked_Room:
		_owner->PostMessageW(WMU_UI_EVENT, WPARAM(UIEvent::LeaveRoomCompleted_Room), 0);
		break;
	case UIEvent::LeaveRoomCompleted_Room:
		page->ShowWindow(SW_HIDE);
		_owner->PostMessageW(WMU_UI_EVENT, WPARAM(UIEvent::LeaveRoomCompleted_Lobby), 0);
		break;
	}

}

void RoomHandler::OnCreateRoomCompleted(CPageRoom* roomPg)
{
	bool isEmpty;
	auto& data = CreateRoomModel::Pop(isEmpty);
	roomPg->ShowWindow(SW_SHOW);
}

void RoomHandler::OnJoinRoomCompleted(CPageRoom* roomPg)
{
}

void RoomHandler::OnSendChatClicked(CPageRoom* roomPg)
{
}

void RoomHandler::OnSendChatCompleted(CPageRoom* roomPg)
{
}

void RoomHandler::OnRefreshCompleted(CPageRoom* roomPg)
{

}

void RoomHandler::OnLeaveRoomClicked(CPageRoom* roomPg)
{
}

void RoomHandler::OnLeaveRoomCompleted(CPageRoom* roomPg)
{
}
