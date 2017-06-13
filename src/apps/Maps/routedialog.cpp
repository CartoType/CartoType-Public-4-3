#include "routedialog.h"
#include "ui_routedialog.h"

RouteDialog::RouteDialog(QWidget* aParent):
    QDialog(aParent),
    m_ui(new Ui::RouteDialog)
    {
    m_ui->setupUi(this);
    }

RouteDialog::~RouteDialog()
    {
    delete m_ui;
    }

void RouteDialog::on_setStartOfRoute_clicked()
    {
    EnablePushPinEditing();
    }

void RouteDialog::on_setEndOfRoute_clicked()
    {
    EnablePushPinEditing();
    }

void RouteDialog::on_addPushPin_clicked()
    {
    EnablePushPinEditing();
    }

void RouteDialog::on_cutPushPin_clicked()
    {
    EnablePushPinEditing();
    }

void RouteDialog::EnablePushPinEditing()
    {
    bool enable = m_ui->addPushPin->isChecked() || m_ui->cutPushPin->isChecked();
    m_ui->pushPinNameLabel->setEnabled(enable);
    m_ui->pushPinName->setEnabled(enable);
    m_ui->pushPinDescLabel->setEnabled(enable);
    m_ui->pushPinDesc->setEnabled(enable);
    m_ui->useAsWayPoint->setEnabled(enable);
    }
