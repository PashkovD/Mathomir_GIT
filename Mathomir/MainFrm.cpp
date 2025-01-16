/*****************************************  The MIT License ***********************************************

Copyright  2017, Danijel Gorupec

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and 
associated documentation files (the "Software"), to deal in the Software without restriction, including 
without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the
following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT 
LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN 
NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*********************************************************************************************************/

#include "stdafx.h"
#include "Mathomir.h"

#include "MainFrm.h"
#include "./mainfrm.h"
#include "toolbox.h"
#include "popupmenu.h"
#include "drawing.h"
#include "MathomirDoc.h"
#include "MathomirView.h"
#include <string>

extern CMathomirView* pMainView;
extern CExpression* ClipboardExpression;
extern CDrawing* ClipboardDrawing;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//extern CExpression* ClipboardExpression;
extern CView* pMainViewBase;
extern CToolbox* Toolbox;
extern PopupMenu* Popup;
extern CBitmap* SpacingBitmap;
extern CBitmap* GuidlineBitmap;
extern RECT TheClientRect;

extern CExpression* LongClickObject;
extern CDrawing* tmpDrawing;
extern CDrawing* tmpDrawing2;
extern tDocumentStruct* SpecialDrawingHover;
extern tDocumentStruct* prevSpecialDrawingHover;
extern tDocumentStruct* SelectedDocumentObject;
extern tDocumentStruct* SelectedDocumentObject2;
extern tDocumentStruct* prevTouchedObject;

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    ON_WM_CREATE()
    ON_WM_MOVING()
    ON_WM_MOVE()
    ON_WM_PAINT()
    ON_WM_SIZE()
    //ON_WM_MOUSEACTIVATE()
    ON_WM_WINDOWPOSCHANGED()
    //ON_WM_SYSKEYDOWN()
    ON_WM_SYSCOMMAND()
    ON_UPDATE_COMMAND_UI(ID_EDIT_IMAGE, OnUpdateEditImage)
    ON_UPDATE_COMMAND_UI(ID_EDIT_SAVEEQUATIONIMAGE, OnUpdateEditSaveequationimage)
    ON_UPDATE_COMMAND_UI(ID_EDIT_COPYMATHMLCODE, OnUpdateEditCopymathmlcode)
    ON_UPDATE_COMMAND_UI(ID_EDIT_COPYLATEXCODE, OnUpdateEditCopymathmlcode)
    ON_WM_ACTIVATE()
    ON_WM_ACTIVATEAPP()
    ON_WM_MENUCHAR()
    {
        WM_DISPLAYCHANGE, 0, 0, 0, AfxSig_vwp,
        static_cast<AFX_PMSG>((AFX_PMSGW)static_cast<void (AFX_MSG_CALL CWnd::*)(WPARAM, LPARAM)>(&
            ThisClass::OnDisplayChange))
    },
END_MESSAGE_MAP()

HPEN HDottedLineBlack;
HPEN HDottedLineBlue;
HPEN HSolidLineBlack[5];
HPEN HSolidLineBlue[5];
HPEN HOtherLine;


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
    //create some often used pens
    HDottedLineBlack = CreatePen(PS_DOT, 1,RGB(0, 0, 0));
    HDottedLineBlue = CreatePen(PS_DOT, 1,BLUE_COLOR);
    for (int i = 0; i < 5; i++)
    {
        HSolidLineBlack[i] = CreatePen(PS_SOLID, 1 + i,RGB(0, 0, 0));
        HSolidLineBlue[i] = CreatePen(PS_SOLID, 1 + i,BLUE_COLOR);
    }
    HOtherLine = nullptr;
}

extern CBitmap* GentlyPaintBitmap;
extern CDC* GentlyPaintBitmapDC;
extern CExpression* ExtractedSelection;
#pragma optimize("s",on)
CMainFrame::~CMainFrame()
{
    ClearDocument();
    ClearFontPool();

    DeleteObject(HDottedLineBlack);
    DeleteObject(HDottedLineBlue);
    for (int i = 0; i < 5; i++)
    {
        DeleteObject(HSolidLineBlack[i]);
        DeleteObject(HSolidLineBlue[i]);
    }

    if (HOtherLine) DeleteObject(HOtherLine);
    if (ClipboardExpression)
    {
        delete ClipboardExpression;
        ClipboardExpression = nullptr;
    }
    UndoRelease(1); //release undo memory
    delete SpacingBitmap;
    delete GuidlineBitmap;
    delete Toolbox;
    delete Popup;
    ReleaseMyPainting();
    if (GentlyPaintBitmap)
    {
        delete GentlyPaintBitmapDC;
        GentlyPaintBitmap->DeleteObject();
        delete GentlyPaintBitmap;
    }
    delete ExtractedSelection;
    if (TheDocument) free(TheDocument);
}

#pragma optimize("s",on)
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;
    return 0;
}


#pragma optimize("s",on)
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if (!CFrameWnd::PreCreateWindow(cs))
        return FALSE;
    // TODO: Modify the Window class or styles here by modifying
    //  the CREATESTRUCT cs

    cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE /*| WS_CLIPCHILDREN*/
        | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU | WS_SIZEBOX;

    cs.x = 300;
    cs.y = 200;
    cs.cx = 200;
    cs.cy = 100;

    return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
    CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers
std::string FontFacenames[5];
char FontAdjustedSizes[5];
unsigned char FontCharSet[5];
unsigned int FontWeight[5];

#pragma optimize("s",on)
void CMainFrame::SetFontsToDefaults()
{
    FontFacenames[0] = "Arial";
    FontFacenames[1] = "Times New Roman";
    FontFacenames[2] = "Courier New";
    FontFacenames[3] = "Symbol";
    FontFacenames[4] = "Arial";
    FontAdjustedSizes[0] = 95;
    FontAdjustedSizes[1] = 109;
    FontAdjustedSizes[2] = 109;
    FontAdjustedSizes[3] = 114;
    FontAdjustedSizes[4] = 114;
    FontCharSet[0] = DEFAULT_CHARSET;
    FontCharSet[1] = DEFAULT_CHARSET;
    FontCharSet[2] = DEFAULT_CHARSET;
    FontCharSet[3] = DEFAULT_CHARSET;
    FontCharSet[4] = DEFAULT_CHARSET;
    FontWeight[0] = FW_NORMAL;
    FontWeight[1] = FW_NORMAL;
    FontWeight[2] = FW_NORMAL;
    FontWeight[3] = FW_NORMAL;
    FontWeight[4] = FW_NORMAL;
    ClearFontPool();
}

#pragma optimize("s",on)
void CMainFrame::GetLogicalFont(int font_no, LOGFONT* lf, const CDC* DC)
{
    ZeroMemory(lf, sizeof(LOGFONT));
    lf->lfWeight = FontWeight[font_no];
    lf->lfHeight = 20 + (FontAdjustedSizes[font_no] - 100) / 2;
    lf->lfHeight = -MulDiv(lf->lfHeight, DC->GetDeviceCaps(LOGPIXELSY), 72);

    strcpy_s(lf->lfFaceName, FontFacenames[font_no].c_str());
    lf->lfCharSet = FontCharSet[font_no];
}

#pragma optimize("s",on)
void CMainFrame::SetLogicalFont(int font_no, LOGFONT* lf, const CDC* DC)
{
    FontWeight[font_no] = min(600, lf->lfWeight);
    int r = DC->GetDeviceCaps(LOGPIXELSY);
    lf->lfHeight = MulDiv(-lf->lfHeight, 72, DC->GetDeviceCaps(LOGPIXELSY));
    FontAdjustedSizes[font_no] = static_cast<char>((abs(lf->lfHeight) - 20) * 2 + 100);
    FontFacenames[font_no] = lf->lfFaceName;
    FontCharSet[font_no] = lf->lfCharSet;

    ClearFontPool();
}
#pragma optimize("",on)

#define MAX_NUM_FONTS 30

struct tFontPool
{
    HFONT Font;
    int NumRequests;
    unsigned short Size;
    byte Combination;
};

int NumFontsInPool;
tFontPool FontPoolList[MAX_NUM_FONTS];

void ClearFontPool()
{
    for (int i = 0; i < NumFontsInPool; i++) DeleteObject(FontPoolList[i].Font);
    NumFontsInPool = 0;
}


HFONT GetFontFromPool(char Face, bool Italic, bool Bold, unsigned short Size)
{
    return GetFontFromPool(Face << 5 | Italic << 1 | Bold, Size);
}

HFONT GetFontFromPool(byte combination, unsigned short Size)
{
    tFontPool* tp = FontPoolList;
    for (int i = 0; i < NumFontsInPool; i++, tp++)
    {
        //check if the font is already in pool
        if (combination == tp->Combination && tp->Size == Size)
        {
            //we found exact mach - we will reward this font by adding some points
            if (tp->NumRequests < 20000) tp->NumRequests += 3;

            //return this font
            return tp->Font;
        }
    }

    //font not found, we must create one
    //first, adjust usage counters
    tp = FontPoolList;
    int min_pos = 0, min_requests = 21000;
    for (int i = 0; i < NumFontsInPool; i++, tp++)
    {
        //find the least used font
        if (tp->NumRequests <= min_requests)
        {
            min_requests = tp->NumRequests;
            min_pos = i;
        }
        if (tp->NumRequests > 0) tp->NumRequests--;
    }

    byte Face = (combination & 0xE0) >> 5;
    int sze;
    if (Size & 0x8000)
    {
        sze = (FontAdjustedSizes[Face] * (Size & 0x7FFF) + 0) / 100;
        sze = sze * 22 / 24 + 2;
    }
    else
        sze = Size;

    HFONT theFont = CreateFont(
        sze, //Size
        0, //width
        0, //escapement
        0, //Orientation
        combination & 0x01 ? FontWeight[Face] + 300 : FontWeight[Face], //Weight
        static_cast<BYTE>(combination & 0x02 ? 1 : 0), //Italic
        0, //Underline
        0, //Strikeout
        FontCharSet[Face], //CharSet
        0, //OutPrecision
        0, //ClipPrecision
        0, //Quality
        0, //Pitch and Family
        FontFacenames[Face].c_str()); //facename

    if (theFont == nullptr) return static_cast<HFONT>(GetStockObject(SYSTEM_FONT));

    //store the font to the font pool (if there is no space, delete one)
    if (NumFontsInPool < MAX_NUM_FONTS)
    {
        tp = FontPoolList + NumFontsInPool;
        NumFontsInPool++;
    }
    else
    {
        tp = FontPoolList + min_pos;
        DeleteObject(tp->Font);
    }

    tp->Combination = combination;
    tp->Size = Size;
    tp->NumRequests = 10000;
    tp->Font = theFont;

    return theFont;
}


