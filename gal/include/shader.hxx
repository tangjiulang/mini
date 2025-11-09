#ifndef SHADER_H_
#define SHADER_H_

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLVersionFunctionsFactory>
#include <QOpenGLShaderProgram>

#include "vector2d.hxx"

#include <string>
#include <deque>

namespace KIGFX
{
class OPENGL_GAL;

/// Type definition for the shader
enum SHADER_TYPE
{
    SHADER_TYPE_VERTEX   = QOpenGLShader::Vertex,    ///< Vertex shader
    SHADER_TYPE_FRAGMENT = QOpenGLShader::Fragment,  ///< Fragment shader
    SHADER_TYPE_GEOMETRY = QOpenGLShader::Geometry   ///< Geometry shader
};

namespace DETAIL {

inline const char* translateStringArg( const std::string& str )
{
    return str.c_str();
}

inline const char* translateStringArg( const char* str )
{
    return str;
}

}


/**
 * Provide the access to the OpenGL shaders.
 *
 * The purpose of this class is advanced drawing with OpenGL. One example is using the pixel
 * shader for drawing exact circles or for anti-aliasing. This class supports vertex, geometry
 * and fragment shaders.
 *
 * Make sure that the hardware supports these features. This can be identified with the "GLEW"
 * library.
 */
class SHADER
{
public:

    SHADER();

    virtual ~SHADER();

    /**
     * Add a shader and compile the shader sources.
     *
     * @param aArgs is the list of strings (std::string or convertible to const char*) which
     *              are concatenated and compiled as a single shader source code.
     * @param aShaderType is the type of the shader.
     * @return True in case of success, false otherwise.
     */


    /**
     * Load one of the built-in shaders and compiles it.
     *
     * @param aShaderSourceName is the shader source file name.
     * @param aShaderType is the type of the shader.
     * @return True in case of success, false otherwise.
     */
    bool LoadShaderFromFile(QOpenGLShader::ShaderType, const std::string& aShaderSourceName );
    
    
    QString LoadShaderSourceFromStrings(const std::string& aShaderSourceName);

    bool LoadShaderFromString(QOpenGLShader::ShaderType aShaderType, const QString& aShaderSource);
    /**
     * Link the shaders.
     *
     * @return true in case of success, false otherwise.
     */
    bool Link();

    /**
     * Return true if shaders are linked correctly.
     */
    bool IsLinked() const
    {
        return isShaderLinked;
    }

    /**
     * Use the shader.
     */
    inline void Use()
    {
        program->bind();
        //QOpenGLFunctions_3_3_Core* function = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_3_3_Core>(QOpenGLContext::currentContext());
        //function->glUseProgram( programNumber );
        active = true;
    }

    /**
     * Deactivate the shader and use the default OpenGL program->
     */
    inline void Deactivate()
    {
        program->release();
        //QOpenGLFunctions_3_3_Core* function = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_3_3_Core>(QOpenGLContext::currentContext());
        //function->glUseProgram(0);
        active = false;
    }

    /**
     * Return the current state of the shader.
     *
     * @return True if any of shaders is enabled.
     */
    inline bool IsActive() const
    {
        return active;
    }

    /**
     * Configure the geometry shader - has to be done before linking!
     *
     * @param maxVertices is the maximum of vertices to be generated.
     * @param geometryInputType is the input type [e.g. GL_LINES, GL_TRIANGLES, GL_QUADS etc.]
     * @param geometryOutputType is the output type [e.g. GL_LINES, GL_TRIANGLES, GL_QUADS etc.]
     */
    void ConfigureGeometryShader( GLuint maxVertices, GLuint geometryInputType,
                                  GLuint geometryOutputType );

    /**
     * Add a parameter to the parameter queue.
     *
     * To communicate with the shader use this function to set up the names for the uniform
     * variables. These are queued in a list and can be assigned with the SetParameter(..)
     * method using the queue position.
     *
     * @param aParameterName is the name of the parameter.
     * @return the added parameter location.
     */
    int AddParameter( const char* aParameterName );

    /**
     * Set a parameter of the shader.
     *
     * @param aParameterNumber is the number of the parameter.
     * @param aValue is the value of the parameter.
     */
    void SetParameter( int aParameterNumber, float aValue );
    void SetParameter( int aParameterNumber, int aValue );
    void SetParameter( int aParameterNumber, const VECTOR2D& aValue );
    void SetParameter( int aParameterNumber, float f0, float f1, float f2, float f3 );
    void SetParameter(int parameterNumber, GLfloat f[16]);
    void SetParameter(int aParameterNumber, QMatrix4x4 mat);
    /**
     * Get an attribute location.
     *
     * @param aAttributeName is the name of the attribute.
     * @return the location.
     */
    int GetAttribute( const std::string& aAttributeName ) const;

    /**
     * Read the shader source file
     *
     * @param aShaderSourceName is the shader source file name.
     * @return the source as string
     */
    static std::string ReadSource( const std::string& aShaderSourceName );

    void InitProgram(QObject* parent);

    QOpenGLShaderProgram *program = nullptr;

private:
    /**
     * Compile vertex of fragment shader source code into the program.
     */

    /**
     * Get the shader program information.
     *
     * @param aProgram is the program number.
     */
    void programInfo( GLuint aProgram );

    /**
     * Get the shader information.
     *
     * @param aShader is the shader number.
     */
    void shaderInfo( GLuint aShader );

    std::deque<GLuint>  shaderNumbers;      ///< Shader number list
    GLuint              programNumber;      ///< Shader program number
    bool                isProgramCreated;   ///< Flag for program creation
    bool                isShaderLinked;     ///< Is the shader linked?
    bool                active;             ///< Is any of shaders used?
    GLuint              maximumVertices;    ///< The maximum of vertices to be generated

    ///< Input type [e.g. GL_LINES, GL_TRIANGLES, GL_QUADS etc.]
    GLuint              geomInputType;

    ///< Output type [e.g. GL_LINES, GL_TRIANGLES, GL_QUADS etc.]
    GLuint              geomOutputType;
    std::deque<GLint>   parameterLocation;  ///< Location of the parameter

};
} // namespace KIGFX

#endif /* SHADER_H_ */
