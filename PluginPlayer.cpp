/**********************************************************\

  Auto-generated PluginPlayer.cpp

  This file contains the auto-generated main plugin object
  implementation for the PluginPlayer project

\**********************************************************/

#include "PluginPlayerAPI.h"

#include "PluginWindowWin.h"

#include "PluginPlayer.h"
#include <atlimage.h>
#include "vxIniFile.h"
#include "INIReader.h"
#include <log4cplus/logger.h>
#include "log4cplus/fileappender.h"
#include <log4cplus/loggingmacros.h>

using namespace log4cplus;
using namespace log4cplus::helpers;
//extern __declspec(dllimport) log4cplus::Logger pTestLogger;
//#include <log4cxx/logger.h>
//#include <log4cxx/propertyconfigurator.h>
//using namespace log4cxx;
//using namespace log4cxx::helpers;


///////////////////////////////////////////////////////////////////////////////
/// @fn PluginPlayer::StaticInitialize()
///
/// @brief  Called from PluginFactory::globalPluginInitialize()
///
/// @see FB::FactoryBase::globalPluginInitialize
///////////////////////////////////////////////////////////////////////////////
void PluginPlayer::StaticInitialize()
{
    // Place one-time initialization stuff here; As of FireBreath 1.4 this should only
    // be called once per process
}

///////////////////////////////////////////////////////////////////////////////
/// @fn PluginPlayer::StaticInitialize()
///
/// @brief  Called from PluginFactory::globalPluginDeinitialize()
///
/// @see FB::FactoryBase::globalPluginDeinitialize
///////////////////////////////////////////////////////////////////////////////
void PluginPlayer::StaticDeinitialize()
{
    // Place one-time deinitialization stuff here. As of FireBreath 1.4 this should
    // always be called just before the plugin library is unloaded
}

///////////////////////////////////////////////////////////////////////////////
/// @brief  PluginPlayer constructor.  Note that your API is not available
///         at this point, nor the window.  For best results wait to use
///         the JSAPI object until the onPluginReady method is called
///////////////////////////////////////////////////////////////////////////////
PluginPlayer::PluginPlayer()
:m_player()
,m_window(0)
,isRecordVideo(false)
,isAllowFrame(false)
,m_bIsMouseDown(false)
,m_mousePosXold(0)
,m_mousePosYold(0)
,m_mousePosXnew(0)
,m_mousePosYnew(0)
,bCaptureScreen(false)
,focus(false)
,deviceStatus("")
,drawStr("")
,isShowSelectedFrame("true")
,isShowBullseye(false)
,stopConnect(false)
,pole("")
,playerName("")
,tryNum(0)
,playerTips("")
,beforeSetURL(false)
{	
	hSemaphore = CreateSemaphore(NULL, 1, 1, NULL);
	vxRTSPClient::initial_lib(NULL);	
}

///////////////////////////////////////////////////////////////////////////////
/// @brief  PluginPlayer destructor.
///////////////////////////////////////////////////////////////////////////////
PluginPlayer::~PluginPlayer()
{
    // This is optional, but if you reset m_api (the shared_ptr to your JSAPI
    // root object) and tell the host to free the retained JSAPI objects then
    // unless you are holding another shared_ptr reference to your JSAPI object
    // they will be released here.
	CloseHandle(hSemaphore);
	vxRTSPClient::uninitial_lib();

    releaseRootJSAPI();
    m_host->freeRetainedObjects();
	FBLOG_INFO("info","PluginPlayer::~PluginPlayer()");
}

void PluginPlayer::onPluginReady()
{
    // When this is called, the BrowserHost is attached, the JSAPI object is
    // created, and we are ready to interact with the page and such.  The
    // PluginWindow may or may not have already fire the AttachedEvent at
    // this point.	
	this->m_rtspClient.setCBK(&PluginPlayer::RealFrameCBK, &PluginPlayer::PauseFrameCBK,&PluginPlayer::PlayerStatusCBK,&PluginPlayer::PlayerTipsCBK);
}

void PluginPlayer::shutdown()
{
    // This will be called when it is time for the plugin to shut down;
    // any threads or anything else that may hold a shared_ptr to this
    // object should be released here so that this object can be safely
    // destroyed. This is the last point that shared_from_this and weak_ptr
    // references to this object will be valid
}

///////////////////////////////////////////////////////////////////////////////
/// @brief  Creates an instance of the JSAPI object that provides your main
///         Javascript interface.
///
/// Note that m_host is your BrowserHost and shared_ptr returns a
/// FB::PluginCorePtr, which can be used to provide a
/// boost::weak_ptr<PluginPlayer> for your JSAPI class.
///
/// Be very careful where you hold a shared_ptr to your plugin class from,
/// as it could prevent your plugin class from getting destroyed properly.
///////////////////////////////////////////////////////////////////////////////
FB::JSAPIPtr PluginPlayer::createJSAPI()
{
    // m_host is the BrowserHost
	m_player = PluginPlayerAPIPtr(new PluginPlayerAPI(FB::ptr_cast<PluginPlayer>(shared_from_this()), m_host));
	m_player->setWindow(m_window);	
	FBLOG_INFO("","FB::JSAPIPtr PluginPlayer::createJSAPI()");
    return m_player;
}

