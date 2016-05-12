#include "vxDC.h"

#include <assert.h>
#include <math.h>
#include <atlimage.h>
#include <algorithm>
#define DRAWDIB_INCLUDE_STRETCHDIB
#include <Vfw.h>

#include <log4cplus/loggingmacros.h>
extern __declspec(dllimport) log4cplus::Logger pTestLogger;

#pragma comment(lib, "vfw32.lib")

///////////////////////////////////////////////////////////////////////////

#define PI     3.141593

//extern int width,height;//窗口的宽度和高度

///////////////////////////////////////////////////////////////////////////

inline int nbytes_line(int widthpix, int nBits)
{
	return (int)(((widthpix * nBits) + 31) / 32 * 4);
}

static HBITMAP vxCreateBitmap(int cx, int cy, int nBits, void** pBits)
{
	assert(0 < cx);
	assert(0 != cy);

	if (pBits != NULL)
	{
		*pBits = NULL;
	}

	if ((cx <= 0) || (cy == 0))
	{
		return NULL;
	}

	BITMAPINFO bi = { 0 };

	// Fill in the BITMAPINFOHEADER
	bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth       = cx;
	bi.bmiHeader.biHeight      = cy;
	bi.bmiHeader.biSizeImage   = 0;
	bi.bmiHeader.biPlanes      = 1;
	bi.bmiHeader.biBitCount    = nBits;
	bi.bmiHeader.biCompression = BI_RGB;

	LPVOID pData = NULL;
	HBITMAP hbmp = ::CreateDIBSection(NULL, &bi, DIB_RGB_COLORS, &pData, NULL, 0);

	if ((NULL != pData) && (NULL != hbmp) && (NULL != pBits))
	{
		*pBits = pData;
	}

	return hbmp;
}

///////////////////////////////////////////////////////////////////////////
// vxMemDC

//=========================================================================

//
// constructor/destructor
//

vxMemDC::vxMemDC(HDC hCompatibleDC, int cx, int cy, int nBits /*= 32*/)
		: m_hDC(NULL)
		, m_hBitmap(NULL)
		, m_hOldBitmap(NULL)
		, m_pBits(NULL)
		, m_nBitCount(0)
		, m_cxMaxDC(0)
		, m_cyMaxDC(0)
{
	bool bInit = Initial(hCompatibleDC, cx, cy, nBits);
	assert(bInit);
}

vxMemDC::vxMemDC(void)
		: m_hDC(NULL)
		, m_hBitmap(NULL)
		, m_hOldBitmap(NULL)
		, m_pBits(NULL)
		, m_cxMaxDC(0)
		, m_cyMaxDC(0)
{

}

vxMemDC::~vxMemDC(void)
{
	Release();
}

//=========================================================================

//
// public interfaces
//

/**************************************************************************
* Description:
*     数据初始化接口。
*/
bool vxMemDC::Initial(HDC hCompatibleDC, int cx, int cy, int nBits /*= 32*/)
{
	if (NULL != m_hDC)
		return FALSE;
	if (NULL != m_hBitmap)
		return FALSE;

	if (!((24 == nBits) || (32 == nBits)))
		return FALSE;
	assert((24 == nBits) || (32 == nBits));
	m_nBitCount = nBits;

	m_cxMaxDC = cx;
	m_cyMaxDC = cy;

	m_hDC = ::CreateCompatibleDC(hCompatibleDC);
	assert(NULL != m_hDC);

	if (NULL == hCompatibleDC)
	{
		m_hBitmap = vxCreateBitmap(m_cxMaxDC, m_cyMaxDC, nBits, (LPVOID*)&m_pBits);
	}
	else
	{
		m_hBitmap = ::CreateCompatibleBitmap(hCompatibleDC, m_cxMaxDC, m_cyMaxDC);
	}
	assert(NULL != m_hBitmap);

	m_hOldBitmap = (HBITMAP)::SelectObject(m_hDC, (HGDIOBJ)m_hBitmap);

	return TRUE;
}

