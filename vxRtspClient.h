#ifndef __VXRTSPCLIENT_H__
#define __VXRTSPCLIENT_H__

#include <string>
#include "logging.h"
#include <log4cplus/loggingmacros.h>
#include <log4cplus/logger.h>   
#include <log4cplus/fileappender.h>   
#include <log4cplus/consoleappender.h>   
#include <log4cplus/layout.h>  
using namespace log4cplus;   
using namespace log4cplus::helpers;   
//#include "SDL.h"
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#ifdef __cplusplus
};
#endif // __cplusplus

#include "vxRTSPClient.inl"
#include "utils.h"
#ifndef	__vxRTSPMemory_h__
#include "vxRTSPMemory.inl"
#endif

///////////////////////////////////////////////////////////////////////////
#define RTSPCLIENT_EXPORTS
#ifdef RTSPCLIENT_EXPORTS
#define RTSPCLIENT_API __declspec(dllexport)
#else
#define RTSPCLIENT_API __declspec(dllimport)
#endif

///////////////////////////////////////////////////////////////////////////

#define X_FALSE           0
#define X_TRUE            1
#define X_NULL            0
#define X_ERR_OK          0
#define X_ERR_UNKNOW      (-1)

#define X_REAL_TYPE_H264  1000
#define X_REAL_TYPE_RGB   1001

typedef char              x_int8_t;
typedef unsigned char     x_uint8_t;
typedef short             x_int16_t;
typedef unsigned short    x_uint16_t;
typedef int               x_int32_t;
typedef unsigned int      x_uint32_t;

typedef unsigned int      x_bool_t;
typedef void *            x_handle_t;

typedef char *            x_string_t;

typedef void (* x_realcbk_func)(x_int32_t xt_dtype, x_uint8_t * xt_buf, x_uint32_t xt_size, x_int32_t xt_width, x_int32_t xt_height, x_uint32_t xt_user,char * str);
typedef void (* x_pausecbk_func)(x_int32_t xt_dtype, x_uint8_t * xt_buf, x_uint32_t xt_size, x_int32_t xt_width, x_int32_t xt_height, x_uint32_t xt_user,char * str);
typedef void (* x_playerStatuscbk_func)(char* playerName,char* events,char* msg,unsigned intxt_user);
typedef void (* x_playerTipscbk_func)(unsigned intxt_user,char * tips);
///////////////////////////////////////////////////////////////////////////
// vxRTSPClient

class RTSPCLIENT_API vxRTSPClient
{
	// constructor/destructor
public:
	vxRTSPClient(void);
	virtual ~vxRTSPClient(void);

public:
	void test();
	/**************************************************************************
	* FunctionName:
	*     initial_lib
	* Description:
	*     库初始化操作。
	* Parameter:
	*     @[in ] xt_pv_param: 预留参数。
	* ReturnValue:
	*     成功，返回 x_err_ok；失败，返回 错误码。
	*/
	static x_int32_t initial_lib(x_handle_t xt_pv_param);

	/**************************************************************************
	* FunctionName:
	*     uninitial_lib
	* Description:
	*     库反初始化操作。
	*/
	static void uninitial_lib(void);

protected:
	/**************************************************************************
	* FunctionName:
	*     thread_work_decode
	* Description:
	*     连接线程入口函数。
	*/
	static x_uint32_t __stdcall thread_work_connect(x_handle_t pv_param);

	/**************************************************************************
	* FunctionName:
	*     thread_work_recv
	* Description:
	*     数据采集的线程入口函数。
	*/
	static x_uint32_t __stdcall thread_work_recv(x_handle_t pv_param);

	/**************************************************************************
	* FunctionName:
	*     thread_work_decode
	* Description:
	*     数据解码线程入口函数。
	*/
	static x_uint32_t __stdcall thread_work_decode(x_handle_t pv_param);

	/**************************************************************************
	* FunctionName:
	*     realframe_cbk_entry
	* Description:
	*     实时数据帧回调接口。
	*/
	static void realframe_cbk_entry(x_handle_t xt_handle, x_handle_t xt_buf, x_uint32_t xt_size, x_uint32_t xt_type, x_handle_t xt_user);

	// public interfaces
public:
	static bool StringToWString(const std::string &str,std::wstring &wstr);
	/**************************************************************************
	* FunctionName:
	*     open
	* Description:
	*     RTSP地址的打开操作。
	* Parameter:
	*     @[in ] xt_rtsp_url: RTSP URL 地址。
	*     @[in ] xfunc_realcbk: 解码数据回调函数接口。
	*     @[in ] xt_user: 解码数据回调的用户数据。
	* ReturnValue:
	*     成功，返回 0；失败，返回 错误码。
	*/
	x_int8_t open(const x_string_t xt_rtsp_url,x_uint32_t xt_user);
	//x_int8_t open(const x_string_t xt_rtsp_url, x_realcbk_func xfunc_realcbk,x_realcbk_func xfunc_pausecbk, x_uint32_t xt_user);

	void setCBK(x_realcbk_func xfunc_realcbk,x_pausecbk_func xfunc_pausecbk, x_playerStatuscbk_func xfunc_playerStatus,x_playerTipscbk_func xfunc_playerTips);
	/**************************************************************************
	* FunctionName:
	*     close
	* Description:
	*     对象关闭操作。
	*/
	void close(void);

	/**************************************************************************
	* FunctionName:
	*     is_working
	* Description:
	*     返回是否处于工作状态。
	*/
	inline x_bool_t is_working(void) const
	{
		return ((0 == m_xt_bexitflag) && (X_NULL != m_xt_hthread_recv) && (X_NULL != m_xt_hthread_decode));
	}

