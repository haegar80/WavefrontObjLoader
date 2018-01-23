#include "WavefrontObjLoader.h"

WavefrontObjLoader::WavefrontObjLoader()
{
    setupUi();
}

void WavefrontObjLoader::setupUi()
{
    if (this->objectName().isEmpty()) {
        this->setObjectName(QString::fromUtf8("MainWindow"));
    }
    this->resize(1024, 1024);
    m_centralwidget = new QWidget(this);
    m_centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
    m_centralwidget->setStyleSheet("background:rgb(50,50,50)");
    m_widget = new QWidget(m_centralwidget);
    m_widget->setObjectName(QString::fromUtf8("widget"));
    m_widget->setGeometry(QRect(146, 12, 1024, 1024));
    m_widget->setAutoFillBackground(true);

    m_quitButton = new QPushButton(m_centralwidget);
    m_quitButton->setObjectName(QString::fromUtf8("quitButton"));
    m_quitButton->setGeometry(QRect(20, 1000, 75, 23));
    this->setCentralWidget(m_centralwidget);
    m_menubar = new QMenuBar(this);
    m_menubar->setObjectName(QString::fromUtf8("menubar"));
    m_menubar->setGeometry(QRect(0, 0, 800, 21));
    this->setMenuBar(m_menubar);
    this->setWindowState(Qt::WindowFullScreen);

    retranslateUi();
    QObject::connect(m_quitButton, SIGNAL(pressed()), this, SLOT(close()));

    QMetaObject::connectSlotsByName(this);
}

void WavefrontObjLoader::retranslateUi()
{
    this->setWindowTitle("MainWindow");
    m_quitButton->setText("Quit");
}

QWidget* WavefrontObjLoader::getMapWidget()
{
    return m_widget;
}
