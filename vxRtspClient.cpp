#include "vxRTSPClient.h"
#include "INIReader.h"
#include "encodeBase64.h"
#include <csignal>





//RTSPCLIENT_API log4cplus::Logger pTestLogger=log4cplus::Logger::getInstance(L"LoggerName");

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

#include <process.h>
#include <cassert>

#include "vxRTSPClient.inl"
#include "vxRTSPMemory.inl"
#include <time.h>


RTSPCLIENT_API int ffmpegWidth;
RTSPCLIENT_API int ffmpegHeight;



///////////////////////////////////////////////////////////////////////////
//����ص�����
long WINAPI FilterFunc(DWORD dwExceptionCode,char *e)

{
	//LOG4CPLUS_WARN(pTestLogger,e);
	//LOG4CPLUS_WARN(pTestLogger,dwExceptionCode);
	//return (dwExceptionCode == STATUS_STACK_OVERFLOW) ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH;	
	exit(0);
	return EXCEPTION_CONTINUE_EXECUTION ;

}

/**************************************************************************
* FunctionName:
*     destroy_thread
* Description:
*     ���ٹ����̡߳�
* Parameter:
*     @[in ] hthread: �߳̾����
*     @[in ] timeout: �ȴ��ĳ�ʱʱ�䡣
* ReturnValue:
*     void
*/
static void destroy_thread(x_handle_t hthread, x_uint32_t xt_timeout)
{		
	DWORD dwExitCode = 0;
	if ((X_NULL != hthread) && GetExitCodeThread(hthread, &dwExitCode) && (STILL_ACTIVE == dwExitCode))
	{
		DWORD dwWait = WaitForSingleObject(hthread, xt_timeout);
		if (WAIT_TIMEOUT == dwWait)
		{
			TerminateThread(hthread, -1);
		}		
	}

	CloseHandle(hthread);	
}

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
x_int32_t vxRTSPClient::initial_lib(x_handle_t xt_pv_param)
{	
	return 0;
}

/**************************************************************************
* FunctionName:
*     uninitial_lib
* Description:
*     �ⷴ��ʼ��������
*/
void vxRTSPClient::uninitial_lib(void)
{

}

/**************************************************************************
* FunctionName:
*     thread_work_recv
* Description:
*     �����߳���ں�����
*/
x_uint32_t __stdcall vxRTSPClient::thread_work_connect(x_handle_t pv_param)
{
	((vxRTSPClient *)pv_param)->connect_fun();
	return 0;
}


/**************************************************************************
* FunctionName:
*     thread_work_recv
* Description:
*     ���ݲɼ����߳���ں�����
*/
x_uint32_t __stdcall vxRTSPClient::thread_work_recv(x_handle_t pv_param)
{
	((vxRTSPClient *)pv_param)->recv_loop();
	return 0;
}

/**************************************************************************
* FunctionName:
*     thread_work_decode
* Description:
*     ���ݽ����߳���ں�����
*/
x_uint32_t __stdcall vxRTSPClient::thread_work_decode(x_handle_t pv_param)
{
	((vxRTSPClient *)pv_param)->decode_loop();
	return 0;
}

