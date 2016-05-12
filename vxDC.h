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
	*     数据初始化接口。
	*/
	bool Initial(HDC hCompatibleDC, int cx, int cy, int nBits = 32);

	/**************************************************************************
	* Description:
	*     数据释放。
	*/
	bool Release(void);

	/**************************************************************************
	* Description:
	*     内存DC的尺寸最大宽度。
	*/
	int MaxCX(void) const;

	/**************************************************************************
	* Description:
	*     内存DC的尺寸最大高度。
	*/
	int MaxCY(void) const;

	/**************************************************************************
	* Description:
	*     附加新的位图句柄至内存 DC 中，返回旧的位图句柄。
	*/
	HBITMAP Attach(HBITMAP hBitmap);

	/**************************************************************************
	* Description:
	*     分离出内存位图句柄。
	*/
	HBITMAP Detach(void);

	/**************************************************************************
	* Description:
	*     HDC 操作符重载。
	*/
	operator HDC();
	operator HDC() const;

	/**************************************************************************
	* Description:
	*     RGB数据。
	*/
	inline void * GetBits(void) { return m_pBits; }

	/**************************************************************************
	* Description:
	*     像素位（24或32）。
	*/
	inline int BitCount(void) const { return m_nBitCount; }

protected:
	HDC     m_hDC;
	HBITMAP m_hBitmap;
	HBITMAP m_hOldBitmap;
	void  * m_pBits;
	int     m_nBitCount;

	int     m_cxMaxDC;    // 内存DC的尺寸最大宽度
	int     m_cyMaxDC;    // 内存DC的尺寸最大高度

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
	enum SAVE_TYPE        ///< 图像保存格式
	{
		STF_RGB  = 0,     ///< RGB 位图格式
		STF_PNG  = 1,     ///< PNG 图像格式
		STF_JPG  = 2,     ///< JPG 图像格式
	};

	// public interfaces
