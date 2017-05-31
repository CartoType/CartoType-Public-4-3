#if !defined(AFX_FINDDIALOG_H__BC253915_DC2E_4FB7_BC09_C574263FB0A3__INCLUDED_)
#define AFX_FINDDIALOG_H__BC253915_DC2E_4FB7_BC09_C574263FB0A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxwin.h>
#include "Resource.h"
#include <cartotype_framework.h>

class CFindTextDialog : public CDialog
    {
    // Construction
    public:
        CFindTextDialog(CartoType::CFramework& aFramework,const CartoType::MString& aText,bool aPrefix,bool aFuzzy,CWnd* pParent = 0);

        // Get the selected object if any and take ownership of it.
        std::unique_ptr<CartoType::CMapObject> SelectedObject()
            {
            if (iSelectedObjectIndex >= 0 && iSelectedObjectIndex < iObjectArray.size())
                return std::move(iObjectArray[iSelectedObjectIndex]);
            return nullptr;
            }

        // Dialog Data
        //{{AFX_DATA(CFindTextDialog)
        enum { IDD = IDD_FIND_TEXT_DIALOG };
        CString	iFindText;
        int iPrefix;
        int iFuzzy;
        //}}AFX_DATA

        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CFindTextDialog)
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        virtual BOOL OnInitDialog();
        //}}AFX_VIRTUAL

        // Implementation
    protected:
        afx_msg void OnEditChange();
        afx_msg void OnGeocode();
        afx_msg void OnComboBoxDoubleClick();
        DECLARE_MESSAGE_MAP()

    private:
        void PopulateComboBox();

        CartoType::CFramework& iFramework;
        CartoType::CMapObjectArray iObjectArray;
        int iSelectedObjectIndex;
        CartoType::TFindParam iFindParam;
    };

class CFindAddressDialog : public CDialog
    {
    // Construction
    public:
        CFindAddressDialog(CWnd* pParent = 0);   // standard constructor

        // Dialog Data
        //{{AFX_DATA(CFindAddressDialog)
        enum { IDD = IDD_FIND_ADDRESS_DIALOG };
        CString	iFindText;
        CString iBuilding;
        CString iFeature;
        CString iStreet;
        CString iSubLocality;
        CString iLocality;
        CString iSubAdminArea;
        CString iAdminArea;
        CString iCountry;
        CString iPostCode;
        //}}AFX_DATA


        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CFindAddressDialog)
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        //}}AFX_VIRTUAL

        // Implementation
    protected:

        // Generated message map functions
        //{{AFX_MSG(CFindAddressDialog)
        // NOTE: the ClassWizard will add member functions here
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDDIALOG_H__BC253915_DC2E_4FB7_BC09_C574263FB0A3__INCLUDED_)
