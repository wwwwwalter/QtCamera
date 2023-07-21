#include "mainwindow.h"


#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMenuBar>
#include <QAction>
#include <QTimer>
#include <QStatusBar>
#include <QDir>
#include <QMessageBox>
#include <QComboBox>
#include <QAudioDevice>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowState(Qt::WindowMaximized);


    //main widget
    QWidget *w = new QWidget;
    setCentralWidget(w);

    QHBoxLayout *hboxlayout_main= new QHBoxLayout;
    QVBoxLayout *vboxlayout_media = new QVBoxLayout;
    QVBoxLayout *vboxlayout_control = new QVBoxLayout;
    QGridLayout *gradlayout_control = new QGridLayout;
    vboxlayout_control->addLayout(gradlayout_control);
    vboxlayout_control->addStretch(0);
    hboxlayout_main->addLayout(vboxlayout_media,3);
    hboxlayout_main->addLayout(vboxlayout_control,1);
    w->setLayout(hboxlayout_main);

    //control layout
    labelCameraDevices = new QLabel(tr("cameras devices:"));
    comboBoxCameraDevices = new QComboBox;
    labelAudioInputDevices = new QLabel(tr("audio inputs:"));
    comboBoxAudioInputDevices = new QComboBox;
    labelAudioOutputDevcies = new QLabel("audio outputs:");
    comboBoxAudioOutputDevcies = new QComboBox;

    gradlayout_control->addWidget(labelAudioInputDevices,0,0);
    gradlayout_control->addWidget(comboBoxAudioInputDevices,0,1);
    gradlayout_control->addWidget(labelAudioOutputDevcies,1,0);
    gradlayout_control->addWidget(comboBoxAudioOutputDevcies,1,1);
    gradlayout_control->addWidget(labelCameraDevices,2,0);
    gradlayout_control->addWidget(comboBoxCameraDevices,2,1);

    UpdateAudioInputDevices();
    UpdateAudioOutputDevices();
    UpdateVideoInputDevices();




    //media layout
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
    vboxlayout_media->addWidget(stackedWidget);
    vboxlayout_media->addLayout(hboxlayout);

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

void MainWindow::SetCamera(const QCameraDevice &cameraDevice)
{
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

void MainWindow::UpdateAudioInputDevices()
{
    QList<QAudioDevice> audioInputDevices = QMediaDevices::audioInputs();
    for(QAudioDevice &audioInputDevice:audioInputDevices){
        comboBoxAudioInputDevices->addItem(audioInputDevice.description(),QVariant::fromValue(audioInputDevice));
    }
}

void MainWindow::UpdateAudioOutputDevices()
{
    QList<QAudioDevice> audioOutputDevices = QMediaDevices::audioOutputs();
    for(QAudioDevice &audioOutputDevice:audioOutputDevices){
        comboBoxAudioOutputDevcies->addItem(audioOutputDevice.description(),QVariant::fromValue(audioOutputDevice));
    }
}

void MainWindow::UpdateVideoInputDevices()
{
    QList<QCameraDevice> cameraDevices = QMediaDevices::videoInputs();
    for(QCameraDevice &cameraDevice:cameraDevices){
        comboBoxCameraDevices->addItem(cameraDevice.description(),QVariant::fromValue(cameraDevice));
    }
}
