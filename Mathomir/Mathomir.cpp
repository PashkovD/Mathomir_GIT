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
#include "toolbox.h"
#include "popupmenu.h"
#include "MathomirDoc.h"
#include "MathomirView.h"
#include "drawing.h"
#include "afxwin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/* TODO 
	- poravnavanje expressiona: gore, dolje, centralno

TODO 
   - ubrzati MouseMove, naroèito Free moving mode (to je za bolji response na miša)
   - u vezi sa time ubrzati SelectExpression, SelectDrawing i CalcChecksum funkcije
   - tastatura - kako iskorisitit space bar za vrijeme crtanja
   - problem sa jedinicama - jedinica je 'naslonjena' na vrijednost - primjer: 1kg/1s -> 1kg/(1s)
   - poboljšati drawing box (tehnièko crtanje?)
   - right-to-left writing
   - object Z adjustment: On top, To bottom.
  - nastavci za linije - strelica, kružiæ, crtica - da se desnim klikom mogu staviti 'line endings'
  - updejtati toolbox drawing boxa (dvije nove opcije umjesto debljine linije?)
  - završiti command line drawing boxa - nove komande (turtle, moveto, lineto...). Moguænost matematike u line editoru

TODO  
	   - dodati radijus, diametar,
	   - dodati oznaku kuta
	   - operator za laplaceovu trasformaciju (o-o)
	   - napraviti da se u function plotteru mogu prikazati vektori/matrice/niz(zarez,toèkazarez) sa fiksnim vrijednostima
	   - da se vektori/matrice/nizovi(zarez,toèkazarez) mogu statistièki obraðivati (zbroj, average, median) i konvertirati iz jednog u drugi
	   - dodati matematièku analizu u function ploter (minumume, maksimume, sjecišta, numerièke integrale)
	   - dodati komande \expand i \condense
	   - napraviti kemijske formule (novi tip elementa - kemijski znak - ima moguænost dodavanja crtica ili toèkica za prikaz veze)
	   - desni klik na row/column insertion point daje opcije : select column/row; delete column/row
	   - ZAVRŠITI LATEX EXPORT
	   - ugraðene priruène tablice (konstante, transformacije, integrali...)
	   - ubrzati plotting (bolji response na miša)
	   - TODO - function ploter - dodati nove funkcije (...min, max, step, rampa...)
	   - prijevod quick guide-a
	   - podrška za DLLove (plotanje u polarnim koordinatama, 3D plotting,...)
	   - podrška za pisanje kemijskih formula (H2O, Hg,...  dva minusa = long dash, tri minusa = double long dash...)
	   - umetanje crteža u jednadžbe (linije, zagrade... naprimjer komandama \line, \brace...)


TODO ideje
- mogućnost movanja selekcije klik-move-klik metodom, a ne samo klik and drag metodom?
- neiskorištene tipke: ?,ALT+;.ALT+:, Ins, Alt+Ins, double zarez(,,) i double točkazarez (;;)
- dodati u typing-control-menu opcije za vektor, dash, dot, (možda i bold,italic?)
- napraviti podršku za bra, ket i superscript indexe


TODO
- da se u virtual keyboard mogu napraviti Always Uppercase i Cast-as-Function
- kako riješiti upisivanje zanaka = unutar argumenta funkcije
- možda neke dulje komande isto na točku...
- što s dobule jednako ==?
- automatska detekcija krivog unosa unutar funkcije (= znak) moguća automatski ispravak sa double-spacie ili Shift+left?
- zvučni feedback u slučaju krivog unosa u funkciju?
- detekcija mistypinga - dvije tipke pritisnute istovremeno
- INS key - ne smije raditi na Acceleratore i neke druge gluposti. Možda da se doda više nivoa.
- double comma - switching math/text mode popraviti - pisanje "text, x, text" - treba tri puta kliknuti zarez!!!