/**************************************************************************
* Description:
*     数据释放。
*/
bool vxMemDC::Release(void)
{
	if (NULL != m_hDC)
	{
		HBITMAP hBitmap = Detach();
		if (NULL != hBitmap)
		{
			::DeleteObject((HGDIOBJ)hBitmap);
			m_pBits = NULL;
			m_nBitCount = 0;
		}

		::DeleteDC(m_hDC);

		m_hDC = NULL;
		m_hBitmap = NULL;
		m_hOldBitmap = NULL;

		m_cxMaxDC = 0;
		m_cyMaxDC = 0;
	}

	return TRUE;
}

/**************************************************************************
* Description:
*     内存DC的尺寸最大宽度。
*/
int vxMemDC::MaxCX(void) const
{
	return (std::abs)(m_cxMaxDC);
}

/**************************************************************************
* Description:
*     内存DC的尺寸最大高度。
*/
int vxMemDC::MaxCY(void) const
{
	return (std::abs)(m_cyMaxDC);
}

/**************************************************************************
* Description:
*     附加新的位图句柄至内存 DC 中，返回旧的位图句柄。
*/
HBITMAP vxMemDC::Attach(HBITMAP hBitmap)
{
	assert(NULL != m_hDC);
	assert(NULL != hBitmap);

	HBITMAP hOldBitmap = Detach();
	m_hBitmap = hBitmap;
	m_hOldBitmap = (HBITMAP)::SelectObject(m_hDC, (HGDIOBJ)m_hBitmap);

	return m_hOldBitmap;
}

/**************************************************************************
* Description:
*     分离出内存位图句柄。
*/
HBITMAP vxMemDC::Detach(void)
{
	assert(NULL != m_hDC);

	::SelectObject(m_hDC, (HGDIOBJ)m_hOldBitmap);
	HBITMAP hDetachBitmap = m_hBitmap;
	m_hBitmap = NULL;
	m_hOldBitmap = NULL;

	return hDetachBitmap;
}

/**************************************************************************
* Description:
*     HDC 操作符重载。
*/
vxMemDC::operator HDC()
{
	return m_hDC;
}

vxMemDC::operator HDC() const
{
	return m_hDC;
}

///////////////////////////////////////////////////////////////////////////
// vxPaintDC

//=========================================================================

//
// constructor/destructor
//


vxPaintDC::vxPaintDC(HDC& hDC, bool bIsDrawOnGlass /* = TRUE */)
		: m_hDC(hDC)
		, m_bIsDrawOnGlass(bIsDrawOnGlass)
{

}

vxPaintDC::~vxPaintDC()
{

}

//=========================================================================

//
// self invoking methods
//

