#pragma once
#include "ICommandHandler.h"

class LoginHandler : public ICommandHandler
{
	virtual void Execute(UIEvent ev, CDialogEx* page) override;
};

