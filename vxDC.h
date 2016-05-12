#ifndef __VXDC_H__
#define __VXDC_H__

#include <windows.h>
#include "logging.h"

//#include <log4cxx/logger.h>
//#include <log4cxx/helpers/pool.h>
//#include <log4cxx/basicconfigurator.h>
//#include <log4cxx/fileappender.h>
//#include <log4cxx/simplelayout.h>
//#include <log4cxx/patternlayout.h>
//using namespace log4cxx;
//using namespace log4cxx::helpers;


///////////////////////////////////////////////////////////////////////////
// vxMemDC

class vxMemDC
{
	// constructor/destructor
public:
	vxMemDC(HDC hCompatibleDC, int cx, int cy, int nBits = 32);
	vxMemDC(void);
	virtual ~vxMemDC(void);

	// public interfaces
public:
	/**************************************************************************
	* Description:
	*     ���ݳ�ʼ���ӿڡ�
	*/
	bool Initial(HDC hCompatibleDC, int cx, int cy, int nBits = 32);

	/**************************************************************************
	* Description:
	*     �����ͷš�
	*/
	bool Release(void);

	/**************************************************************************
	* Description:
	*     �ڴ�DC�ĳߴ�����ȡ�
	*/
	int MaxCX(void) const;

	/**************************************************************************
	* Description:
	*     �ڴ�DC�ĳߴ����߶ȡ�
	*/
	int MaxCY(void) const;

	/**************************************************************************
	* Description:
	*     �����µ�λͼ������ڴ� DC �У����ؾɵ�λͼ�����
	*/
	HBITMAP Attach(HBITMAP hBitmap);

	/**************************************************************************
	* Description:
	*     ������ڴ�λͼ�����
	*/
	HBITMAP Detach(void);

	/**************************************************************************
	* Description:
	*     HDC ���������ء�
	*/
	operator HDC();
	operator HDC() const;

	/**************************************************************************
	* Description:
	*     RGB���ݡ�
	*/
	inline void * GetBits(void) { return m_pBits; }

	/**************************************************************************
	* Description:
	*     ����λ��24��32����
	*/
	inline int BitCount(void) const { return m_nBitCount; }

protected:
	HDC     m_hDC;
	HBITMAP m_hBitmap;
	HBITMAP m_hOldBitmap;
	void  * m_pBits;
	int     m_nBitCount;

	int     m_cxMaxDC;    // �ڴ�DC�ĳߴ������
	int     m_cyMaxDC;    // �ڴ�DC�ĳߴ����߶�

};

///////////////////////////////////////////////////////////////////////////
// vxPaintDC

class vxPaintDC
{
	// constructor/destructor
public:
	vxPaintDC(HDC& hDC, bool bIsDrawOnGlass = TRUE);
	virtual ~vxPaintDC();

	// class property
public:

	// public interfaces
public:
	void FillGradient(const LPRECT lpRect, COLORREF colorStart, COLORREF colorFinish,
					bool bHorz = TRUE, int nStartFlatPercentage = 0, int nEndFlatPercentage = 0);

	void FillGradient2(const LPRECT lpRect, COLORREF colorStart, COLORREF colorFinish,
					int nAngle = 0 /* 0 - 360 */);

	// self invoking methods
protected:
	void _FillGradient(const LPRECT lpRect, COLORREF colorStart, COLORREF colorFinish,
					bool bHorz = TRUE, int nStartFlatPercentage = 0, int nEndFlatPercentage = 0);

	// class data
protected:
	HDC   & m_hDC;
	bool    m_bIsDrawOnGlass;

};

///////////////////////////////////////////////////////////////////////////
// vxRgbPainter

class vxRgbPainter
{
	// constructor/destructor
public:
	vxRgbPainter(void);
	virtual ~vxRgbPainter(void);

	// class properties
public:
	enum SAVE_TYPE        ///< ͼ�񱣴��ʽ
	{
		STF_RGB  = 0,     ///< RGB λͼ��ʽ
		STF_PNG  = 1,     ///< PNG ͼ���ʽ
		STF_JPG  = 2,     ///< JPG ͼ���ʽ
	};

	// public interfaces
public:
	/**************************************************************************
	* FunctionName:
	*     Initial
	* Description:
	*     �����ʼ���ӿڡ�
	* Parameter:
	*     @[in ] cx: ���õĻ�ͼ��������ȡ�
	*     @[in ] cy: ���õĻ�ͼ�������߶ȡ�
	*     @[in ] nBits: ��ͼ���ر���λ������֧�� 24λ �� 32λ����
	* ReturnValue:
	*     �ɹ������� TRUE��ʧ�ܣ����� FALSE ��
	*/
	bool Initial(int cx, int cy, int nBits = 24);

	/**************************************************************************
	* FunctionName:
	*     Release
	* Description:
	*     ���������ͷŽӿڡ�
	*/
	void Release(void);

	/**************************************************************************
	* FunctionName:
	*     EraseBkgnd
	* Description:
	*     ʹ��ָ������ɫ����������
	* Parameter:
	*     @[in ] clrBkgnd: ָ���ı���ɫ��
	*     @[in ] lpRect: �������򣨴���ָ����Ϊ NULL����Ŀ������Ϊȫ���򣩡�
	* ReturnValue:
	*     �ɹ������� TRUE��ʧ�ܣ����� FALSE ��
	*/
	bool EraseBkgnd(COLORREF clrBkgnd, RECT lpRect);