bool PluginPlayer::onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow * win)
{	
	//FBLOG_INFO("MouseDown()", "INFO MouseDown...................");
	
	//获取id
	std::string getId= m_params["getId"].convert_cast<std::string>();
	std::string st="javascript:"+getId+"('"+m_params["id"].convert_cast<std::string>()+"')";
	m_host->evaluateJavaScript(st);

	if(!beforeSetURL)
	{

		//保存初始坐标
		std::ostringstream oStr;
		oStr << "widthsss:" << m_window->getWindowWidth()<< ",heightsss: " << m_window->getWindowHeight()<<endl;
		this->m_host->htmlLog(oStr.str());

		if(isAllowFrame)
		{					 //右键画框与360浏览器手势冲突
			if(evt->m_Btn==0)
			{
				int fw=ffmpegWidth;
				int fh=ffmpegHeight;
				if(ffmpegWidth!=0||ffmpegHeight!=0)
				{
					int i=ffmpegWidth+ffmpegHeight;
					/*multipleW=(float)width/ffmpegWidth;
					multipleH=(float)height/ffmpegHeight;*/
				}
				m_mousePosXold = evt->m_x;
				m_mousePosYold = evt->m_y;
				m_mousePosXnew = evt->m_x;
				m_mousePosYnew = evt->m_y;
				ox=evt->m_x;
				oy=evt->m_y;
				nx=evt->m_x;
				ny=evt->m_y;
				m_bIsMouseDown = true;
			}
			//printf("Mouse down at: %d, %d\n", evt->m_x, evt->m_y);
			std::ostringstream oStr;
			oStr << "MouseDown at: (" << evt->m_x/multipleW<< ", " << evt->m_y/multipleH<<")"<<endl;
			oStr << "ffmpeg: (width:" << width<<",height: "<<height<<")"<<endl;		
			this->m_host->htmlLog(oStr.str());
		}
	}
	else
	{
		
	}
	
    return false;
}

bool PluginPlayer::onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow *)
{
	if(!beforeSetURL)
	{
		if(isAllowFrame)
		{
			if(evt->m_Btn==0)
			{
				m_mousePosXnew = evt->m_x;
				m_mousePosYnew = evt->m_y;			
				if(m_bIsMouseDown&&!(m_mousePosXold==m_mousePosXnew&&m_mousePosYold==m_mousePosYnew))
				{
					RECT rect1 = {m_mousePosXold/multipleW,m_mousePosYold/multipleH,m_mousePosXnew/multipleW,m_mousePosYnew/multipleH};
					m_vecRect.push_back(rect1);    
				}
				m_bIsMouseDown = false;
			}
			std::ostringstream oStr;
			oStr << "MouseUp at: (" << evt->m_x/multipleW<< ", " << evt->m_y/multipleH<<")"<<endl;
			this->m_host->htmlLog(oStr.str());	
		}
	}
	
    return false;
}

FB::VariantMap PluginPlayer::getParams()
{
	return m_params;
}

bool PluginPlayer::onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow *)
{
	if(!beforeSetURL)
	{
		if(isAllowFrame)
		{
			if(m_bIsMouseDown)
			{
				nx=evt->m_x;
				ny=evt->m_y;
			}
		}
	}
	
    return false;
}


bool PluginPlayer::onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow *win)
{
	m_window = win;
	m_player->setWindow(win);	
	FB::PluginWindowWin *wnd = dynamic_cast<FB::PluginWindowWin*>(win);
	m_hWnd=wnd->getHWND();	

	/*width=m_params["width"].convert_cast<int>();
	height=m_params["height"].convert_cast<int>();*/
	
	/*std::string allowFrame= m_params["allowframe"].convert_cast<std::string>();

	if(allowFrame=="true")
		isAllowFrame=true;
	else
		isAllowFrame=false;*/
	/*std::ostringstream oStr;
	oStr << "width:" << width<< ",height: " << height<<""<<endl;
	this->m_host->htmlLog(oStr.str());*/	

	/******************************设置窗口背景颜色*****************************/
	HBRUSH brush;
	brush=CreateSolidBrush(RGB(125,125,125));
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,(long)brush); 
	/******************************设置窗口背景颜色*****************************/	
    return false;
}

bool PluginPlayer::onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow *win)
{
    // The window is about to be detached; act appropriately
	ClickedBtnClose();
    return false;
}

