#include "WebCamViewCtrl.h"
#include <fstream>
#include <iostream>
#include <tchar.h>
using namespace std;
#include <log4cplus/loggingmacros.h>
extern __declspec(dllimport) log4cplus::Logger pTestLogger;
//#include <log4cxx/logger.h>
//#include <log4cxx/propertyconfigurator.h>
//using namespace log4cxx;
//using namespace log4cxx::helpers;

//extern int ox,oy,nx,ny;
//extern bool m_bIsMouseDown;//鼠标是否按下
//extern bool captureScreen;
//extern int width,height;//窗口的宽度和高度
//extern float multipleW,multipleH;//放大缩小倍数
///////////////////////////////////////////////////////////////////////////

inline void GDI_DrawLine(HDC hDC, POINT pointB, POINT pointE)
{
	MoveToEx(hDC, pointB.x, pointB.y, NULL);
	LineTo(hDC, pointE.x, pointE.y);
}

inline void AppScalePoint(POINT &point, const SIZE &sizeSRC, const SIZE &sizeDST)
{
	if ((0 == sizeSRC.cx) || (0 == sizeSRC.cy) || (0 == sizeDST.cx) || (0 == sizeDST.cy))
	{
		point.x = point.y = 0;
	}
	else
	{
		point.x = (long)(point.x * ((double)sizeDST.cx / (sizeSRC.cx * 1.0)));
		point.y = (long)(point.y * ((double)sizeDST.cy / (sizeSRC.cy * 1.0)));
	}
}

///////////////////////////////////////////////////////////////////////////
// CWebCamViewCtrl

//IMPLEMENT_DYNAMIC(CWebCamViewCtrl, CWnd)

//=========================================================================

//
// CWebCamViewCtrl constructor/destructor
//

CWebCamViewCtrl::CWebCamViewCtrl(void)
: m_bAdjustPaint(FALSE),str("")
{
	m_hPenGreen = ::CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
	m_rgbPainter.Initial(ECV_MAX_MEMDC_WIDTH, ECV_MAX_MEMDC_HEIGHT, 24);
	//DrawTrackArea();
	/*m_rgbPainter.EraseBkgnd()*/
	
	//m_rgbPainter.EraseBkgnd(RGB(255, 0, 1), re);	

}

CWebCamViewCtrl::~CWebCamViewCtrl(void)
{
	::DeleteObject((HGDIOBJ)m_hPenGreen);
}

//=========================================================================

//
// CWebCamViewCtrl overrides
//


void CWebCamViewCtrl::PreSubclassWindow()
{
	//CWnd::PreSubclassWindow();

	m_rgbPainter.Initial(ECV_MAX_MEMDC_WIDTH, ECV_MAX_MEMDC_HEIGHT, 24);
}

bool CWebCamViewCtrl::PreTranslateMessage(MSG* pMsg)
{
	/*
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		{
			if ((VK_ESCAPE == pMsg->wParam) && (GetParent() == NULL) && (GetOwner() != NULL))
			{
				::PostMessage(GetOwner()->GetSafeHwnd(), WM_COMMAND, IDC_STC_PLAYWND, NULL);
			}
		}
		break;

	default:
		break;
	}

	return CWnd::PreTranslateMessage(pMsg);
	*/
	return false;
}

