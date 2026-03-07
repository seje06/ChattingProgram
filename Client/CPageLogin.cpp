// CPageLogin.cpp: 구현 파일
//

#include "pch.h"
#include "Client.h"
#include "afxdialogex.h"
#include "CPageLogin.h"


// CPageLogin 대화 상자

IMPLEMENT_DYNAMIC(CPageLogin, CDialogEx)

CPageLogin::CPageLogin(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PAGE_LOBBY, pParent)
{

}

CPageLogin::~CPageLogin()
{
}

void CPageLogin::SetStatusText(const CString& text)
{
}

void CPageLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_ID, _id);
	DDX_Text(pDX, IDC_EDIT_PW, _pw);
	DDX_Control(pDX, IDC_STATIC_STATUS, _status);
}

BOOL CPageLogin::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	_status.SetWindowTextW(L"Not connected"); //커넥 텍스트 초기 설정

	return true;
}

void CPageLogin::OnBnClickedLogin()
{
	// Edit 값 -> 멤버로 가져오기
	UpdateData(TRUE);

	//최소 검증
	if (_id.IsEmpty() || _pw.IsEmpty())
	{
		SetStatusText(L"ID/PW를 입력하세요.");
		return;
	}

	// 메인에게 로그인 이벤트 전달
	if(GetParent())
	{
		GetParent()->PostMessageW(WMU_UI_EVENT, (WPARAM)UIEvent::LoginClicked_Login, 0);
	}
}


BEGIN_MESSAGE_MAP(CPageLogin, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_LOGIN, &CPageLogin::OnBnClickedLogin)
END_MESSAGE_MAP()


// CPageLogin 메시지 처리기
