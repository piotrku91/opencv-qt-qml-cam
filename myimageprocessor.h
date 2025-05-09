#pragma once
#include <QObject>
#include <QTimer>
#include <QImage>
#include <opencv2/opencv.hpp>

class ImageProvider;

class MyImageProcessor : public QObject {
    Q_OBJECT
public:
    explicit MyImageProcessor(QObject *parent = nullptr);
    void setImageProvider(ImageProvider* provider);
    Q_INVOKABLE void startCamera();
    Q_INVOKABLE void stopCamera();

private:
    void captureFrame();
    QImage cvMatToQImage(const cv::Mat &mat);

    QTimer m_timer;
    cv::VideoCapture m_cap;
    ImageProvider* m_provider = nullptr;

signals:
    void imageUpdated();
};