//returns pen of given width and color - it must work fast
//several pens are pre-created (in CMainFrame constructor)
HPEN GetPenFromPool(int width, bool IsBlue, COLORREF color)
{
    static int LastWidth;
    static COLORREF LastColor;

    if (width <= 0 && IsBlue == 0 && color == 0) return HDottedLineBlack;
    if (width <= 0 && IsBlue == 1) return HDottedLineBlue;
    if (width <= 5)
    {
        if (IsBlue) return HSolidLineBlue[width - 1];
        if (color == 0) return HSolidLineBlack[width - 1];
    }

    {
        COLORREF new_color = IsBlue ? BLUE_COLOR : color;
        if (LastWidth != width || LastColor != new_color || HOtherLine == nullptr)
        {
            if (HOtherLine) DeleteObject(HOtherLine);
            HOtherLine = CreatePen(width > 0 ? PS_SOLID : PS_DOT,max(width, 1), new_color);
            LastWidth = width;
            LastColor = new_color;
        }
        return HOtherLine;
    }
    return nullptr;
}

extern int Reenable;

void CMainFrame::OnMoving(UINT fwSide, LPRECT pRect)
{
    //while it is moving
    CFrameWnd::OnMoving(fwSide, pRect);
    //if (Toolbox) {Sleep(10);Toolbox->AdjustPosition();Reenable=2;}
}

void CMainFrame::OnMove(int x, int y)
{
    //after it was moved
    CFrameWnd::OnMove(x, y);
    /*WINDOWPLACEMENT wp;
    GetWindowPlacement(&wp);
    if (wp.showCmd!=SW_SHOWMINIMIZED)
        if (Toolbox) {Sleep(10);Toolbox->AdjustPosition();Reenable=2;}*/
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
    CFrameWnd::OnSize(nType, cx, cy);
    WINDOWPLACEMENT wp;
    GetWindowPlacement(&wp);
    if (wp.showCmd != SW_SHOWMINIMIZED)
    {
        //if ((Toolbox) && (AutoResizeToolbox))
        //	Toolbox->AutoResize();
        if (Toolbox && ToolboxSize)
        {
            if (AutoResizeToolbox)
                Toolbox->AutoResize();
            Toolbox->AdjustPosition();
        }
        //if ((UseToolbar) && (Toolbox) && (Toolbox->Toolbar)) Toolbox->Toolbar->AdjustPosition(); 
    }
}

unsigned int OWPC_cntr = 0;

void CMainFrame::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
    OWPC_cntr++;
    if (lpwndpos->cx == 200 && lpwndpos->cy == 100 && OWPC_cntr < 3) return;

    CFrameWnd::OnWindowPosChanged(lpwndpos);

    if (static_cast<CWnd*>(pMainView) == this->GetActiveWindow())
        this->SetActiveWindow();
}

void CMainFrame::OnDisplayChange(WPARAM wp, LPARAM lp)
{
    if (pMainView) pMainView->RepaintTheView();
    if (Toolbox) Toolbox->AdjustPosition();
    if (Toolbox->Toolbar) Toolbox->Toolbar->AdjustPosition();
}


extern CExpression* TheKeyboardClipboard;
extern char dont_empty_clipboard;
//when the window gets deactivated, check if there is anything in clipboard
//if yes, convert this into a bitmap so it can be pasted into any other application
void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
    dont_empty_clipboard = 1;
    if (nState == WA_INACTIVE)
    {
        //when the main window gets deactivated, we are generating image of copied objects
        //because we want to be prepared for Copy->Paste operation

        //check if image generation is temporarely disable
        if (NoImageAutogeneration)
        {
            NoImageAutogeneration = 0;
            dont_empty_clipboard = 0;
            return;
        }

        //mark all selected items by seting the high bit in the MovingDotStates
        //this is needed because the OpenMOMFile will deselect everything
        int maxfnd = -1;
        int minfnd = -1;
        for (size_t i = 0; i < NumDocumentElements; i++)
        {
            tDocumentStruct* ds = &TheDocument[i];
            if (ds->MovingDotState == 3)
            {
                ds->MovingDotState |= 0x80;
                maxfnd = i;
                if (minfnd == -1) minfnd = i;
            }
        }

        CMathomirDoc* pDoc = pMainView->GetDocument();
        if (pDoc->OpenMOMFile(nullptr)) //this makes paste from clipboard
        {
            if (TheKeyboardClipboard)
            {
                pMainView->MakeImageOfExpression((CObject*)TheKeyboardClipboard);
            }
            else
            {
                //the pase from clipboard is successful - the pasted data remains selected
                //and we can easily make Image.
                pMainView->OnEditCopyImage();

                for (int i = NumDocumentElements - 1; i >= 0; i--)
                    if (TheDocument[i].MovingDotState == 3)
                        pMainView->DeleteDocumentObject(&TheDocument[i]);
                    else break;
            }
        }

        //restore eselection by checking if the MovingDotState was marked by high bit
        for (int i = max(minfnd, 0); i <= maxfnd; i++)
            if (TheDocument[minfnd].MovingDotState & 0x80) TheDocument[minfnd].MovingDotState = 3;
    }
    dont_empty_clipboard = 0;
}


void CMainFrame::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
    this->SetActiveWindow();
}

//we are going to disable Main Menu activation throught the ALT key
//this is because the ALT key is used for other purposes (greek symbols, snap-to-grid)
void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
    if (nID != SC_KEYMENU) CFrameWnd::OnSysCommand(nID, lParam);
    else if (lParam == 0x11) //the 0x11 code is internally sent from CMathomirView::OnSysKeyDown
        CWnd::DefWindowProc(WM_SYSCOMMAND, nID, 0);
}


int IsMenuAccessKey(UINT nChar)
{
    if (LanguageStrings)
    {
        for (int i = 30001; i < 30004; i++)
            //check first 3 shortcuts (main menu items starting with specific character - File, Edit, View)
            if (LanguagePointers[i] != 0xFFFF && LanguageStrings[LanguagePointers[i]] == static_cast<char>(nChar))
                return 1;
    }
    else
    {
        if (nChar == 'F' || nChar == 'E' || nChar == 'V') return 1;
    }

    return 0;
}

afx_msg LRESULT CMainFrame::OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu)
{
    //this function is called when a non-used menu shortcut is pressed
    //must return non-zero in high word to suppress message beeps when non-used menu shortcut is pressed
    return 1 << 16;
}

//this function is also called from CMathomirView::OnSysKeyDown when the ALT key is pressed
/*void CMainFrame::OnSysKeyDown(UINT nChar,UINT nRepCnt,UINT nFlags)
{
	
	//CFrameWnd::OnSysKeyDown(nChar,nRepCnt,nFlags);
	if ((!IsMenuAccessKey(nChar)) && 
		(nChar!=VK_F4) && (nChar!=VK_MENU) && (nChar!=VK_F10))
	{
		//we will not process this command, 
		//let the CMathomirView::OnSsKeyDown manage (used for greek symbols: ALT+letter)
		MessageBeep(0);
		pMainViewBase->SendMessage(WM_SYSKEYDOWN,nChar,nRepCnt|(nFlags<<16));
	}
	else
	{
		//we will process this command
		CWnd::DefWindowProc(WM_SYSKEYDOWN,nChar,nRepCnt|(nFlags<<16)); //Execute it (will generate OnSysCommand)
		CFrameWnd::OnSysKeyDown(nChar,nRepCnt,nFlags); //default - must be!
	}
	
}*/


void CMainFrame::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    RECT ClientRect;
    this->GetClientRect(&ClientRect);
    //if (UseToolbar)
    //	dc.FillSolidRect(0,0,ClientRect.right,ToolboxSize/2+2,0x808080);
    //ClientRect.right=ToolboxSize+4;
    //dc.FillSolidRect(&ClientRect,0x808080);
    dc.FillSolidRect(0, 0, ClientRect.right, 1, 0x808080);
    RECT ToolboxRect;
    ToolboxRect.top = ToolboxRect.bottom = 0;
    if (Toolbox && ToolboxSize)
    {
        Toolbox->GetWindowRect(&ToolboxRect);
    }
    dc.FillSolidRect(0, ToolboxRect.bottom - ToolboxRect.top, ToolboxSize,
                     ClientRect.bottom - (ToolboxRect.bottom - ToolboxRect.top), 0x808080);
}


//called from pop-up menues to point the 'checked' sign
int PaintCheckedSign(CDC* DC, int x, int y, short size, bool IsChecked)
{
    CBrush* brsh = nullptr;
    CPen* pn = nullptr;

    if (IsChecked) //when IsCheked==2 then paint gray sign
    {
        brsh = new CBrush(IsChecked ? RGB(224, 64, 128) : RGB(192, 192, 192));
        pn = new CPen(PS_SOLID, 1, IsChecked ? RGB(224, 64, 128) : RGB(192, 192, 192)); //190,0,85

        DC->SelectObject(brsh);
        DC->SelectObject(pn);
    }
    else
    {
        DC->SelectObject(GetStockObject(WHITE_BRUSH));
        DC->SelectObject(GetStockObject(WHITE_PEN));
    }

    POINT p[8];

    p[0].x = x;
    p[0].y = y + size / 2;
    p[1].x = x + size / 3;
    p[1].y = y + size;
    p[2].x = x + size / 3;
    p[2].y = y + size;
    p[3].x = x + size;
    p[3].y = y;
    p[4].x = x + size / 3;
    p[4].y = y + 3 * size / 4;

    DC->Polygon(p, 5);


    delete pn;
    delete brsh;

    return 0;
}

/*int CMainFrame::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	return CFrameWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}*/


#pragma optimize("",on)


//**********************************************************************************
// this block is for MyPainting functions - these function paint into memory DC.
// The image is painted four times larger, and when it is finished it is 
// copied into using StretchBlt - it is posible to generate halftones this way
// Used for the 'presentation mode rendering' - should work fast!
//***********************************************************************************
CBitmap* MyBitmap = nullptr;
CDC* MyDC;
int MyBitmapWidth = 0;
int MyBitmapAbove = 0;
int MyBitmapBelow = 0;
int MyBitmapReservedWidth = 0;
int MyBitmapReservedHeight = 0;
int MyColor = 0;
CDC* MyOldDC;
int MyBitmapIsNew = 0;

