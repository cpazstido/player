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
	*     ���ʼ��������
	* Parameter:
	*     @[in ] xt_pv_param: Ԥ��������
	* ReturnValue:
	*     �ɹ������� x_err_ok��ʧ�ܣ����� �����롣
	*/
	static x_int32_t initial_lib(x_handle_t xt_pv_param);

	/**************************************************************************
	* FunctionName:
	*     uninitial_lib
	* Description:
	*     �ⷴ��ʼ��������
	*/
	static void uninitial_lib(void);

protected:
	/**************************************************************************
	* FunctionName:
	*     thread_work_decode
	* Description:
	*     �����߳���ں�����
	*/
	static x_uint32_t __stdcall thread_work_connect(x_handle_t pv_param);

	/**************************************************************************
	* FunctionName:
	*     thread_work_recv
	* Description:
	*     ���ݲɼ����߳���ں�����
	*/
	static x_uint32_t __stdcall thread_work_recv(x_handle_t pv_param);

	/**************************************************************************
	* FunctionName:
	*     thread_work_decode
	* Description:
	*     ���ݽ����߳���ں�����
	*/
	static x_uint32_t __stdcall thread_work_decode(x_handle_t pv_param);

	/**************************************************************************
	* FunctionName:
	*     realframe_cbk_entry
	* Description:
	*     ʵʱ����֡�ص��ӿڡ�
	*/
	static void realframe_cbk_entry(x_handle_t xt_handle, x_handle_t xt_buf, x_uint32_t xt_size, x_uint32_t xt_type, x_handle_t xt_user);

	// public interfaces
public:
	static bool StringToWString(const std::string &str,std::wstring &wstr);
	/**************************************************************************
	* FunctionName:
	*     open
	* Description:
	*     RTSP��ַ�Ĵ򿪲�����
	* Parameter:
	*     @[in ] xt_rtsp_url: RTSP URL ��ַ��
	*     @[in ] xfunc_realcbk: �������ݻص������ӿڡ�
	*     @[in ] xt_user: �������ݻص����û����ݡ�
	* ReturnValue:
	*     �ɹ������� 0��ʧ�ܣ����� �����롣
	*/
	x_int8_t open(const x_string_t xt_rtsp_url,x_uint32_t xt_user);
	//x_int8_t open(const x_string_t xt_rtsp_url, x_realcbk_func xfunc_realcbk,x_realcbk_func xfunc_pausecbk, x_uint32_t xt_user);

	void setCBK(x_realcbk_func xfunc_realcbk,x_pausecbk_func xfunc_pausecbk, x_playerStatuscbk_func xfunc_playerStatus,x_playerTipscbk_func xfunc_playerTips);
	/**************************************************************************
	* FunctionName:
	*     close
	* Description:
	*     ����رղ�����
	*/
	void close(void);

	/**************************************************************************
	* FunctionName:
	*     is_working
	* Description:
	*     �����Ƿ��ڹ���״̬��
	*/
	inline x_bool_t is_working(void) const
	{
		return ((0 == m_xt_bexitflag) && (X_NULL != m_xt_hthread_recv) && (X_NULL != m_xt_hthread_decode));
	}

	/**************************************************************************
	* FunctionName:
	*     real_decode_width
	* Description:
	*     ʵʱ����ʱ�õ���ͼ���ȡ�
	*/
	inline x_int32_t real_decode_width(void) const { return m_xt_width; }

	/**************************************************************************
	* FunctionName:
	*     real_decode_height
	* Description:
	*     ʵʱ����ʱ�õ���ͼ��߶ȡ�
	*/
	inline x_int32_t real_decode_height(void) const { return m_xt_height; }

	/**************************************************************************
	* FunctionName:
	*     real_decode_enable_flip
	* Description:
	*     ʵʱ����ʱ���Ƿ��ͼ����д�ֱ��ת��
	*/
	inline void real_decode_enable_flip(x_bool_t xt_enable) { m_xt_flip = xt_enable; }

	/**************************************************************************
	* FunctionName:
	*     get_realframe_context
	* Description:
	*     ��ȡ ��Ƶ����������Ϣ��
	* Parameter:
	*     @[out] xt_buf: ��Ϣ������档
	*     @[in out] xt_size: ��Σ���Ϣ�������Ĵ�С���زΣ���Ƶ����������Ϣ�Ĵ�С��
	* ReturnValue:
	*     �ɹ������� X_TRUE��ʧ�ܣ����� X_FALSE��
	*/
	x_bool_t get_realframe_context();

	int stopRecordVideo();

	// inner invoking methods
