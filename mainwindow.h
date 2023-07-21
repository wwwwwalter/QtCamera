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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



private:

    //control elements
    QLabel *labelCameraDevices;
    QComboBox *comboBoxCameraDevices;
    QLabel *labelAudioInputDevices;
    QComboBox *comboBoxAudioInputDevices;
    QLabel *labelAudioOutputDevcies;
    QComboBox *comboBoxAudioOutputDevcies;



    //media elements
    QPushButton *startRecordButton;
    QPushButton *stopRecordButton;
    QPushButton *pauseRecordButton;
    QPushButton *captureButton;

    QStackedWidget *stackedWidget;
    QLabel *lastImage;
    //QVideoWidget *videoWidget;



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
    void setCamere_action(QAction *action);
    void on_start_record_clicked();
    void on_pause_record_clicked();
    void on_stop_record_clicked();
    void on_capture_image_clicked();


public:
    void UpdateRecorderState(QMediaRecorder::RecorderState state);
    void DisplayCaptureError(int id,const QImageCapture::Error error,const QString &errorString);
    void DisplayRecorderError();
    void UpdateRecorederTime();
    void ImageSaved(int id,const QString &filename);
    void DisplayCapturedImage();
    void DisplayViewfinder();
    void ProcessCapturedImage(int requestId,const QImage &img);
    void ReadyForCapture(bool ready);
    void SetCamera(const QCameraDevice &cameraDevice);

    //update devices
    void UpdateAudioInputDevices();
    void UpdateAudioOutputDevices();
    void UpdateVideoInputDevices();
    void UpdataCameraDevices();





};
#endif