/**************************************************************************
* FunctionName:
*     decode_loop
* Description:
*     ���ݽ�����¼��������̣����� thread_work_recv() �ӿڻص��ò�������
*/
void vxRTSPClient::decode_loop(void)
{	
	LOG4CPLUS_WARN(pTestLogger,"decode_loop1");		
	//m_xfunc_playerStatus("","info:Decoding thread starts.",m_xt_user);	
	xmemblock_cirqueue * x_cirqueue_ptr = (xmemblock_cirqueue *)m_xt_realframe_queue;
	if (X_NULL == x_cirqueue_ptr)
	{
		return;
	}	
	while (!is_exit_work())
	{				
		if(m_xt_connectSuccess!=2)
		{
			Sleep(1);
			continue;
		}
		Sleep(1);
		int num=pFormatCtx->streams[videoindex]->avg_frame_rate.num;
		int den=pFormatCtx->streams[videoindex]->avg_frame_rate.den;
		int delayTime=0;
		if(num!=0)
		{
			float delay=(1/((num/den)*1.0));
			delayTime=(int)(delay*1000);
		}
		if(delayTime==250)//¼���ļ�
			Sleep(delayTime);		

		char percentages[40]={0};		
		//LOG4CPLUS_WARN(pTestLogger,"11");
		sprintf(percentages,"flush percentages:%f",x_cirqueue_ptr->flushPercentages());	
		//LOG4CPLUS_WARN(pTestLogger,"22");
		//m_xfunc_playerStatus("","info",percentages,m_xt_user);
		/*if(x_cirqueue_ptr->flushPercentages()<0.5)
			Sleep(500);*/
		packet1 = x_cirqueue_ptr->pop_front_from_saved_queue();	
		//LOG4CPLUS_WARN(pTestLogger,"33");
		if(packet1!=NULL&&pCodecCtx!=NULL)
		{				
			if(packet1->stream_index==videoindex)//��Ƶ
			{					
				ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet1);
				//LOG4CPLUS_WARN(pTestLogger,"44");
				if(pFrame==NULL)
					LOG4CPLUS_WARN(pTestLogger,"NULLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL");
				pFrame->data[0] = pFrame->data[0] + pFrame->linesize[0] * (pCodecCtx->height - 1);				
				pFrame->data[1] = pFrame->data[1] + pFrame->linesize[1] * (pCodecCtx->height / 2 - 1);
				pFrame->data[2] = pFrame->data[2] + pFrame->linesize[2] * (pCodecCtx->height / 2 - 1);
				//LOG4CPLUS_WARN(pTestLogger,"444");
				pFrame->linesize[0] *= -1;
				pFrame->linesize[1] *= -1;
				pFrame->linesize[2] *= -1;
				if(pFrame->pict_type==AV_PICTURE_TYPE_I)
					printf("Picture Type: I Frame %d\n",pFrame->pict_type);
				else if(pFrame->pict_type==AV_PICTURE_TYPE_P)
					printf("Picture Type: P Frame %d\n",pFrame->pict_type);
				else if(pFrame->pict_type==AV_PICTURE_TYPE_B)
					printf("Picture Type: B Frame %d\n",pFrame->pict_type);
				else
					printf("Picture Type: None %d\n",pFrame->pict_type);
				if(ret < 0){
					printf("Decode Error.\n");
					//return -1;
				}
				if(got_picture){
					int nwidth=0,nheight=0;
					nwidth=pCodecCtx->width;
					nheight=pCodecCtx->height;
					if(img_convert_ctx==NULL||nwidth!=m_xt_width||nheight!=m_xt_height)
					{
						//LOG4CPLUS_WARN(pTestLogger,"55");
						img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,pCodecCtx->width, pCodecCtx->height, PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL); 
						char resolution[40]={0};		
						sprintf(resolution,"%d %d",pCodecCtx->width,pCodecCtx->height);
						m_xfunc_playerStatus("","resolution",resolution,m_xt_user);
						m_xfunc_playerTip(m_xt_user,resolution);		
						LOG4CPLUS_WARN(pTestLogger,resolution);
					}
					//LOG4CPLUS_WARN(pTestLogger,"1");
					m_xt_width  = pCodecCtx->width;
					m_xt_height = pCodecCtx->height;
					ffmpegHeight=m_xt_height;
					ffmpegWidth=m_xt_width;
					out_buffer=(uint8_t *)av_malloc(avpicture_get_size(PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height));
					//LOG4CPLUS_WARN(pTestLogger,"2");
					pFrameRGB = avcodec_alloc_frame();
					//LOG4CPLUS_WARN(pTestLogger,"3");
					avpicture_fill((AVPicture *)pFrameRGB, out_buffer, PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height);
					//LOG4CPLUS_WARN(pTestLogger,"4");
					sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);
					//LOG4CPLUS_WARN(pTestLogger,"5");
					m_xfunc_realcbk(X_REAL_TYPE_RGB, pFrameRGB->data[0], 3 * m_xt_width * m_xt_height, m_xt_width, m_xt_height, m_xt_user,"");
					//LOG4CPLUS_WARN(pTestLogger,"6");
					av_free(out_buffer);
					//LOG4CPLUS_WARN(pTestLogger,"7");
					av_frame_free(&pFrameRGB);
					//LOG4CPLUS_WARN(pTestLogger,"8");
				}
				//av_free_packet(packet1);//================
				//LOG4CPLUS_WARN(pTestLogger,"9");
			}			
		}		
		//av_free_packet(packet1);//================
		//LOG4CPLUS_WARN(pTestLogger,"10");
		x_cirqueue_ptr->recyc(packet1);	
		//LOG4CPLUS_WARN(pTestLogger,"11");
	}
	x_cirqueue_ptr->clear_cir_queue();//================
	av_frame_free(&pFrame);
	avcodec_close(pCodecCtx);	
	LOG4CPLUS_WARN(pTestLogger,"decode_loop2");
	m_xfunc_playerStatus("","info","decode thread exit...",m_xt_user);	
	m_xfunc_playerTip(m_xt_user,"stop playing.");		
	//m_xfunc_playerStatus("","closed","stop playing.",m_xt_user);
	//m_xfunc_playerTip(m_xt_user,"Please play again.");
	//m_xfunc_playerStatus("","info:Decoding thread end.",m_xt_user);

	//close();
}

