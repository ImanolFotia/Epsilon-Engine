#pragma once
#include <string>
#include <iostream>
#include <tiny_gltf.h>
#include <core/common/common.hpp>
#include "model.h"
namespace framework {
	class gltfModel : public ModelBase {

		template<typename T>
		struct IndexArrayStorage {
			using type = T;
			IndexArrayStorage(const unsigned char* b, const size_t c, const size_t s) : bytes(b), count(c), stride(s) {}
			const unsigned char* bytes;
			const size_t count;
			const size_t stride;

			T operator[](size_t pos) {
				if (pos >= count) throw std::out_of_range(__PRETTY_FUNCTION__);
				return *(reinterpret_cast<const T*>(bytes + pos * stride));
			}
		};

		struct IndexArrayPtrBase {
			virtual uint32_t operator[](size_t pos) = 0;
			virtual size_t size() = 0;
		};

		template<typename T>
		struct IndexArrayPtr : IndexArrayPtrBase {
			IndexArrayStorage<T> storage;

			IndexArrayPtr(const IndexArrayStorage<T>& s) : storage(s) {}

			uint32_t operator[](size_t pos) override {
				return static_cast<uint32_t>(storage[pos]);
			}

			virtual size_t size() override {
				return storage.count;
			}
		};

	public:
		gltfModel(const std::string& path = "") : ModelBase(path) {
			Load(path);
		}

		bool Load(const std::string& path) {

			tinygltf::Model model;
			tinygltf::TinyGLTF loader;
			std::string err;
			std::string warn;
			const std::string ext = path.substr(path.find_last_of('.') + 1, path.length());//GetFilePathExtension(path);

			bool ret = false;
			if (ext.compare("glb") == 0) {
				// assume binary glTF.
				ret = loader.LoadBinaryFromFile(&model, &err, &warn, path.c_str());
			}
			else {
				// assume ascii glTF.
				ret = loader.LoadASCIIFromFile(&model, &err, &warn, path.c_str());
			}

			if (!warn.empty()) {
				std::cout << "glTF parse warning: " << warn << std::endl;
			}

			if (!err.empty()) {
				std::cerr << "glTF parse error: " << err << std::endl;
			}
			if (!ret) {
				std::cerr << "Failed to load glTF: " << path << std::endl;
				return false;
			}

			std::cout << "loaded glTF file has:\n"
				<< model.accessors.size() << " accessors\n"
				<< model.animations.size() << " animations\n"
				<< model.buffers.size() << " buffers\n"
				<< model.bufferViews.size() << " bufferViews\n"
				<< model.materials.size() << " materials\n"
				<< model.meshes.size() << " meshes\n"
				<< model.nodes.size() << " nodes\n"
				<< model.textures.size() << " textures\n"
				<< model.images.size() << " images\n"
				<< model.skins.size() << " skins\n"
				<< model.samplers.size() << " samplers\n"
				<< model.cameras.size() << " cameras\n"
				<< model.scenes.size() << " scenes\n"
				<< model.lights.size() << " lights\n";
			this->mMeshes.resize(model.meshes.size());
			unsigned index = 0;
			for (const auto& gltfMesh : model.meshes) {
				auto& currentMesh = this->mMeshes.at(index);
				for (const auto& meshPrimitive : gltfMesh.primitives) {
					{
						const auto& indicesAccessor = model.accessors[meshPrimitive.indices];
						const auto& bufferView = model.bufferViews[indicesAccessor.bufferView];
						const auto& buffer = model.buffers[bufferView.buffer];
						const auto dataAddress = buffer.data.data() + bufferView.byteOffset + indicesAccessor.byteOffset;
						const auto byteStride = indicesAccessor.ByteStride(bufferView);
						const auto count = indicesAccessor.count;

						std::unique_ptr<IndexArrayPtrBase> indexArrayPtr = nullptr;

						switch (indicesAccessor.componentType) {
						case TINYGLTF_COMPONENT_TYPE_BYTE:
							indexArrayPtr =
								std::unique_ptr<IndexArrayPtr<char>>(new IndexArrayPtr<char>( IndexArrayStorage<char>( dataAddress, count, byteStride)));
							break;

						case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
							indexArrayPtr = std::unique_ptr<IndexArrayPtr<unsigned char> >(new IndexArrayPtr<unsigned char>(IndexArrayStorage< unsigned char>(dataAddress, count, byteStride)));
							break;

						case TINYGLTF_COMPONENT_TYPE_SHORT:
							indexArrayPtr = std::unique_ptr<IndexArrayPtr<short> >(new IndexArrayPtr<short>(IndexArrayStorage<short>(dataAddress, count, byteStride)));
							break;

						case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
							indexArrayPtr = std::unique_ptr<IndexArrayPtr<unsigned short> >(new IndexArrayPtr<unsigned short>(IndexArrayStorage<unsigned short>(dataAddress, count, byteStride)));
							break;

						case TINYGLTF_COMPONENT_TYPE_INT:
							indexArrayPtr = std::unique_ptr<IndexArrayPtr<int> >(new IndexArrayPtr<int>(IndexArrayStorage<int>(dataAddress, count, byteStride)));
							break;

						case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
							indexArrayPtr = std::unique_ptr<IndexArrayPtr<unsigned int> >(new IndexArrayPtr<unsigned int>(IndexArrayStorage<unsigned int>(dataAddress, count, byteStride)));
							break;
						default:
							break;
						}

						for (int i = 0; i < indexArrayPtr->size() ; i++) {
							currentMesh.data().mesh.Indices.push_back((*indexArrayPtr)[i]);
						}

					}


				}
			}
			/*
			// Iterate through all the meshes in the glTF file
			for (const auto& gltfMesh : model.meshes) {
				std::cout << "Current mesh has " << gltfMesh.primitives.size()
					<< " primitives:\n";

				// Create a mesh object
				Mesh<float> loadedMesh(sizeof(float) * 3);

				// To store the min and max of the buffer (as 3D vector of floats)
				v3f pMin = {}, pMax = {};

				// Store the name of the glTF mesh (if defined)
				loadedMesh.name = gltfMesh.name;
			}*/

		}
	};
}