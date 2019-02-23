#include "notesconfiguration.h"
#include "ui_notesconfiguration.h"

#include <QFontDialog>
#include <QColorDialog>

NotesConfiguration::NotesConfiguration(PluginSettings *settings, QWidget *parent) :
    LXQtPanelPluginConfigDialog(settings, parent),
    ui(new Ui::NotesConfiguration)
{
    ui->setupUi(this);
    
    connect(ui->dialogButtons, &QDialogButtonBox::clicked, this, &NotesConfiguration::dialogButtonsAction);
    
    loadSettings();
}

NotesConfiguration::~NotesConfiguration()
{
    delete ui;
}

void NotesConfiguration::on_changeFont_clicked(bool checked)
{
	bool ok;
	QFont font = QFontDialog::getFont(&ok, QFont(), this);
	
	if(ok) {
		settings().setValue("defaultFont", font);
		setFont(font);
	}
}

void NotesConfiguration::on_bgColorButton_clicked(bool checked)
{
	QColor color = QColorDialog::getColor(Qt::yellow, this, "Select Backgroung Color");
	settings().setValue("backgroundColor", color.name());
	setBgColor(color);
}

void NotesConfiguration::on_fgColorButton_clicked(bool checked)
{
	QColor color = QColorDialog::getColor(Qt::yellow, this, "Select Backgroung Color");
	settings().setValue("foregroundColor", color.name());
	setFgColor(color);
}

void NotesConfiguration::on_showWindowFrame_toggled(bool checked)
{
	settings().setValue("showWindowFrame", checked);
}

void NotesConfiguration::on_showNotesOnStartup_toggled(bool checked)
{
	settings().setValue("showNotesOnStartup", checked);
}

void NotesConfiguration::setFont(const QFont &font)
{
	ui->displayFont->setText(font.family());
	ui->displayFont->setFont(font);
}

void NotesConfiguration::setBgColor(const QColor &color)
{
	QPalette pal = ui->bgColorButton->palette();
	pal.setColor(QPalette::Button, color);
	ui->bgColorButton->setAutoFillBackground(true);
	ui->bgColorButton->setPalette(pal);
	ui->bgColorButton->update();
}

void NotesConfiguration::setFgColor(const QColor &color)
{
	QPalette pal = ui->fgColorButton->palette();
	pal.setColor(QPalette::Button, color);
	ui->fgColorButton->setAutoFillBackground(true);
	ui->fgColorButton->setPalette(pal);
	ui->fgColorButton->update();
}

void NotesConfiguration::loadSettings()
{
	QColor bgColorName = settings().value("backgroundColor", "#ffff7f").toString();
	QColor fgColorName = settings().value("foregroundColor", "#000000").toString();
	setBgColor(QColor(bgColorName));
	setFgColor(QColor(fgColorName));
	
	QFont font = qvariant_cast<QFont>(settings().value("defaultFont"));
	setFont(font);
	
	bool showFrame = settings().value("showWindowFrame", "false").toBool();
	bool showOnStartup = settings().value("showNotesOnStartup", "false").toBool();
	ui->showWindowFrame->setChecked(showFrame);
	ui->showNotesOnStartup->setChecked(showOnStartup);
}
