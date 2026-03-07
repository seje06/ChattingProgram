#pragma once
#include "ICommandHandler.h"

class LobbyHandler : public ICommandHandler
{
	virtual void Execute(UIEvent ev, CDialogEx* page) override;
};