void vxPaintDC::_FillGradient(const LPRECT lpRect, COLORREF colorStart, COLORREF colorFinish,
			bool bHorz /* = TRUE */, int nStartFlatPercentage /* = 0 */, int nEndFlatPercentage /* = 0 */)
{
	if (NULL == lpRect)
		return;

	if (colorStart == colorFinish)
	{
		HBRUSH hbr = ::CreateSolidBrush(colorStart);
		assert(NULL != hbr);
		::FillRect(m_hDC, lpRect, hbr);
		::DeleteObject((HGDIOBJ)hbr);
		return;
	}

	if (nStartFlatPercentage > 0)
	{
		assert(nStartFlatPercentage <= 100);

		if (bHorz)
		{
			RECT rectTop;
			memcpy(&rectTop, lpRect, sizeof(RECT));
			rectTop.bottom = rectTop.top + (rectTop.bottom - rectTop.top) * nStartFlatPercentage / 100;
			lpRect->top = rectTop.bottom;

			HBRUSH hbr = ::CreateSolidBrush(colorFinish);
			assert(NULL != hbr);
			::FillRect(m_hDC, &rectTop, hbr);
			::DeleteObject((HGDIOBJ)hbr);
		}
		else
		{
			RECT rectLeft;
			memcpy(&rectLeft, lpRect, sizeof(RECT));
			rectLeft.right = rectLeft.left + (rectLeft.right - rectLeft.left) * nStartFlatPercentage / 100;
			lpRect->left = rectLeft.right;

			HBRUSH hbr = ::CreateSolidBrush(colorStart);
			assert(NULL != hbr);
			::FillRect(m_hDC, &rectLeft, hbr);
			::DeleteObject((HGDIOBJ)hbr);
		}
	}

	if (nEndFlatPercentage > 0)
	{
		assert(nEndFlatPercentage <= 100);

		if (bHorz)
		{
			RECT rectBottom;
			memcpy(&rectBottom, lpRect, sizeof(RECT));
			rectBottom.top = rectBottom.bottom - (rectBottom.bottom - rectBottom.top) * nEndFlatPercentage / 100;
			lpRect->bottom = rectBottom.top;

			HBRUSH hbr = ::CreateSolidBrush(colorStart);
			assert(NULL != hbr);
			::FillRect(m_hDC, &rectBottom, hbr);
			::DeleteObject((HGDIOBJ)hbr);
		}
		else
		{
			RECT rectRight;
			memcpy(&rectRight, lpRect, sizeof(RECT));
			rectRight.left = rectRight.right - (rectRight.right - rectRight.left) * nEndFlatPercentage / 100;
			lpRect->right = rectRight.left;

			HBRUSH hbr = ::CreateSolidBrush(colorFinish);
			assert(NULL != hbr);
			::FillRect(m_hDC, &rectRight, hbr);
			::DeleteObject((HGDIOBJ)hbr);
		}
	}

	if (nEndFlatPercentage + nStartFlatPercentage > 100)
	{
		assert(FALSE);
		return;
	}

	// this will make 2^6 = 64 fountain steps
	int nShift = 6;
	int nSteps = 1 << nShift;

	for (int i = 0; i < nSteps; i++)
	{
		// do a little alpha blending
		BYTE bR = (BYTE)((GetRValue(colorStart) * (nSteps - i) + GetRValue(colorFinish) * i) >> nShift);
		BYTE bG = (BYTE)((GetGValue(colorStart) * (nSteps - i) + GetGValue(colorFinish) * i) >> nShift);
		BYTE bB = (BYTE)((GetBValue(colorStart) * (nSteps - i) + GetBValue(colorFinish) * i) >> nShift);

		HBRUSH hbr = ::CreateSolidBrush(RGB(bR, bG, bB));

		// then paint with the resulting color
		RECT r2;
		memcpy(&r2, lpRect, sizeof(RECT));
		if (bHorz)
		{
			r2.bottom = lpRect->bottom - ((i * (lpRect->bottom - lpRect->top)) >> nShift);
			r2.top = lpRect->bottom - (((i + 1) * (lpRect->bottom - lpRect->top)) >> nShift);
			if ((r2.bottom - r2.top) > 0)
			{
				::FillRect(m_hDC, &r2, hbr);
			}
		}
		else
		{
			r2.left = lpRect->left + ((i * (lpRect->right - lpRect->left)) >> nShift);
			r2.right = lpRect->left + (((i + 1) * (lpRect->right - lpRect->left)) >> nShift);
			if ((r2.right - r2.left) > 0)
			{
				::FillRect(m_hDC, &r2, hbr);
			}
		}

		::DeleteObject((HGDIOBJ)hbr);
	}
}

//=========================================================================

//
// public interfaces
//

