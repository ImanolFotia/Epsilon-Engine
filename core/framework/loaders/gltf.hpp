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
			const unsigned char* bytes;
			const size_t count = 0;
			const size_t stride = 0;
			using type = T;
			IndexArrayStorage(const unsigned char* b, const size_t c, const size_t s) : bytes(b), count(c), stride(s) {}

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
		struct IndexArrayPtr : public IndexArrayPtrBase {
			IndexArrayStorage<T> storage;

			IndexArrayPtr(const IndexArrayStorage<T>& s) : storage(s) {}

			uint32_t operator[](size_t pos) override {
				return static_cast<uint32_t>(storage[pos]);
			}

			size_t size() override {
				return storage.count;
			}
		};

		template<typename T>
		struct VertexArrayStorage {
			using type = T;
			VertexArrayStorage(const unsigned char* b, const size_t c, const size_t s) : bytes(b), count(c), stride(s) {}
			const unsigned char* bytes;
			const size_t count = 0;
			const size_t stride = 0;

			T operator[](size_t pos) {
				if (pos >= count)
					throw std::out_of_range(__PRETTY_FUNCTION__);
				return *(reinterpret_cast<const T*>(bytes + pos * stride));
			}
		};

		struct Vertex3DArrayPtrBase {
			virtual glm::vec3 operator[](size_t pos) = 0;
			virtual size_t size() = 0;
		};

		struct Vertex4DArrayPtrBase {
			virtual glm::lowp_u16vec4 operator[](size_t pos) = 0;
			virtual size_t size() = 0;
		};

		struct Vertex2DArrayPtrBase {
			virtual glm::vec2 operator[](size_t pos) = 0;
			virtual size_t size() = 0;
		};

		template<typename T>
		struct Vertex4DArrayPtr : public Vertex4DArrayPtrBase {
			VertexArrayStorage<T> storage;

			Vertex4DArrayPtr(const VertexArrayStorage<T>& s) : storage(s) {}

			glm::lowp_u16vec4 operator[](size_t pos) override {
				if (pos >= storage.count)
					throw std::out_of_range(__PRETTY_FUNCTION__);
				return static_cast<glm::lowp_u16vec4>(storage[pos]);
			}

			virtual size_t size() override {
				return storage.count;
			}
		};

		template<typename T>
		struct Vertex3DArrayPtr : public Vertex3DArrayPtrBase {
			VertexArrayStorage<T> storage;

			Vertex3DArrayPtr(const VertexArrayStorage<T>& s) : storage(s) {}

			glm::vec3 operator[](size_t pos) override {
				if (pos >= storage.count)
					throw std::out_of_range(__PRETTY_FUNCTION__);
				return static_cast<glm::vec3>(storage[pos]);
			}

			virtual size_t size() override {
				return storage.count;
			}
		};
		template<typename T>
		struct Vertex2DArrayPtr : Vertex2DArrayPtrBase {
			VertexArrayStorage<T> storage;

			Vertex2DArrayPtr(const VertexArrayStorage<T>& s) : storage(s) {}

			glm::vec2 operator[](size_t pos) override {
				return static_cast<glm::vec2>(storage[pos]);
			}

			virtual size_t size() override {
				return storage.count;
			}
		};


		struct temp_mesh {
			bool hasMesh = false;
			tinygltf::Mesh mesh;
			glm::mat4 transform = glm::mat4(1.0f);
		};

		std::vector<temp_mesh> meshes;

		void parse_nodes(tinygltf::Model& model, tinygltf::Node node, glm::mat4 parentTransform) {
			temp_mesh m;
			if (node.mesh != -1) {
				m.hasMesh = true;
				m.mesh = model.meshes[node.mesh];
			}
			else {
				m.hasMesh = false;

			}
			m.transform = glm::mat4(1.0f);
			glm::mat4 rot_matrix = glm::mat4(1.0f);
			glm::mat4 scale_matrix = glm::mat4(1.0f);
			glm::mat4 translation_matrix = glm::mat4(1.0f);

			if (node.matrix.size() > 0) {
				for (int i = 0; i < 4; i++) {
					for (int j = 0; j < 4; j++) {
						m.transform[i][j] = node.matrix[i * 4 + j];
					}
				}
			}
			else {
				if (node.translation.size() > 0) {
					glm::vec3 p;
					for (int i = 0; i < 3; i++) p[i] = node.translation[i];
					translation_matrix = glm::translate(glm::mat4(1.0), p);
				}

				if (node.scale.size() > 0) {
					glm::vec3 s;
					for (int i = 0; i < 3; i++) s[i] = node.scale[i];
					scale_matrix = glm::scale(glm::mat4(1.0), s);
				}

				if (node.rotation.size() > 0) {
					glm::quat r;
					for (int i = 0; i < 4; i++) r[i] = node.rotation[i];
					rot_matrix = glm::toMat4(r);
				}

				m.transform = translation_matrix * rot_matrix * scale_matrix;
			}

			m.transform = parentTransform * m.transform;
			meshes.push_back(m);
			for (const auto& gltfNode : node.children) {
				parse_nodes(model, model.nodes[gltfNode], m.transform);
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

			parse_nodes(model, model.nodes[model.scenes[model.defaultScene].nodes.front()], glm::mat4(1.0f));


			for (const auto& tmpMesh : meshes) {
				if (!tmpMesh.hasMesh) continue;
				const auto& gltfMesh = tmpMesh.mesh;
				const auto meshTransform = tmpMesh.transform;
				auto& currentMesh = this->mMeshes.at(index);
				unsigned currentvOffset = 0;
				unsigned currentiOffset = 0;
				int thisVOffset = 0;
				int thisIOffset = 0;

				for (const auto& meshPrimitive : gltfMesh.primitives) {
					{
						bool convertedToTriangleList = false;
						const auto& indicesAccessor = model.accessors[meshPrimitive.indices];
						const auto& bufferView = model.bufferViews[indicesAccessor.bufferView];
						const auto& buffer = model.buffers[bufferView.buffer];
						const auto dataAddress = buffer.data.data() + bufferView.byteOffset + indicesAccessor.byteOffset;
						const auto byteStride = indicesAccessor.ByteStride(bufferView);
						const auto count = indicesAccessor.count;

						if (meshPrimitive.material != -1) {
							const auto primitiveMaterial = model.materials[meshPrimitive.material];


							thisIOffset += indicesAccessor.count;

							if (primitiveMaterial.pbrMetallicRoughness.baseColorTexture.index != -1)
								currentMesh.Material().albedo = "textures/" + model.images[model.textures[primitiveMaterial.pbrMetallicRoughness.baseColorTexture.index].source].uri;

							if (primitiveMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index != -1)
								currentMesh.Material().metallic = "textures/" + model.images[model.textures[primitiveMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index].source].uri;

							if (primitiveMaterial.normalTexture.index != -1)
								currentMesh.Material().normal = "textures/" + model.images[model.textures[primitiveMaterial.normalTexture.index].source].uri;
						}
						std::unique_ptr<IndexArrayPtrBase> indexArrayPtr = nullptr;

						switch (indicesAccessor.componentType) {
						case TINYGLTF_COMPONENT_TYPE_BYTE:
							indexArrayPtr =
								std::unique_ptr<IndexArrayPtr<char>>(new IndexArrayPtr<char>(IndexArrayStorage<char>(dataAddress, count, byteStride)));
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
						indices.clear();
						for (int i = 0; i < count; i++) {
							currentMesh.data().mesh.Indices.push_back((*indexArrayPtr)[i] + currentvOffset);
							indices.push_back((*indexArrayPtr)[i]);
						}


						std::cout << "TRIANGLES\n";
						auto pos_accesor = model.accessors[meshPrimitive.attributes.at("POSITION")];

						currentMesh.data().mesh.Vertices.resize(pos_accesor.count + currentvOffset);
						switch (meshPrimitive.mode) {
							// We re-arrange the indices so that it describe a simple list of
							// triangles
						case TINYGLTF_MODE_TRIANGLE_FAN:
							if (!convertedToTriangleList) {
								std::cout << "TRIANGLE_FAN\n";
								// This only has to be done once per primitive
								convertedToTriangleList = true;

								// We steal the guts of the vector
								auto triangleFan = std::move(indices);
								//currentMesh.data().mesh.Indices.clear();

								// Push back the indices that describe just one triangle one by one
								for (size_t i{ currentvOffset + 2 }; i < triangleFan.size() + currentvOffset; ++i) {
									currentMesh.data().mesh.Indices[i] = triangleFan[0] + currentvOffset;
									currentMesh.data().mesh.Indices[i + 1] = triangleFan[i - 1] + currentvOffset;
									currentMesh.data().mesh.Indices[i + 2] = triangleFan[i] + currentvOffset;

									currentMesh.data().mesh.Vertices[i].color = glm::vec4(1.0f);
									currentMesh.data().mesh.Vertices[i + 1].color = glm::vec4(1.0f);
									currentMesh.data().mesh.Vertices[i + 2].color = glm::vec4(1.0f);
								}
							}
						case TINYGLTF_MODE_TRIANGLE_STRIP:
							if (!convertedToTriangleList) {
								std::cout << "TRIANGLE_STRIP\n";
								// This only has to be done once per primitive
								convertedToTriangleList = true;

								auto triangleStrip = std::move(indices);
								//currentMesh.data().mesh.Indices.clear();

								for (size_t i{ currentvOffset + 2 }; i < triangleStrip.size() + currentvOffset; ++i) {

									currentMesh.data().mesh.Indices[i] = triangleStrip[i - 2] + currentvOffset;
									currentMesh.data().mesh.Indices[i + 1] = triangleStrip[i - 1] + currentvOffset;
									currentMesh.data().mesh.Indices[i + 2] = triangleStrip[i] + currentvOffset;

									currentMesh.data().mesh.Vertices[i].color = glm::vec4(1.0f);
									currentMesh.data().mesh.Vertices[i + 1].color = glm::vec4(1.0f);
									currentMesh.data().mesh.Vertices[i + 2].color = glm::vec4(1.0f);
								}
							}
						case TINYGLTF_MODE_TRIANGLES:  // this is the simpliest case to handle

						{

							for (const auto& attribute : meshPrimitive.attributes) {
								const auto attribAccessor = model.accessors[attribute.second];
								const auto& bufferView =
									model.bufferViews[attribAccessor.bufferView];
								const auto& buffer = model.buffers[bufferView.buffer];
								const auto dataPtr = buffer.data.data() + bufferView.byteOffset +
									attribAccessor.byteOffset;
								const auto byte_stride = attribAccessor.ByteStride(bufferView);
								const auto count = attribAccessor.count;


								//std::cout << "current attribute has count " << count
								//	<< " and stride " << byte_stride << " bytes\n";

								//std::cout << "attribute string is : " << attribute.first << '\n';
								if (attribute.first == "POSITION") {
									//std::cout << "found position attribute\n";

									// get the position min/max for computing the boundingbox
									/*pMin.x = attribAccessor.minValues[0];
									pMin.y = attribAccessor.minValues[1];
									pMin.z = attribAccessor.minValues[2];
									pMax.x = attribAccessor.maxValues[0];
									pMax.y = attribAccessor.maxValues[1];
									pMax.z = attribAccessor.maxValues[2];*/

									switch (attribAccessor.type) {
									case TINYGLTF_TYPE_VEC3: {
										switch (attribAccessor.componentType) {
										case TINYGLTF_COMPONENT_TYPE_FLOAT:
											std::cout << "Type is FLOAT\n";
											// 3D vector of float
											auto positions = std::unique_ptr<Vertex3DArrayPtr<glm::vec3> >(new Vertex3DArrayPtr<glm::vec3>(VertexArrayStorage< glm::vec3>(dataPtr, count, byte_stride)));

											std::cout << "positions's size : " << positions->size()
												<< '\n';

											for (size_t i{ 0 }; i < positions->size(); ++i) {
												const auto v = (*positions)[i];
												//std::cout << "positions[" << i << "]: (" << v.x << ", "
												//	<< v.y << ", " << v.z << ")\n";

												currentMesh.data().mesh.Vertices[i + currentvOffset].position = glm::vec3(meshTransform * (glm::vec4(v.x, v.y, v.z, 1.0f)));
												currentMesh.data().mesh.Vertices[i + currentvOffset].color = glm::vec4(1.0f);
											}

											thisVOffset += positions->size();
										}
										break;
									case TINYGLTF_COMPONENT_TYPE_DOUBLE: {
										std::cout << "Type is DOUBLE\n";
										switch (attribAccessor.type) {
										case TINYGLTF_TYPE_VEC3: {
											auto positions = std::unique_ptr<Vertex3DArrayPtr<glm::vec3> >(new Vertex3DArrayPtr<glm::vec3>(VertexArrayStorage< glm::vec3>(dataPtr, count, byte_stride)));

											for (size_t i{ 0 }; i < positions->size(); ++i) {
												const auto v = (*positions)[i];
												//std::cout << "positions[" << i << "]: (" << v.x
												//	<< ", " << v.y << ", " << v.z << ")\n";

												currentMesh.data().mesh.Vertices[i + currentvOffset].position = glm::vec3(meshTransform * (glm::vec4(v.x, v.y, v.z, 1.0f)));
											}
											thisVOffset += positions->size();
										} break;
										default:
											// TODO Handle error
											break;
										}
										break;
									default:
										break;
									}
									} break;
									}
								}

								if (attribute.first == "NORMAL") {
									std::cout << "found normal attribute\n";

									switch (attribAccessor.type) {
									case TINYGLTF_TYPE_VEC3: {
										std::cout << "Normal is VEC3\n";
										switch (attribAccessor.componentType) {
										case TINYGLTF_COMPONENT_TYPE_FLOAT: {
											std::cout << "Normal is FLOAT\n";
											auto normals = std::unique_ptr<Vertex3DArrayPtr<glm::vec3> >(new Vertex3DArrayPtr<glm::vec3>(VertexArrayStorage< glm::vec3>(dataPtr, count, byte_stride)));


											// IMPORTANT: We need to reorder normals (and texture
											// coordinates into "facevarying" order) for each face

											// For each triangle :

											for (size_t i{ 0 }; i < indices.size() / 3; ++i) {
												// get the i'th triange's indexes
												auto f0 = indices[3 * i + 0];
												auto f1 = indices[3 * i + 1];
												auto f2 = indices[3 * i + 2];

												// get the 3 normal vectors for that face
												glm::vec3 n0, n1, n2;
												n0 = (*normals)[f0];
												n1 = (*normals)[f1];
												n2 = (*normals)[f2];

												// Put them in the array in the correct order
												currentMesh.data().mesh.Vertices[f0 + currentvOffset].normal = glm::vec3(glm::mat3(meshTransform) * glm::vec4(n0, 1.0f));
												currentMesh.data().mesh.Vertices[f1 + currentvOffset].normal = glm::vec3(glm::mat3(meshTransform) * glm::vec4(n1, 1.0f));
												currentMesh.data().mesh.Vertices[f2 + currentvOffset].normal = glm::vec3(glm::mat3(meshTransform) * glm::vec4(n2, 1.0f));

											}
										} break;
										case TINYGLTF_COMPONENT_TYPE_DOUBLE: {
											std::cout << "Normal is DOUBLE\n";
											auto normals = std::unique_ptr<Vertex3DArrayPtr<glm::vec3> >(new Vertex3DArrayPtr<glm::vec3>(VertexArrayStorage< glm::vec3>(dataPtr, count, byte_stride)));


											// IMPORTANT: We need to reorder normals (and texture
											// coordinates into "facevarying" order) for each face

											// For each triangle :
											for (size_t i{ 0 }; i < indices.size() / 3; ++i) {
												// get the i'th triange's indexes
												auto f0 = indices[3 * i + 0];
												auto f1 = indices[3 * i + 1];
												auto f2 = indices[3 * i + 2];

												// get the 3 normal vectors for that face
												glm::vec3 n0, n1, n2;
												n0 = (*normals)[f0];
												n1 = (*normals)[f1];
												n2 = (*normals)[f2];

												// Put them in the array in the correct order

												currentMesh.data().mesh.Vertices[f0 + currentvOffset].normal = glm::vec3(glm::mat3(meshTransform) * glm::vec4(n0, 1.0f));
												currentMesh.data().mesh.Vertices[f1 + currentvOffset].normal = glm::vec3(glm::mat3(meshTransform) * glm::vec4(n1, 1.0f));
												currentMesh.data().mesh.Vertices[f2 + currentvOffset].normal = glm::vec3(glm::mat3(meshTransform) * glm::vec4(n2, 1.0f));
											}
										} break;
										default:
											std::cerr << "Unhandeled componant type for normal\n";
										}
									} break;
									default:
										std::cerr << "Unhandeled vector type for normal\n";
									}
								}

								// Face varying comment on the normals is also true for the UVs
								if (attribute.first == "TEXCOORD_0") {
									std::cout << "Found texture coordinates\n";

									switch (attribAccessor.type) {
									case TINYGLTF_TYPE_VEC2: {
										std::cout << "TEXTCOORD is VEC2\n";
										switch (attribAccessor.componentType) {
										case TINYGLTF_COMPONENT_TYPE_FLOAT: {
											std::cout << "TEXTCOORD is FLOAT\n";

											auto uvs = std::unique_ptr<Vertex2DArrayPtr<glm::vec2> >(new Vertex2DArrayPtr<glm::vec2>(VertexArrayStorage< glm::vec2>(dataPtr, count, byte_stride)));


											for (size_t i{ 0 }; i < indices.size() / 3; ++i) {
												// get the i'th triange's indexes
												auto f0 = indices[3 * i + 0];
												auto f1 = indices[3 * i + 1];
												auto f2 = indices[3 * i + 2];

												// get the texture coordinates for each triangle's
												// vertices
												glm::vec2 uv0, uv1, uv2;
												uv0 = (*uvs)[f0];
												uv1 = (*uvs)[f1];
												uv2 = (*uvs)[f2];

												// push them in order into the mesh data
												currentMesh.data().mesh.Vertices[f0 + currentvOffset].texCoords = glm::vec2(uv0);
												currentMesh.data().mesh.Vertices[f1 + currentvOffset].texCoords = glm::vec2(uv1);
												currentMesh.data().mesh.Vertices[f2 + currentvOffset].texCoords = glm::vec2(uv2);

											}

										} break;
										case TINYGLTF_COMPONENT_TYPE_DOUBLE: {
											std::cout << "TEXTCOORD is DOUBLE\n";

											auto uvs = std::unique_ptr<Vertex2DArrayPtr<glm::dvec2> >(new Vertex2DArrayPtr<glm::dvec2>(VertexArrayStorage< glm::dvec2>(dataPtr, count, byte_stride)));


											for (size_t i{ 0 }; i < indices.size() / 3; ++i) {
												// get the i'th triange's indexes
												auto f0 = indices[3 * i + 0];
												auto f1 = indices[3 * i + 1];
												auto f2 = indices[3 * i + 2];

												glm::vec2 uv0, uv1, uv2;
												uv0 = (*uvs)[f0];
												uv1 = (*uvs)[f1];
												uv2 = (*uvs)[f2];
												currentMesh.data().mesh.Vertices[f0 + currentvOffset].texCoords = glm::dvec2(uv0);
												currentMesh.data().mesh.Vertices[f1 + currentvOffset].texCoords = glm::dvec2(uv1);
												currentMesh.data().mesh.Vertices[f2 + currentvOffset].texCoords = glm::dvec2(uv2);
											}
										} break;
										default:
											std::cerr << "unrecognized vector type for UV";
										}
									} break;
									default:
										std::cerr << "unreconized componant type for UV";
									}
								}

								// Face varying comment on the normals is also true for the UVs
								if (attribute.first == "COLOR_0") {
									std::cout << "Found color\n";

									switch (attribAccessor.type) {
									case TINYGLTF_TYPE_VEC4: {
										std::cout << "COLOR_0 is VEC4\n";
										switch (attribAccessor.componentType) {
										case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
											std::cout << "COLOR_0 is TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT\n";

											auto color = std::unique_ptr<Vertex4DArrayPtr<glm::lowp_u16vec4> >(new Vertex4DArrayPtr<glm::lowp_u16vec4>(VertexArrayStorage< glm::lowp_u16vec4>(dataPtr, count, byte_stride)));


											for (size_t i{ 0 }; i < indices.size() / 3; ++i) {
												// get the i'th triange's indexes
												auto f0 = indices[3 * i + 0];
												auto f1 = indices[3 * i + 1];
												auto f2 = indices[3 * i + 2];

												// get the texture coordinates for each triangle's
												// vertices
												glm::vec4 color0, color1, color2;
												color0 = (*color)[f0];
												color1 = (*color)[f1];
												color2 = (*color)[f2];


												color0 = color0 / glm::vec4(65535);
												color1 = color1 / glm::vec4(65535);
												color2 = color2 / glm::vec4(65535);

												color0.a = 1.0;
												color1.a = 1.0;
												color2.a = 1.0;

												// push them in order into the mesh data
												currentMesh.data().mesh.Vertices[f0 + currentvOffset].color = glm::vec4(color0);
												currentMesh.data().mesh.Vertices[f1 + currentvOffset].color = glm::vec4(color1);
												currentMesh.data().mesh.Vertices[f2 + currentvOffset].color = glm::vec4(color2);

											}

										} break;
										case TINYGLTF_COMPONENT_TYPE_DOUBLE: {
											std::cout << "COLOR_0 is DOUBLE\n";

											auto color = std::unique_ptr<Vertex4DArrayPtr<glm::dvec4> >(new Vertex4DArrayPtr<glm::dvec4>(VertexArrayStorage< glm::dvec4>(dataPtr, count, byte_stride)));


											for (size_t i{ 0 }; i < indices.size() / 3; ++i) {
												// get the i'th triange's indexes
												auto f0 = indices[3 * i + 0];
												auto f1 = indices[3 * i + 1];
												auto f2 = indices[3 * i + 2];

												glm::vec4 color0, color1, color2;
												color0 = (*color)[f0];
												color1 = (*color)[f1];
												color2 = (*color)[f2];
												color0.a = 1.0;
												color1.a = 1.0;
												color2.a = 1.0;
												currentMesh.data().mesh.Vertices[f0 + currentvOffset].color = glm::dvec4(color0);
												currentMesh.data().mesh.Vertices[f1 + currentvOffset].color = glm::dvec4(color1);
												currentMesh.data().mesh.Vertices[f2 + currentvOffset].color = glm::dvec4(color2);
											}
										} break;
										default:
											std::cerr << "unrecognized vector type for color\n";
										}
									} break;
									default:
										std::cerr << "unreconized componant type for color\n";
									}
								}
							}
						}
						break;

						default:
							std::cerr << "primitive mode not implemented";
							break;

							// These aren't triangles:
						case TINYGLTF_MODE_POINTS:
						case TINYGLTF_MODE_LINE:
						case TINYGLTF_MODE_LINE_LOOP:
							std::cerr << "primitive is not triangle based, ignoring";
						}


					}

					//currentMesh.data().mesh.Indices = indices;
					//currentMesh.data().mesh.Vertices = indices;

					currentiOffset = thisIOffset;
					currentvOffset = thisVOffset;
				}
				currentMesh.generateTangentSpaceVectors();
				auto& meshData = currentMesh.data();
				meshData = {
					.numVertices = meshData.mesh.Vertices.size(),
					.numIndices = meshData.mesh.Indices.size(),
					.vertexOffset = currentvOffset,
					.indexOffset = currentiOffset,
					.mesh = { meshData.mesh.Vertices, meshData.mesh.Indices}
				};
				index++;
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

		std::vector<unsigned int> indices{};
	};
}