/**************************************************************************
* FunctionName:
*     realframe_cbk_entry
* Description:
*     ʵʱ����֡�ص��ӿڡ�
*/
void vxRTSPClient::realframe_cbk_entry(x_handle_t xt_handle, x_handle_t xt_buf, x_uint32_t xt_size, x_uint32_t xt_type, x_handle_t xt_user)
{	
}

//=========================================================================

// 
// vxRTSPClient constructor/destructor
// 

vxRTSPClient::vxRTSPClient(void)
: m_xt_hthread_recv(X_NULL)
, m_xt_hthread_decode(X_NULL)
, m_xt_hthread_connect(X_NULL)
, m_xt_bexitflag(X_FALSE)
, m_xt_rtsp_client(X_NULL)			
, m_xt_realframe_queue(X_NULL)
, m_xt_real_context_valid(X_FALSE)
, m_xt_real_context_info(X_NULL)
, m_xt_rtsp_url(X_NULL)
, m_xt_width(0)
, m_xt_height(0)
, m_xt_flip(X_FALSE)
, m_xfunc_realcbk(X_NULL)				
, m_xt_user(0)
,videoindex(-1)
,m_xt_connectSuccess(0)
,img_convert_ctx(NULL)
,pCodecCtx(NULL)
,pFormatCtx(NULL)
,pFrame(NULL)
,packet1(NULL)
,ret(NULL)
,got_picture(NULL)
,errorURL(0)
,o_fmt_ctx(NULL)
,o_video_stream(NULL)
,i_video_stream(NULL)
,iRecordVideo(NULL)
,receiveThreadExitFlag(0)
,decodeThreadExitFlag(0)
{		
	av_register_all();
	avformat_network_init();

	tchar loggerName[20]={0};
	pluginNum++;
	swprintf(loggerName,L"LoggerName%d",pluginNum);
	pTestLogger=log4cplus::Logger::getInstance(loggerName);
	char szFile[_MAX_PATH];
	SYSTEMTIME stime;
	GetLocalTime(&stime);
	sprintf(szFile,"c:\\PlayerPlugin\\log\\%04d-%02d-%02d %02d-%02d-%02d %02d.log",stime.wYear,stime.wMonth,stime.wDay,stime.wHour, stime.wMinute, stime.wSecond,pluginNum);	
	std::wstring ss;
	StringToWString(szFile,ss);

	// ����һ���ļ�Appender 
	log4cplus::SharedAppenderPtr pFileAppender(new FileAppender(ss)); 
	std::auto_ptr<log4cplus::Layout> pPatternLayout(new PatternLayout(L"%c: %-5p %d{%y/%m/%d %H:%M:%S}- thread:%t - %-50m - [%F:%L]%n"));   
	pFileAppender->setLayout(pPatternLayout); 
	pTestLogger.addAppender(pFileAppender); 

	// �����־��Ϣ 
	LOG4CPLUS_WARN(pTestLogger, "vxRTSPClient::initial_lib...");





	// ���������ڴ����
	m_xt_realframe_queue = (x_handle_t)(new xmemblock_cirqueue());
	if (X_NULL == m_xt_realframe_queue)
	{
		assert(false);
	}

	m_xt_real_context_info = (x_handle_t)(new xmemblock());
	if (X_NULL == m_xt_real_context_info)
	{
		assert(false);
	}		

}
void vxRTSPClient::test()
{
	printf("�����У����Ժ�...\n");
}
vxRTSPClient::~vxRTSPClient(void)
{	
	LOG4CPLUS_WARN(pTestLogger,"~vxRTSPClient1");
	xmemblock * xt_mblk = (xmemblock *)m_xt_real_context_info;
	if (X_NULL != xt_mblk)
	{
		delete xt_mblk;
		xt_mblk = X_NULL;

		m_xt_real_context_info = X_NULL;
	}

	// �����ζ��л���
	xmemblock_cirqueue * xt_cirqueue = (xmemblock_cirqueue *)m_xt_realframe_queue;
	if (X_NULL != xt_cirqueue)
	{
		xt_cirqueue->clear_cir_queue();
		delete xt_cirqueue;
		xt_cirqueue = X_NULL;

		m_xt_realframe_queue = X_NULL;
	}		
	LOG4CPLUS_WARN(pTestLogger,"~vxRTSPClient2");
}

