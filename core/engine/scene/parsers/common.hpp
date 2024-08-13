#pragma once

#include <nlohmann/json.hpp>

#include <core/engine/renderer/types.hpp>

namespace engine::parsers {

	static WindingMode resolveWindingMode(const std::string& x) {
		if (x == "CLOCKWISE") return CLOCKWISE;
		if (x == "COUNTER_CLOCK_WISE") return COUNTER_CLOCK_WISE;
		return COUNTER_CLOCK_WISE;
	}

	static CullMode resolveCullMode(const std::string& x) {
		if (x == "BACK") return BACK;
		if (x == "FRONT") return FRONT;
		if (x == "NONE") return NONE;
		return NONE;
	}

	static Filtering resolveFiltering(const std::string& x) {
		if (x == "POINT") return POINT;
		if (x == "LINEAR") return LINEAR;
		if (x == "ANISOTROPIC") return ANISOTROPIC;
		return LINEAR;
	}


	static CompareFunction resolveCompareFunction(const std::string& x) {
		if (x == "ALWAYS") return ALWAYS;
		if (x == "LESS") return LESS;
		if (x == "LESS_OR_EQUAL") return LESS_OR_EQUAL;
		if (x == "EQUAL") return EQUAL;
		return LESS_OR_EQUAL;
	}

	static UniformBindingType resolveType(const std::string x) {
		if (x == "UNIFORM_BUFFER") return UniformBindingType::UNIFORM_BUFFER;
		if (x == "TEXTURE_SAMPLER") return UniformBindingType::TEXTURE_SAMPLER;
		if (x == "TEXTURE_IMAGE_COMBINED_SAMPLER") return UniformBindingType::TEXTURE_IMAGE_COMBINED_SAMPLER;
		if (x == "SHADER_STORAGE") return UniformBindingType::SHADER_STORAGE;
		if (x == "BINDLESS_TEXTURE") return UniformBindingType::BINDLESS_TEXTURE;
		if (x == "ACCELERATION_STRUCTURE") return UniformBindingType::ACCELERATION_STRUCTURE;
		if (x == "STORAGE_IMAGE") return UniformBindingType::STORAGE_IMAGE;
		
		return UniformBindingType::SIZE;
	}


		static WrapMode resolveWrapMode(const std::string& x) {
		if (x == "REPEAT") return REPEAT;
		if (x == "CLAMP_TO_BORDER") return CLAMP_TO_BORDER;
		if (x == "CLAMP_TO_EDGE") return CLAMP_TO_EDGE;
		return REPEAT;
	}


	static TextureFormat resolveFormat(const std::string& x) {
		if (x == "COLOR_R") return COLOR_R;
		if (x == "COLOR_RG") return COLOR_RG;
		if (x == "COLOR_RGB") return COLOR_RGB;
		if (x == "COLOR_RGBA") return COLOR_RGBA;
		if (x == "NON_COLOR_R") return NON_COLOR_R;
		if (x == "NON_COLOR_RG") return NON_COLOR_RG;
		if (x == "NON_COLOR_RGB") return NON_COLOR_RGB;
		if (x == "NON_COLOR_RGBA") return NON_COLOR_RGBA;

		if (x == "COLOR_R_16F") return COLOR_R_16F;
		if (x == "COLOR_R_32F") return COLOR_R_32F;
		if (x == "COLOR_RG_16F") return COLOR_RG_16F;
		if (x == "COLOR_RG_32F") return COLOR_RG_32F;
		if (x == "COLOR_RGB_16F") return COLOR_RGB_16F;
		if (x == "COLOR_RGB_32F") return COLOR_RGB_32F;
		if (x == "COLOR_RGBA_16F") return COLOR_RGBA_16F;
		if (x == "COLOR_RGBA_32F") return COLOR_RGBA_32F;
		if (x == "NON_COLOR_R_16F") return NON_COLOR_R_16F;
		if (x == "NON_COLOR_R_32F") return NON_COLOR_R_32F;
		if (x == "NON_COLOR_RG_16F") return NON_COLOR_RG_16F;
		if (x == "NON_COLOR_RG_32F") return NON_COLOR_RG_32F;
		if (x == "NON_COLOR_RGB_16F") return NON_COLOR_RGB_16F;
		if (x == "NON_COLOR_RGB_32F") return NON_COLOR_RGB_32F;
		if (x == "NON_COLOR_RGBA_16F") return NON_COLOR_RGBA_16F;
		if (x == "NON_COLOR_RGBA_32F") return NON_COLOR_RGBA_32F;

		if (x == "DEPTH_F32") return DEPTH_F32;
		if (x == "DEPTH_F16") return DEPTH_F16;
		if (x == "DEPTH_UNORM") return DEPTH_UNORM;
		if (x == "DEPTH_F32_STENCIL_8") return DEPTH_F32_STENCIL_8;
		if (x == "DEPTH_F16_STENCIL_8") return DEPTH_F16_STENCIL_8;
		if (x == "DEPTH_UNORM_STENCIL_8") return DEPTH_UNORM_STENCIL_8;

		return COLOR_RGBA;
	}

	static ShaderModuleStage resolveStage(const std::string& x) {

		if (x == "FRAGMENT") return ShaderModuleStage::FRAGMENT;
		if (x == "VERTEX") return ShaderModuleStage::VERTEX;
		if (x == "COMPUTE") return ShaderModuleStage::COMPUTE;
		if (x == "ALL") return ShaderModuleStage::ALL;
		return ShaderModuleStage::ALL;
	}

	static VertexFormat resolveVertexFormat(const std::string& x) {
		if (x == "XY_UINT") return XY_UINT;
		if (x == "XYZ_UINT") return XYZ_UINT;
		if (x == "XYZW_UINT") return XYZW_UINT;

		if (x == "XY_INT") return XY_INT;
		if (x == "XYZ_INT") return XYZ_INT;
		if (x == "XYZW_INT") return XYZW_INT;

		if (x == "XY_FLOAT") return XY_FLOAT;
		if (x == "XYZ_FLOAT") return XYZ_FLOAT;
		if (x == "XYZW_FLOAT") return XYZW_FLOAT;


		if (x == "XYZW_FLOAT16") return XYZW_FLOAT16;
		if (x == "XYZW_INT16") return XYZW_INT16;
		if (x == "XYZW_UINT16") return XYZW_UINT16;

		return VERTEX_FORMAT_SIZE;
	}


}