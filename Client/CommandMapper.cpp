#include "pch.h"
#include "CommandMapper.h"
#include "ICommandHandler.h"
#include "CPageLogin.h"
#include "CPageLobby.h"
#include "CPageRoom.h"

#include "LoginHandler.h"
#include "LobbyHandler.h"
#include "RoomHandler.h"
#include "resource.h"

void CommandMapper::Mapping(std::map<UIEvent, std::pair<class CDialogEx*, class ICommandHandler*>>OUT & map, CWnd* parent)
{
	// 페이지 생성 및 설정
	auto* loginPage = new CPageLogin(parent); 
	auto* lobbyPage = new CPageLobby(parent);
	auto* roomPage = new CPageRoom(parent);

	loginPage->Create(IDD_PAGE_LOGIN, parent);
	lobbyPage->Create(IDD_PAGE_LOBBY, parent);
	roomPage->Create(IDD_PAGE_ROOM, parent);

	PlacePage(loginPage);
	PlacePage(lobbyPage);
	PlacePage(roomPage);

	loginPage->ShowWindow(SW_SHOW);
	lobbyPage->ShowWindow(SW_HIDE);
	roomPage->ShowWindow(SW_HIDE);
	

	// 핸들러 생성
	auto* loginHandler = new LoginHandler(); loginHandler->SetOwner(dynamic_cast<CDialogEx*>(parent));
	auto* lobbyHandler = new LobbyHandler(); lobbyHandler->SetOwner(dynamic_cast<CDialogEx*>(parent));
	auto* roomHandler = new RoomHandler();   roomHandler->SetOwner(dynamic_cast<CDialogEx*>(parent));
	
	// 이벤트와 커맨드 매핑 
	map[UIEvent::LoginClicked_Login] = { loginPage, loginHandler };
	map[UIEvent::LoginCompleted_Login] = { loginPage, loginHandler };
	map[UIEvent::LoginCompleted_Lobby] = { lobbyPage, lobbyHandler };

	map[UIEvent::RegisterClicked_Register] = { loginPage, loginHandler };
	map[UIEvent::RegisterCompleted_Register] = { loginPage, loginHandler };
	map[UIEvent::RegisterCompleted_Lobby] = { lobbyPage, lobbyHandler };

	map[UIEvent::CreateRoomClicked_Lobby] = { lobbyPage, lobbyHandler };
	map[UIEvent::CreateRoomCompleted_Lobby] = { lobbyPage, lobbyHandler };
	map[UIEvent::CreateRoomCompleted_Room] = { roomPage, roomHandler };

	map[UIEvent::JoinRoomClicked_Lobby] = { lobbyPage, lobbyHandler };
	map[UIEvent::JoinRoomCompleted_Lobby] = { lobbyPage, lobbyHandler };
	map[UIEvent::JoinRoomCompleted_Room] = { roomPage, roomHandler };
	
	map[UIEvent::RefreshClicked_Lobby] = { lobbyPage, lobbyHandler };
	map[UIEvent::RefreshCompleted_Lobby] = { lobbyPage, lobbyHandler };

	map[UIEvent::SendChatClicked_Room] = { roomPage, roomHandler };
	map[UIEvent::SendChatCompleted_Room] = { roomPage, roomHandler };
	map[UIEvent::LeaveRoomClicked_Room] = { roomPage, roomHandler };
	map[UIEvent::LeaveRoomCompleted_Room] = { roomPage, roomHandler };
	map[UIEvent::LeaveRoomCompleted_Lobby] = { lobbyPage, lobbyHandler };

}

void CommandMapper::PlacePage(CDialogEx* page)
{
	CWnd* host = page->GetParent()->GetDlgItem(IDC_STATIC_PAGE_HOST);

	CRect rc;
	host->GetWindowRect(&rc);
	page->ScreenToClient(&rc);

	page->SetWindowPos(nullptr,
		rc.left, rc.top,
		rc.Width(), rc.Height(),
		SWP_NOZORDER | SWP_SHOWWINDOW);
}