void vxPaintDC::FillGradient(const LPRECT lpRect, COLORREF colorStart, COLORREF colorFinish,
				bool bHorz /* = TRUE */, int nStartFlatPercentage /* = 0 */, int nEndFlatPercentage /* = 0 */)
{
	if (NULL == lpRect)
		return;

	if (!m_bIsDrawOnGlass)
	{
		_FillGradient(lpRect, colorStart, colorFinish, bHorz, nStartFlatPercentage, nEndFlatPercentage);
	}
	else
	{
		RECT rt;
		memcpy(&rt, lpRect, sizeof(RECT));

		SIZE size;
		size.cx = abs(rt.right - rt.left);
		size.cy = abs(rt.bottom - rt.top);
		if (size.cx == 0 || size.cy == 0)
		{
			return;
		}

		//--------------------------------------------
		// Copy screen content into the memory bitmap:
		//--------------------------------------------
		vxMemDC dcMem(m_hDC, size.cx, size.cy);

		COLORREF* pBits;
		HBITMAP hmbpDib = vxCreateBitmap(size.cx, size.cy, 32, (LPVOID*)&pBits);

		if (hmbpDib == NULL || pBits == NULL)
		{
			assert(FALSE);
			return;
		}

		::SelectObject(dcMem, (HGDIOBJ)hmbpDib);

		HDC hMemDC = dcMem;
		vxPaintDC dm(hMemDC);
		RECT rcPaint;
		rcPaint.left = 0; rcPaint.top = 0; rcPaint.right = size.cx; rcPaint.bottom = size.cy;
		dm._FillGradient(&rcPaint, colorStart, colorFinish, bHorz, nStartFlatPercentage, nEndFlatPercentage);

		int sizeImage = size.cx * size.cy;
		for (int i = 0; i < sizeImage; i++)
		{
			*pBits |= 0xFF000000;
			pBits++;
		}

		//--------------------------------
		// Copy bitmap back to the screen:
		//--------------------------------
		::BitBlt(m_hDC, rt.left, rt.top, size.cx, size.cy, dcMem, 0, 0, SRCCOPY);

		::DeleteObject(hmbpDib);
	}
}

void vxPaintDC::FillGradient2(const LPRECT lpRect, COLORREF colorStart, COLORREF colorFinish, int nAngle /* = 0 */)
{
	if (NULL == lpRect)
	{
		return;
	}

	if (((lpRect->right - lpRect->left) <= 0) || ((lpRect->bottom - lpRect->top) <= 0))
	{
		return;
	}

	if (colorStart == colorFinish)
	{
		HBRUSH hbr = ::CreateSolidBrush(colorStart);
		assert(NULL != hbr);
		::FillRect(m_hDC, lpRect, hbr);
		::DeleteObject((HGDIOBJ)hbr);
		return;
	}

	//----------------------
	// Process simple cases:
	//----------------------
	switch (nAngle)
	{
	case 0:
	case 360:
		FillGradient(lpRect, colorStart, colorFinish, FALSE);
		return;

	case 90:
		FillGradient(lpRect, colorStart, colorFinish, TRUE);
		return;

	case 180:
		FillGradient(lpRect, colorFinish, colorStart, FALSE);
		return;

	case 270:
		FillGradient(lpRect, colorFinish, colorStart, TRUE);
		return;
	}

	//--------------------------------------------
	// Copy screen content into the memory bitmap:
	//--------------------------------------------
	vxMemDC dcMem(m_hDC, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top);

	HPEN hOldPen = (HPEN)::SelectObject(dcMem, (HGDIOBJ)::GetStockObject(NULL_PEN));

	int nShift = 6;
	int nSteps = 1 << nShift;

	const double fAngle = PI * (nAngle + 180) / 180;
	const int nOffset = (int) (cos(fAngle) * (lpRect->bottom - lpRect->top));
	const int nTotalWidth = (lpRect->right - lpRect->left) + abs(nOffset);

	const int xStart = nOffset > 0 ? - nOffset : 0;

	for (int i = 0; i < nSteps; i++)
	{
		// do a little alpha blending
		BYTE bR = (BYTE)((GetRValue(colorStart) * (nSteps - i) + GetRValue(colorFinish) * i) >> nShift);
		BYTE bG = (BYTE)((GetGValue(colorStart) * (nSteps - i) + GetGValue(colorFinish) * i) >> nShift);
		BYTE bB = (BYTE)((GetBValue(colorStart) * (nSteps - i) + GetBValue(colorFinish) * i) >> nShift);

		HBRUSH hbr = ::CreateSolidBrush(RGB(bR, bG, bB));

		int x11 = xStart + ((i * nTotalWidth) >> nShift);
		int x12 = xStart + (((i + 1) * nTotalWidth) >> nShift);

		if (x11 == x12)
		{
			continue;
		}

		int x21 = x11 + nOffset;
		int x22 = x21 + (x12 - x11);

		POINT points[4];
		points[0].x = x11;
		points[0].y = 0;
		points[1].x = x12;
		points[1].y = 0;
		points[2].x = x22;
		points[2].y = (lpRect->bottom - lpRect->top);
		points[3].x = x21;
		points[3].y = (lpRect->bottom - lpRect->top);

		HBRUSH hOldBrush = (HBRUSH)::SelectObject(dcMem, (HGDIOBJ)hbr);
		::Polygon(dcMem, points, 4);
		::SelectObject(dcMem, (HGDIOBJ)hOldBrush);

		::DeleteObject((HGDIOBJ)hbr);
	}

	::SelectObject(dcMem, hOldPen);

	//--------------------------------
	// Copy bitmap back to the screen:
	//--------------------------------
	::BitBlt(m_hDC, lpRect->left, lpRect->top, (lpRect->right - lpRect->left), (lpRect->bottom - lpRect->top), dcMem, 0, 0, SRCCOPY);
}

