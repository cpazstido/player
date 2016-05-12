/**********************************************************\

  Auto-generated PluginPlayer.h

  This file contains the auto-generated main plugin object
  implementation for the PluginPlayer project

\**********************************************************/
#ifndef H_PluginPlayerPLUGIN
#define H_PluginPlayerPLUGIN

#include "E:\OnlineMonitor-new\PlayerPlugin\ffmpegcodec\includeHeader\vxRTSPClient.h"
#include "WebCamViewCtrl.h"
#include "vxCommon.h"

#include <string>
#include <sstream>
#include "PluginWindow.h"
#include "PluginEvents/MouseEvents.h"
#include "PluginEvents/AttachedEvent.h"
#include "PluginEvents/DrawingEvents.h"
#include "PluginEvents/KeyboardEvents.h"

#include "PluginCore.h"
using namespace std;
class PluginPlayerAPI;


//#include <log4cxx/logger.h>
//#include <log4cxx/helpers/pool.h>
//#include <log4cxx/basicconfigurator.h>
//#include <log4cxx/fileappender.h>
//#include <log4cxx/simplelayout.h>
//#include <log4cxx/patternlayout.h>
//using namespace log4cxx;
//using namespace log4cxx::helpers;

extern __declspec(dllimport) int ffmpegWidth;
extern __declspec(dllimport) int ffmpegHeight;
FB_FORWARD_PTR(PluginPlayer)
class PluginPlayer : public FB::PluginCore
{
public:
    static void StaticInitialize();
    static void StaticDeinitialize();
	BOOL StringToWString(const std::string &str,std::wstring &wstr);

public:
    PluginPlayer();
    virtual ~PluginPlayer();

public:
    void onPluginReady();
    void shutdown();
    virtual FB::JSAPIPtr createJSAPI();
    // If you want your plugin to always be windowless, set this to true
    // If you want your plugin to be optionally windowless based on the
    // value of the "windowless" param tag, remove this method or return
    // FB::PluginCore::isWindowless()
    virtual bool isWindowless() { return false; }

    BEGIN_PLUGIN_EVENT_MAP()
        EVENTTYPE_CASE(FB::MouseDownEvent, onMouseDown, FB::PluginWindow)
        EVENTTYPE_CASE(FB::MouseUpEvent, onMouseUp, FB::PluginWindow)
        EVENTTYPE_CASE(FB::MouseMoveEvent, onMouseMove, FB::PluginWindow)
        EVENTTYPE_CASE(FB::MouseMoveEvent, onMouseMove, FB::PluginWindow)
        EVENTTYPE_CASE(FB::AttachedEvent, onWindowAttached, FB::PluginWindow)
        EVENTTYPE_CASE(FB::DetachedEvent, onWindowDetached, FB::PluginWindow)
		EVENTTYPE_CASE(FB::RefreshEvent, draw, FB::PluginWindow)
		EVENTTYPE_CASE(FB::FocusChangedEvent, onchangefocus, FB::PluginWindow)
		EVENTTYPE_CASE(FB::KeyDownEvent, onkeydown, FB::PluginWindow)
		EVENTTYPE_CASE(FB::FocusOutEvent, onfocusout, FB::PluginWindow)		
		EVENTTYPE_CASE(FB::MouseDoubleClickEvent, ondblclick, FB::PluginWindow)		
		EVENTTYPE_CASE(FB::ResizedEvent, onresize, FB::PluginWindow)
    END_PLUGIN_EVENT_MAP()

