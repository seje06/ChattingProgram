#pragma once


class CommandMapper
{
public:
	static void Mapping(std::map<UIEvent, std::pair<class CDialogEx*, class ICommandHandler*>>OUT & map, class CWnd* parent);

private:
	static void PlacePage(class CDialogEx* page);
};