- definitivno riješiti problem upisivanja teksta u math-typing modu - to se redovito događa
- riješiti problem sa odabirom boje i debljine linije prije početka crtanja
- riješiti problem sa promjenom boje i debljine linje nakon crtanja linije
- korištenje TABa umujesto ENTERa za prelazak iz jedne čelije tablice u drugu?
- mogućnost da se sa tipkovnice briše i editira dokumen (selektiranje više objekata npr shift + arrow keys)
- treba istražiti autocomplete i spacebar key (brisanje autocomplete prijedloga) - spacebar key će kreirati spacement neželjeno
- napraviti da se mogu crtati linije i kada se koristi ellipse odn. rectangle drawing tool (uske kružnice/pravokutnici)

PITANJA
- kako nazvati presentation mod i kako ga hendlati?
- kako spriječiti slučajne klikove na touchpadu (pali typing mode, ostavlja točkice u drawing mode-u)


ZAVRŠITI
- kvadriranje diferencijala (različiti prikaz d^2x i dx^2)
- mogućnost kvadriranja d/d elementa

- ništa se ne dogodi kada se pointer približi rubu client area gdje se treba pojaviti ruler - loš osjećaj
- kad tipkam postoj problem kaj zaboravim izači iz phunkcije pa samo nastavim tipkati
- mali spacebar razmak samo ako je prije toga također bila stisnuta razmaknica!!!
- backspace i delete key trebaju spremati obrisano u kratkotrajni clipboard tako da se spremljeno može paste-ati (služi za brzo rearanžiranje)
- mogućnost da se mišem selektira (multi-touch) više itema i dok se nešto carry sa mišem tako da se može replaceati ili napraviti implanting.

- WINDOWS 7
- Kako riješiti problem da više korisnika može imati svako svoj mathomir.set fajl?
- napraviti da se može raditi click-draw-click hand-drawing (ne samo click-and-drag hand-drawing)
- napraviti mekše vertikalno scrollanje na WM_MOUSEWHEEL
- NE RADI CTRL+V ZA INSERTANJE NA INSERTION POINT AKO NIJE UKLJUCEN TYPING MODE (da li to treba?)

Whats NEW
- toolbox menu strechable
- promjenjen način dobivanja upercase karaktera kada se drži long-press (da se ne generiraju eventualni acceleratori pri startu softvera)

- smooth scrooling
- auto scroll (kada se movaju objekti)

- /ceil i /floor naredbe; dodane ceil i floor zagrade

- ne moze se vise touch-ati expression na njezinom gornjem rubu (da se smanji touchines)

*/

// global definitions

const COLORREF ColorTable[] =
{
    RGB(0, 0, 0),
    RGB(204, 0, 0),
    RGB(0, 204, 0),
    RGB(0, 0, 192),
    RGB(144, 144, 144),
};