	/**************************************************************************
	* FunctionName:
	*     real_decode_width
	* Description:
	*     实时解码时得到的图像宽度。
	*/
	inline x_int32_t real_decode_width(void) const { return m_xt_width; }

	/**************************************************************************
	* FunctionName:
	*     real_decode_height
	* Description:
	*     实时解码时得到的图像高度。
	*/
	inline x_int32_t real_decode_height(void) const { return m_xt_height; }

	/**************************************************************************
	* FunctionName:
	*     real_decode_enable_flip
	* Description:
	*     实时解码时，是否对图像进行垂直翻转。
	*/
	inline void real_decode_enable_flip(x_bool_t xt_enable) { m_xt_flip = xt_enable; }

	/**************************************************************************
	* FunctionName:
	*     get_realframe_context
	* Description:
	*     获取 视频编码描述信息。
	* Parameter:
	*     @[out] xt_buf: 信息输出缓存。
	*     @[in out] xt_size: 入参，信息输出缓存的大小；回参，视频编码描述信息的大小。
	* ReturnValue:
	*     成功，返回 X_TRUE；失败，返回 X_FALSE。
	*/
	x_bool_t get_realframe_context();

	int stopRecordVideo();

	// inner invoking methods
protected:
	/**************************************************************************
	* FunctionName:
	*     connect_fun
	* Description:
	*     连接函数。
	*/
	x_uint32_t connect_fun(void);

	/**************************************************************************
	* FunctionName:
	*     recv_loop
	* Description:
	*     数据接收的事件处理流程（仅由 thread_work_recv() 接口回调该操作）。
	*/
	void recv_loop(void);

	/**************************************************************************
	* FunctionName:
	*     decode_loop
	* Description:
	*     数据解码的事件处理流程（仅由 thread_work_recv() 接口回调该操作）。
	*/
	void decode_loop(void);

	/**************************************************************************
	* FunctionName:
	*     realframe_proc
	* Description:
	*     实时数据帧回调接收处理流程（仅由 realframe_cbk_entry() 接口回调该操作）。
	*/
	void realframe_proc(x_handle_t xt_handle, x_handle_t xt_buf, x_uint32_t xt_size, x_uint32_t xt_type);

	/**************************************************************************
	* FunctionName:
	*     is_exit_work
	* Description:
	*     返回工作线程的退出标识（是否退出工作状态）。
	*/
	inline x_bool_t is_exit_work(void) const { return (x_bool_t)m_xt_bexitflag; }

	// class data
public:
	x_handle_t       m_xt_hthread_recv;         ///< RTSP 数据采集线程
	x_handle_t       m_xt_hthread_decode;       ///< H264 数据解码线程
	x_handle_t       m_xt_hthread_connect;       ///连接线程
	x_int8_t         m_xt_bexitflag;            ///< 工作线程的退出标识
	x_uint32_t		  m_xt_connectSuccess;			//连接成功标志	0初始化状态 1连接状态 2连接成功 3连接失败
	x_handle_t       m_xt_rtsp_client;          ///< RTSP 客户端操作对象
	x_handle_t       m_xt_realframe_queue;      ///< 接收回调的视频数据帧的环形队列

	x_bool_t         m_xt_real_context_valid;   ///< 标识当前接收视频头的描述信息是否已经有效
	x_handle_t       m_xt_real_context_info;    ///< 接收视频头的描述信息

	x_string_t       m_xt_rtsp_url;             ///< 打开的 RTSP URL 地址	

	x_int32_t        m_xt_width;                ///< 解码时得到的图像宽度
	x_int32_t        m_xt_height;               ///< 解码时得到的图像高度
	x_bool_t         m_xt_flip;                 ///< 解码时是否对图像进行垂直翻转
	int				  receiveThreadExitFlag;		///<接收线程退出标识
	int				  decodeThreadExitFlag;			///<解码线程退出标识

	x_realcbk_func   m_xfunc_realcbk;           ///< 实时解码后的数据回调函数接口
	x_playerStatuscbk_func m_xfunc_playerStatus;///<获取播放状态函数接口
	x_playerTipscbk_func m_xfunc_playerTip;
	//x_realcbk_func   m_xfunc_pausecbk;           ///< 信号差暂停回调函数接口
	x_pausecbk_func   m_xfunc_pausecbk;           ///< 信号差暂停回调函数接口
	x_uint32_t       m_xt_user;                 ///< 实时解码后的数据回调函数接口的回调用户数据参数
	//vxFFmpegDecode		ffmpegCodec;//编解码器
	 AVCodecContext    * pCodecCtx;
	 AVCodec           * pCodec;
	 AVFormatContext		*pFormatCtx;
	 AVFrame           * pFrame;
	 AVFrame           * pFrameYUV;
	 AVFrame           * pFrameRGB;
	 uint8_t *out_buffer;
	struct SwsContext * img_convert_ctx;
	 AVPacket           * packet1;		 
	int videoindex;
	log4cplus::Logger pTestLogger;

//	x_int32_t           m_xt_width;
//	x_int32_t           m_xt_height;
	//xmemblock           m_xmem_decode;
	  int ret;
	 int got_picture;
	 int errorURL;
	 AVFormatContext *o_fmt_ctx; 
	 AVStream *o_video_stream;
	 AVStream *i_video_stream;
	 int iRecordVideo;	
	 static int pluginNum;
};
int vxRTSPClient::pluginNum=0;
///////////////////////////////////////////////////////////////////////////

#endif // __VXRTSPCLIENT_H__