int CMainFrame::StartMyPainting(CDC* DC, int width, int above, int below, int color)
{
    MyColor = color;
    if (MyBitmap == nullptr || DC != MyOldDC || width > MyBitmapReservedWidth || above + below >
        MyBitmapReservedHeight)
    {
        //creates new memory bitmap if the requested bitmap is larger than alredy existing one
        //(otherwise, the existing one is reused because is faster than creating new one)
        if (MyBitmap)
        {
            MyDC->DeleteDC();
            MyBitmap->DeleteObject();
            delete MyDC;
            delete MyBitmap;
        }

        MyBitmap = new CBitmap();
        bool ret = MyBitmap->CreateCompatibleBitmap(DC, width, above + below);
        MyDC = new CDC();
        MyDC->CreateCompatibleDC(DC);
        MyDC->SelectObject(MyBitmap);
        MyOldDC = DC;

        MyBitmapReservedWidth = width;
        MyBitmapReservedHeight = above + below;
        if (!ret)
        {
            MyBitmapReservedWidth = 0;
            MyBitmapReservedHeight = 0;
        }
    }

    if (IsHalftoneRendering)
    {
        MyDC->SetStretchBltMode(HALFTONE); // must be set for halfton stretch
        MyDC->SetBrushOrg(0, 0);
    }
    else
    {
        MyDC->SetStretchBltMode(/*BLACKONWHITE*/COLORONCOLOR); //this is faster
    }

    MyBitmapWidth = width;
    MyBitmapAbove = above;
    MyBitmapBelow = below;
    MyDC->FillSolidRect(0, 0, width, above + below,RGB(255, 255, 255));
    MyBitmapIsNew = 1;
    return 1;
}

//clears memory and bitmaps (used when exiting application)
int CMainFrame::ReleaseMyPainting()
{
    if (MyDC)
    {
        MyDC->DeleteDC();
        delete MyDC;
        MyDC = nullptr;
    }
    if (MyBitmap)
    {
        MyBitmap->DeleteObject();
        delete MyBitmap;
        MyBitmap = nullptr;
    }
    return 0;
}

//bilts the drawn image into real device context (at x,y position) - the image is stretched
//the problem is speed in halftone mode - it is too slow.
int CMainFrame::EndMyPainting(CDC* DC, int X, int Y, int force_black, int flip_image)
{
    int W = MyBitmapWidth >> 2;
    int H = MyBitmapAbove + MyBitmapBelow >> 2;

    //the force_black is used if we don't want halftoning (useful for vertical and horizontal lines like fraction line)
    if (force_black) MyDC->SetStretchBltMode(BLACKONWHITE);

    if (MyBitmapIsNew)
    {
        MyBitmapIsNew = 0;
        MyDC->StretchBlt(0, 0, W + 1, H + 1, MyDC, 0, 0, MyBitmapWidth + 4, MyBitmapAbove + MyBitmapBelow + 4,SRCCOPY);
        MyDC->FillSolidRect(W, 0, MyBitmapWidth, MyBitmapAbove + MyBitmapBelow, 0x00FFFFFF);
        MyDC->FillSolidRect(0, H, W, MyBitmapAbove + MyBitmapBelow, 0x00FFFFFF);

        if (IsHalftoneRendering)
        {
            //WE HAVE TO CAREFULLY CHECK EVERY PIXEL, 
            //BECAUSE THE STRETCHBLT ON SOME COMPUTERS CHANGES THE BACKGROUND COLOR 
            //TO NOT-EXACTLY-WHITE (this causes problems later with TransparentBlt)

            int fast_method = 1; //the fast method is only provided for 24 bit and 32 bit colors
            int bits_per_pixel = MyDC->GetDeviceCaps(BITSPIXEL);
            if (bits_per_pixel % 8) fast_method = 0;
            int bytes_per_pixel = bits_per_pixel / 8;
            if (bytes_per_pixel < 3) fast_method = 0;

            unsigned char* bits;
            if (fast_method)
            {
                bits = static_cast<unsigned char*>(HeapAlloc(ProcessHeap, 0,
                                                             MyBitmapReservedWidth * H * bytes_per_pixel));
                if (!bits)
                    fast_method = 0;
                else if (!MyBitmap->GetBitmapBits(H * MyBitmapReservedWidth * bytes_per_pixel, bits))
                {
                    fast_method = 0;
                    HeapFree(ProcessHeap, 0, bits);
                }
            }
            if (fast_method)
            {
                int line_size = bytes_per_pixel * MyBitmapReservedWidth;
                byte* src = bits;

                for (int i = 0; i < H; i++, src = bits + i * line_size)
                    for (int j = 0; j < W; j++, src += bytes_per_pixel)
                    {
                        unsigned int clr = *(unsigned int*)src;
                        clr = clr & 0x00FFFFFF;
                        if (clr != 0xFFFFFF)
                        {
                            clr &= 0xC0C0C0;
                            if (clr == 0xC0C0C0)
                            {
                                *src = 0xFF;
                                *(src + 1) = 0xFF;
                                *(src + 2) = 0xFF;
                            }
                        }
                    }

                MyBitmap->SetBitmapBits(H * MyBitmapReservedWidth * bytes_per_pixel, bits);
                HeapFree(ProcessHeap, 0, bits);
            }
            else
            {
                //unsuported bits-per-pixel resolution - do it the slow way - pixel by pixel
                int i, j;
                for (j = 0; j < H; j++)
                    for (i = 0; i < W; i++)
                    {
                        COLORREF clr = MyDC->GetPixel(i, j) & 0x00FFFFFF;
                        if (clr != 0x00FFFFFF) //non-white
                        {
                            clr = clr & 0x00C0C0C0;
                            if (clr == 0x00C0C0C0) MyDC->SetPixelV(i, j, 0x00FFFFFF);
                        }
                    }
            }
        }
    }

    if (flip_image == 1)
    {
        //horizontal flip
        MyDC->SetStretchBltMode(COLORONCOLOR);
        MyDC->StretchBlt(0, 0, W, H, MyDC, W - 1, 0, -W, H,SRCCOPY);
    }
    else if (flip_image == 2)
    {
        //vertical flip
        MyDC->SetStretchBltMode(COLORONCOLOR);
        MyDC->StretchBlt(0, 0, W, H, MyDC, 0, H - 1, W, -H,SRCCOPY);
    }


    DC->TransparentBlt(X, Y - MyBitmapAbove / 4, W, H, MyDC, 0, 0, W, H, 0x00FFFFFF);

    //CDC *pdc=pMainView->GetDC();
    //pdc->BitBlt(0,0,W*4,H*4,MyDC,0,0,SRCCOPY);
    //pMainView->ReleaseDC(pdc);
    return 0;
}

int CMainFrame::MyPolyline(CDC* DC, LPPOINT points, int count, int LineWidth, bool IsBlue)
{
    for (int i = 0; i < count; i++)
        points[i].y += MyBitmapAbove;
    MyDC->SelectObject(GetPenFromPool(LineWidth, IsBlue, MyColor));
    MyDC->Polyline(points, count);
    return 0;
}

int CMainFrame::MyMoveTo(CDC* DC, int X, int Y)
{
    MyDC->MoveTo(X, Y + MyBitmapAbove);
    return 0;
}

int CMainFrame::MyLineTo(CDC* DC, int X, int Y, bool IsBlue)
{
    MyDC->SelectObject(GetPenFromPool(1, IsBlue, MyColor));
    MyDC->LineTo(X, Y + MyBitmapAbove);
    return 0;
}

int CMainFrame::MyCircle(CDC* DC, int X1, int Y1, int X2, int Y2, int PenWidth, char IsBlue)
{
    Y1 += MyBitmapAbove;
    Y2 += MyBitmapAbove;
    MyDC->SelectObject(GetPenFromPool(PenWidth, IsBlue, MyColor));
    MyDC->Arc(X1, Y1, X2, Y2, (X1 + X2) / 2, Y1, (X1 + X2) / 2 + 1, Y1);
    return 0;
}

int CMainFrame::MyArc(CDC* DC, int X1, int Y1, int X2, int Y2, int Xstart, int Ystart, int Xend, int Yend, char IsBlue)
{
    MyDC->SelectObject(GetPenFromPool(1, IsBlue, MyColor));
    MyDC->Arc(X1, Y1 + MyBitmapAbove, X2, Y2 + MyBitmapAbove, Xstart, Ystart + MyBitmapAbove, Xend,
              Yend + MyBitmapAbove);
    return 0;
}

int CMainFrame::MySetPixel(CDC* DC, int X, int Y, bool IsBlue)
{
    MyDC->SetPixelV(X, Y + MyBitmapAbove, IsBlue ? BLUE_COLOR : MyColor);
    return 0;
}

int CMainFrame::MyBitBlt(CDC* DC, int X, int Y, int width, int height, int Xsrc, int Ysrc, bool IsXInvers)
{
    if (IsXInvers)
    {
        //mirrored horizontal
        MyDC->StretchBlt(X + width, Y + MyBitmapAbove,
                         -width, height, MyDC, Xsrc, Ysrc + MyBitmapAbove, width, height,
                         SRCCOPY);
    }
    else
    {
        //non-mirrored blt
        MyDC->BitBlt(X, Y + MyBitmapAbove, width, height, MyDC, Xsrc, Ysrc + MyBitmapAbove,SRCCOPY);
    }
    return 0;
}


//clears the entire document (like File->New)
int CMainFrame::ClearDocument()
{
    for (size_t i = 0; i < NumDocumentElements; i++)
    {
        if (TheDocument[i].Type == EXPRESSION)
        {
            //((CExpression*)(TheDocument[i].Object))->Delete();
            delete TheDocument[i].Object.exp;
        }
        else if (TheDocument[i].Type == DRAWING)
        {
            //((CDrawing*)(TheDocument[i].Object))->Delete();
            delete TheDocument[i].Object.draw;
        }
    }
    free(TheDocument);
    NumDocumentElements = 0;
    NumDocumentElementsReserved = 0;
    ViewX = ViewY = 0;
    if (DefaultZoom != 150 && DefaultZoom != 120 && DefaultZoom != 100 && DefaultZoom != 80) DefaultZoom = 100;
    ViewZoom = DefaultZoom;
#ifdef TEACHER_VERSION
    if (PublicKey) free(PublicKey);
    TheTimeLimit = 0;
    TheMathFlags = 0;
    PublicKey = nullptr;
#endif
    TheFileType = 0;
    return 0;
}


