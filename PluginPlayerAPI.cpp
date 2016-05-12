/**********************************************************\

  Auto-generated PluginPlayerAPI.cpp

\**********************************************************/

#include "JSObject.h"
#include "variant_list.h"
#include "DOM/Document.h"
#include "global/config.h"
#include "PluginWindowWin.h"
#include "PluginPlayerAPI.h"
#include "PluginPlayer.h"
#include <log4cplus/loggingmacros.h>
//extern __declspec(dllimport) log4cplus::Logger pTestLogger;

///////////////////////////////////////////////////////////////////////////////
/// @fn FB::variant PluginPlayerAPI::echo(const FB::variant& msg)
///
/// @brief  Echos whatever is passed from Javascript.
///         Go ahead and change it. See what happens!
///////////////////////////////////////////////////////////////////////////////
FB::variant PluginPlayerAPI::echo(const FB::variant& msg)
{
    static int n(0);
    fire_echo("So far, you clicked this many times: ", n++);

    // return "foobar";
    return msg;
}

FB::variant PluginPlayerAPI::getPlayerStatus(const FB::variant& msg)
{
	//fire_test(msg);
	return msg;
}

///////////////////////////////////////////////////////////////////////////////
/// @fn PluginPlayerPtr PluginPlayerAPI::getPlugin()
///
/// @brief  Gets a reference to the plugin that was passed in when the object
///         was created.  If the plugin has already been released then this
///         will throw a FB::script_error that will be translated into a
///         javascript exception in the page.
///////////////////////////////////////////////////////////////////////////////
PluginPlayerPtr PluginPlayerAPI::getPlugin()
{	
    PluginPlayerPtr plugin(m_plugin.lock());
    if (!plugin) {
        throw FB::script_error("The plugin is invalid");
    }
    return plugin;
}

// Read/Write property testString
std::string PluginPlayerAPI::get_testString()
{
    return m_testString;
}

void PluginPlayerAPI::set_testString(const std::string& val)
{
    m_testString = val;
}

// Read-only property version
std::string PluginPlayerAPI::get_version()
{
    return FBSTRING_PLUGIN_VERSION;
}

void PluginPlayerAPI::testEvent(const FB::variant& events,const FB::variant& msg)
{
    fire_getPlayerStatus(getPlugin()->playerName,events,msg);	
}
DWORD WINAPI ThreadFun(LPVOID pM)  
{
	PluginPlayerAPI * pv_this=(PluginPlayerAPI *)pM;
	pv_this->getPlugin()->ClickedBtnOpen();
	return 0;
}
void PluginPlayerAPI::play()
{
	
	LOG4CPLUS_WARN(getPlugin()->m_rtspClient.pTestLogger,m_szRtspURL.c_str());
	std::ostringstream oStr;
	/*oStr << "width/height: (" <<getPlugin()->width<<","<<getPlugin()->height<<")"<<endl;
	this->m_host->htmlLog(oStr.str());*/	

	
	getPlugin()->ClickedBtnOpen();
	
	//handleStartPlay = CreateThread(NULL, 0, ThreadFun, this, 0, NULL);
	//getPlugin()->ClickedBtnClose();	
	
}

void PluginPlayerAPI::stop()
{
	/*std::ostringstream oStr;
	oStr << "stop function was invoked"<<endl;
	this->m_host->htmlLog(oStr.str());	*/	
	getPlugin()->bg(); 
	getPlugin()->ClickedBtnClose();		
	
}

void PluginPlayerAPI::setWindow(FB::PluginWindow* win)
{

	if(win){
		FB::PluginWindowWin *wnd = dynamic_cast<FB::PluginWindowWin*>(win);

		m_hWnd=wnd->getHWND();		
	}

	//m_player->setWindow(win);
}

int PluginPlayerAPI::setSize(int swidth,int sheight)
{
	getPlugin()->width=swidth;
	getPlugin()->height=sheight;
	/*std::ostringstream oStr;
	oStr << "width/height: (" <<getPlugin()->width<<","<<getPlugin()->height<<")"<<endl;
	this->m_host->htmlLog(oStr.str());	*/
	return swidth+sheight;
}

std::string PluginPlayerAPI::get_m_szRtspURL()
{
	return m_szRtspURL;
}

std::string PluginPlayerAPI::getPluginVersion()
{
	return pluginVersion;
}

void PluginPlayerAPI::set_m_szRtspURL(const std::string& val)
{
	m_szRtspURL = val;
	getPlugin()->setURL(val);
	/*std::ostringstream oStr;
	oStr << "url: (" <<m_szRtspURL<<")"<<endl;
	this->m_host->htmlLog(oStr.str());	*/
	
}