int ToolboxSize = 60; //the width of the toolbox window (zero if not shown)
int BaseToolboxSize;
CToolbox* Toolbox = nullptr; //toolbox object
CSingleDocTemplate* pDocTemplate;
HANDLE ProcessHeap;
int ViewX = 0;
int ViewY = 0;
int ViewMaxX = 500;
int ViewMaxY = 300;
short ViewZoom = 100;
size_t NumDocumentElements = 0;
size_t NumDocumentElementsReserved = 0;
tDocumentStruct* TheDocument;
int MouseMode = 0; //0=free moving, 1=right click moving, (many more)...
CExpression* ClipboardExpression = nullptr;
CExpression* prevClipboardExpression;
CDrawing* ClipboardDrawing = nullptr;
CExpression* KeyboardEntryObject = nullptr;
tDocumentStruct* KeyboardEntryBaseObject = nullptr;
int IsHighQualityRendering = 0;
int IsHalftoneRendering = 0;
int CenterParentheses_not_used = 0; //not used any more
int DefaultParentheseType = 1; //type of parenthese: 0-increasing in size, 1-aligned, 2-small
PopupMenu* Popup;
int FrameSelections = 1; // (only power, function, and symbol: types 3,6,7 and 9)
int FixFontForNumbers = 1; //1 if all numbers when entered from keyboard have the same font (arial)
int UseALTForExponents = 1;
//if ALT+number is used to quickly generate variable exponent (SHIFT+ALT for negative exponents)
int PaperHeight = 1650;
int PaperWidth = 1165;
RECT MainWindowRect;
int ImageSize = 100; //size of the image produced by mathomir when exporting euqation images (in %)
int ForceHighQualityImage = 1;
int ForceHalftoneImage = 1;
int DefaultFontSize = 100;
int MovingDotSize = 6;
int MovingDotPermanent = 0;
int IsDrawingMode = 0;
int AccessLockedObjects = 0;
int GRID = 16;
//int SnapToGrid=0;
int IsShowGrid = 0;
int IsSimpleVariableMode = 1;
int IsMathDisabled = 0;
char ImaginaryUnit = 0;
int ShadowSelection = 0;
int AutosaveOption = 0;
int MoveCursorOnWheel = 1;
int EnableMenuShortcuts = 1;
int F1SetsZoom = 0;
int PrintTextAsImage = 0;
int UseCommaAsDecimal = 0;
int SnapToGuidlines = 1;
int UseCTRLForZoom = 1;
int ViewOnlyMode = 0;
int UseSpecialCapsLock = 1;
int UseToolbar = 1;
int ToolbarUseCross = 0;
int ToolbarEditNodes = 0;
int MouseWheelDirection = 0;
int DefaultZoom = 100;
int RightButtonTogglesWheel = 1;
int WheelScrollingSpeed = 60;
int PageNumeration = 0;
int AutoResizeToolbox = 0;
int UseComplexIndexes = 0;
char* LanguageStrings;
unsigned short* LanguagePointers;
char HelpTutorLoaded = 0;
int UseCapsLock;
char NoImageAutogeneration;


BEGIN_MESSAGE_MAP(CMathomirApp, CWinApp)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
    // Standard file based document commands
    ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
    // Standard print setup command
    ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


CMathomirApp::CMathomirApp()
{
}


// The one and only CMathomirApp object
CMathomirApp theApp;


// CMathomirApp initialization

