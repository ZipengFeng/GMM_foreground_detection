
// VideoPlayerDlg.h : 头文件
//

#pragma once
#include "opencv2/highgui/highgui.hpp"
#include "CvvImage.h"
#include "afxcmn.h"
using namespace cv;

// CVideoPlayerDlg 对话框
class CVideoPlayerDlg : public CDialog
{
// 构造
public:
	CVideoPlayerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_VIDEOPLAYER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	CSliderCtrl Slider;
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	VideoCapture capture;
	CvVideoWriter* writer;
	HBITMAP hbitmap,hbitmap1;
	CRect rect,rect1;
	CStatic* pStc, *pStc1; //标识图像显示的Picture控件
	CDC* pDC, *pDC1; //视频显示控件设备上下文
	HDC hDC, hDC1; //视频显示控件设备句柄

	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnStnClickedView();
	afx_msg void OnStnClickedView2();
	afx_msg void OnBnClickedButton2();
};
