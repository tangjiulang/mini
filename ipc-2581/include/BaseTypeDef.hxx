#pragma once


enum class LineEndType {
	NONE,
	ROUND,
	SQUARE
};

enum class FillPropertyType {
	HOLLOW,
	HATCH,
	MESH,
	FILL,
	VOID
};

enum class UnitsType {
	MILLMETER,
	MICRON,
	INCH
};

enum class WrongType {
	WrongRead = -1,
	Success,
	NotIPC2581File,
	UnsupportedRevision,
	UnSuccessfulReadSection
};