#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "drawables/mesh.hpp"

namespace engine
{
    struct Texture;
    struct Buffer;
    struct Shader;
    struct UniformBindings;
    struct Material;
    struct RenderPass;
    struct Mesh;
    
    enum renderer_type
    {
        vulkan = 0,
        dx12,
        dx11,
        opengl,
        metal,
        size // if we ever want to iterate over renderer types
    };

    enum ShaderModuleStage
    {
        FRAGMENT = 0x1,
        VERTEX = 0x10,
        COMPUTE = 0x100,
        ALL = FRAGMENT | VERTEX | COMPUTE
    };

    struct MeshPushConstant {
        alignas(16) glm::mat4 model;
    };

    struct ShaderData
    {
        alignas(4) float iTime = 0.0f;
        alignas(8) glm::vec2 iResolution;
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
    };

    struct ShaderStageInfo
    {
        const char* entryPoint;
        std::vector<char> shaderCode;
        ShaderModuleStage stage;
    };

    struct ShaderStageFactory
    {
        ShaderStageFactory entryPoint(const char* e)
        {
            info.entryPoint = e;
            return *this;
        }
        ShaderStageFactory shaderCode(std::vector<char> e)
        {
            info.shaderCode = e;
            return *this;
        }
        ShaderStageFactory stage(enum ShaderModuleStage e)
        {
            info.stage = e;
            return *this;
        }
        operator ShaderStageInfo() { return info; }

    private:
        ShaderStageInfo info;
    };

    struct ShaderInfo
    {
        std::vector<ShaderStageInfo> stages;
        ShaderModuleStage usedStages;
    };

    struct ShaderFactory
    {
        ShaderFactory stages(std::vector<ShaderStageInfo> e)
        {
            info.stages = e;
            return *this;
        }
        ShaderFactory usedStages(enum ShaderModuleStage e)
        {
            info.usedStages = e;
            return *this;
        }

        operator ShaderInfo() { return info; }

    private:
        ShaderInfo info;
    };

    enum TextureFormat
    {
        // INTEGER IMAGE
        COLOR_R = 0,
        COLOR_RG,
        COLOR_RGB,
        COLOR_RGBA,
        NON_COLOR_R,
        NON_COLOR_RG,
        NON_COLOR_RGB,
        NON_COLOR_RGBA,
        // FLOATING POINT IMAGE
        COLOR_R_16F,
        COLOR_R_32F,
        COLOR_RG_16F,
        COLOR_RG_32F,
        COLOR_RGB_16F,
        COLOR_RGB_32F,
        COLOR_RGBA_16F,
        COLOR_RGBA_32F,
        NON_COLOR_R_16F,
        NON_COLOR_R_32F,
        NON_COLOR_RG_16F,
        NON_COLOR_RG_32F,
        NON_COLOR_RGB_16F,
        NON_COLOR_RGB_32F,
        NON_COLOR_RGBA_16F,
        NON_COLOR_RGBA_32F,

        //DEPTH IMAGES
        DEPTH_F32,
        DEPTH_F32_STENCIL_8,
        DEPTH_F16,
        DEPTH_F16_STENCIL_8,

        DEPTH_UNORM,
        DEPTH_UNORM_STENCIL_8,
    };

    enum VertexFormat
    {
        XY_UINT,
        XY_FLOAT,
        XYZ_UINT,
        XYZ_FLOAT,
        XYZW_UINT,
        XYZW_FLOAT
    }; 

    struct TextureInfo
    {
        uint32_t width;
        uint32_t height;
        uint32_t numChannels;
        TextureFormat format;
        unsigned char* pixels = nullptr;
    };


    struct TextureBuilder
    {
        TextureBuilder width(uint32_t w)
        {
            self.width = w;
            return *this;
        }
        TextureBuilder height(uint32_t h)
        {
            self.height = h;
            return *this;
        }
        TextureBuilder numChannels(uint32_t nc)
        {
            self.numChannels = nc;
            return *this;
        }
        TextureBuilder format(TextureFormat f)
        {
            self.format = f;
            return *this;
        }

        TextureBuilder pixels(unsigned char* p)
        {
            self.pixels = p;
            return *this;
        }
        operator TextureInfo() { return self; }
        TextureInfo self;
    };

    struct Material
    {
        size_t id;
    };


    struct IndirectPack
    {
        Mesh *mesh;
        Material *material;
        uint32_t first;
        uint32_t count;
    };

    struct SubPassInfo
    {
    };

    struct VertexDescriptorInfo
    {
        VertexFormat format;
        size_t size;
    };

    struct RenderPassAttachment {

        TextureFormat format;
        bool isDepthAttachment;
        bool isSwapChainAttachment = false;
    };

    struct RenderPassInfo
    {
        uint32_t numDescriptors;
        uint32_t numAttributes;
        size_t size;
        bool depthAttachment;
        std::vector<SubPassInfo> subpasses;
        std::vector<VertexDescriptorInfo> vertexLayout;
        std::vector<RenderPassAttachment> attachments;
        ShaderInfo shaderInfo;
    };

    struct RenderPassFactory
    {
        RenderPassFactory numDescriptors(uint32_t n)
        {
            info.numDescriptors = n;
            return *this;
        }
        RenderPassFactory size(size_t s)
        {
            info.size = s;
            return *this;
        }
        RenderPassFactory depthAttachment(bool d)
        {
            info.depthAttachment = d;
            return *this;
        }
        RenderPassFactory subpasses(std::vector<SubPassInfo> sp)
        {
            info.subpasses = sp;
            return *this;
        }
        RenderPassFactory vertexLayout(std::vector<VertexDescriptorInfo> vl)
        {
            info.vertexLayout = vl;
            return *this;
        }
        
        RenderPassFactory attachments(std::vector<RenderPassAttachment> a)
        {
            info.attachments = a;
            return *this;
        }

        RenderPassFactory shaderInfo(ShaderInfo s)
        {
            info.shaderInfo = s;
            return *this;
        }

        operator RenderPassInfo() { return info; }

    private:
        RenderPassInfo info;
    };

    struct BufferInfo {
        size_t size;
        size_t offset;
    };

    struct BufferInfoFactory {

        BufferInfoFactory size(size_t s) {
            info.size = s;
            return *this;
        }

        BufferInfoFactory offset(size_t o) {
            info.offset = o;
            return *this;
        }

        operator BufferInfo() {
            return info;
        }
        BufferInfo info;
    };

    struct UniformBindingInfo {
        //...

        size_t size;
        size_t offset;
    };

    struct UniformBindingFactory {
        operator UniformBindingInfo() { return info; }

        UniformBindingInfo size(size_t s) {
            info.size = s;
            return *this;
        }
        UniformBindingInfo offset(size_t o) {
            info.offset = o;
            return *this;
        }

        UniformBindingInfo info;
    };

    struct MaterialInfo {
        std::vector<TextureInfo> textures;
        UniformBindingInfo bindingInfo;
    };

    struct MeshInfo {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    };

    struct MeshInfoFactory {
        MeshInfoFactory vertices(std::vector<Vertex> v) {
            info.vertices = v;
            return *this;
        }

        MeshInfoFactory indices(std::vector<uint32_t> i) {
            info.indices = i;
            return *this;
        }
        operator MeshInfo() { return info; }
        MeshInfo info;
    };

}