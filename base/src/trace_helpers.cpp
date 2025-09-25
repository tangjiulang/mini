/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2018 Wayne Stambaugh <stambaughw@gmail.com>
 * Copyright The KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

/**
 * @file  trace_helpers.cpp
 * @brief wxLogTrace helper implementation.
 */

#include <QCursor>
#include "trace_helpers.hxx"


const std::string const traceFindReplace = "KICAD_FIND_REPLACE";
const std::string const kicadTraceCoords = "KICAD_COORDS";
const std::string const kicadTraceKeyEvent = "KICAD_KEY_EVENTS";
const std::string const kicadTraceToolStack = "KICAD_TOOL_STACK";
const std::string const kicadTraceCoroutineStack = "KICAD_COROUTINE_STACK";
const std::string const traceSchLibMem = "KICAD_SCH_LIB_MEM";
const std::string const traceFindItem = "KICAD_FIND_ITEM";
const std::string const traceSchLegacyPlugin = "KICAD_SCH_LEGACY_PLUGIN";
const std::string const traceSchPlugin = "KICAD_SCH_PLUGIN";
const std::string const traceGedaPcbPlugin = "KICAD_GEDA_PLUGIN";
const std::string const traceKicadPcbPlugin = "KICAD_PCB_PLUGIN";
const std::string const tracePrinting = "KICAD_PRINT";
const std::string const traceAutoSave = "KICAD_AUTOSAVE";
const std::string const tracePathsAndFiles = "KICAD_PATHS_AND_FILES";
const std::string const traceLocale = "KICAD_LOCALE";
const std::string const traceFonts = "KICAD_FONTS";
const std::string const traceScreen = "KICAD_SCREEN";
const std::string const traceZoomScroll = "KICAD_ZOOM_SCROLL";
const std::string const traceSymbolResolver = "KICAD_SYM_RESOLVE";
const std::string const traceDisplayLocation = "KICAD_DISPLAY_LOCATION";
const std::string const traceSchSheetPaths = "KICAD_SCH_SHEET_PATHS";
const std::string const traceEnvVars = "KICAD_ENV_VARS";
const std::string const traceGalProfile = "KICAD_GAL_PROFILE";
const std::string const traceKiCad2Step = "KICAD2STEP";
const std::string const traceUiProfile = "KICAD_UI_PROFILE";
const std::string const traceGit = "KICAD_GIT";
const std::string const traceEagleIo = "KICAD_EAGLE_IO";
const std::string const traceDesignBlocks = "KICAD_DESIGN_BLOCK";


std::string dump( const QStringList& aArray )
{
    QString tmp;

    for( unsigned i = 0;  i < aArray.size();  i++ )
    {
        if( aArray[i].isEmpty() )
            tmp = "\"\" ";
        else
            tmp = aArray[i] + " ";
    }

    return tmp.toStdString();
}


// The following code was shamelessly copied from the wxWidgets keyboard sample
// at https://github.com/wxWidgets/wxWidgets/blob/master/samples/keyboard/keyboard.cpp.

/////////////////////////////////////////////////////////////////////////////
// Author:      Vadim Zeitlin
// Modified by: Marcin Wojdyr
// Created:     07.04.02
// Copyright:   (c) 2002 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// helper function that returns textual description of wx virtual keycode
std::string GetVirtualKeyCodeName(int keycode)
{
    switch (keycode)
    {
#define QTKEY(x) case Qt::Key_##x: return QStringLiteral(#x).toStdString();

        QTKEY(Backspace)
            QTKEY(Tab)
            QTKEY(Return)
            QTKEY(Escape)
            QTKEY(Space)
            QTKEY(Delete)
            QTKEY(Shift)
            QTKEY(Alt)
            QTKEY(Control)
            QTKEY(Menu)
            QTKEY(Pause)
            QTKEY(CapsLock)
            QTKEY(End)
            QTKEY(Home)
            QTKEY(Left)
            QTKEY(Up)
            QTKEY(Right)
            QTKEY(Down)
            QTKEY(Print)
            QTKEY(Insert)
            QTKEY(Help)
            QTKEY(0)
            QTKEY(1)
            QTKEY(2)
            QTKEY(3)
            QTKEY(4)
            QTKEY(5)
            QTKEY(6)
            QTKEY(7)
            QTKEY(8)
            QTKEY(9)
            QTKEY(Asterisk)
            QTKEY(Plus)
            QTKEY(Minus)
            QTKEY(Period)
            QTKEY(Slash)
            QTKEY(F1)
            QTKEY(F2)
            QTKEY(F3)
            QTKEY(F4)
            QTKEY(F5)
            QTKEY(F6)
            QTKEY(F7)
            QTKEY(F8)
            QTKEY(F9)
            QTKEY(F10)
            QTKEY(F11)
            QTKEY(F12)
            QTKEY(F13)
            QTKEY(F14)
            QTKEY(F15)
            QTKEY(F16)
            QTKEY(F17)
            QTKEY(F18)
            QTKEY(F19)
            QTKEY(F20)
            QTKEY(F21)
            QTKEY(F22)
            QTKEY(F23)
            QTKEY(F24)
            QTKEY(NumLock)
            QTKEY(ScrollLock)
            QTKEY(PageUp)
            QTKEY(PageDown)


            QTKEY(Enter)

            QTKEY(Super_L)   // Windows 左键
            QTKEY(Super_R)   // Windows 右键

#undef QTKEY

    default:
        return QStringLiteral("Unknown").toStdString();
    }
}


