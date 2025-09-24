#ifndef __OPENGL_UTILS_H
#define __OPENGL_UTILS_H

#include <string>

/**
 * Check if a recent OpenGL operation has failed. If so, display the appropriate message
 * starting with \a aInfo string to give more details.
 *
 * @param aInfo is the beginning of the error message.
 * @param aFile is the file where the error occurred defined by the C __FILE__ variable.
 * @param aLine is the line in \a aFile where the error occurred defined by the C __LINE__
 *              variable.
 * @param aThrow an exception is thrown when true, otherwise only an error message is displayed.
 * @return GL_NO_ERROR in case of no errors or one of GL_ constants returned by glGetError().
 */
int checkGlError( const std::string& aInfo, const char* aFile, int aLine, bool aThrow = true );

/**
 * Enable or disable OpenGL driver messages output.
 *
 * @param aEnable decides whether the message should be shown.
 */
void enableGlDebug( bool aEnable );

#endif /* __OPENGL_ERROR_H */
