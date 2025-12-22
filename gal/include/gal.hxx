#pragma once

#include <import_export.hxx>

#ifndef SWIG
	#if defined( GAL_DLL )
		#define GAL_API APIEXPORT
	#else
		#define GAL_API APIIMPORT
	#endif
#else
#define GAL_API
#endif