// helper function that returns textual description of key in the event
std::string GetKeyName( const QKeyEvent &aEvent )
{
    int keycode = aEvent.key();
    std::string virt = GetVirtualKeyCodeName( keycode );

    if( !virt.empty() )
        return virt;

    if( keycode > 0 && keycode < 32 )
        return std::format( "Ctrl-{}", (unsigned char)('A' + keycode - 1) );

    if( keycode >= 32 && keycode < 128 )
        return std::format( "'{}'", (unsigned char)keycode );

#if wxUSE_UNICODE
    int uc = aEvent.GetUnicodeKey();

    if( uc != WXK_NONE )
        return wxString::Format( "'%c'", uc );
#endif

    return "unknown";
}


std::string dump( const QKeyEvent& aEvent )
{
    std::string msg;
    std::string eventType = "unknown";

    if( aEvent.type() == QEvent::KeyPress )
        eventType = "KeyDown";
    else if( aEvent.type() == QEvent::KeyRelease)
        eventType = "KeyUp";
    else if(aEvent.type() == QEvent::KeyPress && !aEvent.text().isEmpty())
        eventType = "Char";
    else if( aEvent.type() == QEvent::ShortcutOverride)
        eventType = "Hook";

    QPoint globalPos = QCursor::pos();                 // 鼠标全局位置（屏幕坐标）

    // event  key_name  KeyCode  modifiers  Unicode  raw_code raw_flags pos
    msg = std::format( "{} {} {}   {}{}{}{}"
#if wxUSE_UNICODE
                "%5d (U+%04x)"
#else
                "    none   "
#endif
#ifdef wxHAS_RAW_KEY_CODES
                "  %7lu    0x%08lx"
#else
                "  not-set    not-set"
#endif
                "  (%5d,%5d)",
                eventType,
                GetKeyName( aEvent ),
                aEvent.key(),
                aEvent.modifiers().testFlag(Qt::ControlModifier) ? 'C' : '-',
                aEvent.modifiers().testFlag(Qt::AltModifier) ? 'A' : '-',
                aEvent.modifiers().testFlag(Qt::ShiftModifier) ? 'S' : '-',
                aEvent.modifiers().testFlag(Qt::MetaModifier) ? 'M' : '-'
#if wxUSE_UNICODE
                , aEvent.GetUnicodeKey()
                , aEvent.GetUnicodeKey()
#endif
#ifdef wxHAS_RAW_KEY_CODES
                , (unsigned long) aEvent.GetRawKeyCode()
                , (unsigned long) aEvent.GetRawKeyFlags()
#endif
                , globalPos.x()
                , globalPos.y()
        );

    return msg;
}


TRACE_MANAGER& TRACE_MANAGER::Instance()
{
    static TRACE_MANAGER* self = nullptr;

    if( !self )
    {
        self = new TRACE_MANAGER;
        self->init();
    }

    return *self;
}


bool TRACE_MANAGER::IsTraceEnabled( const QString& aWhat )
{
    if( !m_printAllTraces )
    {
        if( !m_globalTraceEnabled )
            return false;

        if( m_enabledTraces.find( aWhat ) == m_enabledTraces.end() )
            return false;
    }

    return true;
}


void TRACE_MANAGER::traceV( const QString& aWhat, const QString& aFmt, va_list vargs )
{
    if( !IsTraceEnabled( aWhat ) )
        return;

    char buffer[1024];
    va_list args;
    va_copy(args, vargs);
    vsnprintf(buffer, sizeof(buffer), aFmt.toUtf8().constData(), args);
    va_end(args);

    QString str = QString::fromUtf8(buffer);

    // 使用 spdlog 输出到 stderr
    spdlog::info("{:<30} | {}", aWhat.toStdString(), str.toStdString());
}


void TRACE_MANAGER::init()
{
    QString traceVars = QString::fromLocal8Bit(qgetenv("KICAD_TRACE"));
    m_globalTraceEnabled = !traceVars.isEmpty();
    m_printAllTraces = false;

    if (!m_globalTraceEnabled)
        return;

    // 用逗号分隔
    QStringList tokens = traceVars.split(',', Qt::SkipEmptyParts);

    for (const QString& token : tokens) {
        QString t = token.trimmed();
        m_enabledTraces[t] = true;

        if (t.compare("all", Qt::CaseInsensitive) == 0)
            m_printAllTraces = true;
    }

}
