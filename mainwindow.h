#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAudioInput>
#include <QCamera>
#include <QLabel>
#include <QMainWindow>
#include <QMediaCaptureSession>
#include <QMediaDevices>
#include <QAudioDevice>
#include <QCameraDevice>
#include <QMediaRecorder>
#include <QPushButton>
#include <QStackedWidget>
#include <QVideoWidget>
#include <QImageCapture>
#include <QMediaPlayer>
#include <QActionGroup>
#include <QAudioInput>
#include <QAudioOutput>
#include <QImage>
#include <QComboBox>
#include <QGroupBox>
#include <QLineEdit>

#include <iostream>       // std::cout, std::endl
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
//std::this_thread::sleep_for(std::chrono::seconds(1));


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



private:

    //media elements
    QStackedWidget *stackedWidget;
    QLabel *lastImage;
    //QVideoWidget *videoWidget;



    //setting elements
    //group devices
    QLabel *labelCameraDevices;
    QComboBox *comboBoxCameraDevices;
    QLabel *labelAudioInputDevices;
    QComboBox *comboBoxAudioInputDevices;
    QLabel *labelAudioOutputDevcies;
    QComboBox *comboBoxAudioOutputDevcies;


    //group capture
    QLabel *labelCaptureSavePath;
    QLineEdit *lineEditCaptureSavePath;
    QPushButton *buttonChooseCaptureSavePath;
    QPushButton *captureButton;


    //group recorder
    QLabel *labelRecorderSavePath;
    QLineEdit *lineEditRecorderSavePath;
    QPushButton *buttonChooseRecorderSavePath;
    QPushButton *startRecordButton;
    QPushButton *stopRecordButton;
    QPushButton *pauseRecordButton;


    //group other settings
    QLabel *labelOtherSettings;



private:
    //all devcies
    QMediaDevices *mediaDevices;


    //input
    QCameraDevice *cameraDevice;     //相机
    QCamera *camera = nullptr;       //相机输入流
    QAudioDevice *audioInputDevice;  //麦克风
    QAudioInput *audioInput;         //音频输入流

    //session
    QMediaCaptureSession *session;   //控制中心

    //ouput
    QAudioDevice *audioOutputDevice; //扬声器
    QAudioOutput *audioOutput;       //音频输出流
    QVideoWidget *videoWidget;       //预览
    QImageCapture *imageCapture;     //拍照
    QMediaRecorder *mediaRecored;    //录像





private slots:






public slots:
    //mediaDeives slots
    void UpdateAudioInputDevices();
    void UpdateAudioOutputDevices();
    void UpdateVideoInputDevices();

    //camera slots
    void CameraErrorOccurred(QCamera::Error error, const QString &errorString);
    void CameraDeviceChanged();
    void CameraActiveChanged(bool value);

    //control button slots
    void on_start_record_clicked();
    void on_pause_record_clicked();
    void on_stop_record_clicked();
    void on_capture_image_clicked();

    //
    void UpdateRecorderState(QMediaRecorder::RecorderState state);
    void DisplayCaptureError(int id,const QImageCapture::Error error,const QString &errorString);
    void DisplayRecorderError();
    void UpdateRecorederTime();
    void ImageSaved(int id,const QString &filename);
    void DisplayCapturedImage();
    void DisplayViewfinder();
    void ProcessCapturedImage(int requestId,const QImage &img);
    void ReadyForCapture(bool ready);

    //comboBox slots
    void comboBoxAudioOutputDevciesChanged(const QString &audioOutputDevice);










};
#endif
