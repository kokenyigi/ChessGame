#include "Renderer2D.h"

#include <vector>

#include "stb_image.h"

bool Renderer2D::Init()
{
    axisAllignedQuadColorShader.Init(
        "assets/shaders/AABC_vertex_shader.vert",
        "assets/shaders/AABC_fragment_shader.frag");

    axisAllignedQuadTextureShader.Init(
        "assets/shaders/AABT_vertex_shader.vert",
        "assets/shaders/AABT_fragment_shader.frag");

    axisAllignedQuadColorTexturedShader.Init(
        "assets/shaders/AABCT_vertex_shader.vert",
        "assets/shaders/AABCT_fragment_shader.frag");

    std::vector<VertexP2> simpleQuadVertices = 
    {
        {glm::vec2(-1,-1)},
        {glm::vec2(1,-1)},
        {glm::vec2(1,1)},
        {glm::vec2(-1,1)},
    };

    std::vector<unsigned int> quadIndices =
    {
        0,1,2,
        2,3,0
    };

    simpleQuadMesh.Init(simpleQuadVertices,quadIndices);

    std::vector<VertexP2T2> texturedQuadVertices = 
    {
        {glm::vec2(-1,-1),glm::vec2(0,0)},
        {glm::vec2(1,-1),glm::vec2(1,0)},
        {glm::vec2(1,1),glm::vec2(1,1)},
        {glm::vec2(-1,1),glm::vec2(0,1)},
    };

    texturedQuadMesh.Init(texturedQuadVertices,quadIndices);

    glGenVertexArrays(1,&_boxVao);
    glBindVertexArray(_boxVao);

    glGenBuffers(1,&_boxMeshVbo);
    glBindBuffer(GL_ARRAY_BUFFER, _boxMeshVbo);
    glBufferData(GL_ARRAY_BUFFER,sizeof(VertexP2T2)*4,texturedQuadVertices.data(),GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(VertexP2T2),
        (const void*)offsetof(VertexP2T2,position));
    glVertexAttribDivisor(0,0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(VertexP2T2),
        (const void*)offsetof(VertexP2T2,textureCoords));
    glVertexAttribDivisor(1,0);

    glGenBuffers(1,&_boxMeshIbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _boxMeshIbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(unsigned int) * 6, quadIndices.data(),GL_STATIC_DRAW);
    

    //Instencing stuff
    glGenBuffers(1,&_boxInstancesVbo);
    glBindBuffer(GL_ARRAY_BUFFER, _boxInstancesVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedColoredRectangleInstance) * maximumDrawInstanceCount, 
        nullptr,GL_STREAM_DRAW);

    this->_drawInstances.resize(maximumDrawInstanceCount);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(TexturedColoredRectangleInstance),
        (const void*)offsetof(TexturedColoredRectangleInstance,destRectPosOffset));
    glVertexAttribDivisor(2,1);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3,2,GL_FLOAT,GL_FALSE,sizeof(TexturedColoredRectangleInstance),
        (const void*)offsetof(TexturedColoredRectangleInstance,destRectSizeScale));
    glVertexAttribDivisor(3,1);

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4,2,GL_FLOAT,GL_FALSE,sizeof(TexturedColoredRectangleInstance),
        (const void*)offsetof(TexturedColoredRectangleInstance,srcRectPosOffset));
    glVertexAttribDivisor(4,1);

    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5,2,GL_FLOAT,GL_FALSE,sizeof(TexturedColoredRectangleInstance),
        (const void*)offsetof(TexturedColoredRectangleInstance,srcRectSizeScale));
    glVertexAttribDivisor(5,1);

    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6,3,GL_FLOAT,GL_FALSE,sizeof(TexturedColoredRectangleInstance),
        (const void*)offsetof(TexturedColoredRectangleInstance,color));
    glVertexAttribDivisor(6,1);

    glEnableVertexAttribArray(7);
    glVertexAttribIPointer(7,1,GL_INT,sizeof(TexturedColoredRectangleInstance),
        (const void*)offsetof(TexturedColoredRectangleInstance,textureHandle));
    glVertexAttribDivisor(7,1);

    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8,1,GL_FLOAT,GL_FALSE,sizeof(TexturedColoredRectangleInstance),
        (const void*)offsetof(TexturedColoredRectangleInstance,depth));
    glVertexAttribDivisor(8,1);

    this->instancedColoredTexturedRectangleShader.Init(
        "assets/shaders/AABCT_instanced_vertex_shader.vert",
        "assets/shaders/AABCT_instanced_fragment_shader.frag"
    );


    glBindVertexArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    glGenTextures(1,&_texelTbo);
    glGenTextures(1,&_textureRegistryTbo);


    return true;
}