bool PluginPlayer::draw(FB::RefreshEvent *evt, FB::PluginWindow* win )
{		
	//HDC hDC;	
	//FB::PluginWindowWin *wnd = dynamic_cast<FB::PluginWindowWin*>(win);
	//HWND hWnd=wnd->getHWND();
	
	//hDC=GetDC(hWnd);
	///*PAINTSTRUCT ps;
	//hDC = BeginPaint(hWnd, &ps);	*/
	//int     nOldMode  = SetBkMode(hDC, TRANSPARENT);
	//HPEN m_hPenGreen=::CreatePen(PS_SOLID, 2, RGB(0, 128, 128));
	//HGDIOBJ hOldPen   = SelectObject(hDC, (HGDIOBJ)m_hPenGreen);
	//HGDIOBJ hOldBrush = SelectObject(hDC, GetStockObject(GRAY_BRUSH));		
	////**********灰色背景*************
	////Rectangle(hDC, 0, 0, 20000, 20000);
	///*SelectObject(hDC, hOldBrush);
	//SelectObject(hDC, hOldPen);
	//SetBkMode(hDC, nOldMode);			*/
	//DeleteObject(m_hPenGreen);
	//
	//
	////********************************************************显示在左下角的文字
	////hDC=GetDC(hWnd);	
	//std::wstring wstr;		
	//StringToWString(deviceStatus,wstr);	
	//LPCTSTR pszText =wstr.c_str();
	//SetBkMode(hDC, TRANSPARENT);
	//::SetTextAlign(hDC, TA_LEFT|TA_BOTTOM);	
	//::SetTextColor(hDC, RGB(0, 0, 0));	
	//::TextOut(hDC,4, height-4, pszText, lstrlen(pszText));
	//::TextOut(hDC,4, height-4, pszText, lstrlen(pszText));
	//::TextOut(hDC,4, height-4, pszText, lstrlen(pszText));
	//::TextOut(hDC,4, height-4, pszText, lstrlen(pszText));
	////********************************************************显示在左下角的文字

	//********************************************************显示在左上角的文字
	if(m_rtspClient.m_xt_connectSuccess!=2)
	{
		/*std::ostringstream oStr;
		oStr << "draw() is invoking!"<<endl;
		this->m_host->htmlLog(oStr.str());*/
		HDC hDC;	
		FB::PluginWindowWin *wnd = dynamic_cast<FB::PluginWindowWin*>(win);
		HWND hWnd=wnd->getHWND();

		hDC=GetDC(hWnd);

		HPEN m_hPenGreen=::CreatePen(PS_SOLID, 2, RGB(128, 128, 128));
		HGDIOBJ hOldPen   = SelectObject(hDC, (HGDIOBJ)m_hPenGreen);
		HGDIOBJ hOldBrush = SelectObject(hDC, GetStockObject(GRAY_BRUSH));		
		Rectangle(hDC, 0, 0, 20000, 20000);

		SelectObject(hDC, hOldBrush);
		SelectObject(hDC, hOldPen);

		DeleteObject(m_hPenGreen);


		std::wstring wstr;
		StringToWString(playerTips,wstr);	
		LPCTSTR pszText1 =wstr.c_str();
		SetBkMode(hDC, TRANSPARENT);	
		::SetTextColor(hDC, RGB(0, 0, 255));	
		::TextOut(hDC,8, 20, pszText1, lstrlen(pszText1));
		/*::TextOut(hDC,8, 20, pszText1, lstrlen(pszText1));
		::TextOut(hDC,8, 20, pszText1, lstrlen(pszText1));
		::TextOut(hDC,8, 20, pszText1, lstrlen(pszText1));		
		::TextOut(hDC,8, 20, pszText1, lstrlen(pszText1));		
		::TextOut(hDC,8, 20, pszText1, lstrlen(pszText1));	*/	
		ReleaseDC(hWnd,hDC);
	}	
	//********************************************************显示在左上角的文字



	////CImage img;
	////img.Load(L"D\:\\1.png");

	////int wid=1000;
	////int hei=800;
	////if(img){
	////	wid = img.GetWidth();
	////	hei = img.GetHeight();
	////}

	////hDC=GetDC(hWnd);
	////int height=win->getWindowHeight();
	////int width=win->getWindowWidth();
	////win->getWindowPosition();
	////HBITMAP hBmp = img.Detach();		
	////HDC hMemDC = CreateCompatibleDC(hDC);	
	////HBITMAP hBmpOld = (HBITMAP)::SelectObject(hMemDC, hBmp);	
	////
	////SetStretchBltMode(hDC,STRETCH_HALFTONE);
	////StretchBlt(hDC,(width-100)/2, (height-100)/2, 100, 100, hMemDC,0,0,wid,hei,SRCCOPY);//缩放
	////SelectObject(hMemDC, hBmpOld);
	////DeleteObject(hBmp);

	////DeleteObject(hMemDC);
	////ReleaseDC(hWnd,hDC);

	//

	return true;
	


}

