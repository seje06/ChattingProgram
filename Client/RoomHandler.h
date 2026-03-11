#pragma once
#include "ICommandHandler.h"

class CPageRoom;

class RoomHandler : public IEventHandler
{
public:
	virtual void Execute(UIEvent ev, CDialogEx* page) override;
	void OnCreateRoomCompleted(CPageRoom* roomPg);
	void OnJoinRoomCompleted(CPageRoom* roomPg);
	void OnSendChatClicked(CPageRoom* roomPg);
	void OnSendChatCompleted(CPageRoom* roomPg);
	void OnRefreshCompleted(CPageRoom* roomPg);
	void OnLeaveRoomClicked(CPageRoom* roomPg);
	void OnLeaveRoomCompleted(CPageRoom* roomPg);
};

