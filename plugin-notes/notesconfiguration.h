#ifndef NOTESCONFIGURATION_H
#define NOTESCONFIGURATION_H

#include <QDialog>

#include "../panel/lxqtpanelpluginconfigdialog.h"
#include "../panel/pluginsettings.h"

namespace Ui {
class NotesConfiguration;
}

class NotesConfiguration : public LXQtPanelPluginConfigDialog
{
    Q_OBJECT

public:
    explicit NotesConfiguration(PluginSettings *settings, QWidget *parent = nullptr);
    ~NotesConfiguration();
    
public slots:
	void on_changeFont_clicked(bool checked = true);
	void on_bgColorButton_clicked(bool checked = true);
	void on_fgColorButton_clicked(bool checked = true);
	void on_showWindowFrame_toggled(bool checked);
	void on_showNotesOnStartup_toggled(bool checked);

private slots:
    void loadSettings();

private:
    Ui::NotesConfiguration *ui;
    
    void setBgColor(const QColor &color);
    void setFgColor(const QColor &color);
    void setFont(const QFont &font);
};

#endif // NOTESCONFIGURATION_H
