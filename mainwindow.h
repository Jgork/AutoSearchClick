#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <QTimer>
#include <vector>
#include <windows.h>

extern "C" void incrementClicks(int* counter);
extern int totalClicks;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;

    void on_selectImage_clicked();

    void on_Touch_clicked();

    void on_timerr_checkStateChanged(const Qt::CheckState &arg1);

    void on_Stop_clicked();

    void on_inOrder_checkStateChanged(const Qt::CheckState &arg1);

    void on_MinZ_valueChanged(int arg1);

    void on_MaxZ_valueChanged(int arg1);

    void on_lcdNumber_overflow();

    void on_reset_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *autoClickTimer = nullptr;
    QScreen *screen;
    std::vector<cv::Mat> targetImage;
    int currentTargetIndex;
    cv::Mat screenMat;
    int centerX, centerY;
    cv::Mat currentScreen;
    int totalClicks = 0;
};
#endif // MAINWINDOW_H
