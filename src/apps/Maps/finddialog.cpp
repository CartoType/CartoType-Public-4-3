#include "finddialog.h"
#include "ui_finddialog.h"
#include <QKeyEvent>

FindDialog::FindDialog(QWidget* aParent,CartoType::CFramework& aFramework):
    QDialog(aParent),
    m_ui(new Ui::FindDialog),
    m_framework(aFramework),
    m_lock(0)
    {
    m_ui->setupUi(this);
    m_ui->findText->setFocus();
    m_find_param.iMaxObjectCount = 32;
    m_find_param.iAttributes = "$,name:*,ref,alt_name,int_name,addr:housename";
    m_find_param.iCondition = "OsmType!='bsp'"; // exclude bus stops
    m_find_param.iStringMatchMethod = CartoType::TStringMatchMethod(CartoType::EStringMatchPrefixFlag |
                                                                    CartoType::EStringMatchFoldAccentsFlag |
                                                                    CartoType::EStringMatchFoldCaseFlag |
                                                                    CartoType::EStringMatchIgnoreNonAlphanumericsFlag |
                                                                    CartoType::EStringMatchFastFlag);

    // Install an event filter to intercept up and down arrow events and use them to move between the line editor and the list box.
    m_ui->findText->installEventFilter(this);
    m_ui->findList->installEventFilter(this);
    }

FindDialog::~FindDialog()
    {
    delete m_ui;
    }

CartoType::CMapObjectArray FindDialog::Find(bool aAllAttributes)
    {
    CartoType::TFindParam find_param(m_find_param);
    find_param.iStringMatchMethod = CartoType::TStringMatchMethod(find_param.iStringMatchMethod & ~CartoType::EStringMatchFastFlag);
    find_param.iText = m_ui->findText->text().utf16();
    if (aAllAttributes)
        m_find_param.iAttributes.Clear();

    if (m_ui->prefix->isChecked())
        find_param.iStringMatchMethod = CartoType::TStringMatchMethod(find_param.iStringMatchMethod | CartoType::EStringMatchPrefixFlag);
    if (m_ui->fuzzyMatch->isChecked())
        find_param.iStringMatchMethod = CartoType::TStringMatchMethod(find_param.iStringMatchMethod | CartoType::EStringMatchFuzzyFlag);

    CartoType::CMapObjectArray object_array;
    m_framework.Find(object_array,find_param);

    return object_array;
    }

void FindDialog::on_findText_textChanged(const QString& aText)
    {
    if (m_lock)
        return;
    m_lock++;
    PopulateList(aText);
    m_lock--;
    }

bool FindDialog::eventFilter(QObject* aWatched,QEvent* aEvent)
    {
    if ((aWatched == m_ui->findText || aWatched == m_ui->findList) &&
        aEvent->type() == QEvent::KeyPress)
        {
        auto key_event = static_cast<const QKeyEvent*>(aEvent);
        if (aWatched == m_ui->findText)
            {
            if (key_event->key() == Qt::Key_Down && m_ui->findList->count() > 0)
                {
                m_ui->findList->setFocus();
                return true;
                }
            }
        else
            {
            if (key_event->key() == Qt::Key_Up)
                {
                QModelIndexList index_list { m_ui->findList->selectionModel()->selectedIndexes() };
                if (!index_list.size() || index_list.cbegin()->row() == 0)
                    {
                    m_ui->findText->setFocus();
                    return true;
                    }
                }
            }
        }
    return false;
    }

void FindDialog::PopulateList(const QString& aText)
    {
    CartoType::CString text;
    text.Set(aText.utf16());

    // Find up to 32 items starting with the current text.
    CartoType::CMapObjectArray object_array;
    m_find_param.iText = text;
    m_framework.Find(object_array,m_find_param);

    // Put them in the list.
    m_ui->findList->clear();

    for (const auto& cur_object : object_array)
        {
        CartoType::CMapObject::CMatch match;
        CartoType::TResult error = cur_object->GetMatch(match,text,m_find_param.iStringMatchMethod,&m_find_param.iAttributes);
        if (!error)
            {
            QString qs;
            qs.setUtf16(match.iValue.Text(),match.iValue.Length());
            m_ui->findList->addItem(qs);
            }
        }
    }

void FindDialog::on_findList_currentTextChanged(const QString& aCurrentText)
    {
    if (m_lock)
        return;
    m_lock++;
    m_ui->findText->setText(aCurrentText);
    m_lock--;
    }
