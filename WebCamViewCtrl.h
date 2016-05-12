#pragma once

#include "vxDC.h"
#include <vector>
//#include "vxIniFile.h"
#include "gdiplus.h"
#include "logging.h"
using std::vector;
using namespace std;

//#include <log4cxx/logger.h>
//#include <log4cxx/helpers/pool.h>
//#include <log4cxx/basicconfigurator.h>
//#include <log4cxx/fileappender.h>
//#include <log4cxx/simplelayout.h>
//#include <log4cxx/patternlayout.h>
//using namespace log4cxx;
//using namespace log4cxx::helpers;


//using namespace std;

///////////////////////////////////////////////////////////////////////////
// CWebCamViewCtrl
void WriteBmpFile(char * szFile, LPBYTE pRgb24Data,int nSize, int nWidth, int nHeight);
class CWebCamViewCtrl //: public CWnd
{
	//DECLARE_DYNAMIC(CWebCamViewCtrl)

	// constructor/destructor
public:
	CWebCamViewCtrl(void);
	virtual ~CWebCamViewCtrl(void);

	// internal data types
public:
	typedef enum ConstValueID
	{
		ECV_MAX_MEMDC_WIDTH    = 2000,
		ECV_MAX_MEMDC_HEIGHT   = 1200,
	} ConstValueID;

	// overrides
public:
	
	virtual void PreSubclassWindow();
	virtual bool PreTranslateMessage(MSG* pMsg);
	BOOL StringToWString(const std::string &str,std::wstring &wstr);
	char * str;

	// public interfaces
public:
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
	void RealShowImage(unsigned char * lpRgbData, long lwidth, long lheight
	    , const std::vector< RECT > &vecTrackArea, HWND hWnd,bool m_bIsMouseDown
		,RECT r,int pwidth,int pheight,bool &captureScreen,float multipleW,float multipleH,bool focus,string deviceStatus,bool showBullseye);
	//void PauseShowImage(LPBYTE lpRgbData, long lwidth, long lheight, const std::vector< RECT > &vecTrackArea,HWND hWnd,bool m_bIsMouseDown
	//	,RECT r,int pwidth,int pheight,bool &captureScreen,float multipleW,float multipleH,bool focus);
	void PauseShowImage(LPBYTE lpRgbData, long lwidth, long lheight, const std::vector< RECT > &vecTrackArea,HWND hWnd,bool m_bIsMouseDown
		,RECT r,int pwidth,int pheight,bool &captureScreen,float multipleW,float multipleH,bool focus,char * str);
	void DrawFocusFrame(HWND hWnd,long lwidth, long lheight);
	/**************************************************************************
	* FunctionName:
	*     IsFullScreen
	* Description:
	*     判断当前控件是不是全屏状态。
	*/
	/*
	inline bool IsFullScreen(void) {
		//return (GetParent() != GetOwner());
		return false;
	}
	*/

	// inner invoking methods
protected:
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
	void DrawTrackArea(HDC hDC, const RECT &rcDst, const std::vector< RECT > &vecTrackArea, const SIZE &sizeTrack,float multipleW,float multipleH);
	void DrawPausePicture(HDC hDC, const RECT &rcDst, const std::vector< RECT > &vecTrackArea, const SIZE &sizeTrack);

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
	void DrawTrackAreaReal(HDC hDC, const RECT &rcDst, const SIZE &sizeTrack);
	

	// class data
protected:
	vxRgbPainter           m_rgbPainter;      ///< RGB 数据绘图操作对象
	bool                   m_bAdjustPaint;    ///< 标识是否自适应图像尺寸显示图像

	HPEN                   m_hPenGreen;       ///< 绿色画笔

	// message map handlers
protected:
	
	/*afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()*/
	 //void OnPaint();
	
};

///////////////////////////////////////////////////////////////////////////