public:
	/**************************************************************************
	* FunctionName:
	*     Initial
	* Description:
	*     对象初始化接口。
	* Parameter:
	*     @[in ] cx: 设置的绘图区域最大宽度。
	*     @[in ] cy: 设置的绘图区域最大高度。
	*     @[in ] nBits: 绘图像素比特位数（仅支持 24位 和 32位）。
	* ReturnValue:
	*     成功，返回 TRUE；失败，返回 FALSE 。
	*/
	bool Initial(int cx, int cy, int nBits = 24);

	/**************************************************************************
	* FunctionName:
	*     Release
	* Description:
	*     对象数据释放接口。
	*/
	void Release(void);

	/**************************************************************************
	* FunctionName:
	*     EraseBkgnd
	* Description:
	*     使用指定背景色擦除背景。
	* Parameter:
	*     @[in ] clrBkgnd: 指定的背景色。
	*     @[in ] lpRect: 擦除区域（传参指针若为 NULL，则目标区域为全区域）。
	* ReturnValue:
	*     成功，返回 TRUE；失败，返回 FALSE 。
	*/
	bool EraseBkgnd(COLORREF clrBkgnd, RECT lpRect);

	/**************************************************************************
	* FunctionName:
	*     DrawRgbImage
	* Description:
	*     绘制 RGB 数据图像，要注意，输入的 RGB 数据，像素位数要与初始化位数一致。
	* Parameter:
	*     @[in ] lpRgbData: RGB 图像数据。
	*     @[in ] width: 图像宽度。
	*     @[in ] height: 图像高度。
	*     @[in ] lpSrcRect: 图像源矩形区域（传参指针若为 NULL，则取图像源的全区域）。
	*     @[in ] lpDstRect: 目标矩形区域（传参指针若为 NULL，则取内存 DC 的全区域）。
	* ReturnValue:
	*     成功，返回 TRUE；失败，返回 FALSE 。
	*/
	bool DrawRgbImage(unsigned char * lpRgbData, int width, int height, RECT lpDstRect,int pwidth,int pheight);

	bool captureScreen(unsigned char * lpRgbData, int width, int height, RECT lpDstRect);

	/**************************************************************************
	* FunctionName:
	*     DrawRgbImageEx
	* Description:
	*     绘制 RGB 数据图像，要注意，输入的 RGB 数据，像素位数要与初始化位数一致。
	* Parameter:
	*     @[in ] lpRgbData: RGB 图像数据。
	*     @[in ] width: 图像宽度。
	*     @[in ] height: 图像高度。
	*     @[in ] lpSrcRect: 图像源矩形区域（传参指针若为 NULL，则取图像源的全区域）。
	*     @[in ] lpDstRect: 目标矩形区域（传参指针若为 NULL，则取内存 DC 的全区域）。
	* ReturnValue:
	*     成功，返回 TRUE；失败，返回 FALSE 。
	*/
	bool DrawRgbImageEx(unsigned char * lpRgbData, int width, int height, LPRECT lpSrcRect, LPRECT lpDstRect);

	/**************************************************************************
	* FunctionName:
	*     DrawTextInfo
	* Description:
	*     绘制 文本信息 。
	* Parameter:
	*     @[in ] szTextInfo: 文本信息。
	*     @[in ] clrText: 文本颜色。
	*     @[in ] hbrBkgnd: 背景画刷（若为 NULL，则背景透明）。
	*     @[in ] lpDstRect: 目标矩形区域。
	* ReturnValue:
	*     成功，返回 TRUE；失败，返回 FALSE 。
	*/
	bool DrawTextInfo(LPCTSTR szTextInfo, COLORREF clrText, HBRUSH hbrBkgnd, LPRECT lpDstRect);

	/**************************************************************************
	* FunctionName:
	*     GetAreaRgbBits
	* Description:
	*     获取指定区域的 RGB 图像数据。
	* Parameter:
	*     @[out] lpOutBits: 数据输出的缓存。
	*     @[in ] nOutBufLen: 数据输出的缓存长度。
	*     @[in ] lpRectArea: 指定获取的区域。
	* ReturnValue:
	*     成功，返回 TRUE；失败，返回 FALSE。
	*/
	bool GetAreaRgbBits(unsigned char * lpOutBits, int nOutBufLen, LPRECT lpRectArea);

	/**************************************************************************
	* FunctionName:
	*     SaveToFile
	* Description:
	*     保存绘制的图像至文件。
	* Parameter:
	*     @[in ] szFile: 文件名。
	*     @[in ] fileType: 保存的文件类型。
	* ReturnValue:
	*     成功，返回 TRUE；失败，返回 FALSE 。
	*/
	bool SaveToFile(LPCTSTR szFile, SAVE_TYPE fileType);

	/**************************************************************************
	* FunctionName:
	*     IsInitial
	* Description:
	*     是否初始化。
	*/
	inline bool IsInitial(void) const
	{
		return ((NULL != (HDC)m_hMemDC) && (NULL != m_hDrawDIB));
	}

	/**************************************************************************
	* FunctionName:
	*     HandleDC
	* Description:
	*     对象的内存 DC 句柄。
	*/
	inline HDC HandleDC(void) { return (HDC)m_hMemDC; }

	/**************************************************************************
	* FunctionName:
	*     MemDC
	* Description:
	*     内存 DC 对象。
	*/
	inline vxMemDC& MemDC(void) { return m_hMemDC; }

	/**************************************************************************
	* FunctionName:
	*     GetBits
	* Description:
	*     返回内存 DC 对象的像素（RGB数据）存储地址。
	*/
	inline unsigned char * GetBits(void) { return (unsigned char *)m_hMemDC.GetBits(); }

	/**************************************************************************
	* FunctionName:
	*     GetWidth
	* Description:
	*     返回内存 DC 对象的图像宽度。
	*/
	inline int GetWidth(void) const { return m_hMemDC.MaxCX(); }

	/**************************************************************************
	* FunctionName:
	*     GetHeight
	* Description:
	*     返回内存 DC 对象的图像高度。
	*/
	inline int GetHeight(void) const { return m_hMemDC.MaxCY(); }

	// class data
protected:
	vxMemDC          m_hMemDC;         ///< 绘图使用的内存 DC
	HANDLE           m_hDrawDIB;       ///< 绘图的 DIB 句柄
	BITMAPINFO       m_bmpInfo;        ///< 位图结构信息

};

#endif //__VXDC_H__


