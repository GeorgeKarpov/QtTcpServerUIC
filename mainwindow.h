#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QtNetwork>

#include <QLabel>

#include <tcpserver.h>

#include <telegram.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:

public slots:
     void on_pushButton_clicked();
     void on_pushButton_send_clicked();
     void on_EChanged(QString E);
     void enableControls(bool enabled);

private:
    Ui::MainWindow *ui;
    TcpServer *server;
    QLabel *label;
    QLabel *statusText;
    QStringList telE;
    QStringList opt;
    telegram *Tel;
    void loadOptics();
};

#endif // MAINWINDOW_H
