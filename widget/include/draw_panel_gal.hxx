#pragma once

#include <QAbstractScrollArea>
#include <memory>

#include "gal/include/opengl_gal.hxx"
#include "gal/include/painter.hxx"
#include "view.hxx"


class DrawPanelGal : public QAbstractScrollArea {
public:
    enum GAL_TYPE {
        GAL_TYPE_UNKNOWN = -1,  ///< not specified: a GAL engine must be set by the client
        GAL_TYPE_NONE = 0,      ///< GAL not used (the legacy wxDC engine is used)
        GAL_TYPE_OPENGL,        ///< OpenGL implementation
        GAL_TYPE_CAIRO,         ///< Cairo implementation
        GAL_TYPE_LAST           ///< Sentinel, do not use as a parameter
    };

    DrawPanelGal(QWidget* parent, QSize aSize, GAL_TYPE aGalType = GAL_TYPE::GAL_TYPE_OPENGL);
    virtual ~DrawPanelGal();
    virtual bool SwitchBackend(GAL_TYPE aGalType);

    static constexpr GAL_TYPE GAL_FALLBACK = GAL_TYPE_OPENGL;
protected:
    void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent*) override;
    //void enterEvent(QEnterEvent*) override;
    //void focusOutEvent(QFocusEvent*) override;
    //void timerEvent(QTimerEvent*) override;
    //void showEvent(QShowEvent*) override;


    QWindow*                        m_parent;
    KIGFX::OPENGL_GAL*              m_gal;
    KIGFX::VIEW*                    m_view;
    std::unique_ptr<KIGFX::PAINTER> m_painter;
    GAL_TYPE                        m_backend;
    KIGFX::GAL_DISPLAY_OPTIONS      m_options;
};
