#ifndef ROUTEDIALOG_H
#define ROUTEDIALOG_H

#include <QDialog>

namespace Ui
{
class RouteDialog;
}

class RouteDialog: public QDialog
    {
    Q_OBJECT

    public:
    explicit RouteDialog(QWidget* aParent);
    ~RouteDialog();
    void EnablePushPinEditing();

    Ui::RouteDialog* m_ui;

    private slots:
    void on_setStartOfRoute_clicked();
    void on_setEndOfRoute_clicked();
    void on_addPushPin_clicked();
    void on_cutPushPin_clicked();
    };

#endif // ROUTEDIALOG_H
