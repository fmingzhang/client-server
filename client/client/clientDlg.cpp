
// clientDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "client.h"
#include "clientDlg.h"
#include "afxdialogex.h"
#include<WinSock2.h>
#include<afxsock.h>

#pragma comment(lib, "Ws2_32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CclientDlg 对话框



CclientDlg::CclientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CLIENT_DIALOG, pParent)
	, editip()
	, editshow()
	, editsend()
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CclientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT2, editip);
	DDX_Control(pDX, IDC_EDIT1, editshow);
	DDX_Control(pDX, IDC_EDIT3, editsend);
}

BEGIN_MESSAGE_MAP(CclientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK3, &CclientDlg::OnBnClickedOk3)
	ON_BN_CLICKED(IDOK2, &CclientDlg::OnBnClickedOk2)
	ON_BN_CLICKED(IDOK, &CclientDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CclientDlg 消息处理程序

BOOL CclientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	editip.SetWindowTextW(_T("127.0.0.1"));
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CclientDlg::OnPaint()
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
HCURSOR CclientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


SOCKET sock;
void CclientDlg::OnBnClickedOk3()
{
	// TODO: 在此添加控件通知处理程序代码
	WSADATA wsaData;
	SOCKADDR_IN server_addr;
	WORD version;
	version = MAKEWORD(2, 2);
	WSAStartup(version, &wsaData);
	//获取服务器信息
	/*
	CString IP;
	editip.GetWindowTextW(IP);
	AfxMessageBox(IP);
	*/
	//server_addr.sin_addr.S_un.S_addr = inet_addr((char*)&IP);
	char IP[10] = "127.0.0.1";
	in_addr adr;
	inet_pton(AF_INET, IP, (void*)&adr);
	server_addr.sin_addr.S_un.S_addr = adr.S_un.S_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(5150);
	//创建套接字
	if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		editshow.ReplaceSel(_T("create socket error\r\n"));
	}
	//建立连接
	if (connect(sock, (struct sockaddr *) &server_addr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		editshow.ReplaceSel(_T("连接失败\r\n"));
	}
	else
	{
		editshow.ReplaceSel(_T("连接成功\r\n"));
		CWinThread *pThread;
		pThread = AfxBeginThread(Recv_Th, this);
		//btn_conn.EnableWindow(FALSE);//按钮变灰  
	}


}


void CclientDlg::OnBnClickedOk2()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	char *msg;
	//str = "abc我的";
	editsend.GetWindowText(str);
	msg = (char*)str.GetBuffer(str.GetLength());
	// = "w和";
	int s;
	if ((s = send(sock, msg, CStringA(str).GetLength() + 32, 0)) == SOCKET_ERROR)
	{
		update(_T("发送失败"));
	}
	else if (str == "")
	{
		AfxMessageBox(_T("请输入信息"));

	}
	else
	{
		//	CString r;
		//	r.Format(_T("%d"), s);
		//	AfxMessageBox(r);

		update(_T("client:" + str));//消息上屏，清空输入，并重获焦点  
		editsend.SetWindowText(_T(""));
		editsend.SetFocus();

	}
}


void CclientDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}

void CclientDlg::update(CString str)
{
	editshow.ReplaceSel(str + "\r\n");
}


UINT Recv_Th(LPVOID p)  //线程用于接收数据
{
	int res;
	char msg[1024];
	CString str;
	CclientDlg * dlg = (CclientDlg *)AfxGetApp()->GetMainWnd();
	dlg->update(_T("Initialization"));

	while (1)
	{
		if ((res = recv(sock, msg, 1024, 0))<0)
		{
			dlg->update(_T("失去连接"));
			break;
		}
		else
		{
			msg[res] = '\0';
			CString cs;
			cs.Format(_T("%s"), msg);
			dlg->update(_T("server:") + cs);
		}
	}
	return 0;

}

