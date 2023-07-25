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
#include <QApplication>
#include <QStyleFactory>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    //Mainwindow
    //setWindowState(Qt::WindowMaximized);    
    setWindowIcon(QIcon(":/images/logo2.png"));


    //main widget
    QWidget *w = new QWidget;
    setCentralWidget(w);
    QHBoxLayout *hboxlayout_main= new QHBoxLayout;
    w->setLayout(hboxlayout_main);

    leftWidget = new QWidget;
    rightWidget = new QWidget;
    rightWidget->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
    hboxlayout_main->addWidget(leftWidget);
    hboxlayout_main->addWidget(rightWidget);

    QVBoxLayout *vboxlayout_media = new QVBoxLayout;
    QVBoxLayout *vboxlayout_setting = new QVBoxLayout;

    leftWidget->setLayout(vboxlayout_media);
    rightWidget->setLayout(vboxlayout_setting);



    groupBoxDevices = new QGroupBox(tr("media devices"));
    groupBoxCapture = new QGroupBox(tr("capture"));
    groupBoxRecorder = new QGroupBox(tr("recorder"));
    groupBoxCameraParameters = new QGroupBox(tr("camera parameters"));




    hboxlayout_main->addLayout(vboxlayout_media,3);
    hboxlayout_main->addLayout(vboxlayout_setting,1);
    vboxlayout_setting->addWidget(groupBoxDevices);
    vboxlayout_setting->addWidget(groupBoxCapture);
    vboxlayout_setting->addWidget(groupBoxRecorder);
    vboxlayout_setting->addWidget(groupBoxCameraParameters);
    vboxlayout_setting->addStretch(0);




    //group devices
    QGridLayout *gradlayout_devices = new QGridLayout;
    labelCameraDevices = new QLabel(tr("cameras devices"));
    comboBoxCameraDevices = new QComboBox;
    labelAudioInputDevices = new QLabel(tr("audio inputs"));
    comboBoxAudioInputDevices = new QComboBox;
    labelAudioOutputDevcies = new QLabel("audio outputs");
    comboBoxAudioOutputDevcies = new QComboBox;


    gradlayout_devices->addWidget(labelAudioInputDevices,0,0);
    gradlayout_devices->addWidget(comboBoxAudioInputDevices,0,1);
    gradlayout_devices->addWidget(labelAudioOutputDevcies,1,0);
    gradlayout_devices->addWidget(comboBoxAudioOutputDevcies,1,1);
    gradlayout_devices->addWidget(labelCameraDevices,2,0);
    gradlayout_devices->addWidget(comboBoxCameraDevices,2,1);
    groupBoxDevices->setLayout(gradlayout_devices);




    //connect(comboBoxAudioOutputDevcies,&QComboBox::currentTextChanged,this,&MainWindow::comboBoxAudioOutputDevciesChanged);

    //group capture
    QGridLayout *gradlayout_capture = new QGridLayout;
    labelCaptureSavePath = new QLabel("capture path");
    lineEditCaptureSavePath = new QLineEdit;
    buttonChooseCaptureSavePath = new QPushButton(tr("choose..."));
    captureButton = new QPushButton(tr("capture"));
    openCaptureFolderButton = new QPushButton(tr("directory"));




    gradlayout_capture->addWidget(labelCaptureSavePath,0,0);
    gradlayout_capture->addWidget(lineEditCaptureSavePath,0,1,1,3);
    gradlayout_capture->addWidget(buttonChooseCaptureSavePath,0,4);
    gradlayout_capture->addWidget(captureButton,1,0);
    gradlayout_capture->addWidget(openCaptureFolderButton,1,1);
    groupBoxCapture->setLayout(gradlayout_capture);


    //group recorder
    QGridLayout *gradlayout_recorder = new QGridLayout;
    labelRecorderSavePath = new QLabel("recorder path");
    lineEditRecorderSavePath = new QLineEdit;
    buttonChooseRecorderSavePath = new QPushButton(tr("choose..."));
    startRecordButton = new QPushButton(tr("start"));
    stopRecordButton = new QPushButton(tr("stop"));
    pauseRecordButton = new QPushButton(tr("pause"));
    openRecordFolderButton = new QPushButton(tr("directory"));


    gradlayout_recorder->addWidget(labelRecorderSavePath,0,0);
    gradlayout_recorder->addWidget(lineEditRecorderSavePath,0,1,1,3);
    gradlayout_recorder->addWidget(buttonChooseRecorderSavePath,0,4);
    gradlayout_recorder->addWidget(startRecordButton,1,0);
    gradlayout_recorder->addWidget(pauseRecordButton,1,1);
    gradlayout_recorder->addWidget(stopRecordButton,1,2);
    gradlayout_recorder->addWidget(openRecordFolderButton,1,3);
    groupBoxRecorder->setLayout(gradlayout_recorder);

    //group camera parameters
    QGridLayout *gradlayout_camereParameters = new QGridLayout;

    stylesLabel = new QLabel(tr("app theme"));
    stylesComboBox = new QComboBox;




    gradlayout_camereParameters->setColumnStretch(0,1);
    gradlayout_camereParameters->setColumnStretch(1,1);
    gradlayout_camereParameters->setColumnStretch(2,1);
    gradlayout_camereParameters->setColumnStretch(3,1);
    gradlayout_camereParameters->setColumnStretch(4,1);
    gradlayout_camereParameters->addWidget(stylesLabel,0,0);
    gradlayout_camereParameters->addWidget(stylesComboBox,0,1);
    groupBoxCameraParameters->setLayout(gradlayout_camereParameters);

    stylesComboBox->addItems(QStyleFactory::keys());
    connect(stylesComboBox,&QComboBox::currentTextChanged,this,&MainWindow::UpdateUiStyle);




    //media layout
    videoWidget = new QVideoWidget;
    lastImage = new QLabel;
    stackedWidget = new QStackedWidget;
    stackedWidget->insertWidget(0,videoWidget);
    stackedWidget->insertWidget(1,lastImage);
    stackedWidget->setMinimumWidth(400);
    vboxlayout_media->addWidget(stackedWidget);





    //devices
    mediaDevices = new QMediaDevices(this);
    connect(mediaDevices,&QMediaDevices::audioInputsChanged,this,&MainWindow::UpdateAudioInputDevices);
    connect(mediaDevices,&QMediaDevices::audioOutputsChanged,this,&MainWindow::UpdateAudioOutputDevices);
    connect(mediaDevices,&QMediaDevices::videoInputsChanged,this,&MainWindow::UpdateVideoInputDevices);


    //session
    session = new QMediaCaptureSession(this);

    //camera
    camera = new QCamera;//default cameraDevice
    session->setCamera(camera);
    connect(camera,&QCamera::errorOccurred,this,&MainWindow::CameraErrorOccurred);
    connect(camera,&QCamera::activeChanged,this,&MainWindow::CameraActiveChanged);
    connect(camera,&QCamera::cameraDeviceChanged,this,&MainWindow::CameraDeviceChanged);


    //output stream
    session->setVideoOutput(videoWidget);
    audioOutput = new QAudioOutput(QMediaDevices::defaultAudioOutput());
    session->setAudioOutput(audioOutput);
    imageCapture = new QImageCapture;
    session->setImageCapture(imageCapture);
    mediaRecored = new QMediaRecorder;
    session->setRecorder(mediaRecored);

    //input stream
    audioInput = new QAudioInput(mediaDevices->defaultAudioInput());
    session->setAudioInput(audioInput);

    UpdateAudioInputDevices();
    UpdateAudioOutputDevices();
    UpdateVideoInputDevices();








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



