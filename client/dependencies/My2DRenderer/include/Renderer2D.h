#pragma once
#ifndef RENDERER2D_H
#define RENDERER2D_H

#include "MyOpenGL.h"
#include "Utils.h"

typedef int TextureHandle;

struct TexturedColoredRectangleInstance
{
    glm::vec2 destRectPosOffset;
    glm::vec2 destRectSizeScale;
    glm::vec2 srcRectPosOffset;
    glm::vec2 srcRectSizeScale;
    glm::vec3 color = glm::vec3(1,1,1);
    int textureHandle = -1;
    float depth = 0;
};

struct TexturedColoredRectangleDrawCommand
{
    glm::vec2 destRectPosOffset;
    glm::vec2 destRectSizeScale;
    glm::vec2 srcRectPosOffset;
    glm::vec2 srcRectSizeScale;
    glm::vec3 color = glm::vec3(1,1,1);
    const Texture* texturePtr = nullptr;
    float depth = 0;
};

struct TexelData
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

struct TextureRegistryData
{
    uint32_t startIndex;
    uint32_t width;
    uint32_t height;
    uint32_t reservedPadding;
};

class Renderer2D
{
private:
    Mesh<VertexP2> simpleQuadMesh;
    Mesh<VertexP2T2> texturedQuadMesh;

    Shader axisAllignedQuadColorTexturedShader;
    Shader axisAllignedQuadTextureShader;
    Shader axisAllignedQuadColorShader;

    const unsigned int maximumBasicDrawCallCount = 400;
    std::vector<TexturedColoredRectangleDrawCommand> _basicDrawCommands;
    unsigned int _basicDrawCommandCount = 0;


    GLuint _boxMeshVbo = 0;
    GLuint _boxMeshIbo = 0;
    GLuint _boxVao = 0;
    GLuint _boxInstancesVbo = 0;

    Shader instancedColoredTexturedRectangleShader;

    const unsigned int maximumDrawInstanceCount = 4000;
    std::vector<TexturedColoredRectangleInstance> _drawInstances;
    unsigned int _drawInstanceCount = 0;


    GLuint _texelBuffer = 0;
    GLuint _texelTbo = 0;
    std::vector<TexelData> _texelData; //maybe we dont have to store this cpu side, but ah well

    GLuint _textureRegistryBuffer = 0;
    GLuint _textureRegistryTbo = 0;
    uint32_t alreadyExsistingTexelCount = 0;
    std::vector<TextureRegistryData> _textureRegistry;

public:
    bool Init();

    void RenderRectangleStandalone(const Rectangle& dstRect,const glm::vec3& rgb, float depth);
    void RenderTexturedRectangleStandalone(const Rectangle& dstRect, const Rectangle& srcRect,const Texture* texture, float depth);
    void RenderColoredTexturedRectangleStandalone(const Rectangle& dstRect, const Rectangle& srcRect,
        const glm::vec3& rgb,const Texture* texture, float depth);

    void RenderRectangleInstanced(const Rectangle& dstRect,const glm::vec3& rgb, float depth);
    void RenderTexturedRectangleInstanced(const Rectangle& dstRect, const Rectangle& srcRect,TextureHandle textureHandle, 
        float depth);
    void RenderColoredTexturedRectangleInstanced(const Rectangle& dstRect, const Rectangle& srcRect,const glm::vec3& rgb,
        TextureHandle textureHandle, float depth);
    
    void Render();

    TextureHandle LoadAndRegisterTexture(const std::string& textureFilePath);

    void Delete();

private:
    void TryAddNewBasicDrawCommand(TexturedColoredRectangleDrawCommand& newDrawCommand);
    void TryAddNewDrawInstance(TexturedColoredRectangleInstance& newDrawInstance);
};

#endif