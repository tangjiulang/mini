#ifndef OPENGL_ANTIALIASING_H__
#define OPENGL_ANTIALIASING_H__

#include <memory>
#include <gal/include/shader.hxx>
#include <vector2d.hxx>

namespace MINI {

class OPENGL_COMPOSITOR;

class OPENGL_PRESENTOR
{
public:
    virtual ~OPENGL_PRESENTOR()
    {
    }

    virtual bool Init() = 0;
    virtual unsigned int CreateBuffer() = 0;

    virtual VECTOR2I GetInternalBufferSize() = 0;
    virtual void OnLostBuffers() = 0;

    virtual void Begin() = 0;
    virtual void DrawBuffer( GLuint aBuffer ) = 0;
    virtual void Present() = 0;
};


class ANTIALIASING_NONE : public OPENGL_PRESENTOR
{
public:
    ANTIALIASING_NONE( OPENGL_COMPOSITOR* aCompositor );

    bool Init() override;
    unsigned int CreateBuffer() override;

    VECTOR2I GetInternalBufferSize() override;
    void OnLostBuffers() override;

    void Begin() override;
    void DrawBuffer( GLuint aBuffer ) override;
    void Present() override;

private:
    OPENGL_COMPOSITOR* compositor;
};


class ANTIALIASING_SUPERSAMPLING : public OPENGL_PRESENTOR
{
public:
    ANTIALIASING_SUPERSAMPLING( OPENGL_COMPOSITOR* aCompositor );

    bool Init() override;
    unsigned int CreateBuffer() override;

    VECTOR2I GetInternalBufferSize() override;
    void OnLostBuffers() override;

    void Begin() override;
    void DrawBuffer( GLuint ) override;
    void Present() override;

private:
    OPENGL_COMPOSITOR* compositor;

    unsigned int ssaaMainBuffer;
    bool areBuffersCreated;

    bool areShadersCreated;
};

class ANTIALIASING_SMAA : public OPENGL_PRESENTOR
{
public:
    ANTIALIASING_SMAA( OPENGL_COMPOSITOR* aCompositor );

    bool Init() override;
    unsigned int CreateBuffer () override;

    VECTOR2I GetInternalBufferSize() override;
    void OnLostBuffers() override;

    void Begin() override;
    void DrawBuffer( GLuint buffer ) override;
    void Present() override;

private:
    void loadShaders();
    void updateUniforms();

    bool areBuffersInitialized;

    unsigned int smaaBaseBuffer;    // base + overlay temporary
    unsigned int smaaEdgesBuffer;
    unsigned int smaaBlendBuffer;

    // smaa shader lookup textures
    unsigned int smaaAreaTex;
    unsigned int smaaSearchTex;

    bool shadersLoaded;

    std::unique_ptr<SHADER> pass_1_shader;
    GLint pass_1_metrics;

    std::unique_ptr<SHADER> pass_2_shader;
    GLint pass_2_metrics;

    std::unique_ptr<SHADER> pass_3_shader;
    GLint pass_3_metrics;

    OPENGL_COMPOSITOR* compositor;

    GLint ufm_pass_1_shader_u_mvp;
    GLint ufm_pass_2_shader_u_mvp;
};

}

#endif
