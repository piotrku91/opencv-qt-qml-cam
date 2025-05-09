#include "myimageprocessor.h"
#include "imageprovider.h"

MyImageProcessor::MyImageProcessor(QObject *parent)
    : QObject(parent) {
    connect(&m_timer, &QTimer::timeout, this, &MyImageProcessor::captureFrame);
}

void MyImageProcessor::setImageProvider(ImageProvider *provider) {
    m_provider = provider;
}

void MyImageProcessor::startCamera() {
    if (!m_cap.isOpened()) {
        m_cap.open(0); // 0 = domyślna kamera
        if (!m_cap.isOpened()) {
            qWarning("Nie udało się otworzyć kamery.");
            return;
        }
    }
    m_timer.start(33); // około 30 FPS
}

void MyImageProcessor::stopCamera() {
    m_timer.stop();
    if (m_cap.isOpened()) {
        m_cap.release();
    }
}

void MyImageProcessor::captureFrame() {
    if (!m_cap.isOpened()) return;

    cv::Mat frame;
    m_cap >> frame;
    if (frame.empty()) {
        return;
    }

    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
    QImage image = cvMatToQImage(frame);

    if (m_provider) {
        m_provider->setImage(image);
        emit imageUpdated();
    }
}

QImage MyImageProcessor::cvMatToQImage(const cv::Mat &mat) {
    return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888).copy();
}
