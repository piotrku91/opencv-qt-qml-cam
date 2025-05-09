#include "cv_processor.h"
#include "imageprovider.h"

CvProcessor::CvProcessor(size_t fps, size_t camera_id, QObject *parent)
    : QObject(parent), m_fps(fps), m_camera_id(camera_id), m_current_roi(m_regions_of_interests.end()) {
    connect(&m_timer, &QTimer::timeout, this, &CvProcessor::captureFrame);
}

void CvProcessor::setImageProvider(ImageProvider *provider) {
    m_provider = provider;
}

void CvProcessor::setDisplayMode(ImageDisplayMode mode)
{
    m_display_mode = mode;
}

void CvProcessor::addROI(std::pair<QString, cv::Rect> new_roi)
{
    m_regions_of_interests.insert(new_roi);
}

void CvProcessor::setCurrentROI(QString name)
{
    auto item = m_regions_of_interests.find(name);
    if (item != m_regions_of_interests.end()) {
        m_current_roi = item;
    }
}

void CvProcessor::clearROIs()
{
    m_regions_of_interests.clear();
}

void CvProcessor::addDetection(std::pair<QString, ImageDetectionData> new_detection)
{
    m_detections.insert(new_detection);
}

std::optional<size_t> CvProcessor::getDetectionCount(QString detection_name)
{
    auto item = m_detections.find(detection_name);
    if (item != m_detections.end()) {
        return item->second.detection_count_output;
    } else {
        return std::nullopt;
    }
}

void CvProcessor::resetDetectionCount(QString detection_name)
{
    auto item = m_detections.find(detection_name);
    if (item != m_detections.end()) {
        item->second.detection_count_output = 0;
    }
}

void CvProcessor::clearDetections()
{
    m_detections.clear();
}

void CvProcessor::detectionProcess(cv::Mat image)
{
    for (auto& detection : m_detections) {
        cv::Point2f led_center;
        float led_radius;

        m_detector.setColorRange(detection.second.hsv_ranges);
        m_detector.setThreshold(detection.second.threshold);
        if (m_detector.detect(image, led_center, led_radius)) {
            cv::circle(image, led_center, static_cast<int>(led_radius), cv::Scalar(0, 255, 0), 2);
            cv::putText(image, "Detected: " + detection.first.toStdString(), cv::Point(10, 30),
            cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);
            detection.second.detection_count_output++;
        }
    }
}

void CvProcessor::startCamera() {
    if (!m_cap.isOpened()) {
        m_cap.open(m_camera_id);

        if (!m_cap.isOpened()) {
            qWarning("Nie udało się otworzyć kamery.");
            return;
        }
    }

    const size_t milisecs_in_sec = 1000;
    m_timer.start(std::floor(milisecs_in_sec / m_fps));
}

void CvProcessor::stopCamera() {
    m_timer.stop();
    if (m_cap.isOpened()) {
        m_cap.release();
    }
}

void CvProcessor::captureFrame() {
    if (!m_cap.isOpened()) return;

    cv::Mat frame;
    m_cap >> frame;
    if (frame.empty()) {
        return;
    }

    QImage image;
    if (!m_regions_of_interests.empty()
            && m_display_mode == ImageDisplayMode::Cropped
            && m_current_roi != m_regions_of_interests.end()) {

    cv::Mat cropped_frame = frame(m_current_roi->second);
    if (!m_detections.empty()) {detectionProcess(cropped_frame);};
    cv::cvtColor(cropped_frame, cropped_frame, cv::COLOR_BGR2RGB);
    image = cvMatToQImage(cropped_frame);
    } else {
        if (!m_detections.empty()) {detectionProcess(frame);};
        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
        image = cvMatToQImage(frame);
    }

    if (m_provider) {
        m_provider->setImage(image);
        emit imageUpdated();
    }
}

QImage CvProcessor::cvMatToQImage(const cv::Mat &mat) {
    return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888).copy();
}
