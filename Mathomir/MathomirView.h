// EquationView.h : interface of the CMathomirView class
//


#pragma once


class CMathomirView : public CView
{
protected: // create from serialization only
    CMathomirView();
    DECLARE_DYNCREATE(CMathomirView)

    // Attributes
    CMathomirDoc* GetDocument() const;

    // Operations
    // Overrides
    void OnDraw(CDC* pDC) override; // overridden to draw this view
    BOOL PreCreateWindow(CREATESTRUCT& cs) override;

    //	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
    //	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
    //	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
    //	virtual void OnWindowPosChanging(WINDOWPOS *lpwp);

    // Implementation
public:
    ~CMathomirView() override;
#ifdef _DEBUG
    void AssertValid() const override;
    void Dump(CDumpContext& dc) const override;
#endif

    tDocumentStruct* m_PopupMenuObject;

    // Generated message map functions
protected:
    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    void AdjustPosition();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnZoom40();
    afx_msg void OnZoom60();
    int RepaintTheView(int force_recalculate = 0);
    afx_msg void OnZoom80();
    afx_msg void OnZoom100();
    afx_msg void OnZoom300();
    afx_msg void OnZoom400();
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnSysChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    void SendKeyStroke(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnHqRend();
    //afx_msg void OnParentheseheightEverincreasing();
    //afx_msg void OnParentheseheightNormal();
    //afx_msg void OnParentheseheightSmall();
    int AdjustMenu();
    void SetMousePointer();
    //afx_msg void OnViewCenterparenthesecontent();
    afx_msg void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    int GentlyPaintObject(const tDocumentStruct& ds, CDC* DC);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    int PopupCloses(int UserParam, int ExitCode);
    //afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
    afx_msg void OnSelectionsFrame();
    afx_msg void OnSelectionsUnderline();
    afx_msg void OnSelectionsNone();
    afx_msg void OnSelectionsIntelligentframing();
    afx_msg void OnKeyboardFixfontfornumbers();
    //afx_msg void OnKeyboardAltforexponents();
    //int CheckDocumentMemoryReservations(void);
    int PaintClipboard(int X, int Y);
    int RestoreClipboardBackground();
    void PaintDrawingHotspot(char erase_only = 0);
    int DeleteDocumentObject(tDocumentStruct* object);

protected:
    void OnPrint(CDC* pDC, CPrintInfo* pInfo) override;
    BOOL OnPreparePrinting(CPrintInfo* pInfo) override;

public:
    afx_msg void OnViewHalftonerendering();
    afx_msg void OnPageA4portrait();
    afx_msg void OnPageA4landscape();
    afx_msg void OnToolboxLarge();
    afx_msg void OnToolboxMedium();
    afx_msg void OnToolboxSmall();
    afx_msg void OnSaveoptionsSaveasdefault();
    afx_msg void OnSaveoptionsSaveas();
    afx_msg void OnSaveoptionsLoad();
    afx_msg void OnTextend();
    afx_msg void OnEditCut();
    afx_msg void OnEditPaste();
    afx_msg void OnEditCopy();
    afx_msg void OnEditDelete();
    afx_msg void OnEditCopyImage();
    void SaveImageToFile(int cx, int cy, CDC* bmpDC);
    int MakeImageOfExpression(CObject* expr);
    int MakeImageOfDrawing(CObject* drawing);
    //int CopyMathMLCode(CObject *expr);
    int CopyLaTeXCode(CObject* expr);
    afx_msg void OnOutputimageSize200();
    afx_msg void OnOutputimageSize150();
    afx_msg void OnOutputimageSize100();
    afx_msg void OnOutputimageSize80();
    afx_msg void OnOutputimageForcehighquality();
    afx_msg void OnOutputimageForcehalftone();
    afx_msg void OnFontsizeVerylarge();
    afx_msg void OnFontsizeLarge();
    afx_msg void OnFontsizeNormal();
    afx_msg void OnFontsizeSmall();
    // initializes Undo memory
    int UndoInit();
    int UndoSave(const char* text, int unique_ID = -1);
    int UndoRestore();
    afx_msg void OnEditUndo();
    afx_msg void OnMovingdotLarge();
    afx_msg void OnMovingdotMedium();
    afx_msg void OnMovingdotSmall();
    afx_msg void OnMovingdotPermanent();
    afx_msg void OnHelpQuickguide();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    int PasteDrawing(CDC* DC, int cursorX, int cursorY, CObject* drawing = nullptr, int select = 0);
    afx_msg void OnEditAccesslockedobjects();
    afx_msg void OnGridFine();
    afx_msg void OnGridMedium();
    afx_msg void OnGridCoarse();
    afx_msg void OnGridSnaptogrid();
    afx_msg void OnViewShowgrid();
    CObject* ComposeDrawing(int* X, int* Y, int compose_from_selection, int select_for_moving);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    int InitSpecific();
    afx_msg void OnKeyboardSimplevariablemode();
    afx_msg void OnKeyboardVerysimplevariablemode();
    int StartKeyboardEntryAt(int AbsoluteX, int AbsoluteY, int is_textmode = 0);
    afx_msg void OnSymboliccalculatorEnable();
    afx_msg void OnImaginaryunitI();
    afx_msg void OnImaginaryunitJ();
    afx_msg void OnEditSaveequationimage();
    //afx_msg void OnEditCopymathmlcode();
    afx_msg void OnSelectionsShadowselections();
    afx_msg void OnAutosaveNever();
    afx_msg void OnAutosaveLow();
    afx_msg void OnAutosaveMedium();
    afx_msg void OnAutosaveHigh();
    afx_msg void OnAutosaveLoadtheautosave();
    afx_msg void OnZoomWheelzoomadjustspointer();
    afx_msg void OnKeyboardGeneralvariablemode();
    afx_msg void OnKeyboardAltmenushortcuts();
    afx_msg void OnViewZoomin();
    afx_msg void OnViewZoomout();
    afx_msg void OnFontfacesFont1();
    afx_msg void OnFontfacesFont2();
    afx_msg void OnFontfacesFont3();
    afx_msg void OnFontfacesFont4();
    afx_msg void OnFontfacesSetfontstodefaullts();

    void KeyboardSelectionCut(bool no_copy = false);
    void KeyboardSelectionCopy(bool no_deselect = false);
    void KeyboardSelectionPaste();
    afx_msg void OnEditCopylatexcode();
    afx_msg void OnKeyboardF1setszoomlevelto100();
    afx_msg void OnViewZoomto1();
    afx_msg void OnOutputimagePrintfontsasimages();
    //afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnKeyboardAllowcommaasdecimalseparator();
    //afx_msg void OnSelectionsWidekeyboardcursor();
    afx_msg void OnGridandguidelinesSnaptoguidelines();
    afx_msg void OnZoomUsectrlforwheelzoom();

    int ScrollCursorIntoView();
    void SelectLastOrTouchedObject();
    afx_msg void OnNcRButtonDown(UINT nHitTest, CPoint point);

    afx_msg void OnMenu32880();

    afx_msg void OnToolboxandcontextmenuShowtoolbar();

    afx_msg void OnPageLetterportrait();

    afx_msg void OnPageLetterlandscape();

    afx_msg void OnKeyboardUsecapslocktotoggletypingmode();
    afx_msg void OnToolboxandcontextmenuGigantic();
    afx_msg void OnMouseReversemousewheelscrollingdirection();

    afx_msg void OnFontsizeanddefaultzoomDefaultzoomis150();

    afx_msg void OnFontsizeanddefaultzoomDefaultzoomis120();

    afx_msg void OnFontsizeanddefaultzoomDefaultzoomis100();

    afx_msg void OnFontsizeanddefaultzoomDefaultzoomis80();

    afx_msg void OnMouseMouse();

    afx_msg void OnMouseRightmousebuttontotogglemouse();

    afx_msg void OnMouseSlow();

    afx_msg void OnPagenumerationNone();

    afx_msg void OnPagenumeration();

    afx_msg void OnPagenumeration32899();

    afx_msg void OnPagenumerationPage1of10();

    afx_msg void OnPagenumerationBottom();

    afx_msg void OnPagenumerationRight();

    afx_msg void OnPagenumerationExcludefirstpage();

    afx_msg void OnToolboxandcontextmenuAuto();

    afx_msg void OnMenu32905();

    afx_msg void OnKeyboardUsecomplexindexes();
};

#ifndef _DEBUG  // debug version in EquationView.cpp
inline CMathomirDoc* CMathomirView::GetDocument() const
   { return reinterpret_cast<CMathomirDoc*>(m_pDocument); }
#endif
