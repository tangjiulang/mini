/**
 * @file  trace_helpers.h
 * @brief wxLogTrace helper definitions.
 */

#ifndef _TRACE_HELPERS_H_
#define _TRACE_HELPERS_H_


#include <stdarg.h>
#include <map>
#include <string>
#include <QString>
#include <QKeyEvent>
#include <spdlog/spdlog.h>
/**
 * @defgroup trace_env_vars Trace Environment Variables
 *
 * wxWidgets provides trace control of debug messages using the WXTRACE environment variable.
 * This section defines the strings passed to WXTRACE to for debug output control of various
 * sections of the KiCad code.  See the wxWidgets <a href="http://docs.wxwidgets.org/3.0/
 * group__group__funcmacro__log.html#ga947e317db477914c12b13c4534867ec9"> wxLogTrace </a>
 * documentation for more information.
 */

///@{
/// \ingroup trace_env_vars

/**
 * Flag to enable find debug tracing.
 *
 * Use "KICAD_FIND_ITEM" to enable.
 */
extern const std::string const traceFindItem;

/**
 * Flag to enable find and replace debug tracing.
 *
 * Use "KICAD_FIND_REPLACE" to enable.
 */
extern const std::string const traceFindReplace;

/**
 * Flag to enable draw panel coordinate debug tracing.
 *
 * Use "KICAD_COORDS" to enable.
 */
extern const std::string const kicadTraceCoords;

/**
 * Flag to enable wxKeyEvent debug tracing.
 *
 * Use "KICAD_KEY_EVENTS" to enable.
 */
extern const std::string const kicadTraceKeyEvent;

/**
 * Flag to enable tracing of the tool handling stack.
 *
 * Use "KICAD_TOOL_STACK" to enable.
 */
extern const std::string const kicadTraceToolStack;

/**
 * Flag to enable tracing of the coroutine call stack.
 *
 * Use "KICAD_COROUTINE_STACK" to enable.
 */
extern const std::string const kicadTraceCoroutineStack;

/**
 * Flag to enable auto save feature debug tracing.
 *
 * Use "KICAD_AUTOSAVE" to enable.
 */
extern const std::string const traceAutoSave;

/**
 * Flag to enable schematic library memory deletion debug output.
 *
 * Use "KICAD_SCH_LIB_MEM" to enable.
 */
extern const std::string const traceSchLibMem;

/**
 * Flag to enable legacy schematic plugin debug output.
 *
 * Use "KICAD_SCH_PLUGIN" to enable.
 */
extern const std::string const traceSchPlugin;

/**
 * Flag to enable legacy schematic plugin debug output.
 *
 * Use "KICAD_SCH_LEGACY_PLUGIN" to enable.
 */
extern const std::string const traceSchLegacyPlugin;

/**
 * Flag to enable GEDA PCB plugin debug output.
 *
 * Use "KICAD_PCB_PLUGIN" to enable.
 */
extern const std::string const traceKicadPcbPlugin;

/**
 * Flag to enable GEDA PCB plugin debug output.
 *
 * Use "KICAD_GEDA_PLUGIN" to enable.
 */
extern const std::string const traceGedaPcbPlugin;

/**
 * Flag to enable print controller debug output.
 *
 * Use "KICAD_PRINT" to enable.
 */
extern const std::string const tracePrinting;

/**
 * Flag to enable path and file name debug output.
 *
 * Use "KICAD_PATHS_AND_FILES" to enable.
 */
extern const std::string const tracePathsAndFiles;

/**
 * Flag to enable locale debug output.
 *
 * Use "KICAD_LOCALE" to enable.
 */
extern const std::string const traceLocale;

/**
 * Flag to enable locale debug output.
 *
 * Use "KICAD_FONTS" to enable.
 */
extern const std::string const traceFonts;

/**
 * Flag to enable debug output of #BASE_SCREEN and it's derivatives.
 *
 * Use "KICAD_SCREEN" to enable.
 */
extern const std::string const traceScreen;

/**
 * Flag to enable debug output of display positioning logic.
 *
 * Use "KICAD_DISPLAY_LOCATION" to enable.
 */
extern const std::string const traceDisplayLocation;

