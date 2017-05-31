// FindDialog.cpp : implementation file
//

#include "stdafx.h"
#include "CartoTypeDemo.h"
#include "FindDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CFindTextDialog::CFindTextDialog(CartoType::CFramework& aFramework,const CartoType::MString& aText,bool aPrefix,bool aFuzzy,CWnd* pParent /*=NULL*/) :
CDialog(CFindTextDialog::IDD,pParent),
iPrefix(aPrefix),
iFuzzy(aFuzzy),
iFramework(aFramework),
iSelectedObjectIndex(-1)
    {
    SetString(iFindText,aText);
    iFindParam.iMaxObjectCount = 32;
    iFindParam.iAttributes = "$,name:*,ref,alt_name,int_name,addr:housename,addr::housenumber";
    iFindParam.iStringMatchMethod = CartoType::TStringMatchMethod(CartoType::EStringMatchPrefixFlag |
                                                                  CartoType::EStringMatchFoldAccentsFlag |
                                                                  CartoType::EStringMatchFoldCaseFlag |
                                                                  CartoType::EStringMatchIgnoreNonAlphanumericsFlag |
                                                                  CartoType::EStringMatchFastFlag);
    }

void CFindTextDialog::DoDataExchange(CDataExchange* pDX)
    {
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CFindTextDialog)
    DDX_Text(pDX,IDC_FIND_TEXT,iFindText);
    DDX_Check(pDX,IDC_FIND_PREFIX,iPrefix);
    DDX_Check(pDX,IDC_FIND_FUZZY,iFuzzy);
    //}}AFX_DATA_MAP
    }

BEGIN_MESSAGE_MAP(CFindTextDialog,CDialog)
    ON_CBN_EDITCHANGE(IDC_FIND_TEXT,OnEditChange)
    ON_CBN_DBLCLK(IDC_FIND_TEXT,OnComboBoxDoubleClick)
    ON_BN_CLICKED(IDC_GEOCODE,OnGeocode)
END_MESSAGE_MAP()

BOOL CFindTextDialog::OnInitDialog()
    {
    CComboBox* cb = (CComboBox*)GetDlgItem(IDC_FIND_TEXT);
    cb->SetHorizontalExtent(400);
    UpdateData(0);
    PopulateComboBox();
    return true;
    }

void CFindTextDialog::OnEditChange()
    {
    PopulateComboBox();
    }

void CFindTextDialog::OnComboBoxDoubleClick()
    {
    CComboBox* cb = (CComboBox*)GetDlgItem(IDC_FIND_TEXT);
    iSelectedObjectIndex = cb->GetCurSel();
    if (iSelectedObjectIndex >= 0 && iSelectedObjectIndex < iObjectArray.size())
        {
        CString text;
        cb->GetLBText(iSelectedObjectIndex,iFindText);
        EndDialog(IDOK);
        }
    }

void CFindTextDialog::OnGeocode()
    {
    // Convert the combo box entries to addresses.
    CComboBox* cb = (CComboBox*)GetDlgItem(IDC_FIND_TEXT);
    for (int i = cb->GetCount(); i >= 0; i--)
        cb->DeleteString(i);
    CString text;
    for (const auto& cur_object : iObjectArray)
        {
        CartoType::CString summary;
        CartoType::TResult error = iFramework.GeoCodeSummary(summary,*cur_object);
        if (!error)
            {
            SetString(text,summary);
            cb->AddString(text);
            }
        }
    }

void CFindTextDialog::PopulateComboBox()
    {
    // Get the current text.
    CComboBox* cb = (CComboBox*)GetDlgItem(IDC_FIND_TEXT);
    CString w_text;
    cb->GetWindowText(w_text);
    if (w_text.IsEmpty())
        return;

    CartoType::CString text;
    SetString(text,w_text);

    // Find up to 32 items starting with the current text.
    iObjectArray.clear();
    iFindParam.iText = text;
    iFramework.Find(iObjectArray,iFindParam);

    // Put them in the combo box.
    for (int i = cb->GetCount(); i >= 0; i--)
        cb->DeleteString(i);

    for (const auto& cur_object : iObjectArray)
        {
        CartoType::CMapObject::CMatch match;
        CartoType::TResult error = cur_object->GetMatch(match,text,iFindParam.iStringMatchMethod,&iFindParam.iAttributes);
        if (!error)
            {
            SetString(w_text,match.iValue);
            cb->AddString(w_text);
            }
        }
    }

CFindAddressDialog::CFindAddressDialog(CWnd* pParent /*=NULL*/)
    : CDialog(CFindAddressDialog::IDD,pParent)
    {
    //{{AFX_DATA_INIT(CFindAddressDialog)
    iBuilding = _T("");
    iFeature = _T("");
    iStreet = _T("");
    iSubLocality = _T("");
    iLocality = _T("");
    iSubAdminArea = _T("");
    iAdminArea = _T("");
    iCountry = _T("");
    iPostCode = _T("");
    //}}AFX_DATA_INIT
    }

void CFindAddressDialog::DoDataExchange(CDataExchange* pDX)
    {
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CFindAddressDialog)
    DDX_Text(pDX,IDC_FIND_BUILDING,iBuilding);
    DDX_Text(pDX,IDC_FIND_FEATURE,iFeature);
    DDX_Text(pDX,IDC_FIND_STREET,iStreet);
    DDX_Text(pDX,IDC_FIND_SUBLOCALITY,iSubLocality);
    DDX_Text(pDX,IDC_FIND_LOCALITY,iLocality);
    DDX_Text(pDX,IDC_FIND_SUBADMINAREA,iSubAdminArea);
    DDX_Text(pDX,IDC_FIND_ADMINAREA,iAdminArea);
    DDX_Text(pDX,IDC_FIND_COUNTRY,iCountry);
    DDX_Text(pDX,IDC_FIND_POSTCODE,iPostCode);
    //}}AFX_DATA_MAP
    }

BEGIN_MESSAGE_MAP(CFindAddressDialog,CDialog)
    //{{AFX_MSG_MAP(CFindAddressDialog)
    // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
