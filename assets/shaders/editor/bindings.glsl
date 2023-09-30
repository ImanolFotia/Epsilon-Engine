
#define RENDERPASS_SET 0
#define GLOBAL_SET 1
#define OBJECT_SET 2

#define MATERIALS_BINDING 1

#define INFO_BINDING 4
#define TRANSFORM_BINDING 5
#define OBJECT_BINDING 6
#define BINDLESS_TEXTURES_BINDING 0


#ifdef VERTEX_SHADER

struct RenderPass {
    float iTime;
    vec2 iResolution;
    vec3 lightPosition;
	vec3 viewPosition;
    mat4 view;
    mat4 proj;
    mat4 lightMatrix;
    int iFrame;
    ivec2 iMouse;
};

layout(binding = 0) uniform UniformBufferObject
{
    RenderPass data;
}
RenderPassUBO;

layout(location = 0) out VARYING {
    flat uint material_index;
    flat int object_id;
    vec4 color;
    vec2 texCoords;
    vec3 normal;
    vec3 position;
    mat3 TBN;
} vs_out;

struct ObjectData {
    mat4 modelMatrix;
    uint material_index;
};

struct ObjectData2 {
    int object_id;
    uint transform_index;
    uint material_index[4];
    uint numMaterials;
};

layout( push_constant ) uniform constants
{
	ObjectData object;
} PushConstant;


layout(std430, set = RENDERPASS_SET,  binding = OBJECT_BINDING) buffer readonly ObjectsIn {
   ObjectData2 entry[];
} ObjectBuffer;

layout(std430, set = RENDERPASS_SET,  binding = TRANSFORM_BINDING) buffer readonly TransformsIn {
   mat4 entry[];
} TransformBuffer;

#endif

#ifdef FRAGMENT_SHADER


struct RenderPass {
    float iTime;
    vec2 iResolution;
    vec3 lightPosition;
	vec3 viewPosition;
    mat4 view;
    mat4 proj;
    mat4 lightMatrix;
    int iFrame;
    ivec2 iMouse;
};

layout(binding = 0) uniform UniformBufferObject
{
    RenderPass data;
}
RenderPassUBO;

layout(location = 0) in VARYING {
    flat uint material_index;
    flat int object_id;
    vec4 color;
    vec2 texCoords;
    vec3 normal;
    vec3 position;
    mat3 TBN;
} fs_in;

struct Material {
		int albedo_texture_index;
		int normal_texture_index;
		int metallic_texture_index;
		int roughness_texture_index;
		vec4 albedo_color;
		float metallic_color;
		float roughness_color;
		float transmision;
};

layout(std430, set = RENDERPASS_SET,  binding = MATERIALS_BINDING) buffer readonly MaterialsIn {
   Material entry[];
} MaterialBuffer;

layout(std430, set = RENDERPASS_SET,  binding = INFO_BINDING) buffer InfoUniform {
   vec3 position; float padding0;
   vec3 normal; 
   vec3 selected_item_position;
   int id;
   int selected; 
   int mask;
   float circle_radius;
} CursorInfo;


layout (set = GLOBAL_SET, binding = BINDLESS_TEXTURES_BINDING) uniform sampler2D textures[];


struct SurfaceMaterial {
    
    Material internal_material;

    bool hasAlbedoTexture;
    bool hasMetallicTexture;
    bool hasRoughnessTexture;
    bool hasNormalTexture;

    vec4 albedoTexture;
    float metallicTexture;
    float roughnessTexture;
    vec3 normalTexture;
    
};

struct Surface {
    SurfaceMaterial material;
    vec4 color;
    float alpha;
    vec2 texCoords;
    vec3 normal;
    vec3 position;
    mat3 TBN;
};


vec4 getAlbedo(Surface surface) {
    vec4 outcolor;
    
    if(surface.material.hasAlbedoTexture) {
        outcolor = surface.material.albedoTexture;
        
        if(surface.material.internal_material.transmision >= 0) {
            outcolor.a = min(surface.material.internal_material.transmision, outcolor.a);
        }
    }
    else {
        //outcolor = surface.color;
        outcolor = surface.material.internal_material.albedo_color;
        if(surface.material.internal_material.transmision >= 0) {
            outcolor.a = min(surface.material.internal_material.transmision, outcolor.a);
        }
    }
    
    return outcolor;
}
float getMetallic(Surface surface) {
    if(surface.material.hasMetallicTexture) {
        return surface.material.metallicTexture;
    }
    else {
        return surface.material.internal_material.metallic_color;
    }
}
float getRoughness(Surface surface) {
    if(surface.material.hasMetallicTexture) {
        return surface.material.roughnessTexture;
    }
    else {
        return surface.material.internal_material.roughness_color;
    }
}

vec3 getNormal(in Surface surface) {
    if(surface.material.hasNormalTexture) {
        return normalize((surface.material.normalTexture * 2.0 - 1.0) * surface.TBN);
    }
    else {
        return normalize(surface.normal);
    }
}

vec3 Gamma(in vec3 img)
{
    return pow(img, vec3(1.0 / 2.2));
}

vec3 Degamma(in vec3 img)
{
    return pow(img, vec3(2.2));
}


Surface InitSurface() {
    Surface surface;
    
    surface.color = fs_in.color;
    surface.texCoords = fs_in.texCoords;
    surface.normal = fs_in.normal;
    surface.position = fs_in.position;
    surface.TBN = fs_in.TBN;

    surface.material.internal_material = MaterialBuffer.entry[fs_in.material_index];

    surface.material.hasAlbedoTexture = surface.material.internal_material.albedo_texture_index > -1 ? true : false; 
    surface.material.hasMetallicTexture = surface.material.internal_material.metallic_texture_index > -1 ? true : false;
    surface.material.hasRoughnessTexture = surface.material.internal_material.metallic_texture_index  > -1 ? true : false;
    surface.material.hasNormalTexture = surface.material.internal_material.normal_texture_index  > -1 ? true : false;

    if(surface.material.hasAlbedoTexture) {
        surface.material.albedoTexture = texture(textures[surface.material.internal_material.albedo_texture_index], surface.texCoords);
        surface.alpha = surface.material.albedoTexture.a;
    }
    
    if(surface.material.hasMetallicTexture) {
        vec2 metallicRoughness = texture(textures[surface.material.internal_material.metallic_texture_index], surface.texCoords).bg;
        surface.material.metallicTexture = metallicRoughness.x;
        surface.material.roughnessTexture = metallicRoughness.y;
    }

    if(surface.material.hasNormalTexture) {
        surface.material.normalTexture = texture(textures[surface.material.internal_material.normal_texture_index], surface.texCoords).rgb;
    }


    return surface;
}

#endif


/*
layout(std430, set = RENDERPASS_SET,  binding = OBJECTS_BINDING) buffer ObjectsIn {
   Object entry[];
} ObjectBuffer;

layout(std430, set = RENDERPASS_SET,  binding = TRANSFORMS_BINDING) buffer TransformsIn {
   mat4 model[];
} Transforms;*/

