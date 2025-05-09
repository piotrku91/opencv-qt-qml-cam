#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "cv_processor.h"
#include "imageprovider.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    auto *imageProvider = new ImageProvider();
    CvProcessor processor(30, 0);
    processor.addROI({"test", cv::Rect{0, 0, 150, 150}});
    processor.setCurrentROI("test");
    processor.setDisplayMode(ImageDisplayMode::Cropped);

    ImageDetectionData det_data;
    det_data.detection_count_output = 0;
    det_data.threshold = 15;
    det_data.hsv_ranges = {{
                                {cv::Scalar(0, 120, 200), cv::Scalar(10, 255, 255)},
                                {cv::Scalar(160, 120, 200), cv::Scalar(180, 255, 255)}
                            }};
    processor.addDetection({"red", det_data});

    det_data.detection_count_output = 0;
    det_data.threshold = 15;
    det_data.hsv_ranges = {{
                                {cv::Scalar(10, 120, 200), cv::Scalar(25, 255, 255)}
                            }};
    processor.addDetection({"orange", det_data});

    processor.setImageProvider(imageProvider);
    engine.addImageProvider("opencv", imageProvider);

    engine.rootContext()->setContextProperty("processor", &processor);

    engine.load(QUrl(QStringLiteral("qrc:/cam/main.qml")));

    return app.exec();
}
