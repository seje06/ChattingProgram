#include "pch.h"
#include "RoomHandler.h"
#include "CPageRoom.h"

void RoomHandler::Execute(UIEvent ev, CDialogEx* page)
{
	CPageRoom* roomPg = dynamic_cast<CPageRoom*>(page);
	if (!roomPg) return;

	switch (ev)
	{
	case UIEvent::SendChatClicked_Room:
		_owner->PostMessageW(WMU_UI_EVENT, WPARAM(UIEvent::SendChatCompleted_Room), 0);
		break;
	case UIEvent::SendChatCompleted_Room:
		TRACE(roomPg->GetChatText());
		roomPg->AddCharLine(roomPg->GetChatText());
		break;
	case UIEvent::CreateRoomCompleted_Room:
		page->ShowWindow(SW_SHOW);
		break;
	case UIEvent::JoinRoomCompleted_Room:
		page->ShowWindow(SW_SHOW);
		break;
	case UIEvent::LeaveRoomCompleted_Room:
		page->ShowWindow(SW_HIDE);
		_owner->PostMessageW(WMU_UI_EVENT, WPARAM(UIEvent::LeaveRoomCompleted_Lobby), 0);
		break;
	case UIEvent::LeaveRoomClicked_Room:
		_owner->PostMessageW(WMU_UI_EVENT, WPARAM(UIEvent::LeaveRoomCompleted_Room), 0);
		break;
	}

}