void Renderer2D::RenderRectangleStandalone(const Rectangle &dstRect, const glm::vec3& rgb, float depth)
{
    //axisAllignedQuadColorShader.Bind();
    //axisAllignedQuadColorShader.SetUniform<glm::vec3>("uColor",rgb);

    //glm::vec2 posScale = glm::vec2(dstRect.w, dstRect.h);
    //glm::vec2 posOffset = glm::vec2(dstRect.w-1.0f + dstRect.x*2,-dstRect.h+1.0f-dstRect.y * 2);

    //axisAllignedQuadColorShader.SetUniform<glm::vec2>("uPosScale",posScale);
    //axisAllignedQuadColorShader.SetUniform<glm::vec2>("uPosOffset",posOffset);

    //simpleQuadMesh.Draw();

    //axisAllignedQuadColorShader.Unbind();
    TexturedColoredRectangleDrawCommand newDrawCommand;
    newDrawCommand.depth = depth;
    newDrawCommand.destRectPosOffset = glm::vec2(dstRect.x,dstRect.y);
    newDrawCommand.destRectSizeScale = glm::vec2(dstRect.w,dstRect.h);
    newDrawCommand.color = rgb;

    TryAddNewBasicDrawCommand(newDrawCommand);
}

void Renderer2D::RenderTexturedRectangleStandalone(const Rectangle &dstRect, const Rectangle &srcRect, const Texture* texture, float depth)
{
    /*
    axisAllignedQuadTextureShader.Bind();

    texture.Bind(0);

    axisAllignedQuadTextureShader.SetUniform<int>("uTexture",0);
    
    glm::vec2 posScale = glm::vec2(dstRect.w, dstRect.h);
    glm::vec2 posOffset = glm::vec2(dstRect.w-1.0f + dstRect.x*2,-dstRect.h+1.0f-dstRect.y * 2);

    axisAllignedQuadTextureShader.SetUniform<glm::vec2>("uPosScale",posScale);
    axisAllignedQuadTextureShader.SetUniform<glm::vec2>("uPosOffset",posOffset);

    glm::vec2 textureScale = glm::vec2(srcRect.w,srcRect.h);
    glm::vec2 textureOffset = glm::vec2(srcRect.x,1.0f-srcRect.y-srcRect.h);

    axisAllignedQuadTextureShader.SetUniform<glm::vec2>("uTextureOffset",textureOffset);
    axisAllignedQuadTextureShader.SetUniform<glm::vec2>("uTextureScale",textureScale);

    texturedQuadMesh.Draw();

    texture.Unbind();

    axisAllignedQuadTextureShader.Unbind();
    */

    TexturedColoredRectangleDrawCommand newDrawCommand;
    newDrawCommand.texturePtr = texture;
    newDrawCommand.srcRectPosOffset = glm::vec2(srcRect.x,srcRect.y);
    newDrawCommand.srcRectSizeScale = glm::vec2(srcRect.w,srcRect.h);
    newDrawCommand.depth = depth;
    newDrawCommand.destRectPosOffset = glm::vec2(dstRect.x,dstRect.y);
    newDrawCommand.destRectSizeScale = glm::vec2(dstRect.w,dstRect.h);

    TryAddNewBasicDrawCommand(newDrawCommand);
}

