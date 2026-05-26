#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <opencv2/opencv.hpp>
#include <QFileDialog>
#include <QScreen>
#include <QGuiApplication>
#include <Windows.h>

#define HOTKEY_START_ID 101
#define HOTKEY_STOP_ID 102



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QGuiApplication::primaryScreen();
    if(!screen) qDebug()<<"Экран не найден!";

    autoClickTimer = new QTimer(this);
    autoClickTimer->setSingleShot(true);
    connect(autoClickTimer, &QTimer::timeout, this, &MainWindow::on_Touch_clicked);

    if (RegisterHotKey((HWND)winId(), HOTKEY_START_ID, 0, VK_F7)) {
        qDebug() << "Hotkey F7 (Start) зарегистрирован";
    }
    if (RegisterHotKey((HWND)winId(), HOTKEY_STOP_ID, 0, VK_F6)) {
        qDebug() << "Hotkey F6 (Stop) зарегистрирован";
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, qintptr *result) {
    MSG *msg = static_cast<MSG*>(message);
    if (msg->message == WM_HOTKEY) {
        if (msg->wParam == HOTKEY_START_ID) {
            qDebug() << "Нажата F7 -> СТАРТ";
            on_Touch_clicked(); // Вызываем твою функцию старта
            return true;
        }
        if (msg->wParam == HOTKEY_STOP_ID) {
            qDebug() << "Нажата F6 -> СТОП";
            on_Stop_clicked(); // Вызываем функцию стопа
            return true;
        }
    }
    return QMainWindow::nativeEvent(eventType, message, result);
}

void MainWindow::on_selectImage_clicked()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Select image dir"), "", QFileDialog::DontResolveSymlinks);
    //targetImage = cv::imread(fileName.toStdString(), cv::IMREAD_COLOR);
    QDir directory(dirPath);
    QStringList allFiles = directory.entryList(QDir::Files | QDir::NoDotAndDotDot);

    targetImage.clear();
    currentTargetIndex = 0; // Сброс для режима "по порядку"

    for (const QString &fileName : allFiles) {
        // Склеиваем путь к папке и имя файла
        QString fullPath = directory.absoluteFilePath(fileName);

        // Пытаемся прочитать. Если не картинка — OpenCV вернет empty
        cv::Mat img = cv::imread(fullPath.toStdString(), cv::IMREAD_COLOR);

        if (!img.empty()) {
            targetImage.push_back(img);
        }
    }
}

void MainWindow::on_timerr_checkStateChanged(const Qt::CheckState &arg1) {  }
void MainWindow::on_inOrder_checkStateChanged(const Qt::CheckState &arg1) {  }
void MainWindow::on_MinZ_valueChanged(int arg1) {  }
void MainWindow::on_MaxZ_valueChanged(int arg1) {  }

void MainWindow::on_Touch_clicked()
{
    ui->Touch->setEnabled(false);

    qDebug() << "--- Шаг ---";
    if (!screen) screen = QGuiApplication::primaryScreen();
    QPixmap localPx = screen->grabWindow(0);
    QImage localImg = localPx.toImage().convertToFormat(QImage::Format_RGB888);
    cv::Mat currentScreen = cv::Mat(localImg.height(), localImg.width(), CV_8UC3, (uchar*)localImg.bits(), localImg.bytesPerLine()).clone();
    cv::cvtColor(currentScreen, currentScreen, cv::COLOR_RGB2BGR);

    // Проверка: есть ли вообще что искать
    if(targetImage.empty()) {
        qDebug() << "Список целей пуст!";
        ui->timerr->setChecked(false);
        return;
    }

    bool foundAny = false;

    // --- ЛОГИКА ПОИСКА ---

    if (ui->inOrder->isChecked()) {
        // РЕЖИМ: ПО ПОРЯДКУ
        if (currentTargetIndex >= targetImage.size()) currentTargetIndex = 0;

        cv::Mat& currentTarget = targetImage[currentTargetIndex];
        cv::Mat resultMatrix;
        cv::matchTemplate(currentScreen, currentTarget, resultMatrix, cv::TM_CCOEFF_NORMED);

        double mVal; cv::Point mLoc;
        cv::minMaxLoc(resultMatrix, nullptr, &mVal, nullptr, &mLoc);

        if (mVal > 0.85) {
            foundAny = true;
            qDebug() << "Нашел по порядку! Точность:" << mVal;

            // Клик с рандомом и паузами
            int cX = mLoc.x + currentTarget.cols / 2 + (std::rand() % 7 - 3);
            int cY = mLoc.y + currentTarget.rows / 2 + (std::rand() % 7 - 3);
            SetCursorPos(cX, cY);
            Sleep(30);
            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
            Sleep(15);
            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
            incrementClicks(&totalClicks);
            ui->lcdNumber->display(totalClicks);
            qDebug() << "Кликов зафиксировано:" << totalClicks;


            // Решаем, что делать с картинкой дальше
            if (!ui->timerr->isChecked()) {
                targetImage.erase(targetImage.begin() + currentTargetIndex);
                // Индекс не трогаем, следующий элемент сам встанет на это место
            } else {
                currentTargetIndex++; // Переходим к следующему файлу в папке
            }
        }
    } else {
        // РЕЖИМ: ЧТО ПОПАДЕТСЯ
        for (auto it = targetImage.begin(); it != targetImage.end(); ++it) {
            cv::Mat resultMatrix;
            cv::matchTemplate(currentScreen, *it, resultMatrix, cv::TM_CCOEFF_NORMED);

            double mVal; cv::Point mLoc;
            cv::minMaxLoc(resultMatrix, nullptr, &mVal, nullptr, &mLoc);

            if (mVal > 0.85) {
                foundAny = true;
                qDebug() << "Нашел что попалось! Точность:" << mVal;

                int cX = mLoc.x + it->cols / 2 + (std::rand() % 7 - 3);
                int cY = mLoc.y + it->rows / 2 + (std::rand() % 7 - 3);
                SetCursorPos(cX, cY);
                Sleep(30);
                mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                Sleep(15);
                mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                incrementClicks(&totalClicks);
                ui->lcdNumber->display(totalClicks);
                qDebug() << "Кликов зафиксировано:" << totalClicks;

                if (!ui->timerr->isChecked()) {
                    targetImage.erase(it);
                }
                break; // Выход из цикла после первого найденного объекта
            }
        }
    }

    // --- ШАГ 8: ПЕРЕЗАПУСК ТАЙМЕРА ---
    if (ui->timerr->isChecked() && !targetImage.empty()) {
        // Если ничего не нашли, можно сделать задержку поменьше, чтобы быстрее искать снова
        int min = ui->MinZ->value(); int max = ui->MaxZ->value(); int delay;
        if(min>=max) delay = min;
            else delay = foundAny ? (min + std::rand() % max) : 300;
        autoClickTimer->start(delay);
        qDebug() << "Следующая итерация через" << delay << "мс";
    } else {
        ui->timerr->setChecked(false);
        qDebug() << "Работа завершена.";
    }
}

void MainWindow::on_Stop_clicked()
{
    ui->Touch->setEnabled(true);
    autoClickTimer->stop();
}

void MainWindow::on_lcdNumber_overflow()
{
}

void MainWindow::on_reset_clicked()
{
    totalClicks = 0;
    ui->lcdNumber->display(totalClicks);
}