//=========================================================================

// 
// vxRTSPClient public interfaces
// 

void vxRTSPClient::setCBK(x_realcbk_func xfunc_realcbk,x_pausecbk_func xfunc_pausecbk, x_playerStatuscbk_func xfunc_playerStatus,x_playerTipscbk_func xfunc_playerTips)
{
	m_xfunc_realcbk = xfunc_realcbk;	
	m_xfunc_playerStatus=xfunc_playerStatus;
	m_xfunc_playerTip=xfunc_playerTips;
	
}
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
x_int8_t vxRTSPClient::open(const x_string_t xt_rtsp_url,x_uint32_t xt_user)
//x_int8_t vxRTSPClient::open(const x_string_t xt_rtsp_url, x_realcbk_func xfunc_realcbk,x_realcbk_func xfunc_pausecbk, x_uint32_t xt_user)
{		
	x_int8_t xt_err = X_ERR_UNKNOW;

	LOG4CPLUS_WARN(pTestLogger,"open1");
	// �������
	m_xt_rtsp_url   = _strdup(xt_rtsp_url);
	m_xt_user       = xt_user;
	m_xt_bexitflag  = X_FALSE;
	x_uint32_t xt_tid;		

	/*char   str3[100]={0}; 
	char*   str1="url:";  	
	strcpy(str3,str1);  
	strcat(str3,xt_rtsp_url);  */
	
	m_xfunc_playerStatus("","url",xt_rtsp_url,m_xt_user);
	// ���������߳�
	m_xt_hthread_connect = (x_handle_t)_beginthreadex(X_NULL, 0, &thread_work_connect, this, 0, &xt_tid);
	if (X_NULL == m_xt_hthread_connect)
	{
		return 0;
	}	

	Sleep(1);
	// �������ݽ��յĹ����߳�
	m_xt_hthread_recv = (x_handle_t)_beginthreadex(X_NULL, 0, &thread_work_recv, this, 0, &xt_tid);
	if (X_NULL == m_xt_hthread_recv)
	{
		return 0;
	}

	Sleep(1);
	// �������ݽ���Ĺ����߳�
	m_xt_hthread_decode = (x_handle_t)_beginthreadex(X_NULL, 0, &vxRTSPClient::thread_work_decode, this, 0, &xt_tid);
	if (X_NULL == m_xt_hthread_decode)
	{
		return 0;
	}


	LOG4CPLUS_WARN(pTestLogger,"open2");

	return xt_err;
}

