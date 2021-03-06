
// serverDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "server.h"
#include "serverDlg.h"
#include "afxdialogex.h"
#include <WinSock2.h>
#include<afxsock.h>
#pragma comment(lib,"Ws2_32.lib ")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CserverDlg 对话框



CserverDlg::CserverDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CserverDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT2, showedit);
	DDX_Control(pDX, IDC_EDIT1, sendedit);
}

BEGIN_MESSAGE_MAP(CserverDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CserverDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CserverDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CserverDlg 消息处理程序

BOOL CserverDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CWinThread *pThread;
	pThread = AfxBeginThread(Server_Th, 0);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CserverDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CserverDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CserverDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


SOCKET accept_sock;
void CserverDlg::OnBnClickedOk()  //send
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	char *msg;
	sendedit.GetWindowTextW(str);
	msg = (char*)str.GetBuffer(str.GetLength());
	if (send(accept_sock, msg, CStringA(str).GetLength() + 32, 0) == SOCKET_ERROR)
	{
		sendedit.ReplaceSel(_T("发送失败\n"));
	}
	else if (str == "")
	{
		AfxMessageBox(_T("请输入信息"));
	}
	else
	{
		showedit.ReplaceSel(_T("server：" + str + "\r\n"));
		sendedit.SetWindowTextW(_T(""));
		sendedit.SetFocus();
	}
}


void CserverDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}
void CserverDlg::update(CString str)
{
	showedit.ReplaceSel(str + "\r\n");

}

UINT Server_Th(LPVOID p)
{
	CserverDlg * dlg = (CserverDlg *)AfxGetApp()->GetMainWnd(); //取得主窗口句柄
	WSADATA wsaData;
	WORD version = MAKEWORD(2, 2);
	WSAStartup(version, &wsaData);

	//	AfxMessageBox(_T("thread running!"));
	SOCKET sock; //建立套接字、绑定
				 //SOCKET accept_sock;  //接收连接
				 //确定发送数据
	SOCKADDR_IN local_addr, client_addr;
	int s = sizeof(SOCKADDR_IN);
	int res;

	//创建套接字
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		dlg->update(_T("创建套接字失败\n"));
	}

	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(5150);

	//CString ip;
	//ip= "127.0.0.1";
	char ip[10] = "127.0.0.1";
	in_addr adr;
	inet_pton(AF_INET, ip, (void*)&adr);
	//test
	//CString sd;
	//sd.Format(_T("0x%x"),adr.S_un.S_addr);
	//unsigned long a = 9;
	//sd.Format(_T("%d"),a);
	//AfxMessageBox(sd);

	//
	local_addr.sin_addr.S_un.S_addr = adr.S_un.S_addr;
	//绑定IP和端口、套接字
	if (bind(sock, (sockaddr*)&local_addr, s))
	{
		dlg->update(_T("绑定错误\n"));
	}
	//监听
	listen(sock, 3);
	dlg->update(_T("开始监听\n"));
	//accept
	if ((accept_sock = accept(sock, (sockaddr*)&client_addr, &s)) == INVALID_SOCKET)
	{
		dlg->update(_T("accept error\n"));
	}
	else
	{
		CString port;
		char adr[10];
		port.Format(_T("%d"), (int)client_addr.sin_port);
		inet_ntop(AF_INET, &client_addr.sin_addr, adr, 10);
		dlg->update(_T("已连接来自：" + CString(adr) + "  端口：" + port));

		/*
		ad= ((struct sockaddr_in*)ptr->ai_addr)->sin_addr;
		inet_ntop(AF_INET, &ad, addr, 10);
		IP = addr;
		*/


	}
	//接收数据
	char mes[1024] = { 0 };  //WCHAR

	while (1)
	{
		if ((res = recv(accept_sock, mes, 1024, 0))<0) //返回接收数据的大小res
		{

			dlg->update(_T("失去连接\n"));
			break;
		}
		else
		{

			mes[res] = '\0';
			CString cs, num;
			//	cs = mes;
			cs.Format(_T("%s"), mes);    //char[]  转换成  CString   (不要用强制转换)
										 //	AfxMessageBox(cs);
			dlg->update(_T("client：" + cs));
		}
	}


	return 0;
}