/**
 * Flag to enable debug output of zoom-scrolling calculations in
 * #KIGFX::ZOOM_CONTROLLER and derivatives.
 *
 * Use "KICAD_ZOOM_SCROLL" to enable.
 */
extern const std::string const traceZoomScroll;

/**
 * Flag to enable debug output of symbol library resolver results
 *
 * Use "KICAD_SYM_RESOLVE" to enable.
 */
extern const std::string const traceSymbolResolver;

/**
 * Flag to enable debug output of schematic symbol sheet path manipulation code.
 *
 * Use "KICAD_SCH_SHEET_PATHS" to enable.
 */
extern const std::string const traceSchSheetPaths;

/**
 * Flag to enable debug output of environment variable operations.
 *
 * Use "KICAD_ENV_VARS" to enable.
 */
extern const std::string const traceEnvVars;

/**
 * Flag to enable debug output of GAL performance profiling.
 *
 * Use "KICAD_GAL_PROFILE" to enable.
 */
extern const std::string const traceGalProfile;

/**
 * Flag to enable KiCad2Step debug tracing.
 *
 * Use "KICAD2STEP" to enable.
 */
extern const std::string const traceKiCad2Step;

/**
 * Flag to enable user interface profile tracing.
 *
 * Use "KICAD_UI_PROFILE" to enable.
 */
extern const std::string const traceUiProfile;

/**
 * Flag to enable Git debugging output.
 *
 * Use "KICAD_GIT" to enable.
 */
extern const std::string const traceGit;

/*
 * Flag to enable Eagle I/O debug tracing.
 *
 * Use "KICAD_EAGLE_IO" to enable.
 */
extern const std::string const traceEagleIo;

/*
 * Flag to enable Design Block O debug tracing.
 *
 * Use "KICAD_EAGLE_IO" to enable.
 */
extern const std::string const traceDesignBlocks;

///@}

/**
 * Debug helper for printing wxKeyEvent information.
 *
 * @param aEvent is the wxKeyEvent to generate the print string from.
 * @return the wxKeyEvent information.
 */
extern std::string dump( const QKeyEvent& aEvent );

/**
 * Debug helper for printing wxArrayString contents.
 *
 * @param aArray is the string array to output.
 * @return the wxArrayString contents in a formatted string for debugging output.
 */
extern std::string dump( const QStringList& aArray );

class TRACE_MANAGER
{
public:
    TRACE_MANAGER() :
        m_globalTraceEnabled( false ),
        m_printAllTraces (false )
    {};
    ~TRACE_MANAGER(){};

    static TRACE_MANAGER& Instance();

    void Trace(const QString& tag, const char* fmt, auto&&... args)
    {
        const char* str = tag.toStdString().data();
        std::string src = (std::string("[{}] ") + std::string(fmt));
        spdlog::trace(src.data(), tag.toStdString().data(), std::forward<decltype(args)>(args)...);
    }

    void DoTrace( const QString aWhat, const char* aFmt, ... )
    {
        va_list argptr;
        va_start( argptr, aFmt );
        traceV( aWhat, aFmt, argptr );
        va_end( argptr );
    }

#if wxUSE_UNICODE_UTF8
    void DoTraceUtf8( const wxString aWhat, const wxChar* aFmt, ... )
    {
        va_list argptr;
        va_start( argptr, aFmt );
        traceV( aWhat, aFmt, argptr );
        va_end( argptr );
    }
#endif

    bool IsTraceEnabled( const QString& aWhat );

private:
    void traceV( const QString& aWhat, const QString& aFmt, va_list vargs );
    void init();

    std::map<QString, bool> m_enabledTraces;
    bool                     m_globalTraceEnabled;
    bool                     m_printAllTraces;
};

#define KI_TRACE( aWhat, ... )                                                                     \
    if( TRACE_MANAGER::Instance().IsTraceEnabled( aWhat ) )                                        \
    {                                                                                              \
        TRACE_MANAGER::Instance().Trace( aWhat, __VA_ARGS__ );                                     \
    }                                                                                              \
    else                                                                                           \
    {                                                                                              \
    }

#endif    // _TRACE_HELPERS_H_