void Renderer2D::RenderColoredTexturedRectangleStandalone(const Rectangle &dstRect, const Rectangle &srcRect, const glm::vec3 &rgb, const Texture* texture, float depth)
{
    /*
    axisAllignedQuadColorTexturedShader.Bind();

    texture.Bind(0);

    axisAllignedQuadColorTexturedShader.SetUniform<int>("uTexture",0);
    axisAllignedQuadColorTexturedShader.SetUniform<glm::vec3>("uColor",rgb);
    
    glm::vec2 posScale = glm::vec2(dstRect.w, dstRect.h);
    glm::vec2 posOffset = glm::vec2(dstRect.w-1.0f + dstRect.x*2,-dstRect.h+1.0f-dstRect.y * 2);

    axisAllignedQuadColorTexturedShader.SetUniform<glm::vec2>("uPosScale",posScale);
    axisAllignedQuadColorTexturedShader.SetUniform<glm::vec2>("uPosOffset",posOffset);

    glm::vec2 textureScale = glm::vec2(srcRect.w,srcRect.h);
    glm::vec2 textureOffset = glm::vec2(srcRect.x,1.0f-srcRect.y-srcRect.h);

    axisAllignedQuadColorTexturedShader.SetUniform<glm::vec2>("uTextureOffset",textureOffset);
    axisAllignedQuadColorTexturedShader.SetUniform<glm::vec2>("uTextureScale",textureScale);

    texturedQuadMesh.Draw();

    texture.Unbind();

    axisAllignedQuadColorTexturedShader.Unbind();
    */

    TexturedColoredRectangleDrawCommand newDrawCommand;
    newDrawCommand.texturePtr = texture;
    newDrawCommand.srcRectPosOffset = glm::vec2(srcRect.x,srcRect.y);
    newDrawCommand.srcRectSizeScale = glm::vec2(srcRect.w,srcRect.h);
    newDrawCommand.depth = depth;
    newDrawCommand.destRectPosOffset = glm::vec2(dstRect.x,dstRect.y);
    newDrawCommand.destRectSizeScale = glm::vec2(dstRect.w,dstRect.h);
    newDrawCommand.color = rgb;

    TryAddNewBasicDrawCommand(newDrawCommand);
}

void Renderer2D::RenderRectangleInstanced(const Rectangle &dstRect, const glm::vec3 &rgb, float depth)
{   
    TexturedColoredRectangleInstance newDrawInstance;

    newDrawInstance.color = rgb;
    newDrawInstance.depth = depth;
    newDrawInstance.destRectPosOffset = glm::vec2(dstRect.x,dstRect.y);
    newDrawInstance.destRectSizeScale = glm::vec2(dstRect.w,dstRect.h);

    TryAddNewDrawInstance(newDrawInstance);
}

void Renderer2D::RenderTexturedRectangleInstanced(const Rectangle &dstRect, const Rectangle &srcRect, TextureHandle textureHandle, float depth)
{
    TexturedColoredRectangleInstance newDrawInstance;

    newDrawInstance.textureHandle = textureHandle;
    newDrawInstance.srcRectPosOffset = glm::vec2(srcRect.x,srcRect.y);
    newDrawInstance.srcRectSizeScale = glm::vec2(srcRect.w,srcRect.h);
    newDrawInstance.depth = depth;
    newDrawInstance.destRectPosOffset = glm::vec2(dstRect.x,dstRect.y);
    newDrawInstance.destRectSizeScale = glm::vec2(dstRect.w,dstRect.h);

    TryAddNewDrawInstance(newDrawInstance);
}

void Renderer2D::RenderColoredTexturedRectangleInstanced(const Rectangle &dstRect, const Rectangle &srcRect, const glm::vec3 &rgb, TextureHandle textureHandle, float depth)
{
    
    TexturedColoredRectangleInstance newDrawInstance;

    newDrawInstance.textureHandle = textureHandle;
    newDrawInstance.srcRectPosOffset = glm::vec2(srcRect.x,srcRect.y);
    newDrawInstance.srcRectSizeScale = glm::vec2(srcRect.w,srcRect.h);
    newDrawInstance.depth = depth;
    newDrawInstance.destRectPosOffset = glm::vec2(dstRect.x,dstRect.y);
    newDrawInstance.destRectSizeScale = glm::vec2(dstRect.w,dstRect.h);
    newDrawInstance.color = rgb;

    TryAddNewDrawInstance(newDrawInstance);
}

