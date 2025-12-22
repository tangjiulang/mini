#ifndef IMPORT_EXPORT_H_
#define IMPORT_EXPORT_H_

/// Macros which export functions from a DLL/DSO.
/// See: http://gcc.gnu.org/wiki/Visibility

#if defined(_WIN32)
	#define APIEXPORT __declspec(dllexport)
	#define APIIMPORT __declspec(dllimport)
	#define APILOCAL
#elif defined(__GNUC__) && __GNUC__ >= 4
	// On ELF, we compile with hidden visibility, so unwrap that for specific symbols:
	#define APIEXPORT __attribute__ ((visibility("default")))
	#define APIIMPORT __attribute__ ((visibility("default")))
	#define APILOCAL  __attribute__ ((visibility("hidden")))
#else
	#pragma message ( "warning: a supported C++ compiler is required" )
	#define APIEXPORT
	#define APIIMPORT
	#define APILOCAL
#endif

// We use APIVISIBLE to mark extern template declarations where we cannot use APIEXPORT
// Because MSVC disallows mixing dllexport and extern templates, we can't just use APIEXPORT
// However MSVC is fine with the dllexport in the cpp file and extern in the header
// But we need the visibility declared on both instantiation and extern for GCC/Clang to make
// the symbol visible
#if defined( __GNUC__ ) || defined( __clang__ )
	#define APIVISIBLE __attribute__ ((visibility("default")))
#else
	#define APIVISIBLE
#endif

#if defined(COMPILING_DLL)
	#define KIFACE_API    APIEXPORT
#else
	#define KIFACE_API    APIIMPORT
#endif

#define KIFACE_LOCAL	APILOCAL

#endif  // IMPORT_EXPORT_H_
