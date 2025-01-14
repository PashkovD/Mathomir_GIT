// Equation.h : main header file for the Equation application
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include <string>

#include "drawing.h"
#include "resource.h"

//HQZM (and HQZMp=HQZM/2) define zoom level when doing presentation-mode rendering
#define HQZM 256
#define HQZMp 128

#pragma warning(disable:4996)


class CExpression;

class CMathomirApp : public CWinApp
{
public:
    CMathomirApp();

    BOOL InitInstance() override;

    afx_msg void OnAppAbout();
    DECLARE_MESSAGE_MAP()
};

//the TEACHER_VERSION switch enables certain options like digital exam
#define TEACHER_VERSION

extern HANDLE ProcessHeap;

enum doc_type:byte
{
    EXPRESSION = 1,
    DRAWING = 2
};

//The main document strcture (the main document is stored as an array of these structures)
struct tDocumentStruct
{
    union
    {
        CExpression* exp;
        CDrawing* draw;
        void* v;
    } Object;

    int absolute_X;
    int absolute_Y;
    int Checksum; //calculated for Undo operation
    short Length;
    short Above;
    short Below;
    byte MovingDotState;
    doc_type Type; //1-expression, 2-drawing
};


// *************************************
// initializes Undo memory
#define NUM_UNDO_LEVELS 5

struct tUndoStruct
{
    void* data;
    size_t NumElements;
    char text[32];
};

struct tUndoObjectStruct
{
    union
    {
        CExpression* exp;
        CDrawing* draw;
        void* v;
    } pObject, pOriginal;

    doc_type Type;
    int UsedInLevel; //bitmask that tells us this object is used at what undo level
    int Checksum;
};

extern tUndoStruct UndoStruct[];
extern int UndoNumLevels;
// ***********************************


#define NUM_COLORS 4
extern const COLORREF ColorTable[];

extern size_t NumDocumentElements;
extern size_t NumDocumentElementsReserved;
extern tDocumentStruct* TheDocument;
extern int ViewX;
extern int ViewY;
extern int ViewMaxX;
extern int ViewMaxY;
extern short ViewZoom;
extern CMathomirApp theApp;
extern int SmallCapsFactor;
extern int ToolboxSize;
extern int BaseToolboxSize;
extern int MouseMode;
extern CExpression* KeyboardEntryObject;
extern tDocumentStruct* KeyboardEntryBaseObject;
extern int IsHighQualityRendering;
extern int IsHalftoneRendering;
extern int CenterParentheses_not_used; //not used any more
extern int DefaultParentheseType; //type of parenthese: 0-increasing in size, 1-aligned, 2-small
extern int FrameSelections;
extern int FixFontForNumbers;
extern int UseALTForExponents; //not used any more
extern int PaperWidth;
extern int PaperHeight;
extern RECT MainWindowRect;
extern int ImageSize;
extern int ForceHighQualityImage;
extern int ForceHalftoneImage;
extern int DefaultFontSize;
extern int MovingDotSize;
extern int MovingDotPermanent;
extern int IsDrawingMode; //if nonzero then we are drawing (1=rectangle,...)
extern int AccessLockedObjects; //can access locked object or not by mouse
extern int GRID;
//extern int SnapToGrid;
extern int IsShowGrid;
extern int IsSimpleVariableMode;
extern int IsMathDisabled;
extern char ImaginaryUnit;
extern int ShadowSelection;
extern int AutosaveOption;
extern int MoveCursorOnWheel;
extern int EnableMenuShortcuts;
extern int F1SetsZoom;
extern int PrintTextAsImage;
extern int UseCommaAsDecimal;
extern int UseWideCursor;
extern int UseCTRLForZoom;
extern int SnapToGuidlines;
extern char* LanguageStrings;
extern unsigned short* LanguagePointers;
extern int ViewOnlyMode;
extern int UseSpecialCapsLock;
extern int UseToolbar;
extern int ToolbarUseCross;
extern int ToolbarEditNodes;
extern int UseCapsLock;
extern int MouseWheelDirection;
extern int DefaultZoom;
extern int RightButtonTogglesWheel;
extern int WheelScrollingSpeed;
extern int PageNumeration;
extern int AutoResizeToolbox;
extern int UseComplexIndexes;
extern char NoImageAutogeneration;

//coloring definitions

#define BLACK_COLOR RGB(0,0,0)
#define BLUE_COLOR RGB(92,92,255)
#define GREEN_COLOR RGB(32,224,32)
#define SHADOW_BLUE_COLOR RGB(224,224,255)
#define SHADOW_BLUE_COLOR2 RGB(186,186,255)
#define SHADOW_BLUE_COLOR3 RGB(128,128,174)
#define DOCUMENT_AREA_BACKGROUND RGB(208,208,208)
#define PALE_RGB(x) ((((0xFF-((x>>16)&0xFF))/2+((x>>16)&0xFF))<<16)+(((0xFF-((x>>8)&0xFF))/2+((x>>8)&0xFF))<<8)+((0xFF-(x&0xFF))/2+(x&0xFF)))

#ifdef TEACHER_VERSION
struct tPublicKey
{
    int64_t N;
    int64_t X;
};

extern tPublicKey* PublicKey;
extern unsigned char TheTimeLimit;
extern unsigned char TheMathFlags;
extern DWORD TheExamStartTime;
extern unsigned char DisableEditing;
extern unsigned char WarningDisplayed;
#endif

struct tPasswordDlgStruct
{
    bool is_exam;
    char password[24];
    int time_limit;
    bool disable_symbolic_math;
    bool disable_math;
    bool canceled;
};

extern tPasswordDlgStruct* PasswordDlgStruct;
extern char TheFileType;


HFONT GetFontFromPool(char Face, bool Italic, bool Bold, unsigned short Size);
HFONT GetFontFromPool(byte combination, unsigned short Size);
void ClearFontPool();
HPEN GetPenFromPool(int width, bool IsBlue, COLORREF color = 0);
int PaintCheckedSign(CDC* DC, int x, int y, short size, bool IsChecked);
void DisplayShortText(const std::string& text, int x, int y, int LanguageID, int flags = 0);
int AddDocumentObject(doc_type type, int X, int Y);
int CopyTranslatedString(char* dest, const std::string& defstr, int id, size_t destlen);

extern "C++" {
template <size_t Size>
int CopyTranslatedString(char (&dest)[Size], const std::string& defstr, const int id)
{
    return CopyTranslatedString(dest, defstr, id, Size - 1);
}
}

std::string GetTranslatedString(const std::string& eng_defstr, int id);
void FatalErrorHandling();


//#define CRTDBG_MAP_ALLOC
