#pragma once
#include "ICommandHandler.h"

class LoginHandler : public ICommandHandler
{
public:
	virtual void Execute(UIEvent ev, CDialogEx* page) override;

	void OnClickedLogin(class CPageLogin* loginPg);
	void OnLoginCompleted(class CPageLogin* loginPg);

	void OnClickedRegister(class CPageLogin* loginPg);
	void OnRegisterCompleted(class CPageLogin* loginPg);
};