//************************************
// 方法:    returnCoordinate
// 全名:  PluginPlayerAPI::returnCoordinate
// 访问类型:    public 
// 返回值:   std::string
// 描述: 返回当前画的框的坐标（左上角和右下角）
//************************************
string	PluginPlayerAPI::returnCoordinate()
{							
	string position;
	char ss[10];

	sprintf(ss, "%d",int(getPlugin()->m_mousePosXold/getPlugin()->multipleW));	
	position+=ss;
	position+=",";
	sprintf(ss, "%d",int(getPlugin()->m_mousePosYold/getPlugin()->multipleH));
	position+=ss;
	position+=",";
	sprintf(ss, "%d",int(getPlugin()->m_mousePosXnew/getPlugin()->multipleW));
	position+=ss;
	position+=",";
	sprintf(ss, "%d",int(getPlugin()->m_mousePosYnew/getPlugin()->multipleH));
	position+=ss;	
	std::ostringstream oStr;
	oStr << "position: (" <<getPlugin()->m_mousePosXold<<","<<getPlugin()->multipleW<<","<<getPlugin()->m_mousePosXold/getPlugin()->multipleW<<")"<<endl;
	
	this->m_host->htmlLog(oStr.str());

	return position;
}

//************************************
// 方法:    clearCoordinate
// 全名:  PluginPlayerAPI::clearCoordinate
// 访问类型:    public 
// 返回值:   void
// 描述:  清除在当前鼠标点击的范围内的框
//************************************
void	PluginPlayerAPI::clearCoordinate()
{				  
	
	
	vector<RECT>::iterator itr = getPlugin()->m_vecRect.begin();

	for(; itr!= getPlugin()->m_vecRect.end();)  
	{  
		//if (itr->bottom == m_mousePosYnew && itr->left == m_mousePosXold && itr->right == m_mousePosXnew && itr->top == m_mousePosYold)  
		if(itr->left<=(getPlugin()->m_mousePosXnew/getPlugin()->multipleW) && (getPlugin()->m_mousePosXnew/getPlugin()->multipleW)<=itr->right 
			&& itr->top<=(getPlugin()->m_mousePosYnew/getPlugin()->multipleH) && (getPlugin()->m_mousePosYnew/getPlugin()->multipleH)<=itr->bottom)
		{  
			itr = getPlugin()->m_vecRect.erase(itr);  
		}  
		else  
		{  
			++itr;  
		}  

	}  

}

//************************************
// 方法:    returnAllCoordinate
// 全名:  PluginPlayerAPI::returnAllCoordinate
// 访问类型:    public 
// 返回值:   FB::VariantList
// 描述: 返回所有框的坐标
//************************************
vector<string>	 PluginPlayerAPI::returnAllCoordinate()
{
	vector<string> rectList;
	vector<RECT>::iterator itr1 = getPlugin()->m_vecRect.begin();
	for(;itr1!=getPlugin()->m_vecRect.end();)
	{
		string position;
		char ss[10];

		sprintf(ss, "%d",itr1->left);	
		position+=ss;
		position+=",";
		sprintf(ss, "%d",itr1->top);
		position+=ss;
		position+=",";
		sprintf(ss, "%d",itr1->right);
		position+=ss;
		position+=",";
		sprintf(ss, "%d",itr1->bottom);
		position+=ss;
		rectList.push_back(position);
		++itr1;
	}
	return rectList;
}


//************************************
// 方法:    captureScreen
// 全名:  captureScreen
// 访问类型:    public 
// 返回值:   void
// 描述:  抓图
//************************************
void PluginPlayerAPI::captureScreenM()
{
	getPlugin()->bCaptureScreen=true;
}

//字符串分割函数
std::vector<std::string> split(std::string str,std::string pattern)
{
	std::string::size_type pos;
	std::vector<std::string> result;
	str+=pattern;//扩展字符串以方便操作
	int size=str.size();

	for(int i=0; i<size; i++)
	{
		pos=str.find(pattern,i);
		if(pos < size)
		{
			std::string s=str.substr(i,pos-i);
			result.push_back(s);
			i=pos+pattern.size()-1;
		}
	}
	return result;
}

//************************************
// Method:    set_Areas
// FullName:  set_Areas
// Access:    public 
// Returns:   void
// Qualifier:画出上传的框
// Parameter: const FB::VariantList
//************************************
void PluginPlayerAPI::set_Areas(FB::VariantList& areass)
{
	for(int i=0;i < areass.size();i++)
	{
		std::string area=areass[i].convert_cast<std::string>();
		
		//split(area,",")[0];vector<RECT>::iterator itr1 = m_vecRect.begin();
		int rect[4];
		for(int j=0;j<4;j++)
		{
			rect[j]=atoi(split(area,",")[j].data());
		}
		std::ostringstream oStr;
		oStr << "area: (" <<rect[0]<<","<<rect[1]<<","<<rect[2]<<","<<rect[3]<<")"<<endl;

		this->m_host->htmlLog(oStr.str());
		RECT rec={rect[0],rect[1],rect[2],rect[3]};	
		getPlugin()->m_vecRect.push_back(rec);
	}
		
}

