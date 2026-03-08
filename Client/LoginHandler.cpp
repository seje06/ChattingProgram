#include "pch.h"
#include "LoginHandler.h"
#include "CPageLogin.h"
#include "ContentsService.h"
#include "Model.h"

void LoginHandler::Execute(UIEvent ev, CDialogEx* page)
{
	CPageLogin* loginPg = dynamic_cast<CPageLogin*>(page);
	if (!loginPg) return;

	switch (ev)
	{
	case UIEvent::LoginClicked_Login:
		OnClickedLogin(loginPg);

		break;
	case UIEvent::LoginCompleted_Login:
		OnLoginCompleted(loginPg);
		
		break;
	case UIEvent::RegisterClicked_Register:
		OnClickedRegister(loginPg);
		break;
	case UIEvent::RegisterCompleted_Register:
		OnRegisterCompleted(loginPg);
		break;
	default:
		break;
	}

}

void LoginHandler::OnClickedLogin(CPageLogin* loginPg)
{
	Protocol::C_LOGIN pkt;
	pkt.set_id(CT2A(loginPg->GetId(), CP_UTF8));
	pkt.set_pssd(CT2A(loginPg->GetPw(), CP_UTF8));

	function<void(AuthModel)> callback = [this](AuthModel model) {
		_owner->PostMessageW(WMU_UI_EVENT, (WPARAM)UIEvent::LoginCompleted_Login, 0);
		};

	ContentsService<AuthModel>(pkt, callback);
}

void LoginHandler::OnLoginCompleted(CPageLogin* loginPg)
{	
	bool isEmpty;
	auto data = AuthModel::Pop(isEmpty);
	ASSERT_CRASH(!isEmpty);
	if (!data.isSuccess)
	{
		AfxMessageBox(L"로그인 실패!.", MB_OK | MB_ICONERROR);
		return;
	}
	loginPg->ShowWindow(SW_HIDE);
	_owner->PostMessageW(WMU_UI_EVENT, (WPARAM)UIEvent::LoginCompleted_Lobby, 0);
	
}

void LoginHandler::OnClickedRegister(CPageLogin* loginPg)
{
	Protocol::C_REGISTER pkt;
	pkt.set_id(CT2A(loginPg->GetId(), CP_UTF8));
	pkt.set_pssd(CT2A(loginPg->GetPw(), CP_UTF8));

	function<void(AuthModel)> callback = [this](AuthModel model) {
		_owner->PostMessageW(WMU_UI_EVENT, (WPARAM)UIEvent::RegisterCompleted_Register, 0);
		};

	ContentsService<AuthModel>(pkt, callback);
}

void LoginHandler::OnRegisterCompleted(CPageLogin* loginPg)
{
	bool isEmpty;
	auto data = AuthModel::Pop(isEmpty);
	ASSERT_CRASH(!isEmpty);
	if (!data.isSuccess)
	{
		AfxMessageBox(L"회원가입 실패!.", MB_OK | MB_ICONERROR);
		return;
	}

	AfxMessageBox(L"회원가입이 완료되었습니다.", MB_OK | MB_ICONINFORMATION);
}
