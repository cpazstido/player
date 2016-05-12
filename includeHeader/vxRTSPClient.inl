///////////////////////////////////////////////////////////////////////////

#define RTSP_FRAMETYPE_RCDPARAM     101
#define RTSP_FRAMETYPE_REALH264     102

typedef void (* RTSP_REALFRAME_CBK)(void * pv_handle, void * frame_buf
	, unsigned int frame_size, void * pv_user);
typedef void (* CLI_REALFRAME_CBK)(void * pv_handle, void * frame_buf
	, unsigned int frame_size, unsigned int frame_type, void * pv_user);

