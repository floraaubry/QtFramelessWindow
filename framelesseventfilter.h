#pragma once
#include <QAbstractNativeEventFilter>
#include <QQuickWindow>
#include <QQuickItem>
#include <Windows.h>
#include <windowsx.h>
#include <dwmapi.h>

class FramelessEventFilter : public QAbstractNativeEventFilter
{
public:
    explicit FramelessEventFilter(QQuickWindow *window, int captionHeight = 32)
        : m_window(window)
        , m_captionHeight(captionHeight)
    {
        HWND h = hwnd();

        LONG_PTR style = GetWindowLongPtr(h, GWL_STYLE);
        style |= WS_CAPTION | WS_THICKFRAME | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
        SetWindowLongPtr(h, GWL_STYLE, style);

        LONG_PTR exStyle = GetWindowLongPtr(h, GWL_EXSTYLE);
        exStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
        SetWindowLongPtr(h, GWL_EXSTYLE, exStyle);

        MARGINS margins = {1, 1, 1, 1};
        DwmExtendFrameIntoClientArea(h, &margins);
    }

    void activate()
    {
        SetWindowPos(hwnd(), nullptr, 0, 0, 0, 0,
                     SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE |
                         SWP_NOZORDER | SWP_NOOWNERZORDER);
    }

    void setCaptionHeight(int h) { m_captionHeight = h; }

    static bool hasInteractiveItemAt(QQuickItem *root, const QPointF &pos)
    {
        const auto children = root->childItems();
        for (int i = children.size() - 1; i >= 0; --i) {
            QQuickItem *child = children[i];
            if (!child->isVisible() || !child->isEnabled())
                continue;
            const QPointF childPos = child->mapFromItem(root, pos);
            if (!child->contains(childPos))
                continue;
            if (child->acceptedMouseButtons() != Qt::NoButton
                || hasInteractiveItemAt(child, childPos))
                return true;
        }
        return false;
    }

    bool nativeEventFilter(const QByteArray &, void *message, qintptr *result) override
    {
        MSG *msg = static_cast<MSG *>(message);
        if (msg->hwnd != hwnd())
            return false;

        switch (msg->message)
        {
        case WM_NCCALCSIZE:
        {
            if (!msg->wParam)
                return false;

            if (IsZoomed(hwnd())) {
                MONITORINFO mi{};
                mi.cbSize = sizeof(mi);
                GetMonitorInfo(MonitorFromWindow(hwnd(), MONITOR_DEFAULTTONEAREST), &mi);
                reinterpret_cast<NCCALCSIZE_PARAMS *>(msg->lParam)->rgrc[0] = mi.rcWork;
            }
            *result = 0;
            return true;
        }

        case WM_NCHITTEST:
        {
            LRESULT dwmResult = 0;
            if (DwmDefWindowProc(hwnd(), msg->message, msg->wParam, msg->lParam, &dwmResult)) {
                *result = dwmResult;
                return true;
            }

            const int x     = GET_X_LPARAM(msg->lParam);
            const int y     = GET_Y_LPARAM(msg->lParam);
            const qreal dpr = m_window->devicePixelRatio();
            RECT rc;
            GetWindowRect(hwnd(), &rc);
            const int b = static_cast<int>(8 * dpr);

            if (!IsZoomed(hwnd())) {
                if (y < rc.top + b     && x < rc.left + b)    { *result = HTTOPLEFT;     return true; }
                if (y < rc.top + b     && x >= rc.right - b)  { *result = HTTOPRIGHT;    return true; }
                if (y >= rc.bottom - b && x < rc.left + b)    { *result = HTBOTTOMLEFT;  return true; }
                if (y >= rc.bottom - b && x >= rc.right - b)  { *result = HTBOTTOMRIGHT; return true; }
                if (y < rc.top + b)                            { *result = HTTOP;         return true; }
                if (y >= rc.bottom - b)                        { *result = HTBOTTOM;      return true; }
                if (x < rc.left + b)                           { *result = HTLEFT;        return true; }
                if (x >= rc.right - b)                         { *result = HTRIGHT;       return true; }
            }

            const int capPx = static_cast<int>(m_captionHeight * dpr);
            if (y < rc.top + capPx) {
                const QPointF localPt((x - rc.left) / dpr, (y - rc.top) / dpr);
                *result = hasInteractiveItemAt(m_window->contentItem(), localPt)
                              ? HTCLIENT : HTCAPTION;
                return true;
            }

            *result = HTCLIENT;
            return true;
        }

        case WM_GETMINMAXINFO:
        {
            MINMAXINFO *mmi = reinterpret_cast<MINMAXINFO *>(msg->lParam);
            MONITORINFO mi{};
            mi.cbSize = sizeof(mi);
            GetMonitorInfo(MonitorFromWindow(hwnd(), MONITOR_DEFAULTTONEAREST), &mi);
            mmi->ptMaxPosition.x = mi.rcWork.left - mi.rcMonitor.left;
            mmi->ptMaxPosition.y = mi.rcWork.top  - mi.rcMonitor.top;
            mmi->ptMaxSize.x     = mi.rcWork.right  - mi.rcWork.left;
            mmi->ptMaxSize.y     = mi.rcWork.bottom - mi.rcWork.top;
            mmi->ptMaxTrackSize  = mmi->ptMaxSize;
            *result = 0;
            return true;
        }

        default: break;
        }

        return false;
    }

private:
    HWND hwnd() const { return reinterpret_cast<HWND>(m_window->winId()); }
    QQuickWindow *m_window = nullptr;
    int m_captionHeight = 32;
};