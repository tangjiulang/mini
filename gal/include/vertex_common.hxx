/**
 * @file vertex_common.h
 * @brief Common defines and consts used in vertex related classes.
 */

#ifndef VERTEX_COMMON_H_
#define VERTEX_COMMON_H_

#include <qopengl.h>

#include "vector2d.hxx"

#include <cstddef>

namespace KIGFX
{
///< Possible types of shaders (keep consistent with the actual shader source in
///< kicad_vert.glsl and kicad_frag.glsl).
enum SHADER_MODE
{
    SHADER_NONE = 0,
    SHADER_FILLED_CIRCLE = 2,
    SHADER_STROKED_CIRCLE = 3,
    SHADER_FONT = 4,
    SHADER_LINE_A = 5,
    SHADER_LINE_B = 6,
    SHADER_LINE_C = 7,
    SHADER_LINE_D = 8,
    SHADER_LINE_E = 9,
    SHADER_LINE_F = 10
};

///< Data structure for vertices {X,Y,Z,R,G,B,A,shader&param}
struct VERTEX
{
    GLfloat x, y, z;        // Coordinates
    GLfloat r, g, b, a;     // Color
    GLfloat shader[4];      // Shader type & params
};

static constexpr size_t VERTEX_SIZE   = sizeof( VERTEX );
static constexpr size_t VERTEX_STRIDE = VERTEX_SIZE / sizeof( GLfloat );

static constexpr size_t COORD_OFFSET  = offsetof( VERTEX, x );
static constexpr size_t COORD_SIZE    = sizeof( VERTEX::x ) + sizeof( VERTEX::y ) +
                                        sizeof( VERTEX::z );
static constexpr size_t COORD_STRIDE  = COORD_SIZE / sizeof( GLfloat );

static constexpr size_t COLOR_OFFSET = offsetof( VERTEX, r );
static constexpr size_t COLOR_SIZE = sizeof( VERTEX::r ) + sizeof( VERTEX::g ) +
                                     sizeof( VERTEX::b ) + sizeof( VERTEX::a );
static constexpr size_t COLOR_STRIDE = COLOR_SIZE / sizeof( GLubyte );

// Shader attributes
static constexpr size_t SHADER_OFFSET = offsetof( VERTEX, shader );
static constexpr size_t SHADER_SIZE = sizeof( VERTEX::shader );
static constexpr size_t SHADER_STRIDE = SHADER_SIZE / sizeof( GLfloat );

static constexpr size_t INDEX_SIZE = sizeof( GLuint );

} // namespace KIGFX

#endif /* VERTEX_COMMON_H_ */