//************************************
// Method:    WriteBmpFile
// FullName:  WriteBmpFile
// Access:    public 
// Returns:   void
// Qualifier: 截图
// Parameter: LPCTSTR szFile
// Parameter: LPBYTE pRgb24Data
// Parameter: int nWidth
// Parameter: int nHeight
//************************************
void WriteBmpFile(char * szFile, LPBYTE pRgb24Data,int nSize, int nWidth, int nHeight)
{
	DWORD dwBytesLine = (nWidth);

	BITMAPFILEHEADER bmheader;
	memset(&bmheader, 0, sizeof(BITMAPFILEHEADER));	

	bmheader.bfType = 0x4d42;
	bmheader.bfReserved1 = 0;
	bmheader.bfReserved2 = 0;
	bmheader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmheader.bfSize = bmheader.bfOffBits + nWidth*nHeight*24/8;

	BITMAPINFOHEADER bmInfo;
	memset(&bmInfo, 0, sizeof(BITMAPINFOHEADER));	

	bmInfo.biSize = sizeof(BITMAPINFOHEADER);
	bmInfo.biWidth = nWidth;
	bmInfo.biHeight = nHeight;
	bmInfo.biPlanes = 1;
	bmInfo.biBitCount = 24;
	bmInfo.biCompression = BI_RGB;
	bmInfo.biSizeImage = (nWidth*24+31)/32*4*nHeight;
	bmInfo.biXPelsPerMeter = 100;
	bmInfo.biYPelsPerMeter = 100;
	bmInfo.biClrUsed = 0;
	bmInfo.biClrImportant = 0;

	fstream file;
	file.open(szFile, ios::binary | ios::out);
	if (file)
	{
		file.write((char *)&bmheader, sizeof(BITMAPFILEHEADER));
		file.write((char *)&bmInfo, sizeof(BITMAPINFOHEADER));		
		file.write((char *)pRgb24Data, nWidth*nHeight*24/8);
		file.flush();
		file.close();
	}
}

//=========================================================================

//
// CWebCamViewCtrl public interfaces
//