//adds new object to the document
int AddDocumentObject(doc_type type, int X, int Y)
{
    if (NumDocumentElementsReserved <= NumDocumentElements)
    {
        if (NumDocumentElementsReserved == 0)
        {
            NumDocumentElementsReserved += 20;
            TheDocument = static_cast<tDocumentStruct*>(malloc(NumDocumentElementsReserved * sizeof(tDocumentStruct)));
        }
        else
        {
            long long x1 = -1;
            long long x2 = -1;
            long long x3 = -1;
            long long x4 = -1;
            long long x5 = -1;
            long long x6 = -1;
            if (prevTouchedObject >= TheDocument
                && prevTouchedObject <= &TheDocument[NumDocumentElements - 1])
                x1 = prevTouchedObject - TheDocument;
            if (SelectedDocumentObject >= TheDocument
                && SelectedDocumentObject <= &TheDocument[NumDocumentElements - 1])
                x2 = SelectedDocumentObject - TheDocument;
            if (SelectedDocumentObject2 >= TheDocument
                && SelectedDocumentObject2 <= &TheDocument[NumDocumentElements - 1])
                x3 = SelectedDocumentObject2 - TheDocument;
            if (SpecialDrawingHover >= TheDocument
                && SpecialDrawingHover <= &TheDocument[NumDocumentElements - 1])
                x4 = SpecialDrawingHover - TheDocument;
            if (prevSpecialDrawingHover >= TheDocument
                && prevSpecialDrawingHover <= &TheDocument[NumDocumentElements - 1])
                x5 = prevSpecialDrawingHover - TheDocument;
            if (KeyboardEntryBaseObject >= TheDocument
                && KeyboardEntryBaseObject <= &TheDocument[NumDocumentElements - 1])
                x6 = KeyboardEntryBaseObject - TheDocument;
            NumDocumentElementsReserved += 20;
            TheDocument = static_cast<tDocumentStruct*>(realloc(TheDocument,
                                                                NumDocumentElementsReserved * sizeof(tDocumentStruct)));

            if (x1 < 0) prevTouchedObject = nullptr;
            else prevTouchedObject = TheDocument + x1;
            if (x2 < 0) SelectedDocumentObject = nullptr;
            else SelectedDocumentObject = TheDocument + x2;
            if (x3 < 0) SelectedDocumentObject2 = nullptr;
            else SelectedDocumentObject2 = TheDocument + x3;
            if (x4 < 0) SpecialDrawingHover = nullptr;
            else SpecialDrawingHover = TheDocument + x4;
            if (x5 < 0) prevSpecialDrawingHover = nullptr;
            else prevSpecialDrawingHover = TheDocument + x5;
            if (x6 < 0) KeyboardEntryBaseObject = nullptr;
            else KeyboardEntryBaseObject = TheDocument + x6;
            LongClickObject = nullptr;
            //if (SpecialDrawingHover) SpecialDrawingHover=nullptr;
            //if (prevSpecialDrawingHover) prevSpecialDrawingHover=nullptr;
        }
        if (TheDocument == nullptr)
        {
            AfxMessageBox("Cannot reserve memory for the Document",MB_OK | MB_ICONWARNING,NULL);
            return 0;
        }
    }
    TheDocument[NumDocumentElements].absolute_X = X;
    TheDocument[NumDocumentElements].absolute_Y = Y;
    TheDocument[NumDocumentElements].Type = type;
    TheDocument[NumDocumentElements].MovingDotState = 0;
    TheDocument[NumDocumentElements].Object.exp = nullptr;
    TheDocument[NumDocumentElements].Above = 0;
    TheDocument[NumDocumentElements].Below = 0;
    TheDocument[NumDocumentElements].Length = 0;
    NumDocumentElements++;
    if (UseToolbar && Toolbox && Toolbox->Toolbar) Toolbox->Toolbar->ConfigureToolbar();
    return 1;
}


//adjust the main menu
extern int NumSelectedObjects;
int MenuTranslated = 0;