BOOL PluginPlayer::StringToWString(const std::string &str,std::wstring &wstr)
{    
	LOG4CPLUS_WARN(m_rtspClient.pTestLogger,"StringToWString1");
	int nLen = (int)str.length();    
	wstr.resize(nLen,L' ');
	int nResult = MultiByteToWideChar(CP_ACP,0,(LPCSTR)str.c_str(),nLen,(LPWSTR)wstr.c_str(),nLen);
	if (nResult == 0)
	{
		LOG4CPLUS_WARN(m_rtspClient.pTestLogger,"StringToWString2");
		return FALSE;
	}
	LOG4CPLUS_WARN(m_rtspClient.pTestLogger,"StringToWString22");
	return TRUE;
}
bool PluginPlayer::onchangefocus(FB::FocusChangedEvent *evt, FB::PluginWindow* win )
{
	if(isShowSelectedFrame)
		focus=true;
	/*std::ostringstream oStr;
	oStr << "onchangefocus:"<<focus<<endl;
	this->m_host->htmlLog(oStr.str());	*/

	return true;
}

bool PluginPlayer::onkeydown(FB::KeyDownEvent *evt, FB::PluginWindow* win )
{
	/*std::ostringstream oStr;
	oStr << "keydown"<<evt->m_key_code<<"  "<<evt->m_os_key_code<<endl;
	this->m_host->htmlLog(oStr.str());	*/
	if(evt->m_os_key_code==88)//靶心  k
	{
		if(isShowBullseye==true)
			isShowBullseye=false;
		else
			isShowBullseye=true;
	}

	if(evt->m_os_key_code==70)
	{
		//开启与关闭外破画框功能 f
		if(isAllowFrame)
			isAllowFrame=false;
		else
			isAllowFrame=true;
	}

	//PlayerTipsPro("sssssssssssssss");
	return true;
}

bool PluginPlayer::onfocusout(FB::FocusOutEvent *evt, FB::PluginWindow* win )
{

	if(isShowSelectedFrame)
		focus=false;
	std::ostringstream oStr;
	oStr << "onfocusout"<<endl;
	this->m_host->htmlLog(oStr.str());	

	return true;
}

bool PluginPlayer::ondblclick(FB::MouseDoubleClickEvent *evt, FB::PluginWindow* win )
{		
	std::ostringstream oStr;
	oStr << "dbclick"<<endl;
	this->m_host->htmlLog(oStr.str());
	

	return true;
}

bool PluginPlayer::onresize(FB::ResizedEvent *evt,FB::PluginWindow* win)
{	
	return true;
}

string PluginPlayer::getURL()
{
	LOG4CPLUS_WARN(m_rtspClient.pTestLogger,"getURL1");
	WaitForSingleObject(hSemaphore, INFINITE);
	string temStr=m_szRtspURL;
	ReleaseSemaphore(hSemaphore, 1, NULL);
	LOG4CPLUS_WARN(m_rtspClient.pTestLogger,"getURL2");
	return temStr;
}
void PluginPlayer::setURL(string url)
{
	LOG4CPLUS_WARN(m_rtspClient.pTestLogger,"setURL1");
	WaitForSingleObject(hSemaphore, INFINITE);
	this->m_szRtspURL=url;
	ReleaseSemaphore(hSemaphore, 1, NULL);
	LOG4CPLUS_WARN(m_rtspClient.pTestLogger,"setURL2");
}

void PluginPlayer::RealFrameCBK(int dtype, LPBYTE lpRealFrame, UINT nSize, int nWidth, int nHeight, UINT nUser,char * str)
{	
	PluginPlayer * ptThis = (PluginPlayer *)nUser;
	//LOG4CPLUS_WARN(ptThis->m_rtspClient.pTestLogger,"RealFrameCBK1");
	
	ptThis->RealFrameProc(dtype, lpRealFrame, nSize, nWidth, nHeight,str);	
	ptThis->beforeSetURL=false;
	//LOG4CPLUS_WARN(ptThis->m_rtspClient.pTestLogger,"RealFrameCBK2");
}

void PluginPlayer::PlayerTipsCBK(unsigned int nUser,char* tips)
{
	PluginPlayer * ptThis = (PluginPlayer *)nUser;	
	LOG4CPLUS_WARN(ptThis->m_rtspClient.pTestLogger,"PlayerTipsCBK1");
	
	ptThis->playerTips=tips;
	ptThis->PlayerTipsPro(tips);	
	LOG4CPLUS_WARN(ptThis->m_rtspClient.pTestLogger,"PlayerTipsCBK2");
}