    /** BEGIN EVENTDEF -- DON'T CHANGE THIS LINE **/
    virtual bool onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow *);
    virtual bool onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow *);
    virtual bool onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow *);
    virtual bool onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow *);
    virtual bool onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow *);
	virtual bool draw(FB::RefreshEvent *evt, FB::PluginWindow *);
	virtual bool onchangefocus(FB::FocusChangedEvent *evt, FB::PluginWindow *);
	virtual bool onkeydown(FB::KeyDownEvent *evt, FB::PluginWindow *);
	virtual bool onfocusout(FB::FocusOutEvent *evt, FB::PluginWindow *);
	virtual bool ondblclick(FB::MouseDoubleClickEvent *evt, FB::PluginWindow *);	
	virtual bool onresize(FB::ResizedEvent *evt,FB::PluginWindow* win);
    /** END EVENTDEF -- DON'T CHANGE THIS LINE **/

public:
	static void RealFrameCBK(int dtype, unsigned char * lpRealFrame, UINT nSize, int nWidth, int nHeight, UINT nUser,char * str);
	static void PlayerStatusCBK(char* playerName,char* events,char* msg,unsigned int nUser);
	static void PlayerTipsCBK(unsigned int nUser,char* tips);
	void PlayerTipsPro(char* tips);
	void PlayerStatusProc(char* playerName,char* events,char* msg);
	
	//�����źŲ���ʱ��ʾ����ͣ֡
	static void PauseFrameCBK(int dtype, LPBYTE lpRealFrame, UINT nSize, int nWidth, int nHeight, UINT nUser,char * str);
	//static void PauseFrameCBK(int dtype, LPBYTE lpRealFrame, UINT nSize, int nWidth, int nHeight, UINT nUser);
	void RealFrameProc(int dtype, unsigned char * lpRealFrame, UINT nSize, int nWidth, int nHeight,char * str);
	//��ͣ֡
	void PauseFrameProc(int dtype, LPBYTE lpRealFrame, UINT nSize, int nWidth, int nHeight,char * str);
	//void PauseFrameProc(int dtype, LPBYTE lpRealFrame, UINT nSize, int nWidth, int nHeight);
	void ClickedBtnOpen();
	void ClickedBtnClose();
	void ClickedBtnRcdoc();
	void bg();
	string getURL();
	void setURL(string url);
	FB::VariantMap getParams();

	std::string			playerTips;
	std::string			playerName;
	std::string        m_szRtspURL;
	vxRTSPClient       m_rtspClient;
	CWebCamViewCtrl    m_wndImageShow;
	//bool				enableLeftButton;//�ڻ�ȡ��ַ�����õ�ַ��10s�ڼ�������
	bool				beforeSetURL;//�Ƿ�������url֮ǰ��10s
	//vxSimpleLock       m_lockRcd;
	//CFile              m_rcdFile;
	std::vector< RECT > m_vecRect;//�ͻ��˻���������������б�
	//fstream              m_rcdFile;
	bool isRecordVideo;//=false;
	HWND          m_hWnd;
	typedef boost::shared_ptr<PluginPlayerAPI> PluginPlayerAPIPtr;
	PluginPlayerAPIPtr m_player;
	FB::PluginWindow* m_window;
	bool isAllowFrame;//=true;//�Ƿ�������
	bool m_bIsMouseDown;//����Ƿ���
	int m_mousePosXold,m_mousePosYold;//���滭�������
	int m_mousePosXnew,m_mousePosYnew;
	int ox,oy;//��ʵʱ�������
	int nx,ny;
	int width,height;//���ڵĿ�Ⱥ͸߶�
	float multipleW,multipleH;//�Ŵ���С����
	bool bCaptureScreen;//=false;	
	std::string sURL;
	bool focus;//�Ƿ�۽�
	bool isShowSelectedFrame;//�Ƿ���ʾѡ���Ŀ�
	bool isShowBullseye;//�Ƿ���ʾ����
	string deviceStatus;//�豸״̬
	string drawStr;//���»���ͼƬʱ����ʾ�����Ͻǵ�����
	bool stopConnect;
	string pole;//����
	HANDLE StartPlay;//��ʼ�����߳�
	HANDLE tryPlay;//���������߳�
	HANDLE hSemaphore;
	int tryNum;//���Բ��Ŵ���
};



#endif

