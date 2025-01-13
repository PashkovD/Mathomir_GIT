#pragma once
#include "afxwin.h"


// CPasswordDlg dialog

class CPasswordDlg : public CDialog
{
    DECLARE_DYNAMIC(CPasswordDlg)

    CPasswordDlg(CWnd* pParent = nullptr); // standard constructor
    ~CPasswordDlg() override;
    BOOL OnInitDialog() override;


    // Dialog Data
    enum { IDD = IDD_DIALOG_PASSWORD };

protected:
    void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support

    DECLARE_MESSAGE_MAP()

public:
    CEdit PasswordBox;
    afx_msg void OnBnClickedButton1();
    CEdit CommentBox;
    CEdit TimeLimitBox;
    afx_msg void OnBnClickedOk();
    CButton VisibilityButton;
    CButton OKButton;
};
