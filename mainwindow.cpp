#include "mainwindow.h"


#include <QVBoxLayout>
#include <QMenuBar>
#include <QAction>
#include <QTimer>
#include <QStatusBar>
#include <QDir>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(800,450);
    menu = menuBar()->addMenu(tr("Devices"));
    videoDevicesGroup = new QActionGroup(this);
    videoDevicesGroup->setExclusive(true);
    UpdateCameras();



    QWidget *w = new QWidget;
    setCentralWidget(w);

    QVBoxLayout *vboxlayout = new QVBoxLayout;
    w->setLayout(vboxlayout);


    videoWidget = new QVideoWidget;
    lastImage = new QLabel;
    stackedWidget = new QStackedWidget;
    stackedWidget->insertWidget(0,videoWidget);
    stackedWidget->insertWidget(1,lastImage);

    startRecordButton = new QPushButton(tr("start"));
    stopRecordButton = new QPushButton(tr("stop"));
    pauseRecordButton = new QPushButton(tr("pause"));
    captureButton = new QPushButton(tr("capture"));
    QHBoxLayout *hboxlayout = new QHBoxLayout;
    hboxlayout->addStretch();
    hboxlayout->addWidget(startRecordButton);
    hboxlayout->addWidget(stopRecordButton);
    hboxlayout->addWidget(pauseRecordButton);
    hboxlayout->addWidget(captureButton);
    hboxlayout->addStretch();
    vboxlayout->addWidget(stackedWidget);
    vboxlayout->addLayout(hboxlayout);

    //output
    session = new QMediaCaptureSession(this);
    session->setVideoOutput(videoWidget);
    audioOutput = new QAudioOutput(QMediaDevices::defaultAudioOutput());
    session->setAudioOutput(audioOutput);
    imageCapture = new QImageCapture;
    session->setImageCapture(imageCapture);
    mediaRecored = new QMediaRecorder;
    session->setRecorder(mediaRecored);

    //input
    mediaDevices = new QMediaDevices(this);
    audioInput = new QAudioInput(mediaDevices->defaultAudioInput());
    session->setAudioInput(audioInput);
    SetCamera(QMediaDevices::defaultVideoInput());





    //camera
    connect(mediaDevices,&QMediaDevices::videoInputsChanged,this,&MainWindow::UpdateCameras);
    connect(videoDevicesGroup,&QActionGroup::triggered,this,&MainWindow::setCamere_action);

    //capture
    ReadyForCapture(imageCapture->isReadyForCapture());
    connect(imageCapture,&QImageCapture::readyForCaptureChanged,this,&MainWindow::ReadyForCapture);
    connect(imageCapture,&QImageCapture::imageCaptured,this,&MainWindow::ProcessCapturedImage);
    connect(imageCapture,&QImageCapture::imageSaved,this,&MainWindow::ImageSaved);
    connect(imageCapture,&QImageCapture::errorOccurred,this,&MainWindow::DisplayCaptureError);

    //recorder
    connect(mediaRecored,&QMediaRecorder::recorderStateChanged,this,&MainWindow::UpdateRecorderState);
    connect(mediaRecored,&QMediaRecorder::durationChanged,this,&MainWindow::UpdateRecorederTime);
    connect(mediaRecored,&QMediaRecorder::errorChanged,this,&MainWindow::DisplayRecorderError);

    //button
    connect(startRecordButton,&QPushButton::clicked,this,&MainWindow::on_start_record_clicked);
    connect(pauseRecordButton,&QPushButton::clicked,this,&MainWindow::on_pause_record_clicked);
    connect(stopRecordButton,&QPushButton::clicked,this,&MainWindow::on_stop_record_clicked);
    connect(captureButton,&QPushButton::clicked,this,&MainWindow::on_capture_image_clicked);

}

MainWindow::~MainWindow()
{
}

void MainWindow::setCamere_action(QAction *action)
{
    SetCamera(qvariant_cast<QCameraDevice>(action->data()));
}

void MainWindow::on_start_record_clicked()
{
    mediaRecored->record();
    //UpdateRecorederTime();
}

void MainWindow::on_pause_record_clicked()
{
    mediaRecored->pause();
}

void MainWindow::on_stop_record_clicked()
{
    mediaRecored->stop();
}

void MainWindow::on_capture_image_clicked()
{
    imageCapture->captureToFile();
}

void MainWindow::UpdateRecorderState(QMediaRecorder::RecorderState state)
{
    switch (state) {
    case QMediaRecorder::StoppedState:
        startRecordButton->setEnabled(true);
        pauseRecordButton->setEnabled(false);
        stopRecordButton->setEnabled(false);
        break;
    case QMediaRecorder::PausedState:
        startRecordButton->setEnabled(true);
        pauseRecordButton->setEnabled(false);
        stopRecordButton->setEnabled(true);
        break;
    case QMediaRecorder::RecordingState:
        startRecordButton->setEnabled(false);
        pauseRecordButton->setEnabled(true);
        stopRecordButton->setEnabled(true);
        break;
    default:
        break;
    }
}

void MainWindow::DisplayCaptureError(int id, const QImageCapture::Error error, const QString &errorString)
{
    Q_UNUSED(id);
    Q_UNUSED(error);
    QMessageBox::warning(this,tr("Image Capture Error"),errorString);
}

void MainWindow::DisplayRecorderError()
{
    if(mediaRecored->error()!=QMediaRecorder::NoError){
        QMessageBox::warning(this,tr("Recording Error"),mediaRecored->errorString());
    }
}

void MainWindow::UpdateRecorederTime()
{
    QString str = QString("Recorded %1 sec").arg(mediaRecored->duration()/1000);
    statusBar()->showMessage(str);
}

void MainWindow::ImageSaved(int id, const QString &filename)
{
    Q_UNUSED(id);
    statusBar()->showMessage(tr("Captured \"%1\"").arg(QDir::toNativeSeparators(filename)));
}

void MainWindow::DisplayCapturedImage()
{
    stackedWidget->setCurrentWidget(lastImage);
}

void MainWindow::DisplayViewfinder()
{
    stackedWidget->setCurrentWidget(videoWidget);
}

void MainWindow::ProcessCapturedImage(int requestId, const QImage &img)
{
    Q_UNUSED(requestId);
    QImage scaledImage = img.scaled(videoWidget->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
    lastImage->setPixmap(QPixmap::fromImage(scaledImage));
    lastImage->setAlignment(Qt::AlignCenter);
    DisplayCapturedImage();
    QTimer::singleShot(1500,this,&MainWindow::DisplayViewfinder);
}

void MainWindow::ReadyForCapture(bool ready)
{
    captureButton->setEnabled(ready);
}

void MainWindow::UpdateCameras()
{
    menu->clear();
    QList<QCameraDevice> availableCameras = QMediaDevices::videoInputs();
    for(QCameraDevice &cameraDevice:availableCameras){
        QAction *action = new QAction(cameraDevice.description(),videoDevicesGroup);
        action->setCheckable(true);
        action->setData(QVariant::fromValue(cameraDevice));
        if(cameraDevice == QMediaDevices::defaultVideoInput()){
            action->setChecked(true);
        }
        menu->addAction(action);
    }
}

void MainWindow::SetCamera(const QCameraDevice &cameraDevice)
{
    qDebug()<<cameraDevice;

    if(cameraDevice.isNull()){
        qDebug()<<"cameraDevice is Null";
        return;
    }
    if(camera != nullptr){
        delete camera;
        camera = nullptr;
    }
    camera = new QCamera(cameraDevice);
    session->setCamera(camera);
    camera->start();


}
