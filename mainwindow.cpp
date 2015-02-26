#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QStandardPaths>
#include <QWebPage>
#include <QtCore>
#include <QtNetwork/QNetworkAccessManager>
#include <QWebSettings>
#include <QFontDatabase>
#include <QFileInfo>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    applyNativeFont();

    QSettings settings;
    QVariant value = settings.value("team_domain");

    QUrl url(loginUrl);

    if (value.isValid()){
        url = QUrl(teamLoginUrl.arg(value.toString()));
    }
    qDebug() << "Team login URL: " << url;

    QString path(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    qDebug() << "Cache path" << path;

    ui->webView->page()->settings()->enablePersistentStorage(path);
    ui->webView->settings()->setThirdPartyCookiePolicy(
                QWebSettings::ThirdPartyCookiePolicy::AlwaysAllowThirdPartyCookies);
    ui->webView->settings()->setAttribute(QWebSettings::NotificationsEnabled, true);

    jar = new CookieJar(ui->webView);
    ui->webView->page()->networkAccessManager()->setCookieJar(jar);
    ui->webView->setUrl(url);

    trayIcon = new QSystemTrayIcon(QIcon(QString("://images/png/icon32.png")), this);
    trayIcon->show();
    setWindowIcon(QIcon(QString("://images/png/icon32.png")));

    ui->webView->setContextMenuPolicy(Qt::ContextMenuPolicy::PreventContextMenu);
    connect(ui->webView->page(), SIGNAL(featurePermissionRequested(QWebFrame*,QWebPage::Feature)),
            this, SLOT(featureRequest(QWebFrame*,QWebPage::Feature)));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason )),
            this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));
    connect(ui->webView, SIGNAL(urlChanged(QUrl)), this, SLOT(onUrlChanged(QUrl)));


    button = new QWinTaskbarButton(this);
    button->setWindow(this->windowHandle());
    button->setOverlayIcon(QIcon("://images/png/Slack.png"));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete trayIcon;
    delete jar;
    delete button;
}

void MainWindow::onUrlChanged(QUrl url){
    qDebug() << url.host();

    if (url.host().endsWith(".slack.com", Qt::CaseSensitive)){
        ui->webView->page()->setFeaturePermission(ui->webView->page()->mainFrame(), QWebPage::Feature::Notifications,
                                                  QWebPage::PermissionPolicy::PermissionGrantedByUser);
    }
}

void MainWindow::featureRequest(QWebFrame *frame, QWebPage::Feature feature)
{
    qDebug() << frame->url();

    if (feature == QWebPage::Feature::Notifications)
    {
        int result = QMessageBox::question(this,
                              QString("Notification permission"),
                              QString("%1\nasks for notifications persmission. Should I allow?").arg(frame->url().toString()),
                              QMessageBox::StandardButton::Ok, QMessageBox::Cancel);

        if (result == QMessageBox::StandardButton::Ok)
        {
            ui->webView->page()->setFeaturePermission(frame, feature,
                                                  QWebPage::PermissionPolicy::PermissionGrantedByUser);
        }
    }
}

void MainWindow::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
    case QSystemTrayIcon::Trigger:
    {
        if(isHidden())
            show();
        else
            hide();
        break;
    }
    default: break;
    }
}

void MainWindow::applyNativeFont()
{
//    QFontDatabase::addApplicationFont(QString(":/fonts/Lato-Black.ttf"));
//    QFontDatabase::addApplicationFont(QString(":/fonts/Lato-BlackItalic.ttf"));
//    QFontDatabase::addApplicationFont(QString(":/fonts/Lato-Bold.ttf"));
//    QFontDatabase::addApplicationFont(QString(":/fonts/Lato-BoldItalic.ttf"));
//    QFontDatabase::addApplicationFont(QString(":/fonts/Lato-Hairline.ttf"));
//    QFontDatabase::addApplicationFont(QString(":/fonts/Lato-HairlineItalic.ttf"));
//    QFontDatabase::addApplicationFont(QString(":/fonts/Lato-Italic.ttf"));
//    QFontDatabase::addApplicationFont(QString(":/fonts/Lato-Light.ttf"));
//    QFontDatabase::addApplicationFont(QString(":/fonts/Lato-LightItalic.ttf"));
//    QFontDatabase::addApplicationFont(QString(":/fonts/Lato-Medium.ttf"));
//    QFontDatabase::addApplicationFont(QString(":/fonts/Lato-MediumItalic.ttf"));
//    QFontDatabase::addApplicationFont(QString(":/fonts/Lato-Regular.ttf"));
//    QFontDatabase::addApplicationFont(QString(":/fonts/Lato-Semibold.ttf"));
//    QFontDatabase::addApplicationFont(QString(":/fonts/Lato-SemiboldItalic.ttf"));
//    QFontDatabase::addApplicationFont(QString(":/fonts/Lato-Thin.ttf"));
//    QFontDatabase::addApplicationFont(QString(":/fonts/Lato-ThinItalic.ttf"));

    QWebSettings* settings = ui->webView->page()->settings();
    settings->setFontFamily(QWebSettings::StandardFont, "Segoe UI");
    settings->setFontSize(QWebSettings::DefaultFontSize, 16);
}

void MainWindow::showNotification(QString title, QString message)
{
    item = new AsemanNativeNotificationItem();
    item->setTitle(title);
    item->setBody(message);
    item->setIcon("://images/png/Slack.png");
    item->setTimeOut(100000);
    item->show();

    QApplication::alert(this);
}

void MainWindow::hideEvent(QHideEvent *event){
    QMainWindow::hideEvent(event);

}

void MainWindow::showEvent(QShowEvent *event){
    QMainWindow::showEvent(event);
}