/**************************************************************************
* FunctionName:
*     RealShowImage
* Description:
*     实时显示图像数据。
* Parameter:
*     @[in ] lpRgbData: 图像的 RGB 数据。
*     @[in ] lwidth: 图像宽度。
*     @[in ] lheight: 图像高度。
*     @[in ] vecTrackArea: 图像的跟踪区域。
* ReturnValue:
*     void
*/
void CWebCamViewCtrl::RealShowImage(unsigned char * lpRgbData, long lwidth, long lheight
	, const std::vector< RECT > &vecTrackArea, HWND hWnd,bool m_bIsMouseDown
	,RECT r,int pwidth,int pheight,bool &captureScreen,float multipleW,float multipleH,bool focus,string deviceStatus,bool showBullseye)
{	
	//LOG4CPLUS_INFO(pTestLogger,"进入混合图");	
	RECT rcArea={0,0,pwidth,pheight};
	/*if(captureScreen)
	{
		char szFile[_MAX_PATH];
		SYSTEMTIME stime;
		GetLocalTime(&stime);
		sprintf(szFile,"c:\\pluginfile\\picture\\%04d-%02d-%02d_%02d%02d%02d.bmp",stime.wYear,stime.wMonth,stime.wDay,stime.wHour, stime.wMinute, stime.wSecond);

		MkFilePathDir(szFile);
		WriteBmpFile(szFile, lpRgbData, lwidth, lheight);
	}
	captureScreen=false;*/
	RECT rcImage = rcArea;
	if (m_bAdjustPaint)
	{
		//m_rgbPainter.EraseBkgnd(RGB(128, 128, 128), rcArea);

		//宽度和高度
		long cw = rcArea.right-rcArea.left;//rcArea.Width();
		long ch = rcArea.bottom-rcArea.top;//rcArea.Height();

		// 缩放倍率
		double x_multiple = lwidth  / (cw * 1.0);
		double y_multiple = lheight / (ch * 1.0);
		double v_multiple = (std::max)(x_multiple, y_multiple);

		long cx = (long)(lwidth  / v_multiple);
		long cy = (long)(lheight / v_multiple);

		rcImage.left=(cw - cx) / 2;
		rcImage.top=(ch - cy) / 2;
		rcImage.right=(cw + cx) / 2;
		rcImage.bottom=(ch + cy) / 2;
		//rcImage.SetRect((cw - cx) / 2, (ch - cy) / 2, (cw + cx) / 2, (ch + cy) / 2);
	}
	// 绘制图像
	m_rgbPainter.DrawRgbImage(lpRgbData, lwidth, lheight, rcImage,pwidth,pheight);

	// 绘制检测区域
	SIZE sizeTrack = { lwidth, lheight };	
	HDC hhdc=m_rgbPainter.HandleDC();
	if(m_bIsMouseDown)	  
		DrawTrackAreaReal(hhdc, r, sizeTrack);
	DrawTrackArea(hhdc, rcImage, vecTrackArea, sizeTrack, multipleW, multipleH);

	HDC hDC=GetDC(hWnd);	
	//hDC->BitBlt(rcArea.left, rcArea.top, rcArea.Width(), rcArea.Height(), CDC::FromHandle(m_rgbPainter.HandleDC()), 0, 0, SRCCOPY);
	//BitBlt(hDC, rcArea.left, rcArea.top, rcArea.Width(), rcArea.Height(), CDC::FromHandle(m_rgbPainter.HandleDC()), 0, 0, SRCCOPY);
	::BitBlt(hDC, rcArea.left, rcArea.top, rcArea.right-rcArea.left, rcArea.bottom-rcArea.top, m_rgbPainter.HandleDC(), 0, 0, SRCCOPY);	
	//ReleaseDC(hWnd, hDC);

	//绘制焦点框
	if(focus)
		DrawFocusFrame(hWnd,pwidth,pheight);
	//***********************
	//hDC=GetDC(hWnd);	
	std::wstring wstr;	
	StringToWString(deviceStatus,wstr);	
	LPCTSTR pszText =wstr.c_str();
	::SetTextAlign(hDC, TA_LEFT|TA_BOTTOM);	
	
	int     nOldMode  = SetBkMode(hDC, TRANSPARENT);	
	SetBkMode(hDC, TRANSPARENT);
	::SetTextColor(hDC, RGB(0, 0, 255));	
	
	::TextOut(hDC,4, pheight-4, pszText, lstrlen(pszText));
	::TextOut(hDC,4, pheight-4, pszText, lstrlen(pszText));
	::TextOut(hDC,4, pheight-4, pszText, lstrlen(pszText));
	//this->str="";//正常播放，将播放状态值为空
	//SelectObject(hDC, hOldBrush);
	//SelectObject(hDC, hOldPen);
	//SetBkMode(hDC, nOldMode);	
	//DeleteObject(hOldPen);
	//****************************	

	//********************************显示靶心
	if(showBullseye)
	{
		HPEN m_hPenGreen=::CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
		HGDIOBJ hOldPen   = SelectObject(hDC, (HGDIOBJ)m_hPenGreen);
		//竖线
		//上
		MoveToEx(hDC,pwidth/2,pheight/2-50,NULL);
		LineTo(hDC,pwidth/2,pheight/2-15);

		//下
		MoveToEx(hDC,pwidth/2,pheight/2+15,NULL);
		LineTo(hDC,pwidth/2,pheight/2+50);

		//横线
		MoveToEx(hDC,pwidth/2-50,pheight/2,NULL);
		LineTo(hDC,pwidth/2-15,pheight/2);

		MoveToEx(hDC,pwidth/2+15,pheight/2,NULL);
		LineTo(hDC,pwidth/2+50,pheight/2);
				
	}	
	//********************************显示靶心


	DeleteObject(m_hPenGreen);
	ReleaseDC(hWnd, hDC);	
	//LOG4CPLUS_WARN(pTestLogger,"画完一帧混合图");
}
BOOL CWebCamViewCtrl::StringToWString(const std::string &str,std::wstring &wstr)
{    
	int nLen = (int)str.length();    
	wstr.resize(nLen,L' ');
	int nResult = MultiByteToWideChar(CP_ACP,0,(LPCSTR)str.c_str(),nLen,(LPWSTR)wstr.c_str(),nLen);
	if (nResult == 0)
	{
		return FALSE;
	}
	return TRUE;
}