void Renderer2D::Render()
{
    //std::cout<< "[Renderer2D]: Rendering pass starts.\n";

    bool wasDepthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
    if(wasDepthTestEnabled == false)
    {
        glEnable(GL_DEPTH_TEST);
    }

    glDepthFunc(GL_LEQUAL);

    if(this->_drawInstanceCount > 0)
    {
        //std::cout<<"[Renderer2D]: I will render " << this->_drawInstanceCount << " Number of instances!\n";

        glBindVertexArray(this->_boxVao);
        glBindBuffer(GL_ARRAY_BUFFER,this->_boxInstancesVbo);
        glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(TexturedColoredRectangleInstance) * this->_drawInstanceCount,
            this->_drawInstances.data());

        
        
        this->instancedColoredTexturedRectangleShader.Bind(); // have to init still


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_BUFFER, _texelTbo);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_BUFFER, _textureRegistryTbo);

        instancedColoredTexturedRectangleShader.SetUniform<int>("uTexelBuffer",0);

        instancedColoredTexturedRectangleShader.SetUniform<int>("uTextureRegistryBuffer",1);

        glDrawElementsInstanced(GL_TRIANGLES,6,GL_UNSIGNED_INT,nullptr,this->_drawInstanceCount);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_BUFFER, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_BUFFER, 0);

        this->instancedColoredTexturedRectangleShader.Unbind();

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER,0);   

        this->_drawInstanceCount = 0;
    }
    
    if(this->_basicDrawCommandCount > 0)
    {
        this->axisAllignedQuadColorTexturedShader.Bind();

        for(int i=0;i<this->_basicDrawCommandCount;++i)
        {
            if(_basicDrawCommands[i].texturePtr != nullptr)
            {
                _basicDrawCommands[i].texturePtr->Bind(0);

                axisAllignedQuadColorTexturedShader.SetUniform<int>("uTexture",0);
            }
            
            axisAllignedQuadColorTexturedShader.SetUniform<glm::vec3>("uColor",_basicDrawCommands[i].color);

            axisAllignedQuadColorTexturedShader.SetUniform<glm::vec2>("uPosScale",_basicDrawCommands[i].destRectSizeScale);
            axisAllignedQuadColorTexturedShader.SetUniform<glm::vec2>("uPosOffset",_basicDrawCommands[i].destRectPosOffset);

            axisAllignedQuadColorTexturedShader.SetUniform<glm::vec2>("uTextureOffset",_basicDrawCommands[i].srcRectPosOffset);
            axisAllignedQuadColorTexturedShader.SetUniform<glm::vec2>("uTextureScale",_basicDrawCommands[i].srcRectSizeScale);

            axisAllignedQuadColorTexturedShader.SetUniform<float>("uDepth",_basicDrawCommands[i].depth);

            axisAllignedQuadColorTexturedShader.SetUniform<int>("uIsThereTexture",
                _basicDrawCommands[i].texturePtr == nullptr ? 0 : 1);

            texturedQuadMesh.Draw();

            if(_basicDrawCommands[i].texturePtr != nullptr)
            {
                _basicDrawCommands[i].texturePtr->Unbind();
            }
            
        }

        this->axisAllignedQuadColorTexturedShader.Unbind();

        this->_basicDrawCommandCount = 0;
    }
    

    glDepthFunc(GL_LESS);

    if(wasDepthTestEnabled == false)
    {
        glDisable(GL_DEPTH_TEST);
    }
    
}