	/**************************************************************************
	* FunctionName:
	*     DrawRgbImage
	* Description:
	*     ���� RGB ����ͼ��Ҫע�⣬����� RGB ���ݣ�����λ��Ҫ���ʼ��λ��һ�¡�
	* Parameter:
	*     @[in ] lpRgbData: RGB ͼ�����ݡ�
	*     @[in ] width: ͼ���ȡ�
	*     @[in ] height: ͼ��߶ȡ�
	*     @[in ] lpSrcRect: ͼ��Դ�������򣨴���ָ����Ϊ NULL����ȡͼ��Դ��ȫ���򣩡�
	*     @[in ] lpDstRect: Ŀ��������򣨴���ָ����Ϊ NULL����ȡ�ڴ� DC ��ȫ���򣩡�
	* ReturnValue:
	*     �ɹ������� TRUE��ʧ�ܣ����� FALSE ��
	*/
	bool DrawRgbImage(unsigned char * lpRgbData, int width, int height, RECT lpDstRect,int pwidth,int pheight);

	bool captureScreen(unsigned char * lpRgbData, int width, int height, RECT lpDstRect);

	/**************************************************************************
	* FunctionName:
	*     DrawRgbImageEx
	* Description:
	*     ���� RGB ����ͼ��Ҫע�⣬����� RGB ���ݣ�����λ��Ҫ���ʼ��λ��һ�¡�
	* Parameter:
	*     @[in ] lpRgbData: RGB ͼ�����ݡ�
	*     @[in ] width: ͼ���ȡ�
	*     @[in ] height: ͼ��߶ȡ�
	*     @[in ] lpSrcRect: ͼ��Դ�������򣨴���ָ����Ϊ NULL����ȡͼ��Դ��ȫ���򣩡�
	*     @[in ] lpDstRect: Ŀ��������򣨴���ָ����Ϊ NULL����ȡ�ڴ� DC ��ȫ���򣩡�
	* ReturnValue:
	*     �ɹ������� TRUE��ʧ�ܣ����� FALSE ��
	*/
	bool DrawRgbImageEx(unsigned char * lpRgbData, int width, int height, LPRECT lpSrcRect, LPRECT lpDstRect);

	/**************************************************************************
	* FunctionName:
	*     DrawTextInfo
	* Description:
	*     ���� �ı���Ϣ ��
	* Parameter:
	*     @[in ] szTextInfo: �ı���Ϣ��
	*     @[in ] clrText: �ı���ɫ��
	*     @[in ] hbrBkgnd: ������ˢ����Ϊ NULL���򱳾�͸������
	*     @[in ] lpDstRect: Ŀ���������
	* ReturnValue:
	*     �ɹ������� TRUE��ʧ�ܣ����� FALSE ��
	*/
	bool DrawTextInfo(LPCTSTR szTextInfo, COLORREF clrText, HBRUSH hbrBkgnd, LPRECT lpDstRect);

	/**************************************************************************
	* FunctionName:
	*     GetAreaRgbBits
	* Description:
	*     ��ȡָ������� RGB ͼ�����ݡ�
	* Parameter:
	*     @[out] lpOutBits: ��������Ļ��档
	*     @[in ] nOutBufLen: ��������Ļ��泤�ȡ�
	*     @[in ] lpRectArea: ָ����ȡ������
	* ReturnValue:
	*     �ɹ������� TRUE��ʧ�ܣ����� FALSE��
	*/
	bool GetAreaRgbBits(unsigned char * lpOutBits, int nOutBufLen, LPRECT lpRectArea);

	/**************************************************************************
	* FunctionName:
	*     SaveToFile
	* Description:
	*     ������Ƶ�ͼ�����ļ���
	* Parameter:
	*     @[in ] szFile: �ļ�����
	*     @[in ] fileType: ������ļ����͡�
	* ReturnValue:
	*     �ɹ������� TRUE��ʧ�ܣ����� FALSE ��
	*/
	bool SaveToFile(LPCTSTR szFile, SAVE_TYPE fileType);

	/**************************************************************************
	* FunctionName:
	*     IsInitial
	* Description:
	*     �Ƿ��ʼ����
	*/
	inline bool IsInitial(void) const
	{
		return ((NULL != (HDC)m_hMemDC) && (NULL != m_hDrawDIB));
	}

	/**************************************************************************
	* FunctionName:
	*     HandleDC
	* Description:
	*     ������ڴ� DC �����
	*/
	inline HDC HandleDC(void) { return (HDC)m_hMemDC; }

	/**************************************************************************
	* FunctionName:
	*     MemDC
	* Description:
	*     �ڴ� DC ����
	*/
	inline vxMemDC& MemDC(void) { return m_hMemDC; }

	/**************************************************************************
	* FunctionName:
	*     GetBits
	* Description:
	*     �����ڴ� DC ��������أ�RGB���ݣ��洢��ַ��
	*/
	inline unsigned char * GetBits(void) { return (unsigned char *)m_hMemDC.GetBits(); }

	/**************************************************************************
	* FunctionName:
	*     GetWidth
	* Description:
	*     �����ڴ� DC �����ͼ���ȡ�
	*/
	inline int GetWidth(void) const { return m_hMemDC.MaxCX(); }

	/**************************************************************************
	* FunctionName:
	*     GetHeight
	* Description:
	*     �����ڴ� DC �����ͼ��߶ȡ�
	*/
	inline int GetHeight(void) const { return m_hMemDC.MaxCY(); }

	// class data
protected:
	vxMemDC          m_hMemDC;         ///< ��ͼʹ�õ��ڴ� DC
	HANDLE           m_hDrawDIB;       ///< ��ͼ�� DIB ���
	BITMAPINFO       m_bmpInfo;        ///< λͼ�ṹ��Ϣ

};

#endif //__VXDC_H__