#pragma optimize("s",on)
BOOL CMathomirApp::InitInstance()
{
    ProcessHeap = GetProcessHeap(); //Store Process Heap handle for future use

    // InitCommonControls() is required on Windows XP if an application manifest specifies use of 
    // ComCtl32.dll version 6 or later to enable visual styles. Otherwise, any window creation will fail.
    InitCommonControls();

    //call base implementation of InitInstance
    CWinApp::InitInstance();


    // Change the registry key under which our settings are stored
    SetRegistryKey(_T("Mathomir"));
    LoadStdProfileSettings(4); // Load standard INI file options (including MRU)

    // Register the application's document templates.  Document templates
    //  serve as the connection between documents, frame windows and views
    pDocTemplate = new CSingleDocTemplate(IDR_MAINFRAME,RUNTIME_CLASS(CMathomirDoc),RUNTIME_CLASS(CMainFrame),
                                          RUNTIME_CLASS(CMathomirView));
    if (!pDocTemplate) return FALSE;
    AddDocTemplate(pDocTemplate);

    // Parse command line for standard shell commands, DDE, file open
    CCommandLineInfo cmdInfo;
    ParseCommandLine(cmdInfo);

    this->m_nCmdShow = 0; //this will suppress the ProcessShellCommand to show the window

    int origcmd = 0;
    if (cmdInfo.m_nShellCommand == cmdInfo.FilePrint || cmdInfo.m_nShellCommand == cmdInfo.FilePrintTo)
    {
        //for file print and file print to commands we must call the ProcessShellCommand later (when settings are loaded)
        origcmd = cmdInfo.m_nShellCommand == cmdInfo.FilePrint ? 1 : 2;
        cmdInfo.m_nShellCommand = cmdInfo.FileNew;
    }
    ProcessShellCommand(cmdInfo);


    ((CMathomirView*)m_pMainWnd)->InitSpecific(); //adjusting the menu
    ((CMainFrame*)theApp.m_pMainWnd)->SetFontsToDefaults();

    //creates popupmenu
    Popup = new PopupMenu();
    if (Popup)
        Popup->CreateEx(WS_EX_TOPMOST, AfxRegisterWndClass(CS_OWNDC), "PopupMenu",WS_CLIPCHILDREN | WS_POPUP, 5, 5, 10,
                        10, m_pMainWnd->m_hWnd,nullptr, 0);
    else
        AfxMessageBox("Cannot create Popup menu",MB_OK | MB_ICONSTOP,NULL);

    //TODO  should better investigate how the following works when there are more than one monitor (What gets returned by GetDesktopWindow()?)
    RECT desk_rect;
    GetWindowRect(GetDesktopWindow(), &desk_rect);
    if (desk_rect.right >= 1300 && desk_rect.bottom >= 750) { BaseToolboxSize = ToolboxSize = 84; }
    if (desk_rect.right >= 1900 && desk_rect.bottom >= 1100) { BaseToolboxSize = ToolboxSize = 102; }
    if (desk_rect.right >= 1600 && desk_rect.bottom >= 900) { DefaultZoom = ViewZoom = 120; }
    if (desk_rect.right >= 1900 && desk_rect.bottom >= 1100) { DefaultZoom = ViewZoom = 150; }

    //creates toolbox
    int pw = PaperWidth;
    int ph = PaperHeight;
    int pn = PageNumeration;
    Toolbox = new CToolbox(0);
    if (Toolbox)
    {
        Toolbox->CreateEx(0, AfxRegisterWndClass(CS_OWNDC), "Toolbox",WS_CHILD, 5, 5, 10, 10, m_pMainWnd->m_hWnd,nullptr,
                          nullptr);
        NoImageAutogeneration = 2;
        Toolbox->ShowWindow(SW_SHOWNA);
    }
    else
        AfxMessageBox("Cannot create Main Toolbox",MB_OK | MB_ICONSTOP,NULL);
    if (NumDocumentElements)
    {
        PaperWidth = pw;
        PaperHeight = ph;
        PageNumeration = pn;
    } //if the document is alreay loaded, do not change the paper dimension settings (as these are loaded with the document)

    if (origcmd)
    {
        cmdInfo.m_nShellCommand = origcmd == 1 ? cmdInfo.FilePrint : cmdInfo.FilePrintTo;
        ProcessShellCommand(cmdInfo);
    }

    //Default settings are loaded during Toolbox construction (from mathomir.set file)
    //Now adjust main window position according to loaded settings (found in MainWindowRect)

    int X, Y, CX, CY;
    int StartMaximized = 0;
    if (MainWindowRect.right - MainWindowRect.left > 80 &&
        MainWindowRect.bottom - MainWindowRect.top > 60 &&
        MainWindowRect.right > 30 &&
        MainWindowRect.left < desk_rect.right - 30 &&
        MainWindowRect.bottom > 30 &&
        MainWindowRect.top < desk_rect.bottom - 30)
    {
        //the MainWindowRect will be visible on the desktop window; we will use these settings
        X = MainWindowRect.left;
        Y = MainWindowRect.top;
        CX = MainWindowRect.right - MainWindowRect.left;
        CY = MainWindowRect.bottom - MainWindowRect.top;

        if (X <= 0 && Y <= 0 &&
            (CX >= desk_rect.right || CY >= desk_rect.bottom) &&
            CX > 2 * desk_rect.right / 3 &&
            CY > 2 * desk_rect.bottom / 3)
        {
            //the MainWindowRect is big enough, we can start window maximized
            StartMaximized = 1;
            goto InitInstance_SetDefaultSize;
        }
    }
    else
    {
    InitInstance_SetDefaultSize:
        //the MainWindowRect would not be visible
        if (desk_rect.bottom <= 0) desk_rect.bottom = 600;
        if (desk_rect.bottom > 4096) desk_rect.bottom = 1024;
        if (desk_rect.right <= 0) desk_rect.right = 800;
        if (desk_rect.right > 8192) desk_rect.right = 1600;
        CX = max(desk_rect.right/2, 650);
        CY = max(2*desk_rect.bottom/3+65, 525);
        X = desk_rect.right / 2 - CX / 2;
        Y = (desk_rect.bottom - CY) / 3;
        if (HelpTutorLoaded && CX < 800) StartMaximized = 1;
    }

    //finally, position the main window (Toolbox position will be adjusted automatically)

    if (X + CX < 20) X += 20;
    if (Y + CY < 20) Y += 20;
    WINDOWPLACEMENT wp;
    wp.flags = 0;
    wp.length = sizeof(wp);
    wp.ptMaxPosition.x = 0;
    wp.ptMaxPosition.y = 0;
    wp.ptMinPosition.x = 0;
    wp.ptMinPosition.y = 0;
    wp.rcNormalPosition.left = X;
    wp.rcNormalPosition.right = X + CX;
    wp.rcNormalPosition.top = Y;
    wp.rcNormalPosition.bottom = Y + CY;
    wp.showCmd = StartMaximized ? SW_SHOWMAXIMIZED : SW_SHOWNORMAL;
    if (ViewOnlyMode)
    {
        //adjusting the window size for the view-only mode
        int maxy = 100;
        int maxx = 150;
        for (size_t jj = 0; jj < NumDocumentElements; jj++)
        {
            if (TheDocument[jj].absolute_Y + TheDocument[jj].Below > maxy)
                maxy = TheDocument[jj].absolute_Y + TheDocument[jj].Below;
            if (TheDocument[jj].absolute_X + TheDocument[jj].Length > maxx)
                maxx = TheDocument[jj].absolute_X + TheDocument[jj].Length;
        }
        wp.rcNormalPosition.left = X + CX - min(CX, maxx+40);
        wp.rcNormalPosition.bottom = Y + min(CY, maxy+60+((ViewOnlyMode==1)?30:0));
        StartMaximized = 0;
        wp.showCmd = SW_SHOWNORMAL;
    }


    m_pMainWnd->SetWindowPlacement(&wp);
    this->m_nCmdShow = StartMaximized ? SW_SHOWMAXIMIZED : SW_SHOWNORMAL;

    Sleep(20);
    m_pMainWnd->SetActiveWindow();

    return TRUE;
}


