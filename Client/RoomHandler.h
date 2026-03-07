#pragma once
#include "ICommandHandler.h"

class RoomHandler : public ICommandHandler
{
	virtual void Execute(UIEvent ev, CDialogEx* page) override;
};

