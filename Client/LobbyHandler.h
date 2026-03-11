#pragma once
#include "ICommandHandler.h"

class LobbyHandler : public IEventHandler
{
public:
	virtual void Execute(UIEvent ev, CDialogEx* page) override;

	void OnLoginCompleted(class CPageLobby* lobbyPg);
	void OnRefreshClicked(class CPageLobby* lobbyPg);
	void OnRefreshCompleted(class CPageLobby* lobbyPg);
	void OnCreateRoomClicked(class CPageLobby* lobbyPg);
	void OnCreateRoomCompleted(class CPageLobby* lobbyPg);
	void OnJoinRoomClicked(class CPageLobby* lobbyPg);
	void OnJoinRoomCompleted(class CPageLobby* lobbyPg);
	void OnLeaveRoomCompleted(class CPageLobby* lobbyPg);
};