/**************************************************************************
* FunctionName:
*     close
* Description:
*     ����رղ�����
*/
void vxRTSPClient::close(void)
{	
	/*AVFrame           * pFrameRGB1;
	av_frame_free(&pFrameRGB1);*/

	// �����˳���ʶ
	LOG4CPLUS_WARN(pTestLogger,"close1");
	char connectSuccessFlag[40]={0};		
	sprintf(connectSuccessFlag,"exit m_xt_connectSuccess=%d",m_xt_connectSuccess);
	LOG4CPLUS_WARN(pTestLogger,connectSuccessFlag);

	m_xt_bexitflag=true;

	if(m_xt_connectSuccess==2)
	{
		m_xt_connectSuccess=0;

		stopRecordVideo();

		// ���� RTSP ���ݽ��յĹ����߳�
		if (X_NULL != m_xt_hthread_recv)
		{
			//destroy_thread(m_xt_hthread_recv, INFINITE);
			m_xt_hthread_recv = X_NULL;
			LOG4CPLUS_WARN(pTestLogger,"stop reveice thread");
		}	

		// �������ݽ���Ĺ����߳�
		if (X_NULL != m_xt_hthread_decode)
		{			
			//destroy_thread(m_xt_hthread_decode, INFINITE);
			m_xt_hthread_decode = X_NULL;
			LOG4CPLUS_WARN(pTestLogger,"stop decode thread");
		}

		// ���������߳�
		if (X_NULL != m_xt_hthread_connect)
		{
			//destroy_thread(m_xt_hthread_connect, INFINITE);
			m_xt_hthread_connect = X_NULL;
			LOG4CPLUS_WARN(pTestLogger,"stop connect thread");
		}

		if (X_NULL != m_xt_rtsp_url)
		{
			free(m_xt_rtsp_url);
			m_xt_rtsp_url = X_NULL;
		}

		m_xt_width  = 0;
		m_xt_height = 0;		

		//if(packet1!=NULL&&receiveThreadExitFlag==1&&decodeThreadExitFlag==1)
		//{
		//	av_free_packet(packet1);
		//	LOG4CPLUS_WARN(pTestLogger,"freee packet1");
		//	packet1=NULL;
		//}	
		//if(pFrame!=NULL&&receiveThreadExitFlag==1&&decodeThreadExitFlag==1)
		//{
		//	av_frame_free(&pFrame);
		//	LOG4CPLUS_WARN(pTestLogger,"freee pFrame");
		//	pFrame=NULL;
		//}		
		//if(pCodecCtx!=NULL&&receiveThreadExitFlag==1&&decodeThreadExitFlag==1)
		//{
		//	avcodec_close(pCodecCtx);
		//	LOG4CPLUS_WARN(pTestLogger,"freee pCodecCtx");
		//	pCodecCtx=NULL;
		//}
		//if(pFormatCtx!=NULL)
		//{
		//	//avcodec_close(pFormatCtx->streams[videoindex]->codec);		
		//}
		//if(pFormatCtx!=NULL&&receiveThreadExitFlag==1&&decodeThreadExitFlag==1)
		//{
		//	//avformat_close_input(&pFormatCtx);
		//	pFormatCtx=NULL;
		//}	
		m_xfunc_playerStatus("","closed","stop playing...",m_xt_user);//�ŵ�����Ҫ����
		//m_xfunc_playerTip(m_xt_user,"stop playing...");
	}
	Sleep(50);//��ֹ����ͽ����̻߳�û����	
	m_xt_bexitflag = X_TRUE;
	errorURL=0;	
	LOG4CPLUS_WARN(pTestLogger,"close2");
}

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
x_bool_t vxRTSPClient::get_realframe_context()
{
	LOG4CPLUS_WARN(pTestLogger,"get_realframe_context1");
	char szFile[_MAX_PATH]={0};
	char fileName[_MAX_PATH]={0};
	SYSTEMTIME stime;
	GetLocalTime(&stime);
	sprintf(fileName,"%04d-%02d-%02d_%02d%02d%02d.mp4",stime.wYear,stime.wMonth,stime.wDay,stime.wHour, stime.wMinute, stime.wSecond);	

	//��ȡ�����ļ�
	INIReader *iniReader=new INIReader();
	wstring path=L"c:\\PlayerPlugin\\configure.ini";//(p);
	wstring value;
	if(iniReader->isFileExist(path))
	{
		iniReader->readFile(L"c:\\PlayerPlugin\\configure.ini");
		iniReader->GetParamValue(L"video", value);
	}	
	if(value.length()==0)
	{
		sprintf(szFile,"c:\\PlayerPlugin\\video\\%04d-%02d-%02d_%02d%02d%02d.mp4",stime.wYear,stime.wMonth,stime.wDay,stime.wHour, stime.wMinute, stime.wSecond);	
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
	avformat_alloc_output_context2(&o_fmt_ctx, NULL, NULL, szFile);
	o_video_stream = avformat_new_stream(o_fmt_ctx, NULL); 
	{ 		
		AVCodecContext *c; 
		c = o_video_stream->codec; 
		c->bit_rate = i_video_stream->codec->bit_rate;
		c->codec_id = i_video_stream->codec->codec_id; 
		c->codec_type = i_video_stream->codec->codec_type; 
		c->time_base.num = i_video_stream->time_base.num; 
		c->time_base.den = i_video_stream->time_base.den; 
		fprintf(stderr, "time_base.num = %d time_base.den = %d\n", c->time_base.num, c->time_base.den); 
		c->width = i_video_stream->codec->width; 
		c->height = i_video_stream->codec->height; 
		c->pix_fmt = i_video_stream->codec->pix_fmt; 
		printf("%d %d %d", c->width, c->height, c->pix_fmt); 
		c->flags = i_video_stream->codec->flags; 
		c->flags |= CODEC_FLAG_GLOBAL_HEADER; 
		c->me_range = i_video_stream->codec->me_range; 
		c->max_qdiff = i_video_stream->codec->max_qdiff;
		c->qmin = i_video_stream->codec->qmin; 
		c->qmax = i_video_stream->codec->qmax;
		c->qcompress = i_video_stream->codec->qcompress; 
	}

	avio_open(&o_fmt_ctx->pb, szFile, AVIO_FLAG_WRITE);

	avformat_write_header(o_fmt_ctx, NULL);
	iRecordVideo=true;
	LOG4CPLUS_WARN(pTestLogger,"get_realframe_context2");

	return X_TRUE;
}

int vxRTSPClient::stopRecordVideo()
{
	LOG4CPLUS_WARN(pTestLogger,"stopRecordVideo1");
	iRecordVideo=false;
	if(o_fmt_ctx!=NULL)
		av_write_trailer(o_fmt_ctx);	
	if(o_fmt_ctx!=NULL)
	{
		avcodec_close(o_fmt_ctx->streams[videoindex]->codec);		
	}
	if(o_fmt_ctx!=NULL)
	{
		avformat_close_input(&o_fmt_ctx);
		o_fmt_ctx=NULL;
	}
	LOG4CPLUS_WARN(pTestLogger,"stopRecordVideo2");

	return 1;
}

//=========================================================================

// 
// vxRTSPClient inner invoking methods
// 

x_uint32_t vxRTSPClient::connect_fun(void)
{		
	LOG4CPLUS_WARN(pTestLogger,"connect_fun1");
	int tryNums=0;
	while(true)
	{	
		tryNums++;
		if(m_xt_bexitflag)//���ڳ��������У��û�������˳������ٽ��к���ĳ���
			break;
		if(tryNums>3)
		{			
			m_xfunc_playerStatus("","error","connectting failed.",m_xt_user);
			m_xfunc_playerTip(m_xt_user,"connectting failed.");					
			m_xt_bexitflag=true;
			m_xt_connectSuccess=0;
			break;
		}
		if(tryNums!=1)
		{
			Sleep(1*1000);
		}		
		//LOG4CPLUS_WARN(pTestLogger,"�����߳̿�ʼtcp");
		char tryConnectting[40]={0};		
		sprintf(tryConnectting,"try %d times to stream server.",tryNums);	
		m_xfunc_playerStatus("","info",tryConnectting,m_xt_user);	
		m_xfunc_playerTip(m_xt_user,tryConnectting);		
		pFormatCtx = avformat_alloc_context();		
		pCodecCtx=NULL;	
		int i=0;	
		m_xt_connectSuccess=1;//��ʼ����		
		time_t startOpen ,endOpen ,errOpen ;  
		double costOpen;  
		time(&startOpen);
		/////////////////////////////////����rtp��ģʽ  tcp or udp Ĭ��udp
		AVDictionary* options = NULL;  
		av_dict_set(&options, "rtsp_transport", "tcp", 0);  
		/////////////////////////////////
		//if((i=avformat_open_input(&pFormatCtx,m_xt_rtsp_url,NULL,NULL))!=0){	
		if((i=avformat_open_input(&pFormatCtx,m_xt_rtsp_url,NULL,&options))!=0){	
			LOG4CPLUS_WARN(pTestLogger,"Couldn't open input stream.");
			//m_xfunc_playerStatus("","error:Couldn't open input stream.",m_xt_user);					
			m_xt_connectSuccess=3;
			time(&errOpen);
			char timeDiffOpenErr[_MAX_PATH];	
			sprintf(timeDiffOpenErr,"Couldn't open input stream:%f",difftime(errOpen,startOpen));	
			LOG4CPLUS_WARN(pTestLogger,timeDiffOpenErr);
			continue;
		}
		time(&endOpen);
		costOpen=difftime(endOpen,startOpen);  

		char timeDiffOpen[_MAX_PATH];	
		sprintf(timeDiffOpen,"open input stream:%f",costOpen);	
		LOG4CPLUS_WARN(pTestLogger,timeDiffOpen);
		time_t start ,end ;  
		double cost;  
		time(&start);			

		/*AVDictionary* pOptions = NULL;
		pFormatCtx->probesize = 0 ;
		pFormatCtx->max_analyze_duration = 0;
		pFormatCtx->flags |= AVFMT_FLAG_NOBUFFER;
		if(avformat_find_stream_info(pFormatCtx,&pOptions)<0){	*/	
		if (avformat_find_stream_info(pFormatCtx,NULL)<0){
			//��ʱ̫��		
			//m_xfunc_playerStatus("","error:Couldn't find stream information.",m_xt_user);			
			m_xt_connectSuccess=3;
			time(&end);
			char timeDiffOpenErr[_MAX_PATH];	
			sprintf(timeDiffOpenErr,"Couldn't find stream information:%f",difftime(end,start));	
			continue;
		}	
		time(&end); 
		cost=difftime(end,start);  
		printf("%f/n",cost); 
		char timeDiff[_MAX_PATH];	
		sprintf(timeDiff,"find stream information:%f",cost);	

		for(int i=0; i<pFormatCtx->nb_streams; i++) 
			if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO){
				videoindex=i;
				i_video_stream=pFormatCtx->streams[i];
				break;
			}
			if(videoindex==-1){
				printf("Didn't find a video stream.\n");
				m_xfunc_playerStatus("","error","Didn't find a video stream.",m_xt_user);			
				return X_FALSE;
			}
			pCodecCtx=pFormatCtx->streams[videoindex]->codec;
			pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
			if(pCodec==NULL){
				printf("Codec not found.\n");
				m_xfunc_playerStatus("","error","Codec not found.",m_xt_user);
				m_xfunc_playerTip(m_xt_user,"Codec not found.");				
				return X_FALSE;
			}
			if(avcodec_open2(pCodecCtx, pCodec,NULL)<0){
				printf("Could not open codec.\n");
				m_xfunc_playerStatus("","error","Could not open codec.",m_xt_user);
				m_xfunc_playerTip(m_xt_user,"Could not open codec.");			
				return X_FALSE;
			}
			pFrame=av_frame_alloc();		
			av_dump_format(pFormatCtx,0,m_xt_rtsp_url,0);
			m_xt_connectSuccess=2;//��ȡ����Ϣ�ɹ��������ӳɹ���	
			if(!m_xt_bexitflag)
			{
				m_xfunc_playerStatus("","connetced","connectting success.",m_xt_user);
				m_xfunc_playerTip(m_xt_user,"connectting success.Please wait.");					
			}
			char resolution[40]={0};		
			sprintf(resolution,"resolution:%d��%d",pCodecCtx->width,pCodecCtx->height);	
			m_xfunc_playerStatus("","info","connect thread exit...",m_xt_user);	
			break;
	}
	LOG4CPLUS_WARN(pTestLogger,"connect_fun2");

	return X_TRUE;
}

