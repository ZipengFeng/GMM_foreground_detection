
// VideoPlayerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "VideoPlayer.h"
#include "VideoPlayerDlg.h"
#include "GMM.h"
#include <iostream>
#include <cstdio>
#include <stdio.h>  
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//CvVideoWriter *writer;


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CVideoPlayerDlg 对话框




CVideoPlayerDlg::CVideoPlayerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVideoPlayerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVideoPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, Slider);
}

BEGIN_MESSAGE_MAP(CVideoPlayerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CVideoPlayerDlg::OnBnClickedButton1)
	ON_WM_TIMER()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CVideoPlayerDlg::OnNMCustomdrawSlider1)
	ON_STN_CLICKED(IDC_VIEW, &CVideoPlayerDlg::OnStnClickedView)
	ON_STN_CLICKED(IDC_VIEW2, &CVideoPlayerDlg::OnStnClickedView2)
	ON_BN_CLICKED(IDC_BUTTON2, &CVideoPlayerDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CVideoPlayerDlg 消息处理程序

BOOL CVideoPlayerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	pStc=(CStatic *)GetDlgItem(IDC_VIEW);//IDC_VIEW为Picture控件ID
	pStc->GetClientRect(&rect);//将CWind类客户区的坐标点传给矩形
	pDC=pStc->GetDC(); //得到Picture控件设备上下文
	hDC=pDC->GetSafeHdc(); //得到控件设备上下文的句柄 

	pStc1 = (CStatic *)GetDlgItem(IDC_VIEW2);//IDC_VIEW2为Picture控件ID
	pStc1->GetClientRect(&rect1);//将CWind类客户区的坐标点传给矩形
	pDC1 = pStc1->GetDC(); //得到Picture控件设备上下文
	hDC1 = pDC1->GetSafeHdc(); //得到控件设备上下文的句柄 

    hbitmap = ::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP1));
	pStc->ModifyStyle(1,SS_BITMAP);
	pStc->SetBitmap(hbitmap);
	hbitmap1 = ::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP2));
	pStc1->ModifyStyle(1, SS_BITMAP);
	pStc1->SetBitmap(hbitmap1);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CVideoPlayerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CVideoPlayerDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CVideoPlayerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

int numframe = 0;
int fps = 0, frameHeight=0, frameWidth=0 ;
void CVideoPlayerDlg::OnBnClickedButton1()
{
	CString caption;
	GetDlgItemText(IDC_BUTTON1,caption);
	if (caption == _T("关闭视频"))    //关闭视频按钮
	{
		KillTimer(1);
		capture.release();
		SetDlgItemText(IDC_BUTTON1,_T("打开视频"));
		((CSliderCtrl *)GetDlgItem(IDC_SLIDER1))->EnableWindow(FALSE);  //滑动条失效
		((CSliderCtrl *)GetDlgItem(IDC_SLIDER1))->SetPos(0); //设置滑动条位置
		SetDlgItemText(IDC_BUTTON2, _T("暂停"));//修改按钮的Caption
		pStc->SetBitmap(hbitmap); //恢复位图资源
		pStc1->SetBitmap(hbitmap1); //恢复位图资源
		numframe = 0;
		return;
	}
   

 //打开视频,弹出通用对话框,选择要播放的视频文件
	String _Path;  
	CFileDialog Openfiledlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, _T("Video Files (*.rmvb;*.avi)|*.rmvb;*.avi||"));
	if (Openfiledlg.DoModal() == IDOK)
	{
		//CString类型转换为string类型
		CString  filepath;      
		filepath=Openfiledlg.GetPathName();
		CStringA temp(filepath.GetBuffer(0));
		filepath.ReleaseBuffer();
		_Path = temp.GetBuffer(0);
		temp.ReleaseBuffer();
	}
	else
		return ;
	

	capture.open(_Path);  //打开视频
	if (!capture.isOpened())
	{        
		MessageBox(_T("无法打开视频！"),_T("错误"));     
		return;
	}
	else
	{
	
		((CSliderCtrl *)GetDlgItem(IDC_SLIDER1))->EnableWindow(TRUE);  //激活滑动条
		int sumframe = (int)capture.get(CV_CAP_PROP_FRAME_COUNT); //获取总帧数
		fps = capture.get(CV_CAP_PROP_FPS);   //获取视频帧率
		frameHeight = capture.get(CV_CAP_PROP_FRAME_HEIGHT); //获取视频尺寸
		frameWidth = capture.get(CV_CAP_PROP_FRAME_WIDTH);

		//int outCompressCodec = (int)capture.get(CV_CAP_PROP_FOURCC);  
		//定义保存视频流的格式
		writer = cvCreateVideoWriter("Detection_result.avi", CV_FOURCC('X', 'V', 'I', 'D'), fps, Size(frameWidth, frameHeight));
		Slider.SetRange(0, sumframe);//设置滑动条的范围,为视频的总帧数
		SetTimer(1,(unsigned int)(1000*1.0/capture.get(CV_CAP_PROP_FPS)),NULL); //定时器，定时时间和帧率一致
		SetDlgItemText(IDC_BUTTON1,_T("关闭视频"));//修改按钮的Caption
		CString csumframe;
		csumframe.Format("%d", sumframe);
		SetDlgItemText(IDC_EDIT2, _T(csumframe));  //显示总帧数
	
	}
}

void CVideoPlayerDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	CString caption;
	GetDlgItemText(IDC_BUTTON2, caption);
	if (caption == _T("暂停"))    //关闭视频按钮
	{
		KillTimer(1);
		SetDlgItemText(IDC_BUTTON2, _T("重新播放"));
		return;
	}
	else
	{
		SetTimer(1, (unsigned int)(1000 * 1.0 / capture.get(CV_CAP_PROP_FPS)), NULL); //定时器，定时时间和帧率一致
		SetDlgItemText(IDC_BUTTON2, _T("暂停"));//修改按钮的Caption
	}
}

Mat frame, gray, mask;  //每一帧的原图、灰度图、二值图
MOG_BGS Mog_Bgs;
vector<Feather> featherList;
Mat dst;

void CVideoPlayerDlg::OnTimer(UINT_PTR nIDEvent)
{

	if(1 == nIDEvent)
	{

       //取出一帧图像
		capture.read(frame);
			
		if (frame.empty())
		{
			KillTimer(1);
			MessageBox(_T("视频结束"),_T("提示"));
			capture.release();
			//writer.release();
			cvReleaseVideoWriter(&writer);
		}
		else
		{
			numframe+=1;
			cvtColor(frame, gray, CV_RGB2GRAY);

			//将提取到的连通域在图像的每一帧中标示出来
			for (vector<Feather>::iterator it = featherList.begin(); it < featherList.end(); it++)
			{
				rectangle(frame, it->boundingbox, 255);
			}

			CvvImage m_CvvImage;
			IplImage frame(frame);   //Mat 转IplImage


			m_CvvImage.CopyOf(&frame, 1); //复制该帧图像   
			m_CvvImage.DrawToHDC(hDC, &rect); //显示到设备的矩形框内
		
			Slider.SetPos((int)capture.get(CV_CAP_PROP_POS_FRAMES));//设置视频的位置

			CString cnumframe;
			cnumframe.Format("%d", numframe);
			SetDlgItemText(IDC_EDIT1, _T(cnumframe));

			if (numframe == 1)
			{
				Mog_Bgs.init(gray);  //初始化
				Mog_Bgs.processFirstFrame(gray);
				//cout << " Using " << TRAIN_FRAMES << " frames to training GMM..." << endl;
			}
			else if (numframe < TRAIN_FRAMES)
			{
				Mog_Bgs.trainGMM(gray);  //开始训练
			}
			else if (numframe == TRAIN_FRAMES)
			{
				Mog_Bgs.getFitNum(gray);   //返回训练参数
				//cout << " Training GMM complete!" << endl;
			}
			else
			{
				Mog_Bgs.testGMM(gray);   //开始测试
				mask = Mog_Bgs.getMask();
				Mat element = getStructuringElement(MORPH_RECT, Size(2, 2));   //图像开操作
				morphologyEx(mask, mask, CV_MOP_OPEN, element);

				int contours = bwLabel(mask, dst, featherList);  //连通域提取

				CvvImage m_CvvImage1;
				IplImage mask(mask);   //Mat 转IplImage
				m_CvvImage1.CopyOf(&mask, 1); //复制该帧图像   
				m_CvvImage1.DrawToHDC(hDC1, &rect1); //显示到设备的矩形框内

			    //将检测图像保存为视频流
				cvWriteFrame(writer, &frame);

			}


		}
	}
	CDialog::OnTimer(nIDEvent);
}

void CVideoPlayerDlg::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
   capture.set(CV_CAP_PROP_POS_FRAMES,Slider.GetPos());  //设置视频的起始帧
	*pResult = 0;
}


void CVideoPlayerDlg::OnStnClickedView()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CVideoPlayerDlg::OnStnClickedView2()
{
	// TODO: 在此添加控件通知处理程序代码
}



