#include <iostream>
#include <fstream>
#include <stdexcept>

#include <cstring>
#include <cassert>

#include "gal/include/shader.hxx"
#include <vector>

using namespace KIGFX;

SHADER::SHADER() :
        isProgramCreated( false ),
        isShaderLinked( false ),
        active( false ),
        maximumVertices( 4 ),
        geomInputType( GL_LINES ),
        geomOutputType( GL_LINES )

{
    // Do not have uninitialized members:
    programNumber = 0;
}


SHADER::~SHADER()
{
    if( active )
        Deactivate();

    //if( isProgramCreated )
    //{
    //    if( glIsShader )
    //    {
    //        // Delete the shaders and the program
    //        for( std::deque<GLuint>::iterator it = shaderNumbers.begin(); it != shaderNumbers.end();
    //             ++it )
    //        {
    //            GLuint shader = *it;
    //
    //            if( glIsShader( shader ) )
    //            {
    //                glDetachShader( programNumber, shader );
    //                glDeleteShader( shader );
    //            }
    //        }
    //
    //        glDeleteProgram( programNumber );
    //    }
    //}
}


bool SHADER::LoadShaderFromFile(QOpenGLShader::ShaderType aShaderType, const std::string& aShaderSourceName )
{
    // Load shader sources
    QOpenGLShader* vs = new QOpenGLShader(aShaderType);
    QString source = aShaderSourceName.data();
    vs->compileSourceFile(source);

    if (!vs->isCompiled()) {
        delete vs;
    }

    
    return program.addShader(vs);
}


void SHADER::ConfigureGeometryShader( GLuint maxVertices, GLuint geometryInputType,
                                      GLuint geometryOutputType )
{
    maximumVertices = maxVertices;
    geomInputType = geometryInputType;
    geomOutputType = geometryOutputType;
}


bool SHADER::Link()
{
    // Shader linking
    program.link();
    //glLinkProgram( programNumber );
    programInfo( programNumber );

    // Check the Link state
    isShaderLinked = program.isLinked();

#ifdef DEBUG
    if( !isShaderLinked )
    {
        int maxLength;
        glGetProgramiv( programNumber, GL_INFO_LOG_LENGTH, &maxLength );
        maxLength = maxLength + 1;
        char* linkInfoLog = new char[maxLength];
        glGetProgramInfoLog( programNumber, maxLength, &maxLength, linkInfoLog );
        std::cerr << "Shader linking error:" << std::endl;
        std::cerr << linkInfoLog;
        delete[] linkInfoLog;
    }
#endif /* DEBUG */

    return isShaderLinked;
}


int SHADER::AddParameter( const std::string& aParameterName )
{
    GLint location = program.uniformLocation(aParameterName.data());
    if( location >= 0 )
        parameterLocation.push_back( location );
    else
        throw std::runtime_error( "Could not find shader uniform: " + aParameterName );

    return static_cast<int>( parameterLocation.size() ) - 1;
}


void SHADER::SetParameter( int parameterNumber, float value )
{
    assert( (unsigned) parameterNumber < parameterLocation.size() );

    program.setUniformValue((int)parameterLocation[parameterNumber], (GLfloat)value);

}


void SHADER::SetParameter( int parameterNumber, int value )
{
    assert( (unsigned) parameterNumber < parameterLocation.size() );

    program.setUniformValue(parameterLocation[parameterNumber], value);
}


void SHADER::SetParameter( int parameterNumber, float f0, float f1, float f2, float f3 )
{
    assert( (unsigned) parameterNumber < parameterLocation.size() );
    float arr[4] = { f0, f1, f2, f3 };
    program.setUniformValue(parameterLocation[parameterNumber], f0, f1, f2, f3);
}


void SHADER::SetParameter( int aParameterNumber, const QVector2D& aValue )
{
    assert( (unsigned) aParameterNumber < parameterLocation.size() );
    program.setUniformValue(parameterLocation[aParameterNumber], aValue);
}


int SHADER::GetAttribute( const std::string& aAttributeName ) const
{
    QString attribute = aAttributeName.data();
    return program.attributeLocation(attribute);
}


void SHADER::programInfo( GLuint aProgram )
{
    GLint glInfoLogLength = 0;
    GLint writtenChars = 0;

    // Get the length of the info string
    QOpenGLFunctions* function = QOpenGLContext::currentContext()->functions();
    function->glGetProgramiv(aProgram, GL_INFO_LOG_LENGTH, &glInfoLogLength);

    // Print the information
    if( glInfoLogLength > 2 )
    {
        GLchar* glInfoLog = new GLchar[glInfoLogLength];

        function->glGetProgramInfoLog( aProgram, glInfoLogLength, &writtenChars, glInfoLog );

        delete[] glInfoLog;
    }
}


void SHADER::shaderInfo( GLuint aShader )
{
    GLint glInfoLogLength = 0;
    GLint writtenChars = 0;

    // Get the length of the info string
    QOpenGLFunctions* function = QOpenGLContext::currentContext()->functions();
    function->glGetShaderiv( aShader, GL_INFO_LOG_LENGTH, &glInfoLogLength );

    // Print the information
    if( glInfoLogLength > 2 )
    {
        GLchar* glInfoLog = new GLchar[glInfoLogLength];
        function->glGetShaderInfoLog( aShader, glInfoLogLength, &writtenChars, glInfoLog );

        delete[] glInfoLog;
    }
}


std::string SHADER::ReadSource( const std::string& aShaderSourceName )
{
    // Open the shader source for reading
    std::ifstream inputFile( aShaderSourceName.c_str(), std::ifstream::in );
    std::string   shaderSource;

    if( !inputFile )
        throw std::runtime_error( "Can't read the shader source: " + aShaderSourceName );

    std::string shaderSourceLine;

    // Read all lines from the text file
    while( getline( inputFile, shaderSourceLine ) )
    {
        shaderSource += shaderSourceLine;
        shaderSource += "\n";
    }

    return shaderSource;
}


