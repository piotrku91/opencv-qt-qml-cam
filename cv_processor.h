#pragma once
#include <QObject>
#include <QTimer>
#include <QImage>
#include <opencv2/opencv.hpp>

class ImageProvider;

class ColorLedDetector {
public:
    ColorLedDetector(int threshold = 50)
        : m_pixel_threshold(threshold) {}

    void setColorRange(const std::vector<std::pair<cv::Scalar, cv::Scalar>>& hsvRanges) {
        m_hsv_ranges = hsvRanges;
    }

    void setThreshold(int threshold) {
        m_pixel_threshold = threshold;
    }

    bool detect(const cv::Mat& frame, cv::Point2f& ledCenter, float& ledRadius) {
        if (frame.empty() || m_hsv_ranges.empty()) return false;

        cv::Mat hsv, mask, combinedMask;
        cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

        for (const auto& range : m_hsv_ranges) {
            cv::Mat tempMask;
            cv::inRange(hsv, range.first, range.second, tempMask);
            if (combinedMask.empty()) {
                combinedMask = tempMask;
            } else {
                cv::bitwise_or(combinedMask, tempMask, combinedMask);
            }
        }

        cv::erode(combinedMask, combinedMask, cv::Mat(), cv::Point(-1, -1), 1);
        cv::dilate(combinedMask, combinedMask, cv::Mat(), cv::Point(-1, -1), 3);

        int activePixels = cv::countNonZero(combinedMask);
        if (activePixels > m_pixel_threshold) {
            std::vector<std::vector<cv::Point>> contours;
            cv::findContours(combinedMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

            if (!contours.empty()) {
                size_t largestIndex = 0;
                double maxArea = 0;
                for (size_t i = 0; i < contours.size(); ++i) {
                    double area = cv::contourArea(contours[i]);
                    if (area > maxArea) {
                        maxArea = area;
                        largestIndex = i;
                    }
                }

                cv::minEnclosingCircle(contours[largestIndex], ledCenter, ledRadius);
                return true;
            }
        }

        return false;
    }

private:
    int m_pixel_threshold;
    std::vector<std::pair<cv::Scalar, cv::Scalar>> m_hsv_ranges;
};

enum ImageDisplayMode {
    FullImage,
    Cropped
};

struct ImageDetectionData {
    size_t threshold;
    std::vector<std::pair<cv::Scalar, cv::Scalar>> hsv_ranges;
    size_t detection_count_output = 0;
};

class CvProcessor : public QObject {
    Q_OBJECT
private:
    QTimer m_timer;
    cv::VideoCapture m_cap;
    ImageProvider* m_provider = nullptr;
    size_t m_fps;
    size_t m_camera_id;
    ImageDisplayMode m_display_mode;
    ColorLedDetector m_detector;

    std::map<QString, cv::Rect> m_regions_of_interests;
    std::map<QString, cv::Rect>::iterator m_current_roi;

    std::map<QString, ImageDetectionData> m_detections;

public:
    explicit CvProcessor(size_t fps, size_t camera_id, QObject *parent = nullptr);
    void setImageProvider(ImageProvider* provider);
    void setDisplayMode(ImageDisplayMode mode);

    void addROI(std::pair<QString, cv::Rect> new_roi);
    void setCurrentROI(QString roi_name);
    void clearROIs();

    void addDetection(std::pair<QString, ImageDetectionData> new_detection);
    std::optional<size_t> getDetectionCount(QString detection_name);
    void resetDetectionCount(QString detection_name);
    void clearDetections();
    void detectionProcess(cv::Mat frame);

    Q_INVOKABLE void startCamera();
    Q_INVOKABLE void stopCamera();

private:
    void captureFrame();
    QImage cvMatToQImage(const cv::Mat &mat);

signals:
    void imageUpdated();
};