// ***********************************************************
// CAboutDlg dialog used for App About
// ***********************************************************
class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

    // Dialog Data
    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
    virtual BOOL OnInitDialog();

    // Implementation
protected:
    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnBnClickedButton1();
    afx_msg void OnBnClickedButton2();
    CButton SendMailButton;
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    CButton WebPageButton;
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BUTTON2, SendMailButton);
    DDX_Control(pDX, IDC_BUTTON1, WebPageButton);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
    ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
    ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
    ON_WM_CREATE()
END_MESSAGE_MAP()

// App command to run the dialog
void CMathomirApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}


// CMathomirApp message handlers


void CAboutDlg::OnBnClickedButton1()
{
    ShellExecute(nullptr,nullptr, "http://gorupec.awardspace.com/mathomir.html",nullptr,nullptr,SW_SHOWNORMAL);
}

void CAboutDlg::OnBnClickedButton2()
{
    ShellExecute(nullptr,nullptr, "mailto:danijel.gorupec@gmail.com",nullptr,nullptr,SW_SHOWNORMAL);
}

int CAboutDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDialog::OnCreate(lpCreateStruct) == -1)
        return -1;

    //HICON icon=::LoadIcon(theApp.m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME));
    //SendMailButton.SetIcon(icon);

    return 0;
}

#pragma optimize("s",on)
BOOL CAboutDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    HICON icon = ::LoadIcon(theApp.m_hInstance,MAKEINTRESOURCE(IDI_ICON1));
    SendMailButton.SetIcon(icon);
    icon = ::LoadIcon(theApp.m_hInstance,MAKEINTRESOURCE(IDI_ICON2));
    WebPageButton.SetIcon(icon);

    return 1;
}
