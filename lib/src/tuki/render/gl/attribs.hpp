#pragma once

enum class AttribLocation
{
	POS = 0,	// must be 0!
	COLOR,
	TEX_COORD,
	NORMAL,
	TANGENT,
	BITANGENT,
	TEX_COORD_1,
	TEX_COORD_2,
	TEX_COORD_3,

	NUM_ATTRIBS
};
enum class AttribBitMask
{
	NONE = 0,
	POS = 1 << (unsigned)AttribLocation::POS,
	COLOR = 1 << (unsigned)AttribLocation::COLOR,
	TEX_COORD = 1 << (unsigned)AttribLocation::TEX_COORD,
	NORMAL = 1 << (unsigned)AttribLocation::NORMAL,
	TANGENT = 1 << (unsigned)AttribLocation::TANGENT,
	BITANGENT = 1 << (unsigned)AttribLocation::BITANGENT,
	TEX_COORD_1 = 1 << (unsigned)AttribLocation::TEX_COORD_1,
	TEX_COORD_2 = 1 << (unsigned)AttribLocation::TEX_COORD_2,
	TEX_COORD_3 = 1 << (unsigned)AttribLocation::TEX_COORD_3,
};
inline AttribBitMask operator|(AttribBitMask a, AttribBitMask b)
{
	return static_cast<AttribBitMask>((int)a | (int)b);
}
inline AttribBitMask operator&(AttribBitMask a, AttribBitMask b)
{
	return static_cast<AttribBitMask>((int)a & (int)b);
}
inline AttribBitMask& operator |=(AttribBitMask& a, AttribBitMask b)
{
	return a = a | b;
}
inline AttribBitMask& operator &=(AttribBitMask& a, AttribBitMask b)
{
	return a = a & b;
}

const unsigned ATTRIB_NUM_COMPONENTS[(int)AttribLocation::NUM_ATTRIBS] =
{
	3,		// POS
	3,		// COLOR
	2,		// TEX_COORD
	3,		// NORMAL
	3,		// TANGENT
	3,		// BITANGENT
	2,		// TEX_COORD_1
	2,		// TEX_COORD_2
	2,		// TEX_COORD_3
};

extern const char* ATTRIB_NAMES[];