//void CWebCamViewCtrl::PauseShowImage(LPBYTE lpRgbData, long lwidth, long lheight, const std::vector< RECT > &vecTrackArea,HWND hWnd,bool m_bIsMouseDown
//									 ,RECT r,int pwidth,int pheight,bool &captureScreen,float multipleW,float multipleH,bool focus)
void CWebCamViewCtrl::PauseShowImage(LPBYTE lpRgbData, long lwidth, long lheight, const std::vector< RECT > &vecTrackArea,HWND hWnd,bool m_bIsMouseDown
									 ,RECT r,int pwidth,int pheight,bool &captureScreen,float multipleW,float multipleH,bool focus,char * str)
{		
	

	int i=strcmp(this->str,str);
	if(i)
	{//解决问题：频繁执行下面的代码，导致插件崩溃，有流量没画面
		/*std::wstring ss;

		StringToWString(str,ss);	
		OutputDebugString(ss.c_str());*/
		this->str=str;

		RECT rcArea={0,0,pwidth,pheight};
		/*if(captureScreen)
		{
			char szFile[_MAX_PATH];
			SYSTEMTIME stime;
			GetLocalTime(&stime);
			sprintf(szFile,"c:\\pluginfile\\picture\\%04d-%02d-%02d_%02d%02d%02d.bmp",stime.wYear,stime.wMonth,stime.wDay,stime.wHour, stime.wMinute, stime.wSecond);
			MkFilePathDir(szFile);
			WriteBmpFile(szFile, lpRgbData, lwidth, lheight);
		}
		captureScreen=false;*/
		RECT rcImage = rcArea;
		if (m_bAdjustPaint)
		{
			//m_rgbPainter.EraseBkgnd(RGB(128, 128, 128), rcArea);

			//宽度和高度
			long cw = rcArea.right-rcArea.left;//rcArea.Width();
			long ch = rcArea.bottom-rcArea.top;//rcArea.Height();

			// 缩放倍率
			double x_multiple = lwidth  / (cw * 1.0);
			double y_multiple = lheight / (ch * 1.0);
			double v_multiple = (std::max)(x_multiple, y_multiple);

			long cx = (long)(lwidth  / v_multiple);
			long cy = (long)(lheight / v_multiple);

			rcImage.left=(cw - cx) / 2;
			rcImage.top=(ch - cy) / 2;
			rcImage.right=(cw + cx) / 2;
			rcImage.bottom=(ch + cy) / 2;
			//rcImage.SetRect((cw - cx) / 2, (ch - cy) / 2, (cw + cx) / 2, (ch + cy) / 2);
		}

		// 绘制图像
		//m_rgbPainter.DrawRgbImage(lpRgbData, lwidth, lheight, NULL, rcImage);

		// 绘制检测区域
		SIZE sizeTrack = { lwidth, lheight };
		/*RECT rr={10,10,300,300};	
		std::vector< RECT > vv;
		vv.push_back(rr);
		DrawPausePicture(m_rgbPainter.HandleDC(), rcImage, vv, sizeTrack);*/
		HDC hDC=GetDC(hWnd);	
		//hDC->BitBlt(rcArea.left, rcArea.top, rcArea.Width(), rcArea.Height(), CDC::FromHandle(m_rgbPainter.HandleDC()), 0, 0, SRCCOPY);
		//BitBlt(hDC, rcArea.left, rcArea.top, rcArea.Width(), rcArea.Height(), CDC::FromHandle(m_rgbPainter.HandleDC()), 0, 0, SRCCOPY);
		::BitBlt(hDC, rcArea.left, rcArea.top, rcArea.right-rcArea.left, rcArea.bottom-rcArea.top, m_rgbPainter.HandleDC(), 0, 0, SRCCOPY);	
		//ReleaseDC(hWnd, hDC);

		//HDC hDC =GetDC(hWnd);	//m_rgbPainter.HandleDC()调用，导致插件崩溃

		/**************************************************灰色背景
		int     nOldMode  = SetBkMode(hDC, TRANSPARENT);
		HPEN m_hPenGreen=::CreatePen(PS_SOLID, 2, RGB(0, 0, 128));
		HGDIOBJ hOldPen   = SelectObject(hDC, (HGDIOBJ)m_hPenGreen);
		HGDIOBJ hOldBrush = SelectObject(hDC, GetStockObject(GRAY_BRUSH));		
		Rectangle(hDC, 0, 0, 20000, 20000);
		**************************************************灰色背景*/

		std::wstring wstr;	
		StringToWString(str,wstr);	
		LPCTSTR pszText =wstr.c_str();		
		int     nOldMode  = SetBkMode(hDC, TRANSPARENT);	
		SetBkMode(hDC, TRANSPARENT);
		::SetTextColor(hDC, RGB(0, 0, 0));			
		TextOut(hDC,4, pheight-20, pszText, lstrlen(pszText));	
		/*TextOut(hDC, 2, 2, pszText, lstrlen(pszText));	
		TextOut(hDC, 2, 2, pszText, lstrlen(pszText));	
		TextOut(hDC, 2, 2, pszText, lstrlen(pszText));	
		TextOut(hDC, 2, 2, pszText, lstrlen(pszText));	
		TextOut(hDC, 2, 2, pszText, lstrlen(pszText));	*/

		/*DeleteObject(m_hPenGreen);
		DeleteObject(hOldPen);
		DeleteObject(hOldBrush);*/
		/*SelectObject(hDC, hOldBrush);
		SelectObject(hDC, hOldPen);
		SetBkMode(hDC, nOldMode);*/				
		ReleaseDC(hWnd, hDC);
	}
}

