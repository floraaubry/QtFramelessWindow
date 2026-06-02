#pragma once
#include <QObject>
#include <QQmlEngine>
#include <QQuickWindow>
#include "FramelessEventFilter.h"

class FramelessHelper : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    static FramelessHelper *create(QQmlEngine *, QJSEngine *)
    {
        auto *obj = instance();
        QJSEngine::setObjectOwnership(obj, QJSEngine::CppOwnership);
        return obj;
    }

    static FramelessHelper *instance()
    {
        static FramelessHelper self;
        return &self;
    }

    Q_INVOKABLE void setup(QQuickWindow *window, int captionHeight = 32)
    {
        if (m_filter)
            return;

        window->winId();

        m_filter = new FramelessEventFilter(window, captionHeight);
        qGuiApp->installNativeEventFilter(m_filter);
        m_filter->activate();
    }

    Q_INVOKABLE void setCaptionHeight(int h)
    {
        if (m_filter)
            m_filter->setCaptionHeight(h);
    }

private:
    explicit FramelessHelper(QObject *parent = nullptr) : QObject(parent) {}
    FramelessEventFilter *m_filter = nullptr;
};