void MainWindow::on_start_record_clicked()
{
    mediaRecored->record();
    //UpdateRecorederTime();
    //qDebug()<<mediaRecored->actualLocation();
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
    //QApplication::beep();
}

void MainWindow::comboBoxAudioOutputDevciesChanged(const QString &audioOutputDevice)
{
    qDebug()<<audioOutputDevice;
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

//camera slots
void MainWindow::CameraErrorOccurred(QCamera::Error error, const QString &errorString)
{
    qDebug()<<"cameraError:"<<errorString;
}

void MainWindow::CameraDeviceChanged()
{
    qDebug()<<"cameraDevice changed:"<<camera->cameraDevice().description();
}

void MainWindow::CameraActiveChanged(bool value)
{
    qDebug()<<"CameraActiveChanged:"<<value;


}

//ui style slots
void MainWindow::UpdateUiStyle(){
    QApplication::setStyle(QStyleFactory::create(stylesComboBox->currentText()));
}


//mediaDevices slots
void MainWindow::UpdateAudioInputDevices()
{
    comboBoxAudioInputDevices->clear();
    QList<QAudioDevice> audioInputDevices = QMediaDevices::audioInputs();
    for(QAudioDevice &audioInputDevice:audioInputDevices){
        comboBoxAudioInputDevices->addItem(audioInputDevice.description(),QVariant::fromValue(audioInputDevice));
    }
    comboBoxAudioInputDevices->setCurrentText(QMediaDevices::defaultAudioInput().description());
}

void MainWindow::UpdateAudioOutputDevices()
{
    comboBoxAudioOutputDevcies->clear();
    QList<QAudioDevice> audioOutputDevices = QMediaDevices::audioOutputs();
    for(QAudioDevice &audioOutputDevice:audioOutputDevices){
        comboBoxAudioOutputDevcies->addItem(audioOutputDevice.description(),QVariant::fromValue(audioOutputDevice));
    }
    comboBoxAudioOutputDevcies->setCurrentText(QMediaDevices::defaultAudioOutput().description());
}

void MainWindow::UpdateVideoInputDevices()
{
    comboBoxCameraDevices->clear();
    QList<QCameraDevice> cameraDevices = QMediaDevices::videoInputs();
    for(QCameraDevice &cameraDevice:cameraDevices){
        comboBoxCameraDevices->addItem(cameraDevice.description(),QVariant::fromValue(cameraDevice));
    }
    comboBoxCameraDevices->setCurrentText(QMediaDevices::defaultVideoInput().description());


    camera->setCameraDevice(qvariant_cast<QCameraDevice>(comboBoxCameraDevices->currentData()));
    qDebug()<<"debug:"<<camera->cameraDevice().description();
    qDebug()<<camera->isAvailable();
    if(camera->isAvailable()){
        camera->start();
    }





}