///////////////////////////////////////////////////////////////////////////
// vxRgbPainter

//=========================================================================

//
// vxRgbPainter constructor/destructor
//

vxRgbPainter::vxRgbPainter(void)
{
	m_hDrawDIB = NULL;
	memset(&m_bmpInfo, 0, sizeof(BITMAPINFO));
}

vxRgbPainter::~vxRgbPainter(void)
{
	Release();
}

//=========================================================================

//
// vxRgbPainter public interfaces
//

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
bool vxRgbPainter::Initial(int cx, int cy, int nBits /*= 24*/)
{
	bool bRet = FALSE;

	do
	{
		// 初始化内存 DC
		if (!m_hMemDC.Initial(NULL, cx, cy, nBits)){
			printf("m_hMemDC.Initial is false\n");
			break;
		}

		// 设置位图头结构信息
		memset(&m_bmpInfo, 0, sizeof(BITMAPINFO));
		m_bmpInfo.bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
		m_bmpInfo.bmiHeader.biWidth         = cx;
		m_bmpInfo.bmiHeader.biHeight        = cy;
		m_bmpInfo.bmiHeader.biPlanes        = 1;
		m_bmpInfo.bmiHeader.biBitCount      = nBits;
		m_bmpInfo.bmiHeader.biCompression   = BI_RGB;
		m_bmpInfo.bmiHeader.biSizeImage     = 0;
		m_bmpInfo.bmiHeader.biXPelsPerMeter = 0;
		m_bmpInfo.bmiHeader.biYPelsPerMeter = 0;
		m_bmpInfo.bmiHeader.biClrUsed       = 0;
		m_bmpInfo.bmiHeader.biClrImportant  = 0;

		// 使用 VFW 的 API 绘制 RGB 数据
		m_hDrawDIB = ::DrawDibOpen();
		if (NULL == m_hDrawDIB)
			break;

		bRet = TRUE;

	} while (0);

	return bRet;
}

