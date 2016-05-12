/**********************************************************\

  Auto-generated PluginPlayerAPI.h

\**********************************************************/
#include <string>
#include <sstream>
#include <boost/weak_ptr.hpp>
#include "JSAPIAuto.h"
#include "BrowserHost.h"
#include "PluginPlayer.h"
#include "MediaPlayer.h"
#include "Base64.h"

#ifndef H_PluginPlayerAPI
#define H_PluginPlayerAPI

class PluginPlayerAPI : public FB::JSAPIAuto
{
public:
    ////////////////////////////////////////////////////////////////////////////
    /// @fn PluginPlayerAPI::PluginPlayerAPI(const PluginPlayerPtr& plugin, const FB::BrowserHostPtr host)
    ///
    /// @brief  Constructor for your JSAPI object.
    ///         You should register your methods, properties, and events
    ///         that should be accessible to Javascript from here.
    ///
    /// @see FB::JSAPIAuto::registerMethod
    /// @see FB::JSAPIAuto::registerProperty
    /// @see FB::JSAPIAuto::registerEvent
    ////////////////////////////////////////////////////////////////////////////
    PluginPlayerAPI(const PluginPlayerPtr& plugin, const FB::BrowserHostPtr& host) :
        m_plugin(plugin), m_host(host), m_player(), m_valid(false),pluginVersion("3.428")
    {			
		LOG4CPLUS_WARN(getPlugin()->m_rtspClient.pTestLogger,pluginVersion.c_str());
		std::ostringstream oStr;
		oStr << "Version:" <<pluginVersion<<endl;
		this->m_host->htmlLog(oStr.str());
		
        registerMethod("echo",      make_method(this, &PluginPlayerAPI::echo));
        registerMethod("testEvent", make_method(this, &PluginPlayerAPI::testEvent));
		registerMethod("play", make_method(this, &PluginPlayerAPI::play));
		registerMethod("stop", make_method(this, &PluginPlayerAPI::stop));
		registerMethod("setSize", make_method(this, &PluginPlayerAPI::setSize));
		registerMethod  ("set_m_szRtspURL", make_method(this, &PluginPlayerAPI::set_m_szRtspURL));//��ȡrtsp��
		registerMethod("returnCoordinate",  make_method(this, &PluginPlayerAPI::returnCoordinate));//���ص�ǰ���������
		registerMethod("returnAllCoordinate",  make_method(this, &PluginPlayerAPI::returnAllCoordinate));//�������п������
		registerMethod("clearCoordinate",  make_method(this, &PluginPlayerAPI::clearCoordinate));//�������
		registerMethod  ("clearAllCoordinate", make_method(this, &PluginPlayerAPI::clearAllCoordinate));//�����������
		registerMethod  ("captureScreen",  make_method(this, &PluginPlayerAPI::captureScreenM));//ץͼ
		registerMethod  ("set_Areas",  make_method(this, &PluginPlayerAPI::set_Areas));//��������
		registerMethod("isAllowDrawFrame",  make_method(this, &PluginPlayerAPI::isAllowDrawFrame));//�Ƿ�������
		registerMethod  ("recordVideo",         make_method  (this, &PluginPlayerAPI::recordVideo));//¼��
		registerMethod  ("setDeviceStatus",         make_method  (this, &PluginPlayerAPI::setDeviceStatus));//�����豸״̬
		registerMethod  ("ShowSelectedFrame",         make_method  (this, &PluginPlayerAPI::ShowSelectedFrame));//�Ƿ���ʾѡ���Ŀ�
		registerMethod  ("ShowBullseye",         make_method  (this, &PluginPlayerAPI::ShowBullseye));//�Ƿ���ʾ����
		registerMethod  ("getPluginVersion",         make_method  (this, &PluginPlayerAPI::getPluginVersion));//���ز���汾��		
		registerMethod  ("setPole",         make_method  (this, &PluginPlayerAPI::setPole));//���ø���
		registerMethod  ("beforeSetURL",         make_method  (this, &PluginPlayerAPI::BeforeSetURL));////�Ƿ�������url֮ǰ��10s		
		registerMethod  ("refreshTips",         make_method  (this, &PluginPlayerAPI::RefreshTips));//ˢ����ʾ��Ϣ����С����ı��������С�ᵼ����ʾ��Ϣ��ʧ��ͨ��ˢ����ʾ��������ʾ
		registerMethod  ("showMessage",         make_method  (this, &PluginPlayerAPI::ShowMessage));//��ʾ��Ϣ

        
        // Read-write property
        registerProperty("testString",make_property(this,&PluginPlayerAPI::get_testString,&PluginPlayerAPI::set_testString));
        
        // Read-only property
        registerProperty("version",make_property(this,&PluginPlayerAPI::get_version));
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @fn PluginPlayerAPI::~PluginPlayerAPI()
    ///
    /// @brief  Destructor.  Remember that this object will not be released until
    ///         the browser is done with it; this will almost definitely be after
    ///         the plugin is released.
    ///////////////////////////////////////////////////////////////////////////////
    virtual ~PluginPlayerAPI() {		
	};

    PluginPlayerPtr getPlugin();
	void setWindow(FB::PluginWindow*);

    // Read/Write property ${PROPERTY.ident}
    std::string get_testString();
    void set_testString(const std::string& val);

    // Read-only property ${PROPERTY.ident}
    std::string get_version();

    // Method echo
    FB::variant echo(const FB::variant& msg);
	FB::variant getPlayerStatus(const FB::variant& msg);
    
    // Event helpers
	FB_JSAPI_EVENT(getPlayerStatus, 3, (const FB::variant&,const FB::variant&,const FB::variant&));
    FB_JSAPI_EVENT(echo, 2, (const FB::variant&, const int));	

    // Method test-event
    void testEvent(const FB::variant& events,const FB::variant& msg);
	

	void play();
	void stop();
	int setSize(int width,int height);
	std::string get_m_szRtspURL();
	void set_m_szRtspURL(const std::string& val);
	string	returnCoordinate();
	vector<string>	returnAllCoordinate();
	void	clearCoordinate();
	void captureScreenM();	
	void clearAllCoordinate();
	void RefreshTips();//ˢ����ʾ��Ϣ
	void ShowMessage(std::string val);//��ʾ��Ϣ
	void set_Areas(FB::VariantList& areas);
	void isAllowDrawFrame(boolean allow);
	void BeforeSetURL(boolean enable);
	void recordVideo(const std::string& val);
	void setDeviceStatus(std::string val);
	BOOL StringToWString(const std::string &str,std::wstring &wstr);
	void ShowSelectedFrame(boolean showFrame);
	void ShowBullseye(boolean showBullseye);
	void setPole(const std::string& pole);
	std::string getPluginVersion();

private:
    PluginPlayerWeakPtr m_plugin;
    FB::BrowserHostPtr m_host;

    std::string m_testString;
	HWND          m_hWnd;
	typedef boost::shared_ptr<MediaPlayer> MediaPlayerPtr;
	MediaPlayerPtr m_player;
	bool m_valid;
	string        m_szRtspURL;
	HANDLE handleStartPlay;
	std::string pluginVersion;	
};

#endif // H_PluginPlayerAPI

