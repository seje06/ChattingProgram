#pragma once

class ICommandHandler
{
public:
	virtual ~ICommandHandler() = default;
	virtual void Execute(UIEvent ev, CDialogEx* page) = 0;
	void SetOwner(CDialogEx* owner) 
	{
		_owner = owner;
	}
protected:
	CDialogEx* _owner;
};

