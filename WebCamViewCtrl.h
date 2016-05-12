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
	*     ʵʱ��ʾͼ�����ݡ�
	* Parameter:
	*     @[in ] lpRgbData: ͼ��� RGB ���ݡ�
	*     @[in ] lwidth: ͼ���ȡ�
	*     @[in ] lheight: ͼ��߶ȡ�
	*     @[in ] vecTrackArea: ͼ��ĸ�������
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
	*     �жϵ�ǰ�ؼ��ǲ���ȫ��״̬��
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
	*     ���Ƹ�����������
	* Parameter:
	*     @[in ] hDC: Ŀ���ͼ DC��
	*     @[in ] rcDst: Ŀ���ͼ����
	*     @[in ] vecTrackArea: ����������򼯺ϡ�
	*     @[in ] sizeImage: ���ͼ��ĳߴ硣
	* ReturnValue:
	*     void
	*/
	void DrawTrackArea(HDC hDC, const RECT &rcDst, const std::vector< RECT > &vecTrackArea, const SIZE &sizeTrack,float multipleW,float multipleH);
	void DrawPausePicture(HDC hDC, const RECT &rcDst, const std::vector< RECT > &vecTrackArea, const SIZE &sizeTrack);

	/**************************************************************************
	* FunctionName:
	*     DrawTrackArea
	* Description:
	*     ���Ƹ�����������
	* Parameter:
	*     @[in ] hDC: Ŀ���ͼ DC��
	*     @[in ] rcDst: Ŀ���ͼ����
	*     @[in ] vecTrackArea: ����������򼯺ϡ�
	*     @[in ] sizeImage: ���ͼ��ĳߴ硣
	* ReturnValue:
	*     void
	*/
	void DrawTrackAreaReal(HDC hDC, const RECT &rcDst, const SIZE &sizeTrack);
	

	// class data
protected:
	vxRgbPainter           m_rgbPainter;      ///< RGB ���ݻ�ͼ��������
	bool                   m_bAdjustPaint;    ///< ��ʶ�Ƿ�����Ӧͼ��ߴ���ʾͼ��

	HPEN                   m_hPenGreen;       ///< ��ɫ����

	// message map handlers
protected:
	
	/*afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()*/
	 //void OnPaint();
	
};

///////////////////////////////////////////////////////////////////////////