void CWebCamViewCtrl::DrawPausePicture(HDC hDC, const RECT &rcDst, const std::vector< RECT > &vecTrackArea, const SIZE &sizeTrack)
{
	SIZE sizeDst = { rcDst.right - rcDst.left, rcDst.bottom - rcDst.top };

	int     nOldMode  = SetBkMode(hDC, TRANSPARENT);
	HGDIOBJ hOldPen   = SelectObject(hDC, (HGDIOBJ)m_hPenGreen);
	HGDIOBJ hOldBrush = SelectObject(hDC, GetStockObject(NULL_BRUSH));

	std::vector< RECT >::const_iterator itvec = vecTrackArea.begin();
	for (; itvec != vecTrackArea.end(); ++itvec)
	{
		RECT rcArea = *itvec;

		AppScalePoint(*((LPPOINT)&rcArea + 0), sizeTrack, sizeDst);
		AppScalePoint(*((LPPOINT)&rcArea + 1), sizeTrack, sizeDst);
		OffsetRect(&rcArea, rcDst.left, rcDst.top);

		Rectangle(hDC, rcArea.left, rcArea.top, rcArea.right, rcArea.bottom);
	}

	SelectObject(hDC, hOldBrush);
	SelectObject(hDC, hOldPen);
	SetBkMode(hDC, nOldMode);
	
}

void CWebCamViewCtrl::DrawFocusFrame(HWND hWnd,long lwidth, long lheight)
{
	//LOG4CPLUS_WARN(pTestLogger,"begin-----绘制焦点框");
	HDC hDC=::GetDC(hWnd);//m_rgbPainter.HandleDC();
	int     nOldMode  = SetBkMode(hDC, TRANSPARENT);
	HPEN pen=CreatePen(PS_SOLID, 7, RGB(255, 0, 0));
	HGDIOBJ hOldPen   = SelectObject(hDC, (HGDIOBJ)pen);
	HGDIOBJ hOldBrush = SelectObject(hDC, GetStockObject(NULL_BRUSH));	
	Rectangle(hDC, 0, 0, lwidth, lheight);
	SelectObject(hDC, hOldBrush);
	SelectObject(hDC, hOldPen);
	SetBkMode(hDC, nOldMode);
	DeleteObject((HGDIOBJ)pen);	
	::ReleaseDC(hWnd,hDC);
	//LOG4CPLUS_WARN(pTestLogger,"e n d-----绘制焦点框");
}

//=========================================================================

//
// CWebCamViewCtrl inner invoking methods
//

