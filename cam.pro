QT += quick

INCLUDEPATH += Z:/core/internal-pc-app/libs/opencv/build/include
LIBS += -LZ:/core/internal-pc-app/libs/opencv/build/x64/vc16/lib \
        -lopencv_world4110

SOURCES += \
        cv_processor.cpp \
        imageprovider.cpp \
        main.cpp \
        myimageprocessor.cpp

resources.files = main.qml 
resources.prefix = /$${TARGET}
RESOURCES += resources

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    cv_processor.h \
    imageprovider.h \
    myimageprocessor.h