void PluginPlayer::PlayerTipsPro(char* tips)
{
	LOG4CPLUS_WARN(m_rtspClient.pTestLogger,"PlayerTipsPro1");
	HDC hDC;	
	FB::PluginWindowWin *wnd = dynamic_cast<FB::PluginWindowWin*>(m_window);
	HWND hWnd=wnd->getHWND();
	hDC=GetDC(hWnd);

	//int i=1;
	//	
	//while (true)
	//{
	//	HDC hMemDC = CreateCompatibleDC(hDC);
	//
	//	CImage img;
	//	i++;
	//	if(i>13)
	//		i=i%13;
	//	char ssss[200]={0};
	//	//string ssss="c\:\\PlayerPlugin\\progress\\1.png";
	//	sprintf(ssss,"c\:\\PlayerPlugin\\progress\\%d.png",i);
	//	std::wstring wstr;
	//	StringToWString(ssss,wstr);	
	//	LPCTSTR pszText1 =wstr.c_str();
	//	//char str[]="c\:\\PlayerPlugin\\progress\\1.png";
	//	//img.Load(L"c\:\\PlayerPlugin\\progress\\1.png");
	//	//LPCTSTR  lps=(LPCTSTR)(LPTSTR)str;
	//	img.Load(pszText1);

	//	int wid=1000;
	//	int hei=800;
	//	if(img){
	//		wid = img.GetWidth();
	//		hei = img.GetHeight();
	//	}

	//	hDC=GetDC(hWnd);
	//	int height=wnd->getWindowHeight();
	//	int width=wnd->getWindowWidth();
	//	wnd->getWindowPosition();
	//	/*HBITMAP hBmp = img.Detach();		

	//	HBITMAP hBmpOld = (HBITMAP)::SelectObject(hMemDC, hBmp);	

	//	SetStretchBltMode(hDC,STRETCH_HALFTONE);*/
	//	////StretchBlt(hDC,(width-100)/2, (height-100)/2, 100, 100, hMemDC,0,0,wid,hei,SRCCOPY);//缩放
	//	//TransparentBlt(hDC, (width-100)/2, (height-100)/2, 32, 32, hMemDC, 0, 0, 32, 32, RGB(0xff,0xff,0xff));
	//	////hBmpOld.SelectObject(hBmpOld);
	//	//SelectObject(hMemDC, hBmpOld);
	//	//DeleteObject(hBmpOld);
	//	//DeleteObject(hBmp);
	//	//
	//	//DeleteObject(hMemDC);
	//	//ReleaseDC(hWnd,hDC);
	//	Sleep(100);
	//}
	

	



	HPEN m_hPenGreen=::CreatePen(PS_SOLID, 2, RGB(128, 128, 128));
	HGDIOBJ hOldPen   = SelectObject(hDC, (HGDIOBJ)m_hPenGreen);
	HGDIOBJ hOldBrush = SelectObject(hDC, GetStockObject(GRAY_BRUSH));		
	Rectangle(hDC, 0, 0, 20000, 20000);

	SelectObject(hDC, hOldBrush);
	SelectObject(hDC, hOldPen);
	
	DeleteObject(m_hPenGreen);


	std::wstring wstr;
	StringToWString(tips,wstr);	
	LPCTSTR pszText1 =wstr.c_str();	
	SetBkMode(hDC, TRANSPARENT);	
	::SetTextColor(hDC, RGB(0, 0, 255));	
	::TextOut(hDC,8, 20, pszText1, lstrlen(pszText1));
	::TextOut(hDC,8, 20, pszText1, lstrlen(pszText1));
	::TextOut(hDC,8, 20, pszText1, lstrlen(pszText1));
	::TextOut(hDC,8, 20, pszText1, lstrlen(pszText1));		
	ReleaseDC(hWnd,hDC);
	LOG4CPLUS_WARN(m_rtspClient.pTestLogger,"PlayerTipsPro2");
}
void PluginPlayer::PlayerStatusCBK(char* playerName,char* events,char* msg,unsigned int nUser)
{
	/*CRITICAL_SECTION   m_cs_lock;
	InitializeCriticalSection(&m_cs_lock);
	EnterCriticalSection(&m_cs_lock);*/
	PluginPlayer * ptThis = (PluginPlayer *)nUser;
	std::string str=ptThis->playerName;
	ptThis->PlayerStatusProc((char*)str.c_str(),events,msg);	
	/*LeaveCriticalSection(&m_cs_lock);*/
}
void PluginPlayer::PlayerStatusProc(char* playerName,char* events,char* msg)
{
	LOG4CPLUS_WARN(m_rtspClient.pTestLogger,"PlayerStatusProc1");
	std::vector<FB::variant> ve;	
	ve.push_back(events);
	ve.push_back(msg);

	m_player->Invoke("testEvent",ve);	
	LOG4CPLUS_WARN(m_rtspClient.pTestLogger,"PlayerStatusProc2");
}

