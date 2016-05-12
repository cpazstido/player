/**********************************************************\ 
 
 Auto-generated Factory.cpp
 
 This file contains the auto-generated factory methods 
 for the PluginPlayer project
 
\**********************************************************/

#include "FactoryBase.h"
#include "PluginPlayer.h"
#include <boost/make_shared.hpp>

class PluginFactory : public FB::FactoryBase
{
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// @fn FB::PluginCorePtr createPlugin(const std::string& mimetype)
    ///
    /// @brief  Creates a plugin object matching the provided mimetype
    ///         If mimetype is empty, returns the default plugin
    ///////////////////////////////////////////////////////////////////////////////
    FB::PluginCorePtr createPlugin(const std::string& mimetype)
    {
        return boost::make_shared<PluginPlayer>();
    }
    
    ///////////////////////////////////////////////////////////////////////////////
    /// @see FB::FactoryBase::globalPluginInitialize
    ///////////////////////////////////////////////////////////////////////////////
    void globalPluginInitialize()
    {
        PluginPlayer::StaticInitialize();
    }
    
    ///////////////////////////////////////////////////////////////////////////////
    /// @see FB::FactoryBase::globalPluginDeinitialize
    ///////////////////////////////////////////////////////////////////////////////
    void globalPluginDeinitialize()
    {
        PluginPlayer::StaticDeinitialize();
    }

	//添加日志功能
	//void getLoggingMethods( FB::Log::LogMethodList& outMethods )
	//{
	//	// The next line will enable logging to the console (think: printf).
	//	outMethods.push_back(std::make_pair(FB::Log::LogMethod_Console, std::string()));

	//	char szFile[_MAX_PATH];
	//	SYSTEMTIME stime;
	//	GetLocalTime(&stime);
	//	sprintf(szFile,"c:\\pluginfile\\log\\%04d-%02d-%02d_%02d%02d%02d.log",stime.wYear,stime.wMonth,stime.wDay,stime.wHour, stime.wMinute, stime.wSecond);
	//	// The next line will enable logging to a logfile.
	//	outMethods.push_back(std::make_pair(FB::Log::LogMethod_File, szFile));

	//	// Obviously, if you use both lines, you will get output on both sinks.
	//}

	//日志等级
	//FB::Log::LogLevel getLogLevel(){
	//	return FB::Log::LogLevel_Debug; // Now Debug and above is logged.
	//}
};

///////////////////////////////////////////////////////////////////////////////
/// @fn getFactoryInstance()
///
/// @brief  Returns the factory instance for this plugin module
///////////////////////////////////////////////////////////////////////////////
FB::FactoryBasePtr getFactoryInstance()
{
    static boost::shared_ptr<PluginFactory> factory = boost::make_shared<PluginFactory>();
    return factory;
}