protected:
	/**************************************************************************
	* FunctionName:
	*     connect_fun
	* Description:
	*     ���Ӻ�����
	*/
	x_uint32_t connect_fun(void);

	/**************************************************************************
	* FunctionName:
	*     recv_loop
	* Description:
	*     ���ݽ��յ��¼��������̣����� thread_work_recv() �ӿڻص��ò�������
	*/
	void recv_loop(void);

	/**************************************************************************
	* FunctionName:
	*     decode_loop
	* Description:
	*     ���ݽ�����¼��������̣����� thread_work_recv() �ӿڻص��ò�������
	*/
	void decode_loop(void);

	/**************************************************************************
	* FunctionName:
	*     realframe_proc
	* Description:
	*     ʵʱ����֡�ص����մ������̣����� realframe_cbk_entry() �ӿڻص��ò�������
	*/
	void realframe_proc(x_handle_t xt_handle, x_handle_t xt_buf, x_uint32_t xt_size, x_uint32_t xt_type);

	/**************************************************************************
	* FunctionName:
	*     is_exit_work
	* Description:
	*     ���ع����̵߳��˳���ʶ���Ƿ��˳�����״̬����
	*/
	inline x_bool_t is_exit_work(void) const { return (x_bool_t)m_xt_bexitflag; }

	// class data
public:
	x_handle_t       m_xt_hthread_recv;         ///< RTSP ���ݲɼ��߳�
	x_handle_t       m_xt_hthread_decode;       ///< H264 ���ݽ����߳�
	x_handle_t       m_xt_hthread_connect;       ///�����߳�
	x_int8_t         m_xt_bexitflag;            ///< �����̵߳��˳���ʶ
	x_uint32_t		  m_xt_connectSuccess;			//���ӳɹ���־	0��ʼ��״̬ 1����״̬ 2���ӳɹ� 3����ʧ��
	x_handle_t       m_xt_rtsp_client;          ///< RTSP �ͻ��˲�������
	x_handle_t       m_xt_realframe_queue;      ///< ���ջص�����Ƶ����֡�Ļ��ζ���

	x_bool_t         m_xt_real_context_valid;   ///< ��ʶ��ǰ������Ƶͷ��������Ϣ�Ƿ��Ѿ���Ч
	x_handle_t       m_xt_real_context_info;    ///< ������Ƶͷ��������Ϣ

	x_string_t       m_xt_rtsp_url;             ///< �򿪵� RTSP URL ��ַ	

	x_int32_t        m_xt_width;                ///< ����ʱ�õ���ͼ����
	x_int32_t        m_xt_height;               ///< ����ʱ�õ���ͼ��߶�
	x_bool_t         m_xt_flip;                 ///< ����ʱ�Ƿ��ͼ����д�ֱ��ת
	int				  receiveThreadExitFlag;		///<�����߳��˳���ʶ
	int				  decodeThreadExitFlag;			///<�����߳��˳���ʶ

	x_realcbk_func   m_xfunc_realcbk;           ///< ʵʱ���������ݻص������ӿ�
	x_playerStatuscbk_func m_xfunc_playerStatus;///<��ȡ����״̬�����ӿ�
	x_playerTipscbk_func m_xfunc_playerTip;
	//x_realcbk_func   m_xfunc_pausecbk;           ///< �źŲ���ͣ�ص������ӿ�
	x_pausecbk_func   m_xfunc_pausecbk;           ///< �źŲ���ͣ�ص������ӿ�
	x_uint32_t       m_xt_user;                 ///< ʵʱ���������ݻص������ӿڵĻص��û����ݲ���
	//vxFFmpegDecode		ffmpegCodec;//�������
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