void PluginPlayer::PauseFrameCBK(int dtype, LPBYTE lpRealFrame, UINT nSize, int nWidth, int nHeight, UINT nUser,char * str)
{
	PluginPlayer * ptThis = (PluginPlayer *)nUser;
	ptThis->PauseFrameProc(dtype, lpRealFrame, nSize, nWidth, nHeight,str);
	ptThis->drawStr=str;	
}

void PluginPlayer::RealFrameProc(int dtype, LPBYTE lpRealFrame, UINT nSize, int nWidth, int nHeight,char * str)
{
	//LOG4CPLUS_WARN(m_rtspClient.pTestLogger,"RealFrameProc1");
	//LOG4CXX_INFO(log4cxx::Logger::getLogger(""),"void PluginPlayer::RealFrameProc");
	if (X_REAL_TYPE_H264 == dtype)
	{		
	}
	else if (X_REAL_TYPE_RGB == dtype)
	{
		//LOG4CPLUS_WARN(pTestLogger,"类型：X_REAL_TYPE_RGB");		
		RECT r={ox,oy,nx,ny};		
		int jjj=ffmpegWidth;		
		int kkk=ffmpegHeight;		
		multipleW=(float)width/ffmpegWidth;//计算放大倍率		
		multipleH=(float)height/ffmpegHeight;//计算放大倍率		
		if(bCaptureScreen)
		{
			char szFile[_MAX_PATH]={0};
			char fileName[_MAX_PATH]={0};
			SYSTEMTIME stime;
			GetLocalTime(&stime);
			sprintf(fileName,"%04d-%02d-%02d_%02d%02d%02d.bmp",stime.wYear,stime.wMonth,stime.wDay,stime.wHour, stime.wMinute, stime.wSecond);	

			//读取配置文件
			INIReader *iniReader=new INIReader();
			wstring path=L"c:\\PlayerPlugin\\configure.ini";//(p);
			wstring value;
			if(iniReader->isFileExist(path))
			{
				iniReader->readFile(L"c:\\PlayerPlugin\\configure.ini");
				iniReader->GetParamValue(L"picture", value);
			}	
			if(value.length()==0)
			{
				sprintf(szFile,"c:\\PlayerPlugin\\picture\\%04d-%02d-%02d_%02d%02d%02d.bmp",stime.wYear,stime.wMonth,stime.wDay,stime.wHour, stime.wMinute, stime.wSecond);	
			}
			else
			{
				std::string st;
				iniReader->WStringToString(value,st);
				char szPath[_MAX_PATH]={0};
				strcat(szPath,st.data());
				strcat(szPath,"\\");
				strcat(szPath,fileName);

				strcpy(szFile,szPath);
			}

			MkFilePathDir(szFile);
			//sprintf(szFile,"c:\\pluginfile\\picture\\%04d-%02d-%02d_%02d%02d%02d.bmp",stime.wYear,stime.wMonth,stime.wDay,stime.wHour, stime.wMinute, stime.wSecond);
			WriteBmpFile(szFile, lpRealFrame,nSize,nWidth, nHeight);
			bCaptureScreen=false;
		}
		//m_wndImageShow.RealShowImage(lpRealFrame, nWidth, nHeight, m_vecRect, m_hWnd,m_bIsMouseDown,r,width,height,bCaptureScreen,multipleW,multipleH,focus,deviceStatus);
		m_wndImageShow.RealShowImage(lpRealFrame, nWidth, nHeight, m_vecRect, m_hWnd,m_bIsMouseDown,r,width,height,bCaptureScreen,multipleW,multipleH,focus,pole,isShowBullseye);
		//LOG4CPLUS_WARN(m_rtspClient.pTestLogger,"e n d=========画图");
	}
	//LOG4CPLUS_WARN(m_rtspClient.pTestLogger,"RealFrameProc2");
}

void PluginPlayer::PauseFrameProc(int dtype, LPBYTE lpRealFrame, UINT nSize, int nWidth, int nHeight,char * str)
//void PluginPlayer::PauseFrameProc(int dtype, LPBYTE lpRealFrame, UINT nSize, int nWidth, int nHeight)
{
	//LOG4CPLUS_WARN(m_rtspClient.pTestLogger,"PauseFrameProc1");
	if (X_REAL_TYPE_H264 == dtype)
	{		
	}
	else if (X_REAL_TYPE_RGB == dtype)
	{
		RECT r={ox,oy,nx,ny};
		int jjj=ffmpegWidth;
		int kkk=ffmpegHeight;
		multipleW=(float)width/ffmpegWidth;//计算放大倍率
		multipleH=(float)height/ffmpegHeight;//计算放大倍率
		
		m_wndImageShow.PauseShowImage(lpRealFrame, nWidth, nHeight, m_vecRect, m_hWnd,m_bIsMouseDown,r,width,height,bCaptureScreen,multipleW,multipleH,focus,str);		
	}
	//LOG4CPLUS_WARN(m_rtspClient.pTestLogger,"PauseFrameProc2");
}