TextureHandle Renderer2D::LoadAndRegisterTexture(const std::string &textureFilePath)
{
    TextureHandle newTextureHandle = -1;

    stbi_set_flip_vertically_on_load(1);

    unsigned char* byteDataOfTexture = nullptr;
    int width = 0;
    int height = 0;
    int channelsInTexture = 0;

    byteDataOfTexture = stbi_load(textureFilePath.c_str(), &width, &height, &channelsInTexture, 4); //forces rgba

    if(byteDataOfTexture == nullptr)
    {
        //fail
        return newTextureHandle;
    }

    TextureRegistryData newTextureRegistryEntryData;

    uint32_t currentTexelCount = width * height;

    newTextureRegistryEntryData.width = width;
    newTextureRegistryEntryData.height = height;
    newTextureRegistryEntryData.startIndex = alreadyExsistingTexelCount;

    //Lets create the new GPU-side tbos for the texel storage and the registry.
    GLuint newTexelBuffer = 0;
    glGenBuffers(1,&newTexelBuffer);
    glBindBuffer(GL_TEXTURE_BUFFER,newTexelBuffer);
    glBufferData(GL_TEXTURE_BUFFER,sizeof(TexelData)*(alreadyExsistingTexelCount + currentTexelCount),nullptr,GL_STATIC_DRAW);

    GLuint newTextureRegistryBuffer = 0;
    glGenBuffers(1,&newTextureRegistryBuffer);
    glBindBuffer(GL_TEXTURE_BUFFER,newTextureRegistryBuffer);
    glBufferData(GL_TEXTURE_BUFFER,sizeof(TextureRegistryData) * (_textureRegistry.size() + 1),nullptr,GL_STATIC_DRAW);

    //If we already loaded a texture beforehand we have to do some copying from one buffer to another, 
    // and delete the old buffers
    if(alreadyExsistingTexelCount > 0 && this->_textureRegistry.size() > 0)
    {
        glBindBuffer(GL_COPY_READ_BUFFER,_texelBuffer);
        glBindBuffer(GL_COPY_WRITE_BUFFER,newTexelBuffer);

        glCopyBufferSubData(GL_COPY_READ_BUFFER,GL_COPY_WRITE_BUFFER,0,0,sizeof(TexelData) * alreadyExsistingTexelCount);

        glBindBuffer(GL_COPY_READ_BUFFER,_textureRegistryBuffer);
        glBindBuffer(GL_COPY_WRITE_BUFFER,newTextureRegistryBuffer);

        glCopyBufferSubData(GL_COPY_READ_BUFFER,GL_COPY_WRITE_BUFFER,0,0,sizeof(TextureRegistryData) * _textureRegistry.size());
        
        glBindBuffer(GL_COPY_READ_BUFFER,0);
        glBindBuffer(GL_COPY_WRITE_BUFFER,0);

        glDeleteBuffers(1,&_texelBuffer);
        glDeleteBuffers(1,&_textureRegistryBuffer);
    }

    _texelBuffer = newTexelBuffer;
    _textureRegistryBuffer = newTextureRegistryBuffer;

    //Load the new data into the texel Buffer, and re-assign it to the given texture buffer object(TBO)
    glBindBuffer(GL_TEXTURE_BUFFER, _texelBuffer);
    glBufferSubData(GL_TEXTURE_BUFFER,
        sizeof(TexelData) * alreadyExsistingTexelCount,
        sizeof(TexelData) * currentTexelCount,
        byteDataOfTexture
    );
    glBindTexture(GL_TEXTURE_BUFFER, _texelTbo);
    glTexBuffer(GL_TEXTURE_BUFFER,GL_RGBA8,_texelBuffer);

    alreadyExsistingTexelCount += currentTexelCount;

    //Now let add the new texture registry data to the correct buffer
    glBindBuffer(GL_TEXTURE_BUFFER, _textureRegistryBuffer);
    glBufferSubData(GL_TEXTURE_BUFFER,
        sizeof(TextureRegistryData) * _textureRegistry.size(),
        sizeof(TextureRegistryData),
        &newTextureRegistryEntryData
    );
    glBindTexture(GL_TEXTURE_BUFFER, _textureRegistryTbo);
    glTexBuffer(GL_TEXTURE_BUFFER,GL_RGBA32UI,_textureRegistryBuffer);

    newTextureHandle = _textureRegistry.size();

    _textureRegistry.push_back(newTextureRegistryEntryData);

    glBindTexture(GL_TEXTURE_BUFFER, 0);
    glBindBuffer(GL_TEXTURE_BUFFER,0);
    stbi_image_free(byteDataOfTexture);

    return newTextureHandle;
}

void Renderer2D::TryAddNewBasicDrawCommand(TexturedColoredRectangleDrawCommand &newDrawCommand)
{
    if(this->maximumBasicDrawCallCount > this->_basicDrawCommandCount)
    {
        newDrawCommand.destRectPosOffset = glm::vec2(
            newDrawCommand.destRectSizeScale.x-1.0f + newDrawCommand.destRectPosOffset.x*2,
            -newDrawCommand.destRectSizeScale.y+1.0f-newDrawCommand.destRectPosOffset.y * 2);

        newDrawCommand.srcRectPosOffset = glm::vec2(
            newDrawCommand.srcRectPosOffset.x,
            1.0f-newDrawCommand.srcRectPosOffset.y-newDrawCommand.srcRectSizeScale.y);

        this->_basicDrawCommands[this->_basicDrawCommandCount] = newDrawCommand;
        ++this->_basicDrawCommandCount;
    }
}

void Renderer2D::TryAddNewDrawInstance(TexturedColoredRectangleInstance &newDrawInstance)
{
    if(this->maximumDrawInstanceCount > this->_drawInstanceCount)
    {
        newDrawInstance.destRectPosOffset = glm::vec2(
            newDrawInstance.destRectSizeScale.x-1.0f + newDrawInstance.destRectPosOffset.x*2,
            -newDrawInstance.destRectSizeScale.y+1.0f-newDrawInstance.destRectPosOffset.y * 2);

        newDrawInstance.srcRectPosOffset = glm::vec2(
            newDrawInstance.srcRectPosOffset.x,
            1.0f-newDrawInstance.srcRectPosOffset.y-newDrawInstance.srcRectSizeScale.y);

        this->_drawInstances[this->_drawInstanceCount] = newDrawInstance;
        ++this->_drawInstanceCount;
    }
}