/**************************************************************************
* FunctionName:
*     Release
* Description:
*     对象数据释放接口。
*/
void vxRgbPainter::Release(void)
{
	if (NULL != m_hDrawDIB)
	{
		::DrawDibEnd(m_hDrawDIB);
		::DrawDibClose(m_hDrawDIB);
	}

	m_hMemDC.Release();
}

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
bool vxRgbPainter::EraseBkgnd(COLORREF clrBkgnd, RECT lpRect)
{
	bool bRet = FALSE;

	do
	{
		// 初始化判断
		if (!IsInitial())
		{
			break;
		}

		/*
		RECT rect;
		rect.left   = (NULL == lpRect) ? 0 : lpRect->left;
		rect.top    = (NULL == lpRect) ? 0 : lpRect->top;
		rect.right  = (NULL == lpRect) ? m_hMemDC.MaxCX() : lpRect->right;
		rect.bottom = (NULL == lpRect) ? m_hMemDC.MaxCY() : lpRect->bottom;
		*/

		COLORREF clrOldBk = ::SetBkColor(HandleDC(), clrBkgnd);
		::ExtTextOut(HandleDC(), 0, 0, ETO_OPAQUE, &lpRect, NULL, 0, NULL);
		::SetBkColor(HandleDC(), clrOldBk);

		bRet = TRUE;

	} while (0);

	return bRet;
}

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
bool vxRgbPainter::DrawRgbImage(unsigned char * lpRgbData, int width, int height, RECT lpDstRect,int pwidth,int pheight)
{
	//LOG4CPLUS_WARN(pTestLogger,"begin............画实时图");
	bool bRet = FALSE;

	do
	{
		// 初始化判断
		if (!IsInitial())
		{
			break;
		}

		// 输入数据有效性判断
		if (NULL == lpRgbData)
		{
			break;
		}

		// 构建绘图的图像信息
		BITMAPINFO biRGB;
		memcpy(&biRGB, &m_bmpInfo, sizeof(BITMAPINFO));
		biRGB.bmiHeader.biWidth  = width;
		biRGB.bmiHeader.biHeight = height;

		// 绘图流程
		HDC hMemDC = HandleDC();
		bRet = ::DrawDibBegin(m_hDrawDIB, hMemDC, width, height, &biRGB.bmiHeader, width, height, DDF_BUFFER);

		// 目标绘图坐标
		/*
		int xDst  = (NULL == lpDstRect) ? 0           : lpDstRect->left;
		int yDst  = (NULL == lpDstRect) ? 0           : lpDstRect->top;
		int dxDst = (NULL == lpDstRect) ? GetWidth()  : (lpDstRect->right - lpDstRect->left);
		int dyDst = (NULL == lpDstRect) ? GetHeight() : (lpDstRect->bottom - lpDstRect->top);
		*/
		int xDst  = 0;
		int yDst  = 0;
		int dxDst = pwidth;
		int dyDst = pheight;
		// 源图像坐标
		/*
		int xSrc  = (NULL == lpSrcRect) ? 0      : lpSrcRect->left;
		int ySrc  = (NULL == lpSrcRect) ? 0      : lpSrcRect->top;
		int dxSrc = (NULL == lpSrcRect) ? width  : (lpSrcRect->right - lpSrcRect->left);
		int dySrc = (NULL == lpSrcRect) ? height : (lpSrcRect->bottom - lpSrcRect->top);
		*/
		int xSrc  = 0;
		int ySrc  = 0;
		int dxSrc = width;
		int dySrc = height;

		bRet &= ::DrawDibDraw(m_hDrawDIB, hMemDC, xDst, yDst, dxDst, dyDst,
						&biRGB.bmiHeader, lpRgbData, xSrc, ySrc, dxSrc, dySrc, DDF_SAME_HDC);
		if (!bRet)
		{
			int nError = GetLastError();
		}

		bRet &= ::DrawDibEnd(m_hDrawDIB);

	} while (0);

	//LOG4CPLUS_WARN(pTestLogger,"end............画实时图");
	return bRet;
}



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
bool vxRgbPainter::DrawRgbImageEx(unsigned char * lpRgbData, int width, int height, LPRECT lpSrcRect, LPRECT lpDstRect)
{
	bool bRet = FALSE;

	do
	{
		// 初始化判断
		if (!IsInitial())
		{
			break;
		}

		// 输入数据有效性判断
		if (NULL == lpRgbData)
		{
			break;
		}

		// 构建绘图的图像信息
		BITMAPINFO biRGB;
		memcpy(&biRGB, &m_bmpInfo, sizeof(BITMAPINFO));
		biRGB.bmiHeader.biWidth  = width;
		biRGB.bmiHeader.biHeight = height;

		// 目标绘图坐标
		int xDst  = (NULL == lpDstRect) ? 0           : lpDstRect->left;
		int yDst  = (NULL == lpDstRect) ? 0           : lpDstRect->top;
		int dxDst = (NULL == lpDstRect) ? GetWidth()  : (lpDstRect->right - lpDstRect->left);
		int dyDst = (NULL == lpDstRect) ? GetHeight() : (lpDstRect->bottom - lpDstRect->top);
		// 源图像坐标
		int xSrc  = (NULL == lpSrcRect) ? 0      : lpSrcRect->left;
		int ySrc  = (NULL == lpSrcRect) ? 0      : lpSrcRect->top;
		int dxSrc = (NULL == lpSrcRect) ? width  : (lpSrcRect->right - lpSrcRect->left);
		int dySrc = (NULL == lpSrcRect) ? (std::abs)(height) : (lpSrcRect->bottom - lpSrcRect->top);

		StretchDIB(&m_bmpInfo.bmiHeader, GetBits(), xDst, yDst, dxDst, dyDst, &biRGB.bmiHeader, lpRgbData, xSrc, ySrc, dxSrc, dySrc);
		bRet = TRUE;

	} while (0);

	return bRet;
}

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
bool vxRgbPainter::DrawTextInfo(LPCTSTR szTextInfo, COLORREF clrText, HBRUSH hbrBkgnd, LPRECT lpDstRect)
{
	bool bRet = FALSE;

	do
	{
		// 初始化判断
		if (!IsInitial())
		{
			break;
		}

		// 输入数据有效性判断
		if ((NULL == szTextInfo) || (_tcslen(szTextInfo) <= 0) || (NULL == lpDstRect))
		{
			break;
		}

		// 绘制文本信息流程
		HDC hMemDC = HandleDC();
		bool bTransparent = (NULL == hbrBkgnd);
		HGDIOBJ  hOldBrush  = SelectObject(hMemDC, bTransparent ? GetStockObject(NULL_BRUSH) : (HGDIOBJ)hbrBkgnd);
		int      nOldBkMode = bTransparent ? SetBkMode(hMemDC, TRANSPARENT) : GetBkMode(hMemDC);
		COLORREF clrOldText = SetTextColor(hMemDC, clrText);

		int cchText = _tcslen(szTextInfo);
		DrawText(hMemDC, szTextInfo, cchText, lpDstRect, DT_LEFT | DT_CALCRECT);
		DrawText(hMemDC, szTextInfo, cchText, lpDstRect, DT_LEFT);

		SelectObject(hMemDC, hOldBrush);
		SetBkMode(hMemDC, nOldBkMode);
		SetTextColor(hMemDC, clrOldText);

		bRet = TRUE;
	} while (0);

	return bRet;
}

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
bool vxRgbPainter::GetAreaRgbBits(unsigned char * lpOutBits, int nOutBufLen, LPRECT lpRectArea)
{
	bool bRet = FALSE;

	do
	{
		// 参数有效判断
		if ((NULL == lpOutBits) || (nOutBufLen <= 0) || (NULL == lpRectArea))
		{
			break;
		}

		int cx = lpRectArea->right - lpRectArea->left;
		int cy = lpRectArea->bottom - lpRectArea->top;
		if ((cx <= 0) || (cy <= 0))
		{
			break;
		}

		// 缓存大小有效判断
		int dst_bytesline = nbytes_line(cx, MemDC().BitCount());
		if (nOutBufLen < (dst_bytesline * cy))
		{
			break;
		}

		// 区域有效判断
		RECT rectSRC = { 0, 0, MemDC().MaxCX(), MemDC().MaxCY() };
		POINT ptLT = { lpRectArea->left, lpRectArea->top };
		POINT ptBR = { lpRectArea->right, lpRectArea->bottom };
		if (!PtInRect(&rectSRC, ptLT) || !PtInRect(&rectSRC, ptBR))
		{
			break;
		}

		// 执行拷贝操作
		BYTE * lpBits = GetBits();
		int src_bytesline = nbytes_line(GetWidth(), MemDC().BitCount());
		int src_bytesleft = lpRectArea->left * MemDC().BitCount() / 8;
		for (int i = lpRectArea->top; i < lpRectArea->bottom; ++i)
		{
			BYTE * srcBits = lpBits + src_bytesline * i + src_bytesleft;
			BYTE * dstBits = lpOutBits + dst_bytesline * (i - lpRectArea->top);

			memcpy(dstBits, srcBits, dst_bytesline);
		}

		bRet = TRUE;
	} while (0);

	return bRet;
}

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
bool vxRgbPainter::SaveToFile(LPCTSTR szFile, SAVE_TYPE fileType)
{
	bool bRet = FALSE;

	do
	{
		// 初始化判断
		if (!IsInitial())
		{
			break;
		}

		// 使用 CImage 类的文件保存接口
		CImage img;
		img.Attach(m_hMemDC.Detach());

		HRESULT hr = E_FAIL;
		switch (fileType)
		{
		case STF_RGB: hr = img.Save(szFile, Gdiplus::ImageFormatBMP);  break;
		case STF_PNG: hr = img.Save(szFile, Gdiplus::ImageFormatPNG);  break;
		case STF_JPG: hr = img.Save(szFile, Gdiplus::ImageFormatJPEG); break;
		default: break;
		}

		m_hMemDC.Attach(img.Detach());

		bRet = SUCCEEDED(hr);
	} while (0);

	return bRet;
}