DWORD WINAPI ThreadFuns(LPVOID pM)  
{	
	PluginPlayer * pv_this=(PluginPlayer *)pM;	
	LOG4CPLUS_WARN(pv_this->m_rtspClient.pTestLogger,"ThreadFuns1");
	if(pv_this->m_rtspClient.m_xt_connectSuccess!=0)
	{		
		pv_this->PlayerStatusCBK("","info","blocking",(UINT)pM);
		//pv_this->PlayerTipsPro("connnectting don't return,please wait.");
	}
	else
	{	
		pv_this->PlayerStatusCBK("","info","normal",(UINT)pM);
		//pv_this->PlayerTipsPro("connnectting normal.");
		pv_this->playerName=pv_this->m_player->getPlugin()->getParams()["id"].convert_cast<std::string>();						
		pv_this->m_rtspClient.open((const x_string_t)pv_this->getURL().c_str(),(UINT)pM);								
	}
	LOG4CPLUS_WARN(pv_this->m_rtspClient.pTestLogger,"ThreadFuns2");

	/*pv_this->tryNum=0;
	
	while(true)
		{
			pv_this->tryNum++;
			char connectSuccessFlag[40]={0};				
			if(pv_this->m_rtspClient.m_xt_connectSuccess==1)
			{	
				sprintf(connectSuccessFlag,"info:try to play %d times...",pv_this->tryNum);
				pv_this->PlayerStatusCBK("",connectSuccessFlag,(UINT)pM);
				sprintf(connectSuccessFlag,"try to play %d times...",pv_this->tryNum);
				pv_this->PlayerTipsCBK((UINT)pM,connectSuccessFlag);
				pv_this->PlayerStatusCBK("","error:find stream info didn't return,try again later.",(UINT)pM);
			}
			else if(pv_this->m_rtspClient.m_xt_connectSuccess==2)
			{
				return 0;
			}
			else
			{				
				sprintf(connectSuccessFlag,"info:try to play %d times...",pv_this->tryNum);				
				pv_this->PlayerStatusCBK("",connectSuccessFlag,(UINT)pM);
				sprintf(connectSuccessFlag,"try to play %d times...",pv_this->tryNum);
				pv_this->PlayerTipsCBK((UINT)pM,connectSuccessFlag);
				pv_this->playerName=pv_this->m_player->getPlugin()->getParams()["id"].convert_cast<std::string>();				
				pv_this->m_rtspClient.open((const x_string_t)pv_this->getURL().c_str(), &PluginPlayer::RealFrameCBK, &PluginPlayer::PauseFrameCBK,(UINT)pM ,&PluginPlayer::PlayerStatusCBK,&PluginPlayer::PlayerTipsCBK);								
				LOG4CPLUS_WARN(pv_this->m_rtspClient.pTestLogger,connectSuccessFlag);					
			}
			Sleep(5*1000);
			if(pv_this->tryNum>2)
			{
				pv_this->PlayerTipsCBK((UINT)pM,"Stop playing.");
				return 0;
			}	
		}	*/

	return 0;
}

DWORD WINAPI ThreadFunsTry(LPVOID pM)  
{
	/*PluginPlayer * pv_this=(PluginPlayer *)pM;	
	int i=0;
	while(pv_this->m_rtspClient.m_xt_connectSuccess==1)
	{
		pv_this->m_rtspClient.open((const x_string_t)pv_this->sURL.c_str(), &PluginPlayer::RealFrameCBK, &PluginPlayer::PauseFrameCBK,(UINT)pM,&PluginPlayer::PlayerStatusCBK);	
		if(i>2)
		{
			return 0;
		}
		char connectSuccessFlag[40]={0};		
		i++;
		sprintf(connectSuccessFlag,"连接第%d次...",i);
		LOG4CPLUS_WARN(pv_this->m_rtspClient.pTestLogger,connectSuccessFlag);	
		Sleep(5*1000);		
	}*/
	return 0;
}

