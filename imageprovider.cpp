#include "imageprovider.h"

ImageProvider::ImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image) {}

void ImageProvider::setImage(const QImage& image) {
    m_image = image;
}

QImage ImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize) {
    if (size)
        *size = m_image.size();
    return m_image;
}