/**************************************************************************
* FunctionName:
*     DrawTrackArea
* Description:
*     绘制各个检测区域框。
* Parameter:
*     @[in ] hDC: 目标绘图 DC。
*     @[in ] rcDst: 目标绘图区。
*     @[in ] vecTrackArea: 各个检测区域集合。
*     @[in ] sizeImage: 检测图像的尺寸。
* ReturnValue:
*     void
*/
void CWebCamViewCtrl::DrawTrackArea(HDC hDC, const RECT &rcDst, const std::vector< RECT > &vecTrackArea, const SIZE &sizeTrack,float multipleW,float multipleH)
{
	//LOG4CPLUS_WARN(pTestLogger,"begin............画已有框");
	SIZE sizeDst = { rcDst.right - rcDst.left, rcDst.bottom - rcDst.top };

	int     nOldMode  = SetBkMode(hDC, TRANSPARENT);
	HGDIOBJ hOldPen   = SelectObject(hDC, (HGDIOBJ)m_hPenGreen);
	HGDIOBJ hOldBrush = SelectObject(hDC, GetStockObject(NULL_BRUSH));
	std::vector< RECT >::const_iterator itvec = vecTrackArea.begin();
	for (; itvec != vecTrackArea.end(); ++itvec)
	{
		RECT rcArea = *itvec;

		/*AppScalePoint(*((LPPOINT)&rcArea + 0), sizeTrack, sizeDst);
		AppScalePoint(*((LPPOINT)&rcArea + 1), sizeTrack, sizeDst);*/
		OffsetRect(&rcArea, rcDst.left*multipleW, rcDst.top*multipleH);
		Rectangle(hDC, rcArea.left*multipleW, rcArea.top*multipleH, rcArea.right*multipleW, rcArea.bottom*multipleH);
	}
	SelectObject(hDC, hOldBrush);
	SelectObject(hDC, hOldPen);
	SetBkMode(hDC, nOldMode);
	//LOG4CPLUS_WARN(pTestLogger,"end............画已有框");
}

/**************************************************************************
* FunctionName:
*     DrawTrackArea
* Description:
*     实时绘制检测区域框。
* Parameter:
*     @[in ] hDC: 目标绘图 DC。
*     @[in ] rcDst: 目标绘图区。
*     @[in ] vecTrackArea: 各个检测区域集合。
*     @[in ] sizeImage: 检测图像的尺寸。
* ReturnValue:
*     void
*/
void CWebCamViewCtrl::DrawTrackAreaReal(HDC hDC, const RECT &rcDst, const SIZE &sizeTrack)
{
	//LOG4CPLUS_WARN(pTestLogger,"begin-------------画框动画");
	SIZE sizeDst = { rcDst.right - rcDst.left, rcDst.bottom - rcDst.top };

	int     nOldMode  = SetBkMode(hDC, TRANSPARENT);
	HGDIOBJ hOldPen   = SelectObject(hDC, (HGDIOBJ)m_hPenGreen);
	HGDIOBJ hOldBrush = SelectObject(hDC, GetStockObject(NULL_BRUSH));
	Rectangle(hDC, rcDst.left, rcDst.top, rcDst.right, rcDst.bottom);
	//LOG4CPLUS_WARN(pTestLogger,"e n d-------------画框动画");
	int jj;
	jj=0;
}

//=========================================================================

//
// CWebCamViewCtrl message handlers
//

/*
BEGIN_MESSAGE_MAP(CWebCamViewCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()
*/
/**************************************************************************
* Description:
*     WM_CREATE message handler.
*/
/*
int CWebCamViewCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (0 != CWnd::OnCreate(lpCreateStruct))
	{
		return -1;
	}

	m_rgbPainter.Initial(ECV_MAX_MEMDC_WIDTH, ECV_MAX_MEMDC_HEIGHT, 24);

	return 0;
}
*/

/**************************************************************************
* Description:
*     WM_DESTROY message handler.
*/
/*
void CWebCamViewCtrl::OnDestroy()
{
	CWnd::OnDestroy();
}
*/

/**************************************************************************
* Description:
*     WM_PAINT message handler.
*/
/*
void CWebCamViewCtrl::OnPaint()
{
	CPaintDC dc(this);

	CRect rcWnd;
	GetClientRect(rcWnd);

	dc.FillSolidRect(rcWnd, RGB(128, 128, 128));
}
*/

/**************************************************************************
* Description:
*     WM_LBUTTONDBLCLK message handler.
*/

/*
void CWebCamViewCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	::PostMessage(GetOwner()->GetSafeHwnd(), WM_COMMAND, (WPARAM)IDC_STC_PLAYWND, NULL);

	CWnd::OnLButtonDblClk(nFlags, point);
}
*/


//void CWebCamViewCtrl::OnPaint()
//{
//	CPaintDC dc(this);
//
//	CRect rcWnd;
//	GetClientRect(rcWnd);
//
//	dc.FillSolidRect(rcWnd, RGB(128, 128, 128));
//}