void PluginPlayer::ClickedBtnOpen()
{		
	LOG4CPLUS_WARN(m_rtspClient.pTestLogger,"ClickedBtnOpen1");
	char connectSuccessFlag[40]={0};		
	sprintf(connectSuccessFlag,"before play m_xt_connectSuccess=%d",m_rtspClient.m_xt_connectSuccess);
	LOG4CPLUS_WARN(m_rtspClient.pTestLogger,connectSuccessFlag);	
	DWORD  m_hExitCode;
	if(GetExitCodeThread(StartPlay,&m_hExitCode))
	{  
		if (m_hExitCode!=STILL_ACTIVE)//线程已经退出
		{		
			StartPlay = CreateThread(NULL, 0, ThreadFuns, this, 0, NULL);
		}
		else
		{
			TerminateThread(StartPlay,0);
			WaitForSingleObject(StartPlay,INFINITE);
			StartPlay = CreateThread(NULL, 0, ThreadFuns, this, 0, NULL);
		}
	
	}
	else
	{
		StartPlay = CreateThread(NULL, 0, ThreadFuns, this, 0, NULL);
	}
	LOG4CPLUS_WARN(m_rtspClient.pTestLogger,"ClickedBtnOpen2");

	//if(m_rtspClient.m_xt_connectSuccess==0||m_rtspClient.m_xt_connectSuccess==3)//上次播放没停止，或连接没返回不再打开视频，否则导致等待崩溃
	//	StartPlay = CreateThread(NULL, 0, ThreadFuns, this, 0, NULL);	
	//else if(m_rtspClient.m_xt_connectSuccess==1)
	//{
	//	tryPlay = CreateThread(NULL, 0, ThreadFunsTry, this, 0, NULL);	
	//}
	/*Sleep(10);
	FB::RefreshEvent *re=new FB::RefreshEvent();
	draw(re,m_window);*/
}

void PluginPlayer::ClickedBtnClose()
{		
	LOG4CPLUS_WARN(m_rtspClient.pTestLogger,"ClickedBtnClose1");
	DWORD  m_hExitCode;
	if(GetExitCodeThread(StartPlay,&m_hExitCode))
	{  
		if (m_hExitCode==STILL_ACTIVE)//线程已经退出
		{				
			TerminateThread(StartPlay,0);
			WaitForSingleObject(StartPlay,INFINITE);			
		}

	}
	m_vecRect.clear();
	m_rtspClient.close();
	isAllowFrame=false;
	beforeSetURL=false;
	LOG4CPLUS_WARN(m_rtspClient.pTestLogger,"ClickedBtnClose2");
}

void PluginPlayer::ClickedBtnRcdoc()
{
	LOG4CPLUS_WARN(m_rtspClient.pTestLogger,"ClickedBtnRcdoc1");
	if (!m_rtspClient.is_working())
	{
		return;
	}

	if(!m_rtspClient.iRecordVideo)
	{
		m_rtspClient.get_realframe_context();
	}
	else
	{
		m_rtspClient.iRecordVideo=false;
		m_rtspClient.stopRecordVideo();
	}
	LOG4CPLUS_WARN(m_rtspClient.pTestLogger,"ClickedBtnRcdoc2");
}

void PluginPlayer::bg()
{		
	LOG4CPLUS_WARN(m_rtspClient.pTestLogger,"bg1");
	HDC hDC=GetDC(m_hWnd);

	int     nOldMode  = SetBkMode(hDC, TRANSPARENT);
	HPEN m_hPenGreen=::CreatePen(PS_SOLID, 2, RGB(128, 128, 128));
	HGDIOBJ hOldPen   = SelectObject(hDC, (HGDIOBJ)m_hPenGreen);
	HGDIOBJ hOldBrush = SelectObject(hDC, GetStockObject(GRAY_BRUSH));		
	Rectangle(hDC, 0, 0, 20000, 20000);

	SelectObject(hDC, hOldBrush);
	SelectObject(hDC, hOldPen);
	SetBkMode(hDC, nOldMode);
	DeleteObject(m_hPenGreen);

		
	std::wstring wstr;	
	//deviceStatus="连接已断开，1请重新播放...";
	StringToWString(deviceStatus,wstr);	
	LPCTSTR pszText =wstr.c_str();
	SetBkMode(hDC, TRANSPARENT);
	::SetTextColor(hDC, RGB(0, 0, 255));	
	::TextOut(hDC,4, height-19, pszText, lstrlen(pszText));
	ReleaseDC(m_hWnd,hDC);
	LOG4CPLUS_WARN(m_rtspClient.pTestLogger,"bg2");

	//HWND hWnd=m_hWnd;
	////***********************
	//::SetTextAlign(GetDC(hWnd), TA_CENTER|TA_BASELINE);
	//LPCTSTR pszText = _T("四川汇源光通信有限公司");	
	//
	//nOldMode  = SetBkMode(GetDC(hWnd), TRANSPARENT);
	//hOldPen   = SelectObject(GetDC(hWnd), (HGDIOBJ)m_hPenGreen);
	//hOldBrush = SelectObject(GetDC(hWnd), GetStockObject(NULL_PEN));
	//::TextOut(GetDC(hWnd), 2, 2, pszText, lstrlen(pszText));

	//SelectObject(GetDC(hWnd), hOldBrush);
	//SelectObject(GetDC(hWnd), hOldPen);
	//SetBkMode(GetDC(hWnd), nOldMode);
	////****************************
	
}