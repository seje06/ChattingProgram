#include "pch.h"
#include "LoginHandler.h"
#include "CPageLogin.h"

void LoginHandler::Execute(UIEvent ev, CDialogEx* page)
{
	CPageLogin* loginPg = dynamic_cast<CPageLogin*>(page);
	if (!loginPg) return;

	switch (ev)
	{
	case UIEvent::LoginClicked_Login:
		TRACE(loginPg->GetId());
		TRACE(loginPg->GetPw());
		_owner->PostMessageW(WMU_UI_EVENT, (WPARAM)UIEvent::LoginCompleted_Login, 0);
		break;
	case UIEvent::LoginCompleted_Login:
		page->ShowWindow(SW_HIDE);
		_owner->PostMessageW(WMU_UI_EVENT, (WPARAM)UIEvent::LoginCompleted_Lobby, 0);
		break;
	default:
		break;
	}

}