/**************************************************************************
* FunctionName:
*     recv_loop
* Description:
*     ���ݽ��յ��¼��������̣����� thread_work_recv() �ӿڻص��ò�������
*/
void vxRTSPClient::recv_loop(void)
{
	//m_xfunc_playerStatus("","info:receiving thread starts",m_xt_user);
	LOG4CPLUS_WARN(pTestLogger,"recv_loop1");	
	xmemblock_cirqueue * x_cirqueue_ptr = (xmemblock_cirqueue *)m_xt_realframe_queue;
	if (X_NULL == x_cirqueue_ptr)
	{
		return;
	}		
	x_cirqueue_ptr->cleanup_saved_queue();//�����һ�λ���
	while(!is_exit_work())
	{	//�˳�����
		if(errorURL)
		{//��ַ����
			LOG4CPLUS_WARN(pTestLogger,"error url,exit program.");
			m_xfunc_playerStatus("","error","error url,exit program.",m_xt_user);
			m_xfunc_playerTip(m_xt_user,"error url,exit program.");
			Sleep(1000);
			break;
		}
		if(m_xt_connectSuccess!=2)
		{	//�ȴ������߳�		
			Sleep(1000);
			continue;
		}
		Sleep(1);
		if(m_xt_connectSuccess!=2)
			continue;
		AVPacket * x_block_ptr = x_cirqueue_ptr->alloc();
		if (X_NULL == x_block_ptr)
		{
			continue;
		}				
		if(av_read_frame(pFormatCtx, x_block_ptr)>=0)
		{						
			if(x_block_ptr->stream_index==videoindex)
			{
				//int iDelayTime=(1/pFormatCtx->streams[videoindex]->avg_frame_rate.num/(pFormatCtx->streams[videoindex]->avg_frame_rate.den * 1.0))*1000;
				/*int num=pFormatCtx->streams[videoindex]->avg_frame_rate.num;
				int den=pFormatCtx->streams[videoindex]->avg_frame_rate.den;
				int delayTime=40;
				if(num!=0)
				{
					float delay=(1/((num/den)*1.0));
					delayTime=(int)(delay*1000);
				}

				Sleep(delayTime);*/
				// ʵʱ H264 �����ص�
				if (X_NULL != m_xfunc_realcbk)
				{
					//m_xfunc_realcbk(X_REAL_TYPE_H264, (x_uint8_t *)x_block_ptr->data(), x_block_ptr->size(), 0, 0, m_xt_user,"");
				}
				if(iRecordVideo)
					av_write_frame(o_fmt_ctx, x_block_ptr); 
				x_cirqueue_ptr->push_back_to_saved_queue(x_block_ptr);
				printf("stream type:Video\n");
			}
			else
			{
				printf("stream type:Audio\n");
			}					
		}
		else
		{
			LOG4CPLUS_WARN(pTestLogger,"Read the package failure,exit program.");
			m_xfunc_playerStatus("","error","Read the package failure,exit program.",m_xt_user);
			//m_xfunc_playerStatus("","info","receive thread exit...",m_xt_user);	
			m_xfunc_playerTip(m_xt_user,"Stop playing...");
			break;
		}				
	}
// 	LOG4CPLUS_WARN(pTestLogger,"�˳�����");
// 	break;
	m_xt_bexitflag=true;
	m_xt_connectSuccess=0;
	stopRecordVideo();
	m_xfunc_playerStatus("","info","receive thread exit...",m_xt_user);	
	m_xfunc_playerTip(m_xt_user,"stop playing.");			
	LOG4CPLUS_WARN(pTestLogger,"recv_loop2");
	//m_xfunc_playerStatus("","info:receiving thread end.",m_xt_user);
	//close();
}

bool vxRTSPClient::StringToWString(const std::string &str,std::wstring &wstr)
{  	
	int nLen = (int)str.length();    
	wstr.resize(nLen,L' ');
	int nResult = MultiByteToWideChar(CP_ACP,0,(LPCSTR)str.c_str(),nLen,(LPWSTR)wstr.c_str(),nLen);
	if (nResult == 0)
	{
		return FALSE;
	}	

	return true;
}
/**************************************************************************
* FunctionName:
*     realframe_proc
* Description:
*     ʵʱ����֡�ص����մ������̣����� realframe_cbk_entry() �ӿڻص��ò�������
*/
void vxRTSPClient::realframe_proc(x_handle_t xt_handle, x_handle_t xt_buf, x_uint32_t xt_size, x_uint32_t xt_type)
{		
}