//************************************
// Method:    clearAllCoordinate
// FullName:  clearAllCoordinate
// Access:    public 
// Returns:   void
// Qualifier:清除所有的框
//************************************
void PluginPlayerAPI::clearAllCoordinate()
{
	getPlugin()->m_vecRect.clear();//除最后一个的其他框
	getPlugin()->ox=0;//最后一个框
	getPlugin()->oy=0;
	getPlugin()->nx=0;
	getPlugin()->ny=0;
}

void PluginPlayerAPI::RefreshTips()
{
	getPlugin()->PlayerTipsPro((char *)getPlugin()->playerTips.c_str());
}

void PluginPlayerAPI::ShowMessage(std::string val)
{
	LOG4CPLUS_WARN(getPlugin()->m_rtspClient.pTestLogger,"ShowMessage1");
	std::string strGB2312DbName = CBase64::Decode(val);//解码
	std::string strDnName = CBase64::UTF8ToGB(strGB2312DbName.c_str());//utf8->GB2312
	getPlugin()->PlayerTipsPro((char*)strDnName.c_str());
	LOG4CPLUS_WARN(getPlugin()->m_rtspClient.pTestLogger,"ShowMessage2");
}

void PluginPlayerAPI::isAllowDrawFrame(boolean allow)
{
	/*getPlugin()->isAllowFrame=allow;*/
	getPlugin()->focus=allow;
}

void PluginPlayerAPI::BeforeSetURL(boolean enable)
{
	LOG4CPLUS_WARN(getPlugin()->m_rtspClient.pTestLogger,"BeforeSetURL1");
	if(enable)
	{
		getPlugin()->beforeSetURL=true;
		getPlugin()->PlayerTipsPro("getting URL,please wait.");		
	}
	else
	{
		getPlugin()->beforeSetURL=false;
	}
	LOG4CPLUS_WARN(getPlugin()->m_rtspClient.pTestLogger,"BeforeSetURL2");
}

void PluginPlayerAPI::ShowSelectedFrame(boolean showFrame)
{
	getPlugin()->isShowSelectedFrame=showFrame;
}

void PluginPlayerAPI::ShowBullseye(boolean showBullseye)
{
	getPlugin()->isShowBullseye=showBullseye;
}

void PluginPlayerAPI::recordVideo(const std::string& val)
{
	if(val=="true")
		getPlugin()->isRecordVideo=true;
	else
		getPlugin()->isRecordVideo=false;
	getPlugin()->ClickedBtnRcdoc();
}

void PluginPlayerAPI::setDeviceStatus(std::string val)
{	
	LOG4CPLUS_WARN(getPlugin()->m_rtspClient.pTestLogger,"setDeviceStatus1");
	std::wstring ws;
	StringToWString(val,ws);
	OutputDebugString(ws.c_str());

	std::string strGB2312DbName = CBase64::Decode(val);//解码
	std::string strDnName = CBase64::UTF8ToGB(strGB2312DbName.c_str());//utf8->GB2312
	getPlugin()->deviceStatus=strDnName;
	getPlugin()->bg();
	LOG4CPLUS_WARN(getPlugin()->m_rtspClient.pTestLogger,"setDeviceStatus2");
}

void PluginPlayerAPI::setPole(const std::string& pole)
{	
	std::string strGB2312DbName = CBase64::Decode(pole);//解码
	std::string strDnName = CBase64::UTF8ToGB(strGB2312DbName.c_str());//utf8->GB2312
	getPlugin()->pole=strDnName.c_str();
}

BOOL PluginPlayerAPI::StringToWString(const std::string &str,std::wstring &wstr)
{    
	LOG4CPLUS_WARN(getPlugin()->m_rtspClient.pTestLogger,"StringToWString1");
	int nLen = (int)str.length();    
	wstr.resize(nLen,L' ');
	int nResult = MultiByteToWideChar(CP_ACP,0,(LPCSTR)str.c_str(),nLen,(LPWSTR)wstr.c_str(),nLen);
	if (nResult == 0)
	{
		return FALSE;
	}
	LOG4CPLUS_WARN(getPlugin()->m_rtspClient.pTestLogger,"StringToWString2");
	return TRUE;
}