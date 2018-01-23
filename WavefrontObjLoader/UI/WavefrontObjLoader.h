#pragma once

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QMenuBar>

class WavefrontObjLoader : public QMainWindow
{
    Q_OBJECT

public:
    WavefrontObjLoader();
    virtual ~WavefrontObjLoader() = default;

    void setupUi();
    void retranslateUi();
    QWidget* getMapWidget();

private:
    QWidget* m_centralwidget;
    QWidget* m_widget;
    QPushButton* m_quitButton;
    QMenuBar* m_menubar;
    QStatusBar* m_statusbar;
};
