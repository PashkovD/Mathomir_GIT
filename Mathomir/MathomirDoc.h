// EquationDoc.h : interface of the CMathomirDoc class
//


#pragma once

class CMathomirDoc : public CDocument
{
protected: // create from serialization only
    CMathomirDoc();
    DECLARE_DYNCREATE(CMathomirDoc)

    // Attributes
    // Operations
    // Overrides
    BOOL OnNewDocument() override;
    //virtual void Serialize(CArchive& ar);

    // Implementation
    //virtual ~CMathomirDoc();
#ifdef _DEBUG
    void AssertValid() const override;
    void Dump(CDumpContext& dc) const override;
#endif

    // Generated message map functions
    DECLARE_MESSAGE_MAP()

public:
    BOOL OnSaveDocument(LPCTSTR lpszPathName) override;
    afx_msg void OnFileSave();
    afx_msg void OnFileOpen();
    afx_msg void OnFileNew();
    afx_msg void OnFileSaveAs();
    afx_msg BOOL SaveModified() override;
    BOOL OnOpenDocument(LPCTSTR lpszPathName) override;
    int OpenMOMFile(const char* filename);
    int SaveMOMFile(const char* filename, char filetype);
    int ScrambleMOMFile(char** bufer, int len, char type);
    int UnscrambleMOMFile(char** bufer, int len);
};
