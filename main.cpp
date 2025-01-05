// #include <QGuiApplication>
// #include <QQmlApplicationEngine>

// int main(int argc, char *argv[])
// {
//     QGuiApplication app(argc, argv);

//     QQmlApplicationEngine engine;
//     QObject::connect(
//         &engine,
//         &QQmlApplicationEngine::objectCreationFailed,
//         &app,
//         []() { QCoreApplication::exit(-1); },
//         Qt::QueuedConnection);
//     engine.loadFromModule("qwidgetinqml", "Main");

//     return app.exec();
// }
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickPaintedItem>
#include <QPainter>
#include <QApplication>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QVideoSink>
#include <QMediaDevices>
#include <QImage>
#include <QTimer>
#include <QWidget>
#include <QtQml>

class VideoDisplayWidget : public QWidget {
    Q_OBJECT
public:
    VideoDisplayWidget(QWidget *parent = nullptr) : QWidget(parent) {}

    void updateFrame(const QImage &image) {
        currentFrame = image;
        update(); // Trigger a repaint
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        if (!currentFrame.isNull()) {
            QPainter painter(this);
            painter.drawImage(rect(), currentFrame);
        }
    }

private:
    QImage currentFrame;
};

class CameraFeedWidget : public QWidget {
    Q_OBJECT
public:
    explicit CameraFeedWidget(QWidget *parent = nullptr) : QWidget(parent) {
        QMediaDevices mediaDevices;
        QCameraDevice cameraDevice = mediaDevices.defaultVideoInput();

        QCamera *camera = new QCamera(cameraDevice, this);
        videoDisplayWidget = new VideoDisplayWidget(this);
        videoDisplayWidget->setGeometry(0, 0, this->width(), this->height());

        captureSession = new QMediaCaptureSession(this);
        captureSession->setCamera(camera);

        videoSink = new QVideoSink(this);
        connect(videoSink, &QVideoSink::videoFrameChanged, this, &CameraFeedWidget::invertFrame);
        captureSession->setVideoOutput(videoSink);

        camera->start();
    }

    void resizeview(int width, int height) {
        resize(width, height);
        videoDisplayWidget->setGeometry(0, 0, width, height);
    }

private:
    QMediaCaptureSession *captureSession;
    QVideoSink *videoSink;
    VideoDisplayWidget *videoDisplayWidget;

    void invertFrame(const QVideoFrame &frame) {
        QVideoFrame readFrame = frame;
        readFrame.map(QVideoFrame::ReadOnly);
        QImage image = readFrame.toImage().mirrored(true, false);
        videoDisplayWidget->updateFrame(image);
        readFrame.unmap();
    }
};

class CameraFeedItem : public QQuickPaintedItem {
    Q_OBJECT
public:
    CameraFeedItem(QQuickItem *parent = nullptr) : QQuickPaintedItem(parent) {
        cameraFeedWidget = new CameraFeedWidget();

        // Set up a timer to trigger periodic updates
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [&](){update();});
        timer->start(30); // Update every 30 milliseconds (approximately 33 frames per second)
    }

    void paint(QPainter *painter) override {
        QImage image(cameraFeedWidget->size(), QImage::Format_ARGB32);
        image.fill(Qt::transparent);

        QPainter widgetPainter(&image);
        cameraFeedWidget->render(&widgetPainter);

        painter->drawImage(boundingRect(), image);
    }

private:
    CameraFeedWidget *cameraFeedWidget;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    qmlRegisterType<CameraFeedItem>("CustomWidgets", 1, 0, "CameraFeedItem");

    QQmlApplicationEngine engine;
    engine.loadFromModule("qwidgetinqml", "Main");

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}

#include "main.moc"