//************************************
// 方法:    captureScreen
// 全名:  captureScreen
// 访问类型:    public 
// 返回值:   bool
// 描述:  截屏
// 参数: unsigned char * lpRgbData
// 参数: int width
// 参数: int height
// 参数: RECT lpDstRect
//************************************
bool captureScreen(unsigned char * lpRgbData, int width, int height, RECT lpDstRect)
{	   
	bool bRet = FALSE;

	//do
	//{
	//	// 初始化判断
	//	if (!IsInitial())
	//	{
	//		break;
	//	}

	//	// 输入数据有效性判断
	//	if (NULL == lpRgbData)
	//	{
	//		break;
	//	}

	//	// 构建绘图的图像信息
	//	BITMAPINFO biRGB;
	//	memcpy(&biRGB, &m_bmpInfo, sizeof(BITMAPINFO));
	//	biRGB.bmiHeader.biWidth  = width;
	//	biRGB.bmiHeader.biHeight = height;

	//	// 绘图流程
	//	HDC hMemDC = HandleDC();
	//	bRet = ::DrawDibBegin(m_hDrawDIB, hMemDC, width, height, &biRGB.bmiHeader, width, height, DDF_BUFFER);

	//	// 目标绘图坐标

	//	int xDst  = 0;
	//	int yDst  = 0;
	//	int dxDst = 704;
	//	int dyDst = 576;
	//	// 源图像坐标

	//	int xSrc  = 0;
	//	int ySrc  = 0;
	//	int dxSrc = width;
	//	int dySrc = height;

	//	bRet &= ::DrawDibDraw(m_hDrawDIB, hMemDC, xDst, yDst, dxDst, dyDst,
	//		&biRGB.bmiHeader, lpRgbData, xSrc, ySrc, dxSrc, dySrc, DDF_SAME_HDC);
	//	if (!bRet)
	//	{
	//		int nError = GetLastError();
	//	}

	//	bRet &= ::DrawDibEnd(m_hDrawDIB);

	//} while (0);

	return bRet;
}