int CMainFrame::AdjustMenu(bool adjust_undo_only)
{
    CMenu* theMenu;
    theMenu = GetMenu();

    if (ViewOnlyMode == 2)
    {
        SetMenu(nullptr);
        return 0;
    }


    std::string UndoText = "Undo";
    std::string HandyHelpText = "Handy help...";
    std::string F1Text = "Zoom to &Default (1:1)";
    if (LanguageStrings)
    {
        if (LanguagePointers[31101] != 0xFFFF)
        {
            UndoText = LanguageStrings + LanguagePointers[31101];
        }
        if (LanguagePointers[ID_HELP_QUICKGUIDE] != 0xFFFF)
            HandyHelpText = LanguageStrings + LanguagePointers[
                ID_HELP_QUICKGUIDE];
        if (LanguagePointers[ID_VIEW_ZOOMTO1] != 0xFFFF) F1Text = LanguageStrings + LanguagePointers[ID_VIEW_ZOOMTO1];
    }

    if (theMenu == nullptr) return 0;

    if (LanguageStrings && !MenuTranslated)
    {
        MenuTranslated = 1;
        for (int i = 30000; i < 33000; i++)
        {
            unsigned short pntr = LanguagePointers[i];
            if (pntr != 0xFFFF)
            {
                int cmd = i;
                int submenu1 = 0;
                int submenu2 = 0;
                int mainmenu = 0;
                if (cmd == 30001) { mainmenu = 1; } //the FILE menu
                if (cmd == 30002) { mainmenu = 2; } //the EDIT menu
                if (cmd == 30003) { mainmenu = 3; } //the VIEW menu
                if (cmd == 30004) { mainmenu = 4; } //the OPTIONS menu
                if (cmd == 30005) { mainmenu = 5; } //the HELP menu
                if (cmd == 30101)
                {
                    mainmenu = 1;
                    submenu1 = 1;
                } //the autosave menu
                if (cmd == 30301)
                {
                    mainmenu = 3;
                    submenu1 = 1;
                } //the zoom menu
                if (cmd == 30302)
                {
                    mainmenu = 3;
                    submenu1 = 2;
                } //the page menu
                if (cmd == 32896)
                {
                    mainmenu = 3;
                    submenu1 = 2;
                    submenu2 = 1;
                } //the Page numbering menu
                if (cmd == 30303)
                {
                    mainmenu = 3;
                    submenu1 = 3;
                } //the toolbox size menu
                if (cmd == 30401)
                {
                    mainmenu = 4;
                    submenu1 = 1;
                } //the Selections menu
                if (cmd == 30402)
                {
                    mainmenu = 4;
                    submenu1 = 2;
                } //the Moving dot menu
                if (cmd == 30410)
                {
                    mainmenu = 4;
                    submenu1 = 3;
                } //the Mouse menu
                if (cmd == 30403)
                {
                    mainmenu = 4;
                    submenu1 = 4;
                } //the Keyboard menu
                if (cmd == 30404)
                {
                    mainmenu = 4;
                    submenu1 = 5;
                } //the Output image menu
                if (cmd == 30405)
                {
                    mainmenu = 4;
                    submenu1 = 6;
                } //the FontSize menu
                //if (cmd==30406) {mainmenu=4;submenu1=6;} //the Parentheses height menu
                if (cmd == 30407)
                {
                    mainmenu = 4;
                    submenu1 = 7;
                } //the Grid and guidelines menu
                if (cmd == 30408)
                {
                    mainmenu = 4;
                    submenu1 = 8;
                } //the Symbolic calculator menu
                if (cmd == 30409)
                {
                    mainmenu = 4;
                    submenu1 = 9;
                } //the Save settings menu
                if (cmd == 30451)
                {
                    mainmenu = 4;
                    submenu1 = 6;
                    submenu2 = 1;
                } //Font faces menu
                if (cmd == 30481)
                {
                    mainmenu = 4;
                    submenu1 = 8;
                    submenu2 = 1;
                } //Imaginary unit
                if (cmd == 31000) { cmd = ID_FILE_NEW; }
                if (cmd == 31001) { cmd = ID_FILE_OPEN; }
                if (cmd == 31002) { cmd = ID_FILE_SAVE; }
                if (cmd == 31003) { cmd = ID_FILE_SAVE_AS; }
                if (cmd == 31004) { cmd = ID_FILE_PRINT; }
                if (cmd == 31005) { cmd = ID_FILE_PRINT_PREVIEW; }
                if (cmd == 31006) { cmd = ID_FILE_PRINT_SETUP; }
                if (cmd == 31007) { cmd = ID_APP_EXIT; }
                if (cmd == 31101) { cmd = ID_EDIT_UNDO; }
                if (cmd == 31102) { cmd = ID_EDIT_CUT; }
                if (cmd == 31103) { cmd = ID_EDIT_COPY; }
                if (cmd == 31104) { cmd = ID_EDIT_PASTE; }
                if (cmd == 31105) { cmd = ID_EDIT_DELETE; }
                if (cmd == 31501) { cmd = ID_APP_ABOUT; }
                if (mainmenu)
                {
                    if (submenu1)
                    {
                        int cnt = 0;
                        CMenu* smenu1 = theMenu->GetSubMenu(mainmenu - 1);
                        CMenu* smenu2;
                        int j;
                        for (j = 0; j < 50; j++)
                        {
                            smenu2 = smenu1->GetSubMenu(j);
                            if (smenu2) cnt++;
                            if (cnt == submenu1) break;
                        }
                        if (cnt == submenu1)
                        {
                            if (submenu2)
                            {
                                CMenu* smenu3;
                                cnt = 0;
                                for (j = 0; j < 50; j++)
                                {
                                    smenu3 = smenu2->GetSubMenu(j);
                                    if (smenu3) cnt++;
                                    if (cnt == submenu2) break;
                                }
                                if (cnt == submenu2)
                                {
                                    //modifying second level submenu
                                    smenu2->ModifyMenu(j,MF_BYPOSITION | MF_STRING, 0, LanguageStrings + pntr);
                                }
                            }
                            else
                            {
                                //modifying first level submenu
                                smenu1->ModifyMenu(j,MF_BYPOSITION | MF_STRING, 0, LanguageStrings + pntr);
                            }
                        }
                    }
                    else
                    {
                        //modifying main menu bar
                        theMenu->ModifyMenu(mainmenu - 1,MF_BYPOSITION | MF_STRING, 0, LanguageStrings + pntr);
                    }
                }
                else
                    theMenu->ModifyMenu(cmd,MF_BYCOMMAND | MF_STRING, cmd, LanguageStrings + pntr);
            }
        }
        DrawMenuBar();
    }


    if (UndoNumLevels)
    {
        theMenu->ModifyMenu(ID_EDIT_UNDO,MF_BYCOMMAND | MF_STRING,ID_EDIT_UNDO,
                            (UndoText + " " + UndoStruct[UndoNumLevels - 1].text + "\tCtrl+Z").c_str());
        //DrawMenuBar();
    }
    else
    {
        theMenu->ModifyMenu(ID_EDIT_UNDO,MF_BYCOMMAND | MF_STRING,ID_EDIT_UNDO,
                            (UndoText + "\tCtrl+Z").c_str());
        //DrawMenuBar();
    }
    if (adjust_undo_only) return 0;

    if (F1SetsZoom)
    {
        theMenu->ModifyMenu(ID_VIEW_ZOOMTO1,MF_BYCOMMAND | MF_STRING,ID_VIEW_ZOOMTO1,
                            (F1Text + "\tF1").c_str());
        theMenu->ModifyMenu(ID_HELP_QUICKGUIDE,MF_BYCOMMAND | MF_STRING,ID_HELP_QUICKGUIDE,
                            HandyHelpText.c_str());
    }
    else
    {
        theMenu->ModifyMenu(ID_VIEW_ZOOMTO1,MF_BYCOMMAND | MF_STRING,ID_VIEW_ZOOMTO1,
                            F1Text.c_str());
        theMenu->ModifyMenu(ID_HELP_QUICKGUIDE,MF_BYCOMMAND | MF_STRING,ID_HELP_QUICKGUIDE,
                            (HandyHelpText + "\tF1").c_str());
    }

    theMenu->CheckMenuItem(ID_HQ_REND, IsHighQualityRendering ? MF_CHECKED : MF_UNCHECKED);
    /*theMenu->CheckMenuItem(ID_PARENTHESEHEIGHT_EVERINCREASING,(DefaultParentheseType==0)?MF_CHECKED:MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_PARENTHESEHEIGHT_NORMAL,(DefaultParentheseType==1)?MF_CHECKED:MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_PARENTHESEHEIGHT_SMALL,(DefaultParentheseType==2)?MF_CHECKED:MF_UNCHECKED);*/

    //theMenu->CheckMenuItem(ID_VIEW_CENTERPARENTHESECONTENT,(CenterParentheses)?MF_CHECKED:MF_UNCHECKED);
    //theMenu->CheckMenuItem(ID_SELECTIONS_FRAME,(FrameSelections==0x01)?MF_CHECKED:MF_UNCHECKED);
    //theMenu->CheckMenuItem(ID_SELECTIONS_UNDERLINE,(FrameSelections==0x03)?MF_CHECKED:MF_UNCHECKED);
    //theMenu->CheckMenuItem(ID_SELECTIONS_NONE,(FrameSelections==0x00)?MF_CHECKED:MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_SELECTIONS_INTELLIGENTFRAMING, FrameSelections ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_SELECTIONS_SHADOWSELECTIONS, ShadowSelection ? MF_CHECKED : MF_UNCHECKED);
    //theMenu->CheckMenuItem(ID_SELECTIONS_WIDEKEYBOARDCURSOR,(UseWideCursor)?MF_CHECKED:MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_KEYBOARD_FIXFONTFORNUMBERS, FixFontForNumbers ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_KEYBOARD_ALTFOREXPONENTS, UseALTForExponents ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_VIEW_HALFTONERENDERING, IsHalftoneRendering ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_PAGE_A4PORTRAIT, PaperWidth == 1165 && PaperHeight == 1650
                                                   ? MF_CHECKED
                                                   : MF_UNCHECKED);
    theMenu->CheckMenuItem(
        ID_PAGE_A4LANDSCAPE, PaperWidth == 1650 && PaperHeight == 1165 ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(
        ID_PAGE_LETTERPORTRAIT, PaperWidth == 1200 && PaperHeight == 1553 ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(
        ID_PAGE_LETTERLANDSCAPE, PaperWidth == 1553 && PaperHeight == 1200 ? MF_CHECKED : MF_UNCHECKED);

    int page_numeration = PageNumeration & 0x0F;
    int is_bottom = PageNumeration & 0x10;
    int is_right = PageNumeration & 0x20;
    int is_excludefirst = PageNumeration & 0x40;
    theMenu->CheckMenuItem(ID_PAGENUMERATION_NONE, page_numeration == 0 ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_PAGENUMERATION_, page_numeration == 1 ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_PAGENUMERATION_32899, page_numeration == 2 ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_PAGENUMERATION_PAGE1OF10, page_numeration == 3 ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_PAGENUMERATION_BOTTOM, is_bottom ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_PAGENUMERATION_RIGHT, is_right ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_PAGENUMERATION_EXCLUDEFIRSTPAGE, is_excludefirst ? MF_CHECKED : MF_UNCHECKED);

    theMenu->CheckMenuItem(ID_ZOOM_USECTRLFORWHEELZOOM, UseCTRLForZoom ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_MOUSE_MOUSE, !UseCTRLForZoom ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(
        ID_MOUSE_RIGHTMOUSEBUTTONTOTOGGLEMOUSE, RightButtonTogglesWheel ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_MOUSE_SLOW, WheelScrollingSpeed < 50 ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_GRIDANDGUIDELINES_SNAPTOGUIDELINES, SnapToGuidlines ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_Menu32905, Toolbox && Toolbox->Keyboard && Toolbox->Keyboard->IsWindowVisible()
                                             ? MF_CHECKED
                                             : MF_UNCHECKED);

    theMenu->CheckMenuItem(
        ID_TOOLBOXANDCONTEXTMENU_GIGANTIC, ToolboxSize > 115 && !AutoResizeToolbox ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_TOOLBOX_LARGE, ToolboxSize > 90 && ToolboxSize <= 115 && !AutoResizeToolbox
                                                 ? MF_CHECKED
                                                 : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_TOOLBOX_MEDIUM, ToolboxSize >= 70 && ToolboxSize <= 90 && !AutoResizeToolbox
                                                  ? MF_CHECKED
                                                  : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_TOOLBOX_SMALL, ToolboxSize < 70 && !AutoResizeToolbox ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_TOOLBOXANDCONTEXTMENU_AUTO, AutoResizeToolbox ? MF_CHECKED : MF_UNCHECKED);

    theMenu->CheckMenuItem(ID_TOOLBOXANDCONTEXTMENU_SHOWTOOLBAR, UseToolbar ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_OUTPUTIMAGE_SIZE200, ImageSize == 200 ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_OUTPUTIMAGE_SIZE150, ImageSize == 150 ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_OUTPUTIMAGE_SIZE100, ImageSize == 100 ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_OUTPUTIMAGE_SIZE80, ImageSize == 80 ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_OUTPUTIMAGE_FORCEHIGHQUALITY, ForceHighQualityImage ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_OUTPUTIMAGE_FORCEHALFTONE, ForceHalftoneImage ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(
        ID_FONTSIZEANDDEFAULTZOOM_DEFAULTZOOMIS150, DefaultZoom == 150 ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(
        ID_FONTSIZEANDDEFAULTZOOM_DEFAULTZOOMIS120, DefaultZoom == 120 ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(
        ID_FONTSIZEANDDEFAULTZOOM_DEFAULTZOOMIS100, DefaultZoom == 100 ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_FONTSIZEANDDEFAULTZOOM_DEFAULTZOOMIS80, DefaultZoom == 80 ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_FONTSIZE_VERYLARGE, DefaultFontSize >= 150 ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_FONTSIZE_LARGE, DefaultFontSize == 120 ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_FONTSIZE_NORMAL, DefaultFontSize == 100 ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_FONTSIZE_SMALL, DefaultFontSize == 85 ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_MOVINGDOT_SMALL, MovingDotSize < 6 ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_MOVINGDOT_MEDIUM, MovingDotSize == 6 ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_MOVINGDOT_LARGE, MovingDotSize > 6 ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_MOVINGDOT_PERMANENT, MovingDotPermanent ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_EDIT_ACCESSLOCKEDOBJECTS, AccessLockedObjects ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_GRID_FINE, GRID < 10 ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_GRID_MEDIUM, GRID >= 10 && GRID <= 20 ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_GRID_COARSE, GRID > 20 ? MF_CHECKED : MF_UNCHECKED);
    //theMenu->CheckMenuItem(ID_GRID_SNAPTOGRID,(SnapToGrid)?MF_CHECKED:MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_VIEW_SHOWGRID, IsShowGrid ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_SYMBOLICCALCULATOR_ENABLE, IsMathDisabled ? MF_UNCHECKED : MF_CHECKED);
    theMenu->CheckMenuItem(ID_IMAGINARYUNIT_I, ImaginaryUnit == 'i' ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_IMAGINARYUNIT_J, ImaginaryUnit == 'j' ? MF_CHECKED : MF_UNCHECKED);
    if (GRID < 5) GRID = 5;
    if (GRID > 80) GRID = 80;
    theMenu->CheckMenuItem(ID_KEYBOARD_GENERALVARIABLEMODE, IsSimpleVariableMode == 0 ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_KEYBOARD_VERYSIMPLEVARIABLEMODE, IsSimpleVariableMode ? MF_CHECKED : MF_UNCHECKED);
    //theMenu->CheckMenuItem(ID_KEYBOARD_SIMPLEVARIABLEMODE,(IsSimpleVariableMode==1)?MF_CHECKED:MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_KEYBOARD_ALLOWCOMMAASDECIMALSEPARATOR, UseCommaAsDecimal ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_AUTOSAVE_NEVER, AutosaveOption == 0 ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_AUTOSAVE_LOW, AutosaveOption == 1 ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_AUTOSAVE_MEDIUM, AutosaveOption == 2 ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_AUTOSAVE_HIGH, AutosaveOption == 3 ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_ZOOM_WHEELZOOMADJUSTSPOINTER, MoveCursorOnWheel ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(
        ID_MOUSE_REVERSEMOUSEWHEELSCROLLINGDIRECTION, MouseWheelDirection ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_KEYBOARD_ALTMENUSHORTCUTS, EnableMenuShortcuts ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_KEYBOARD_F1SETSZOOMLEVELTO100, F1SetsZoom ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_OUTPUTIMAGE_PRINTFONTSASIMAGES, PrintTextAsImage ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_OUTPUTIMAGE_PRINTFONTSASIMAGES, PrintTextAsImage ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_KEYBOARD_USECAPSLOCKTOTOGGLETYPINGMODE, UseCapsLock ? MF_CHECKED : MF_UNCHECKED);
    theMenu->CheckMenuItem(ID_KEYBOARD_USECOMPLEXINDEXES, UseComplexIndexes ? MF_CHECKED : MF_UNCHECKED);
    theMenu->ModifyMenu(ID_FONTFACES_FONT1,MF_BYCOMMAND | MF_STRING,ID_FONTFACES_FONT1,
                        (GetTranslatedString("Font &1", 32866) + " - " + FontFacenames[0] + "...").c_str());
    theMenu->ModifyMenu(ID_FONTFACES_FONT2,MF_BYCOMMAND | MF_STRING,ID_FONTFACES_FONT2,
                        (GetTranslatedString("Font &2", 32867) + " - " + FontFacenames[1] + "...").c_str());
    theMenu->ModifyMenu(ID_FONTFACES_FONT3,MF_BYCOMMAND | MF_STRING,ID_FONTFACES_FONT3,
                        (GetTranslatedString("Font &3", 32868) + " - " + FontFacenames[2] + "...").c_str());
    theMenu->ModifyMenu(ID_FONTFACES_FONT4,MF_BYCOMMAND | MF_STRING,ID_FONTFACES_FONT4,
                        (GetTranslatedString("Font &4", 32869) + " - " + FontFacenames[3] + "...").c_str());

    //theMenu->CheckMenuItem(ID_EDIT_IMAGE,(NumSelectedObjects)?M
    return 0;
}


// ********************************************************************************
// the following functions are used for Undo handling
//
// ********************************************************************************
tUndoStruct UndoStruct[NUM_UNDO_LEVELS];
int UndoNumLevels = 0;
int UndoNumObjects = 0;
int UndoNumObjectsReserved = 0;
tUndoObjectStruct* pUndoObjectList;

int CMainFrame::UndoInit()
{
    //delete all previous undo data
    if (pUndoObjectList)
    {
        for (int i = 0; i < UndoNumObjects; i++)
        {
            tUndoObjectStruct* us = &pUndoObjectList[i];
            if (us->Type == EXPRESSION)
            {
                //((CExpression*)(us->pObject))->Delete();
                delete us->pObject.exp;
            }
            else if (us->Type == DRAWING)
            {
                //((CDrawing*)(us->pObject))->Delete();
                delete us->pObject.draw;
            }
        }
        delete pUndoObjectList;
        pUndoObjectList = nullptr;
    }

    UndoNumObjects = 0;
    UndoNumObjectsReserved = 10;
    pUndoObjectList = new tUndoObjectStruct[10];

    for (int i = 0; i < UndoNumLevels; i++)
    {
        UndoStruct[i].text[0] = 0;
        UndoStruct[i].NumElements = 0;
        if (UndoStruct[i].data)
            free(UndoStruct[i].data);
    }
    UndoNumLevels = 0;

    return 0;
}

int UndoSavingDisabled = 0;

int CMainFrame::UndoDisableSaving()
{
    UndoSavingDisabled++;
    return 1;
}

int CMainFrame::UndoEnableSaving()
{
    UndoSavingDisabled--;
    return 1;
}

extern DWORD AutosavePoints;


//saves undo information (saves the current sate of documment)
int CMainFrame::UndoSave(const std::string& undo_text, int unique_ID)
{
    if (UndoSavingDisabled) return 1;

    //if ((UseToolbar) && (Toolbox->Toolbar)) Toolbox->Toolbar->ConfigureToolbar();
    std::string text = undo_text;
    if (LanguageStrings && unique_ID >= 20000 && unique_ID < 30000)
    {
        unsigned short pntr = LanguagePointers[unique_ID];
        if (pntr != 0xFFFF)
        {
            text = &LanguageStrings[pntr];
        }
    }

    AutosavePoints++; //for the autosave function

    if (pMainViewBase)
    {
        CDocument* pDoc = pMainViewBase->GetDocument();
        pDoc->SetModifiedFlag(true);
    }

    //first check if the oldest undo level must be deleted (only limited numer of undo levels is possible)
    if (UndoNumLevels >= NUM_UNDO_LEVELS)
    {
        int i;
        //first check if any of newer undo layers re-uses the same data (main document strucutre)
        for (i = 1; i < UndoNumLevels; i++)
            if (UndoStruct[i].data == UndoStruct[0].data) break;
        if (i == UndoNumLevels) //no other level uses these data anymore, we can delete 
            if (UndoStruct[0].data) free(UndoStruct[0].data);

        //shift undo data
        memmove(UndoStruct, UndoStruct + 1, (UndoNumLevels - 1) * sizeof(tUndoStruct));
        UndoNumLevels--;

        //now clear objects that are no more used
        tUndoObjectStruct* us = pUndoObjectList;
        for (size_t i = 0; i < UndoNumObjects; i++, us++)
        {
            //shift the 'used in level' bitmask - this bitmask tells at what undo level is
            //this object used
            us->UsedInLevel = us->UsedInLevel >> 1;
            if (us->UsedInLevel == 0)
            {
                //neither bit is set anymore -> this object is not used anymore
                if (us->Type == EXPRESSION)
                    delete us->pObject.exp;
                else if (us->Type == DRAWING)
                    delete us->pObject.draw;

                memmove(us, us + 1, (UndoNumObjects - i - 1) * sizeof(tUndoObjectStruct));
                i--;
                us--;
                UndoNumObjects--;
            }
        }
    }

    //store the undo text (this is shown in main menu -> for example 'Undo insert object')
    strcpy_s(UndoStruct[UndoNumLevels].text, text.c_str());
    if (KeyboardEntryBaseObject && KeyboardEntryObject)
        strcat_s(UndoStruct[UndoNumLevels].text, "**");

    //calculate checksums
    for (size_t i = 0; i < NumDocumentElements; i++)
    {
        tDocumentStruct* ds = &TheDocument[i];
        if (ds->Object.exp)
        {
            if (ds->Type == EXPRESSION) ds->Checksum = ds->Object.exp->CalcChecksum();
            else if (ds->Type == DRAWING) ds->Checksum = ds->Object.draw->CalcChecksum();
        }
    }

    //store the main document structure
    UndoStruct[UndoNumLevels].data = malloc(NumDocumentElements * sizeof(tDocumentStruct));
    if (UndoStruct[UndoNumLevels].data == nullptr)
    {
        AfxMessageBox("Cannot reserve undo memory [3]",MB_OK | MB_ICONWARNING,NULL);
        return 0;
    }
    memcpy(UndoStruct[UndoNumLevels].data, TheDocument, NumDocumentElements * sizeof(tDocumentStruct));
    UndoStruct[UndoNumLevels].NumElements = NumDocumentElements;


    //finally, check if there is any changed object
    for (size_t i = 0; i < NumDocumentElements; i++)
    {
        tDocumentStruct* ds = &TheDocument[i];

        int j;
        tUndoObjectStruct* us = pUndoObjectList;
        for (j = 0; j < UndoNumObjects; j++, us++)
            if (us->Type == ds->Type && ds->Object.exp == us->pOriginal.exp && ds->Checksum == us->Checksum)
                break;

        if (j == UndoNumObjects)
        //object doesnt exist in undo database (or is changed), therefore we are creating the new one
        {
            if (UndoNumObjects >= UndoNumObjectsReserved)
            {
                UndoNumObjectsReserved += 50;
                pUndoObjectList = static_cast<tUndoObjectStruct*>(realloc(pUndoObjectList,
                                                                          UndoNumObjectsReserved * sizeof(
                                                                              tUndoObjectStruct)));
                if (pUndoObjectList == nullptr)
                {
                    AfxMessageBox("Cannot reserve Undo memory",MB_OK | MB_ICONWARNING,NULL);
                    return 0;
                }
                us = pUndoObjectList + UndoNumObjects;
            }
            if (ds->Type == EXPRESSION)
            {
                us->Checksum = ds->Object.exp->CalcChecksum();
                us->pObject.exp = new CExpression(nullptr, nullptr, ds->Object.exp->m_FontSize);
                us->pObject.exp->CopyExpression(ds->Object.exp, 2);
                us->pOriginal.exp = ds->Object.exp;
            }
            else if (ds->Type == DRAWING)
            {
                us->Checksum = ds->Object.draw->CalcChecksum();
                us->pObject.draw = new CDrawing();
                us->pObject.draw->CopyDrawing(ds->Object.draw);
                us->pOriginal.draw = ds->Object.draw;
            }
            us->Type = ds->Type;
            us->UsedInLevel = 0;
            UndoNumObjects++;
        }
        us->UsedInLevel |= 1 << UndoNumLevels;
    }

    UndoNumLevels++;

    AdjustMenu(1);
    return 0;
}


//extern tDocumentStruct *SpecialDrawingHover;
int CMainFrame::UndoCheckText(const std::string& text_def, int unique_ID)
{
    if (UndoNumLevels == 0) return 0;

    std::string text = text_def;
    if (LanguageStrings && unique_ID >= 20000 && unique_ID < 30000)
    {
        unsigned short pntr = LanguagePointers[unique_ID];
        if (pntr != 0xFFFF)
        {
            text = LanguageStrings[pntr];
        }
    }

    return text != UndoStruct[UndoNumLevels - 1].text;
}


#pragma optimize("s",on)
int CMainFrame::UndoRestore()
{
    if (UndoNumLevels == 0) return 0; //nothing to restore		

    tmpDrawing = nullptr;
    tmpDrawing2 = nullptr;
    SelectedDocumentObject = nullptr;
    SelectedDocumentObject2 = nullptr;
    prevTouchedObject = nullptr;
    SpecialDrawingHover = nullptr;
    prevSpecialDrawingHover = nullptr;


    if (pMainViewBase)
    {
        CDocument* pDoc = pMainViewBase->GetDocument();
        pDoc->SetModifiedFlag(1);
    }

    //first check what object do we need to delete
    //only objects that were modified are deleted and (latter) recreated,
    //objects that were not modified are not touched by undo restore.
    for (size_t i = 0; i < NumDocumentElements; i++)
    {
        if (TheDocument[i].Type == EXPRESSION)
        {
            int found = 0;
            for (int j = 0; j < UndoNumObjects; j++)
                if (TheDocument[i].Object.exp == pUndoObjectList[j].pOriginal.exp &&
                    pUndoObjectList[j].Type == TheDocument[i].Type &&
                    TheDocument[i].Object.exp->CalcChecksum() == pUndoObjectList[j].Checksum)
                {
                    const int tmp = 1 << (UndoNumLevels - 1);
                    if (pUndoObjectList[j].UsedInLevel & tmp)
                    {
                        found = true;
                        break;
                    }
                }

            if (!found) //didn't find the exact object in our history database
            {
                //obviously, as the item is not already in our history database then 
                //it was modified
                //so we will delete and latter recreate.
                //((CExpression*)(TheDocument[i].Object))->Delete(); //MOD - removed (redundant)
                delete TheDocument[i].Object.exp;
                TheDocument[i].Object.exp = nullptr;
            }
        }
        else if (TheDocument[i].Type == DRAWING)
        {
            bool found = false;
            for (int j = 0; j < UndoNumObjects; j++)
                if (TheDocument[i].Object.draw == pUndoObjectList[j].pOriginal.draw &&
                    pUndoObjectList[j].Type == TheDocument[i].Type &&
                    TheDocument[i].Object.draw->CalcChecksum() == pUndoObjectList[j].Checksum)
                {
                    const int tmp = 1 << (UndoNumLevels - 1);
                    if (pUndoObjectList[j].UsedInLevel & tmp)
                    {
                        found = true;
                        break;
                    }
                }

            if (!found) //didn't find the exact object in our history database
            {
                //obviously, as the item is not already in our history database then 
                //it was modified
                //so we will delete and latter recreate.
                //((CDrawing*)(TheDocument[i].Object))->Delete(); //MOD - removed (redundant)
                delete TheDocument[i].Object.draw;
                TheDocument[i].Object.draw = nullptr;
            }
        }
    }

    //then copy the main document data
    tDocumentStruct* oldDoc = TheDocument;
    int oldDocNumElements = NumDocumentElements;

    TheDocument = static_cast<tDocumentStruct*>(malloc(
        UndoStruct[UndoNumLevels - 1].NumElements * sizeof(tDocumentStruct)));
    if (TheDocument == nullptr)
    {
        NumDocumentElementsReserved = NumDocumentElements = 0;
        AfxMessageBox("Cannot reserve document memory [undo]",MB_OK | MB_ICONWARNING,NULL);
        free(oldDoc);
        return 0;
    }
    NumDocumentElementsReserved = NumDocumentElements = UndoStruct[UndoNumLevels - 1].NumElements;
    memcpy(TheDocument, UndoStruct[UndoNumLevels - 1].data, NumDocumentElements * sizeof(tDocumentStruct));

    //go through restored data and adjust pointer to objects
    for (size_t i = 0; i < NumDocumentElements; i++)
    {
        //if (TheDocument[i].MovingDotState==4) TheDocument[i].MovingDotState=0;
        if (TheDocument[i].Object.exp)
        {
            const int cc = 1 << (UndoNumLevels - 1); //MOD - added
            int j;
            bool found = false;
            for (j = 0; j < UndoNumObjects; j++)
                if (TheDocument[i].Object.exp == pUndoObjectList[j].pOriginal.exp &&
                    pUndoObjectList[j].Type == TheDocument[i].Type &&
                    TheDocument[i].Checksum == pUndoObjectList[j].Checksum &&
                    pUndoObjectList[j].UsedInLevel & cc) //MOD - strenghtened
                {
                    found = true;
                    break;
                }

            if (found) //found the object in our history list, 
            {
                int k;
                //check if it also exists in the original documment, if yes keep it.
                for (k = 0; k < oldDocNumElements; k++)
                    if (oldDoc[k].Object.v == TheDocument[i].Object.v) break;
                if (k == oldDocNumElements)
                {
                    //we have to copy the object from our history list because it 
                    //doesn't exist in the original document
                    if (pUndoObjectList[j].Type == EXPRESSION)
                    {
                        TheDocument[i].Object.exp = new CExpression(
                            nullptr, nullptr, pUndoObjectList[j].pObject.exp->m_FontSize);
                        TheDocument[i].Object.exp->CopyExpression(
                            pUndoObjectList[j].pObject.exp, 2);
                    }
                    else if (pUndoObjectList[j].Type == DRAWING)
                    {
                        TheDocument[i].Object.draw = new CDrawing();
                        TheDocument[i].Object.draw->CopyDrawing(pUndoObjectList[j].pObject.draw);
                    }
                }
            }
            else
            {
                //this should never happened - the object was not found in our history list
                AfxMessageBox("Error during Undo operation",MB_OK | MB_ICONWARNING,NULL);
                //DeleteDocumentObject(&TheDocument[i]);
                free(oldDoc);
                return 0;
            }
        }
        TheDocument[i].Checksum = 0;
    }

    free(oldDoc);

    //finally delete newest undo and non-needed objects
    for (int i = 0; i < UndoNumLevels - 1; i++)
        if (UndoStruct[i].data == UndoStruct[UndoNumLevels - 1].data)
        {
            if (i == UndoNumLevels - 1) //no other level uses these data anymore, will be deleted
                if (UndoStruct[UndoNumLevels - 1].data) free(UndoStruct[UndoNumLevels - 1].data);
            break;
        }
    UndoNumLevels--;

    for (int i = 0; i < UndoNumObjects; i++)
    {
        int tmp = 0xFFFF << UndoNumLevels; //MOD -it wa 1<<UndoNumLevels
        tmp = ~tmp;
        pUndoObjectList[i].UsedInLevel = pUndoObjectList[i].UsedInLevel & tmp;
        if (pUndoObjectList[i].UsedInLevel == 0) //this object is not used anymore
        {
            if (pUndoObjectList[i].Type == EXPRESSION && pUndoObjectList[i].pObject.exp)
            {
                //((CExpression*)(pUndoObjectList[i].pObject))->Delete(); //MOD - removed (redundant)
                delete pUndoObjectList[i].pObject.exp;
            }
            else if (pUndoObjectList[i].Type == DRAWING && pUndoObjectList[i].pObject.draw)
            {
                //((CDrawing*)(pUndoObjectList[i].pObject))->Delete();  //MOD - removed (redundant)
                delete pUndoObjectList[i].pObject.draw;
            }

            //int j;								//MOD - following three lines modified (changed to single memmove)
            //for (j=i;j<UndoNumObjects-1;j++)
            //	pUndoObjectList[j]=pUndoObjectList[j+1];
            memmove(pUndoObjectList + i, pUndoObjectList + i + 1, (UndoNumObjects - i - 1) * sizeof(tUndoObjectStruct));

            i--;
            UndoNumObjects--;
        }
    }
    SpecialDrawingHover = nullptr; //othwervise crashes because its address is changed

    AdjustMenu();
    return 0;
}
#pragma optimize("",on)

//clears undo memory - when exiting application
int CMainFrame::UndoRelease(int exit_app)
{
    UndoInit();

    delete pUndoObjectList;

    if (!exit_app) AdjustMenu();
    return 0;
}


//parsing XML string until it finds the specified text
//function must be fast!
char* CMainFrame::XML_search(const std::string& text, char* file)
{
    bool is_quotation = false;
    bool is_inside = false;
    size_t text_len = strlen(text.c_str());
    while (*file != 0)
    {
        const char ch = *file;
        if (ch == '<' && !is_inside) is_inside = true;
        if (ch == '>' && is_inside && !is_quotation) is_inside = false;
        if (ch == '"' && is_inside && is_quotation) is_quotation = false;
        else if (ch == '"' && is_inside && !is_quotation) is_quotation = true;
        if (strncmp(file, text.c_str(), text_len) == 0 && is_inside && !is_quotation && ch > ' ' && ch != '<')
        {
            return file;
        }
        file++;
    }
    return nullptr;
}

//parses the XML file and reads the next attribute-value pair
//this function must work fast!

char* CMainFrame::XML_read_attribute(char* attribute, char* value, char* file, int value_buffer_size)
{
    int j = 0;
    int k = 0;
    char started_attribute = 0;
    char started_value = 0;
    while (*file != 0)
    {
        if (char ch = *file; ch > ' ' || started_value == 1)
        {
            if (started_value == 0 && ch == '>') //no atribute-value pair found
            {
                attribute[0] = 0;
                value[0] = 0;
                return file + 1;
            }

            if (ch == '\\' && started_value == 1) //special character \XX (hex-code)
            {
                char tmp[3];
                tmp[0] = *(file + 1);
                tmp[1] = *(file + 2);
                tmp[2] = 0;
                if (tmp[0] == 0) return nullptr;
                if (tmp[1] == 0) return nullptr;
                int tt = 0;
                sscanf(tmp, "%X", &tt);
                ch = static_cast<char>(tt);
                file += 2;
            }

            if (started_value == 1 && *file == '"') //value finished - return results
            {
                attribute[j] = 0;
                value[k] = 0;
                return file + 1;
            }

            if (started_attribute == 2 && started_value == 0 && *file == '"') started_value = 1;
            if (started_attribute == 1 && ch == '=') started_attribute = 2;
            if (!started_attribute) started_attribute = 1;

            if (started_attribute == 1) attribute[j++] = ch;
            if (started_value == 1 && *file != '"') value[k++] = ch;
            if (j > 47) j = 47;
            if (k >= value_buffer_size) k = value_buffer_size - 1;
        }


        file++;
    }

    return nullptr;
}

//rearanges object positions - objects are pushed to down by moving one single object
int CMainFrame::RearangeObjects(int delta)
{
    //we check how much we need to push objects
    int mx = 0;
    for (size_t i = 0; i < NumDocumentElements; i++)
    {
        const tDocumentStruct& ds = TheDocument[i];
        if ((ds.MovingDotState & 0xC0) != 0x80)
            continue;
        int minx = ds.absolute_X;
        int maxx = ds.absolute_X + ds.Length;
        int miny = ds.absolute_Y - delta;
        int maxy = ds.absolute_Y + ds.Below;

        for (int j = 0; j < NumDocumentElements; j++)
        {
            const tDocumentStruct& ds2 = TheDocument[j];
            if ((ds2.MovingDotState & 0xC0) != 0 || ds2.MovingDotState == 5)
                continue;
            int minx2 = ds2.absolute_X;
            int maxx2 = ds2.absolute_X + ds2.Length;
            int miny2 = ds2.absolute_Y - ds2.Above;
            if (maxx2 > minx && minx2 < maxx &&
                miny2 > miny && miny2 < maxy)
            {
                int d = maxy - miny2;
                if (d > mx) mx = d;
            }
        }
        break;
    }

    if (mx == 0)
    {
        for (size_t i = 0; i < NumDocumentElements; i++)
            TheDocument[i].MovingDotState &= 0x3F;

        return 1;
    }
    delta = mx;


    bool fnd = true;
    while (fnd)
    {
        fnd = false;
        for (size_t i = 0; i < NumDocumentElements; i++)
        {
            tDocumentStruct& ds = TheDocument[i];
            if ((ds.MovingDotState & 0xC0) != 0x80)
                continue;
            //we found an object that is marked with high bit - we must 
            //rearange other objects around it not to touch them (by moving them down)
            int minx = ds.absolute_X;
            int maxx = ds.absolute_X + ds.Length;
            int miny = ds.absolute_Y - delta;
            int maxy = ds.absolute_Y + ds.Below;
            ds.MovingDotState |= 0x40;
            //if (ds->Type==1)
            //	if (delta==0) maxy+=((CExpression*)(ds->Object))->m_FontSize/5;

            for (size_t j = 0; j < NumDocumentElements; j++)
            {
                tDocumentStruct& ds2 = TheDocument[j];
                if ((ds2.MovingDotState & 0xC0) != 0 || ds2.MovingDotState == 5)
                    continue;
                int minx2 = ds2.absolute_X;
                int maxx2 = ds2.absolute_X + ds2.Length;
                int miny2 = ds2.absolute_Y - ds2.Above;
                int maxy2 = ds2.absolute_Y + (ds2.Type == EXPRESSION ? 0 : ds2.Below);
                if (maxx2 > minx && minx2 < maxx &&
                    maxy2 > miny && miny2 < maxy)
                {
                    ds2.absolute_Y += delta;
                    ds2.MovingDotState |= 0x80;
                    fnd = true;
                }
            }
        }
    }

    for (size_t i = 0; i < NumDocumentElements; i++)
        TheDocument[i].MovingDotState &= 0x3F;

    return 1;
}


void CMainFrame::OnUpdateEditImage(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(NumSelectedObjects ? 1 : 0);
    pCmdUI->SetCheck(0);
}

void CMainFrame::OnUpdateEditSaveequationimage(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(NumSelectedObjects ? 1 : 0);
    pCmdUI->SetCheck(0);
}

void CMainFrame::OnUpdateEditCopymathmlcode(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(NumSelectedObjects ? 1 : 0);
    pCmdUI->SetCheck(0);
}

void CMainFrame::OnUpdateEditCopylatexcode(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(NumSelectedObjects ? 1 : 0);
    pCmdUI->SetCheck(0);
}

//CRichEditCtrl *StaticMessageWindow;
CEdit* StaticMessageWindow;

int StaticMessageWindowCntr;
int StaticMessageWindowTextclr;
int StaticMessageWindowBkclr;
HFONT StaticMessageWindowFont;
extern int PresentationModeActiveTimer;

#pragma optimize("s",on)
void DisplayShortText(const std::string& text, int x, int y, int langID, int flags)
{
    //displays a message in child Edit box (CEdit) shown somewhere over main view
    //(note that CMathomirView uses OnCtlColor to define text/background colors)

    const std::string buff = GetTranslatedString(text, langID);

    if (flags != 1) PresentationModeActiveTimer = 0;
    if (!StaticMessageWindow)
    {
        StaticMessageWindow = new CEdit();
        StaticMessageWindow->Create(WS_CHILD | WS_VISIBLE, CRect(10, 10, 150, 50), pMainView, 123);
        //StaticMessageWindow->CreateEx(0,"edit","text",WS_POPUP,5,5,10,10,pMainView->m_hWnd,nullptr,0);
    }

    int fsize = 15;
    StaticMessageWindowTextclr = RGB(0, 160, 0);
    StaticMessageWindowBkclr = RGB(208, 255, 208);
    if (flags == 4)
    {
        //toolbar help tooltip
        StaticMessageWindowBkclr = RGB(255, 255, 128);
        StaticMessageWindowTextclr = RGB(96, 96, 96);
    }
    if (flags == 1)
    {
        fsize = 12;
        StaticMessageWindowTextclr = RGB(160, 160, 160);
        StaticMessageWindowBkclr = RGB(240, 240, 240);
    }
    if (flags == 2 || flags == 3)
    {
        //sigma and 'T' signs for math/text mode
        fsize = 18;
        if (flags == 2)
        {
            StaticMessageWindowTextclr = RGB(0, 0, 240);
            StaticMessageWindowBkclr = RGB(208, 208, 255);
        }
        else
        {
            StaticMessageWindowTextclr = RGB(0, 192, 0);
            StaticMessageWindowBkclr = RGB(208, 255, 208);
        }
    }
    fsize += max(0, ToolboxSize-60) / 12;

    HFONT pref = StaticMessageWindowFont;
    int fff = 4;
    if (flags == 2) fff = 3;

    StaticMessageWindowFont = CreateFont(
        fsize, //Size
        0, //width
        0, //escapement
        0, //Orientation
        flags == 2 || flags == 3 ? FW_BOLD : 0, //Weight
        0, //Italic
        0, //Underline
        0, //Strikeout
        FontCharSet[fff], //CharSet
        0, //OutPrecision
        0, //ClipPrecision
        0, //Quality
        0, //Pitch and Family
        FontFacenames[fff].c_str()); //facename

    StaticMessageWindow->SendMessage(WM_SETFONT, (WPARAM)StaticMessageWindowFont, 1);
    if (pref)
        DeleteObject(pref);

    //determine text size
    CDC* DC = theApp.m_pMainWnd->GetDC();
    DC->SelectObject(StaticMessageWindowFont);
    CSize g = DC->GetTextExtent(buff.data());
    theApp.m_pMainWnd->ReleaseDC(DC);
    StaticMessageWindow->SetWindowText(buff.data());

    int ysize = g.cy;
    RECT r;
    pMainView->GetClientRect(&r);
    if (y > r.bottom - ysize)
    {
        y -= ysize;
        if (KeyboardEntryBaseObject && (flags == 2 || flags == 3))
            y -= (KeyboardEntryBaseObject->Above + KeyboardEntryBaseObject->Below + 4) * ViewZoom / 100 + 2;
    }


    /*RECT rrr;
    pMainView->GetWindowRect(&rrr);
    x+=rrr.left;
    y+=rrr.top;*/

    if (flags == 4)
    {
        x -= (g.cx + fsize / 2) / 2;
    }
    if (x + g.cx > r.right - 10)
    {
        x = r.right - g.cx - 10;
    }
    if (x < -2) x = -2;
    StaticMessageWindow->SetWindowPos(nullptr, x, y, g.cx + fsize / 2, ysize,SWP_NOZORDER);
    StaticMessageWindow->ShowWindow(SW_SHOW);
    StaticMessageWindow->UpdateWindow();
    StaticMessageWindowCntr = 2 + static_cast<int>(buff.size()) / 15;
    if (flags == 4) StaticMessageWindowCntr += 5;
}
#pragma optimize("",on)

int CopyTranslatedString(char* dest, const std::string& eng_defstr, int id, size_t destlen)
{
    const char* defstr = eng_defstr.c_str();
    if (LanguageStrings && id < 36000)
    {
        //language database exists - check for the translation
        if (id < 0) id = 1000 - id;
        unsigned short pntr = LanguagePointers[id];
        if (pntr != 0xFFFF)
        {
            defstr = LanguageStrings + pntr;
        }
    }

    if (strlen(defstr) + 1 > destlen)
    {
        memcpy(dest, defstr, destlen - 1);
        dest[destlen - 1] = 0;
    }
    else
    {
        strcpy_s(dest, destlen, defstr);
    }
    return 1;
}

std::string GetTranslatedString(const std::string& eng_defstr, int id)
{
    if (LanguageStrings && id < 36000)
    {
        //language database exists - check for the translation
        if (id < 0) id = 1000 - id;
        unsigned short pntr = LanguagePointers[id];
        if (pntr != 0xFFFF)
        {
            return LanguageStrings + pntr;
        }
    }

    return eng_defstr;
}

#pragma optimize("s",on)
int ExecuteLink(const char* command)
{
    char buff[512];
    int match = 0;
    int found = -1;
    int d1 = static_cast<int>(strlen(command));
    for (size_t i = 0; i < NumDocumentElements; i++)
    {
        tDocumentStruct* ds = &TheDocument[i];

        if (ds->Type == EXPRESSION)
        {
            auto e = (CExpression*)ds->Object.draw;
            CExpression* label = e->GetLabel();
            if (e->m_IsHeadline || label)
            {
                buff[0] = 0;
                if (e->m_IsHeadline) e->ConvertToPlainText(500, buff);
                else label->ConvertToPlainText(500, buff);
                buff[500] = 0;
                int d2 = static_cast<int>(strlen(buff));
                if (strcmp(buff, command) == 0)
                {
                    match = 0x7FFF;
                    found = i;
                    break;
                } //perfect match
                int tmp = 0;
                for (int j = 0; j < min(d1, d2); j++)
                {
                    char ch1 = command[j];
                    char ch2 = buff[j];
                    if (ch1 == ch2) tmp += 3;
                    else if (toupper(ch1) == toupper(ch2)) tmp += 2;
                    else
                    {
                        if (ch1 == '_' || ch1 == '-') ch1 = ' ';
                        if (ch2 == '_' || ch2 == '-') ch2 = ' ';
                        if (ch1 == ch2) tmp += 1;
                    }
                }
                if (d1 == d2) tmp += 2;
                if (tmp > match)
                {
                    match = tmp;
                    found = i;
                }
            }
        }
    }
    if (found > -1 && match > d1 / 2 && _strnicmp("http:", command, 5))
    {
        //we found internal mark
        if (ViewX + TheClientRect.right * 100 / ViewZoom < TheDocument[found].absolute_X + TheDocument[found].Length)
            ViewX = TheDocument[found].absolute_X + TheDocument[found].Length - TheClientRect.right * 100 / ViewZoom;
        if (ViewX > TheDocument[found].absolute_X) ViewX = TheDocument[found].absolute_X;

        ViewY = TheDocument[found].absolute_Y - TheDocument[found].Above - 10;
        if (ViewY < 0) ViewY = 0;
        if (ViewX < 0) ViewX = 0;
        pMainView->RepaintTheView();
    }
    else
    {
        ShellExecute(nullptr, nullptr, command, nullptr, nullptr,SW_SHOWNORMAL);
    }

    return 0;
}
#pragma optimize("",on)
