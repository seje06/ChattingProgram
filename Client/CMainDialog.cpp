// CMainDialog.cpp: 구현 파일
//

#include "pch.h"
#include "Client.h"
#include "afxdialogex.h"
#include "CMainDialog.h"
#include "ICommandHandler.h"

// CMainDialog 대화 상자

IMPLEMENT_DYNAMIC(CMainDialog, CDialogEx)

CMainDialog::CMainDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MAIN_DIALOG, pParent)
{

}

CMainDialog::~CMainDialog()
{
	set<void*> mustDeleteSet;
	auto it = _eventMap.begin();
	while (it != _eventMap.end())
	{
		mustDeleteSet.insert(it->second.first);
		mustDeleteSet.insert(it->second.second);
		it++;
	}
	for (auto it : mustDeleteSet)
	{
		delete it;
	}
}

void CMainDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL CMainDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CWnd* host = GetDlgItem(IDC_STATIC_PAGE_HOST);
	host->ModifyStyle(0, SS_ETCHEDFRAME);

	_cmdMapper.Mapping(OUT _eventMap, this);

	return TRUE;
}

LRESULT CMainDialog::OnUIEvent(WPARAM wParam, LPARAM lParam)
{
	UIEvent ev = static_cast<UIEvent>(wParam);

	auto it = _eventMap.find(ev);
	if (it == _eventMap.end()) return -1;

	CDialogEx* sender = it->second.first;
	it->second.second->Execute(ev, sender);

	return 0;
}


BEGIN_MESSAGE_MAP(CMainDialog, CDialogEx)
	ON_MESSAGE(WMU_UI_EVENT, &CMainDialog::OnUIEvent)
	ON_STN_CLICKED(IDC_STATIC_PAGE_HOST, &CMainDialog::OnStnClickedStaticPageHost)
END_MESSAGE_MAP()


void CMainDialog::OnStnClickedStaticPageHost()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
