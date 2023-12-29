#pragma once
#include <string>
#include <iostream>
#include <tiny_gltf.h>
#include <core/common/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include "model.h"
// #include <core/framework/vfs/filesystem.hpp>

namespace framework
{

	class gltfModel : public ModelBase
	{

		template <typename T>
		struct IndexArrayStorage
		{
			const unsigned char *bytes;
			const size_t count = 0;
			const size_t stride = 0;
			using type = T;
			IndexArrayStorage(const unsigned char *b, const size_t c, const size_t s) : bytes(b), count(c), stride(s) {}

			T operator[](size_t pos)
			{
				if (pos >= count)
					throw std::out_of_range(__PRETTY_FUNCTION__);
				return *(reinterpret_cast<const T *>(bytes + pos * stride));
			}
		};

		struct IndexArrayPtrBase
		{
			virtual uint32_t operator[](size_t pos) = 0;
			virtual size_t size() = 0;
		};

		template <typename T>
		struct IndexArrayPtr : public IndexArrayPtrBase
		{
			IndexArrayStorage<T> storage;

			IndexArrayPtr(const IndexArrayStorage<T> &s) : storage(s) {}

			uint32_t operator[](size_t pos) override
			{
				return static_cast<uint32_t>(storage[pos]);
			}

			size_t size() override
			{
				return storage.count;
			}
		};

		template <typename T>
		struct VertexArrayStorage
		{
			using type = T;
			VertexArrayStorage(const unsigned char *b, const size_t c, const size_t s) : bytes(b), count(c), stride(s) {}
			const unsigned char *bytes;
			const size_t count = 0;
			const size_t stride = 0;

			T operator[](size_t pos)
			{
				if (pos >= count)
					throw std::out_of_range(__PRETTY_FUNCTION__);
				return *(reinterpret_cast<const T *>(bytes + pos * stride));
			}
		};

		struct Vertex3DArrayPtrBase
		{
			virtual glm::vec3 operator[](size_t pos) = 0;
			virtual size_t size() = 0;
		};

		struct Vertex4DArrayPtrBase
		{
			virtual glm::vec4 operator[](size_t pos) = 0;
			virtual size_t size() = 0;
		};

		struct Vertex2DArrayPtrBase
		{
			virtual glm::vec2 operator[](size_t pos) = 0;
			virtual size_t size() = 0;
		};

		struct UnsignedVertex4DArrayPtrBase
		{
			virtual glm::lowp_u16vec4 operator[](size_t pos) = 0;
			virtual size_t size() = 0;
		};

		template <typename T>
		struct UnsignedVertex4DArrayPtr : public UnsignedVertex4DArrayPtrBase
		{
			VertexArrayStorage<T> storage;

			UnsignedVertex4DArrayPtr(const VertexArrayStorage<T> &s) : storage(s) {}

			glm::lowp_u16vec4 operator[](size_t pos) override
			{
				if (pos >= storage.count)
					throw std::out_of_range(__PRETTY_FUNCTION__);
				return static_cast<glm::lowp_u16vec4>(storage[pos]);
			}

			virtual size_t size() override
			{
				return storage.count;
			}
		};

		template <typename T>
		struct Vertex4DArrayPtr : public Vertex4DArrayPtrBase
		{
			VertexArrayStorage<T> storage;

			Vertex4DArrayPtr(const VertexArrayStorage<T> &s) : storage(s) {}

			glm::vec4 operator[](size_t pos) override
			{
				if (pos >= storage.count)
					throw std::out_of_range(__PRETTY_FUNCTION__);
				return static_cast<glm::vec4>(storage[pos]);
			}

			virtual size_t size() override
			{
				return storage.count;
			}
		};

		template <typename T>
		struct Vertex3DArrayPtr : public Vertex3DArrayPtrBase
		{
			VertexArrayStorage<T> storage;

			Vertex3DArrayPtr(const VertexArrayStorage<T> &s) : storage(s) {}

			glm::vec3 operator[](size_t pos) override
			{
				if (pos >= storage.count)
					throw std::out_of_range(__PRETTY_FUNCTION__);
				return static_cast<glm::vec3>(storage[pos]);
			}

			virtual size_t size() override
			{
				return storage.count;
			}
		};
		template <typename T>
		struct Vertex2DArrayPtr : Vertex2DArrayPtrBase
		{
			VertexArrayStorage<T> storage;

			Vertex2DArrayPtr(const VertexArrayStorage<T> &s) : storage(s) {}

			glm::vec2 operator[](size_t pos) override
			{
				return static_cast<glm::vec2>(storage[pos]);
			}

			virtual size_t size() override
			{
				return storage.count;
			}
		};

		struct temp_mesh
		{
			bool hasMesh = false;
			tinygltf::Mesh mesh;
			int node_index = 0;
			glm::mat4 transform = glm::mat4(1.0f);
		};

		std::vector<temp_mesh> meshes;

		glm::mat4 getTransformFromNode(tinygltf::Node &node)
		{
			glm::mat4 result = glm::mat4(1.0f);
			if (node.matrix.size() > 0)
			{
				for (int i = 0; i < 4; i++)
				{
					for (int j = 0; j < 4; j++)
					{
						result[i][j] = node.matrix[i * 4 + j];
					}
				}
			}
			else
			{
				glm::mat4 translation_matrix = glm::mat4(1.0f), scale_matrix = glm::mat4(1.0f), rot_matrix = glm::mat4(1.0f);
				if (node.translation.size() > 0)
				{
					glm::vec3 p;
					for (int i = 0; i < 3; i++)
						p[i] = node.translation[i];
					translation_matrix = glm::translate(glm::mat4(1.0f), p);
				}

				if (node.scale.size() > 0)
				{
					glm::vec3 s;
					for (int i = 0; i < 3; i++)
						s[i] = node.scale[i];
					scale_matrix = glm::scale(glm::mat4(1.0f), s);
				}

				if (node.rotation.size() > 0)
				{
					glm::quat r;
					for (int i = 0; i < 4; i++)
						r[3 - i] = node.rotation[i];
					rot_matrix = glm::mat4(glm::normalize(r));
				}

				result = translation_matrix * rot_matrix * scale_matrix;
			}

			return result;
		}

		std::unordered_map<int, int> seen_meshes;

		void parse_nodes(tinygltf::Model &model, tinygltf::Node node, glm::mat4 parentTransform)
		{
			temp_mesh m;
			if (node.mesh != -1)
			{
				if (seen_meshes.contains(node.mesh))
				{
					m.hasMesh = false;
				}
				else
				{
					seen_meshes[node.mesh]++;
					m.hasMesh = true;
					m.mesh = model.meshes[node.mesh];
				}
			}
			else
			{
				m.hasMesh = false;
			}
			m.transform = glm::mat4(1.0f);
			glm::mat4 rot_matrix = glm::mat4(1.0f);
			glm::mat4 scale_matrix = glm::mat4(1.0f);
			glm::mat4 translation_matrix = glm::mat4(1.0f);

			m.transform = parentTransform * getTransformFromNode(node);

			meshes.push_back(m);
			for (const auto &gltfNode : node.children)
			{
				seen_nodes.insert(gltfNode);
				parse_nodes(model, model.nodes[gltfNode], m.transform);
			}
		};

		auto changeColumnOrder(glm::mat4 input)
		{
			glm::mat4 output = glm::mat4(1.0);

			output[0][0] = input[0][0];
			output[1][0] = input[0][1];
			output[2][0] = input[0][2];
			output[3][0] = input[0][3];
			output[0][1] = input[1][0];
			output[1][1] = input[1][1];
			output[2][1] = input[1][2];
			output[3][1] = input[1][3];
			output[0][2] = input[2][0];
			output[1][2] = input[2][1];
			output[2][2] = input[2][2];
			output[3][2] = input[2][3];
			output[0][3] = input[3][0];
			output[1][3] = input[3][1];
			output[2][3] = input[3][2];
			output[3][3] = input[3][3];

			return output;
		}

		void parse_skeleton(tinygltf::Model &model, tinygltf::Node node, SkeletonNode &anim_node, glm::mat4 parentTransform)
		{
			anim_node.transform = anim_node.transform * getTransformFromNode(node);
			for (const auto &child : node.children)
			{
				if (m_pSkeleton.Joints.contains(local_nodes[child].name))
				{
					anim_node.children.push_back(local_nodes[child].name);
					parse_skeleton(model, local_nodes[child], m_pSkeleton.Nodes[local_nodes[child].name], anim_node.transform);
				}
			}
		}

		void parse_samplers(tinygltf::Model &model, tinygltf::Animation &animation, const framework::Skeleton &skeleton, int anim_index)
		{
			Animation &current_animation = m_pAnimations.emplace_back();
			current_animation.name = animation.name;
			current_animation.index = anim_index;
			current_animation.samplers.resize(animation.samplers.size());
			for (int i = 0; i < animation.samplers.size(); i++)
			{
				AnimationSampler &sampler = current_animation.samplers[i];

				if (animation.samplers[i].interpolation == "LINEAR")
				{
					sampler.interpolation = ANIMATION_INTERPOLATION::LINEAR;
				}
				else if (animation.samplers[i].interpolation == "STEP")
				{
					sampler.interpolation = ANIMATION_INTERPOLATION::STEP;
				}
				else if (animation.samplers[i].interpolation == "CATMULLROMSPLINE")
				{
					sampler.interpolation = ANIMATION_INTERPOLATION::CATMULLROMSPLINE;
				}
				else if (animation.samplers[i].interpolation == "CUBICSPLINE")
				{
					sampler.interpolation = ANIMATION_INTERPOLATION::CUBICSPLINE;
				}

				{
					auto inputAccesor = model.accessors[animation.samplers[i].input];
					const auto &bufferView = model.bufferViews[inputAccesor.bufferView];
					const auto &buffer = model.buffers[bufferView.buffer];
					const auto dataAddress = buffer.data.data() + bufferView.byteOffset + inputAccesor.byteOffset;
					const auto byteStride = inputAccesor.ByteStride(bufferView);

					sampler.inputs.resize(inputAccesor.count);
					for (int i = 0; i < inputAccesor.count; i++)
						sampler.inputs[i] = *(reinterpret_cast<const float *>(dataAddress + i * byteStride));
					float max_t = 0.0f, min_t = 9999999.0f;
					for (auto input : sampler.inputs)
					{
						if (input < min_t)
						{
							min_t = input;
						};
						if (input > max_t)
						{
							max_t = input;
						}
					}
					current_animation.start = min_t;
					current_animation.end = max_t;
					current_animation.duration = current_animation.end - current_animation.start;
				}

				{
					auto outputAccesor = model.accessors[animation.samplers[i].output];
					const auto &bufferView = model.bufferViews[outputAccesor.bufferView];
					const auto &buffer = model.buffers[bufferView.buffer];
					const void *dataAddress = &buffer.data[bufferView.byteOffset + outputAccesor.byteOffset];
					const auto byteStride = outputAccesor.ByteStride(bufferView);

					sampler.outputs.resize(outputAccesor.count);
					for (int j = 0; j < outputAccesor.count; j++)
					{
						switch (outputAccesor.type)
						{
						case TINYGLTF_TYPE_VEC3:
						{
							const glm::vec3 *buf = static_cast<const glm::vec3 *>(dataAddress);
							sampler.outputs[j] = glm::vec4(buf[j], 0.0);
							break;
						}
						case TINYGLTF_TYPE_VEC4:
							const glm::vec4 *buf = static_cast<const glm::vec4 *>(dataAddress);
							sampler.outputs[j] = glm::vec4(buf[j].w, buf[j].x, buf[j].y, buf[j].z);
							break;
						}
					}
				}
			}

			current_animation.channels.resize(animation.channels.size());
			for (size_t j = 0; j < animation.channels.size(); j++)
			{
				auto &channel = current_animation.channels[j];

				channel.sampler_index = animation.channels[j].sampler;
				channel.target_node = animation.channels[j].target_node;
				channel.target_node_name = model.nodes[animation.channels[j].target_node].name;
				glm::mat4 t = getTransformFromNode(model.nodes[animation.channels[j].target_node]);
				glm::vec3 scale{};
				glm::vec3 position{};
				glm::quat rotation{};
				glm::vec3 skew;
				glm::vec4 perspective;
				glm::decompose(t, scale, rotation, position, skew, perspective);

				if (animation.channels[j].target_path == "translation")
				{
					channel.property = ANIMATION_PROPERTY::TRANSLATION;
					m_pSkeleton.Nodes[channel.target_node_name].hasPosition = true;
				}
				else if (animation.channels[j].target_path == "scale")
				{
					channel.property = ANIMATION_PROPERTY::SCALE;
					m_pSkeleton.Nodes[channel.target_node_name].hasScale = true;
				}
				else if (animation.channels[j].target_path == "rotation")
				{
					channel.property = ANIMATION_PROPERTY::ROTATION;
					m_pSkeleton.Nodes[channel.target_node_name].hasRotation = true;
				}

				if (!m_pSkeleton.Nodes[channel.target_node_name].hasPosition)
					m_pSkeleton.Nodes[channel.target_node_name].translation = glm::translate(glm::mat4(1.0f), position);

				if (!m_pSkeleton.Nodes[channel.target_node_name].hasScale)
					m_pSkeleton.Nodes[channel.target_node_name].scale = glm::scale(glm::mat4(1.0f), scale);

				if (!m_pSkeleton.Nodes[channel.target_node_name].hasRotation)
					m_pSkeleton.Nodes[channel.target_node_name].rotation = glm::mat4_cast(glm::conjugate(glm::normalize(rotation)));

				m_pSkeleton.Nodes[channel.target_node_name].node_matrix = t;
			}
		}

		auto findNodeChildren(const std::string &name) -> std::vector<std::string>
		{
			std::vector<std::string> output;
			for (int i = 0; i < local_nodes.size(); i++)
			{
				if (local_nodes[i].name == name)
				{
					for (auto &child : local_nodes[i].children)
					{
						output.push_back(local_nodes[child].name);
					}
				}
			}

			return output;
		};

		auto findJointChildren(const std::string &name) -> std::vector<std::string>
		{
			std::vector<std::string> output;
			for (int i = 0; i < local_nodes.size(); i++)
			{
				if (local_nodes[i].name == name)
				{
					for (auto &child : local_nodes[i].children)
					{
						bool found = false;
						for (int j = 0; j < model.skins[0].joints.size(); j++)
						{
							if (i == model.skins[0].joints[j])
								found = true;
						}
						if (found)
							output.push_back(local_nodes[child].name);
					}
				}
			}

			return output;
		};

		auto containsNode(const tinygltf::Skin &skin, int index)
		{
			for (int i = 0; i < skin.joints.size(); i++)
			{
				if (skin.joints[i] == index)
				{
					return i;
				}
			}

			return -1;
		};

		auto containsNodeName(const std::string &name)
		{
			for (int i = 0; i < model.nodes.size(); i++)
			{
				if (model.nodes[i].name == name)
				{
					return i;
				}
			}

			return -1;
		};

		void CreateChild(tinygltf::Node &lnode, glm::mat4 parentTransform)
		{
			if (!m_pSkeleton.Nodes.contains(lnode.name))
			{
				SkeletonNode node;
				node.name = lnode.name;
				m_pSkeleton.Nodes[node.name] = node;

				int index = containsNodeName(node.name);
				m_pSkeleton.Nodes[node.name].index = index;

				m_pSkeleton.Nodes[node.name].transform = getTransformFromNode(model.nodes[index]);

				for (int i = 0; i < lnode.children.size(); i++)
				{
					if (lnode.name == local_nodes[lnode.children[i]].name)
					{
						local_nodes[lnode.children[i]].name += std::to_string(i);
						model.nodes[lnode.children[i]].name += std::to_string(i);
					}
					m_pSkeleton.Nodes[node.name].children.push_back(local_nodes[lnode.children[i]].name);
					CreateChild(local_nodes[lnode.children[i]], m_pSkeleton.Nodes[node.name].transform);
				}
			}
		}

		void createTree()
		{
			for (auto &lnode : local_nodes)
			{
				CreateChild(lnode, glm::mat4(1.0f));
			}
		}

		struct ModelNode
		{
			std::string name;
			glm::mat4 transform;
			std::vector<ModelNode> children;
			std::string parent;
		};

		tinygltf::Node RootNode;
		std::vector<tinygltf::Node> local_nodes;

		std::set<int> seen_nodes;

	public:
		gltfModel(const std::string &path = "") : ModelBase(path)
		{
			std::cout << path << std::endl;
			Load(path);
		}

		tinygltf::Model model;

		bool Load(const std::string &path)
		{
			tinygltf::TinyGLTF loader;

			std::string err;
			std::string warn;
			const std::string ext = path.substr(path.find_last_of('.') + 1, path.length()); // GetFilePathExtension(path);

			bool ret = false;
			if (ext.compare("glb") == 0)
			{
				// assume binary glTF.
				ret = loader.LoadBinaryFromFile(&model, &err, &warn, path.c_str());
			}
			else
			{ /*
			std::string name = filename;
			if (filename.starts_with("./")) {
				name = filename.substr(2, filename.size());
			}
			if (filename.starts_with("/")) {
				name = filename.substr(1, filename.size());
			}
			if (bool isPresent = engine::Filesystem::is_present(name); isPresent) {
				std::cout << "found in zip\n";
			}*/
				// assume ascii glTF.
				ret = loader.LoadASCIIFromFile(&model, &err, &warn, path.c_str());
			}

			if (!warn.empty())
			{
				std::cout << "glTF parse warning: " << warn << std::endl;
			}

			if (!err.empty())
			{
				std::cerr << "glTF parse error: " << err << std::endl;
			}
			if (!ret)
			{
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
			// this->mMeshes.resize(model.meshes.size());
			unsigned index = 0;
			unsigned numRootNodes = model.nodes.size();
			std::string root_name;
			if (numRootNodes >= 0)
			{ // a single root node: use it
				RootNode = model.nodes[0];
			}

			root_name = RootNode.name;

			local_nodes = model.nodes;

			if (model.skins.size() > 0)
			{

				m_pHasAnimation = true;
				// mAnimatedMeshes.resize(model.meshes.size());

				auto skin = model.skins[0];
				auto matricesAccesor = model.accessors[skin.inverseBindMatrices];
				const auto &bufferView = model.bufferViews[matricesAccesor.bufferView];
				const auto &buffer = model.buffers[bufferView.buffer];
				const auto dataAddress = buffer.data.data() + bufferView.byteOffset + matricesAccesor.byteOffset;
				const auto byteStride = matricesAccesor.ByteStride(bufferView);

				m_pSkeleton.inverse_matrices.resize(matricesAccesor.count);

				createTree();

				m_pSkeleton.root = root_name;

				for (int i = 0; i < local_nodes.size(); i++)
				{
					if (int index = containsNode(model.skins[0], i); index >= 0)
					{
						m_pSkeleton.Joints[local_nodes[i].name] = m_pSkeleton.Nodes[local_nodes[i].name];
						m_pSkeleton.Joints[local_nodes[i].name].index = index;
						m_pSkeleton.Joints[local_nodes[i].name].children.clear();
					}
				}

				for (auto &[name, joint] : m_pSkeleton.Joints)
				{
					auto children = findJointChildren(name);

					for (auto &child : children)
					{
						if (m_pSkeleton.Joints.contains(child))
							joint.children.push_back(child);
					}
				}

				// try to guess the root node
				std::unordered_map<std::string, bool> names_seen;
				for (auto &[name, node] : m_pSkeleton.Nodes)
				{
					names_seen[name] = false;
				}

				for (auto &[name, node] : m_pSkeleton.Nodes)
				{
					for (auto &child : node.children)
					{
						names_seen[child] = true;
					}
				}

				for (auto &[key, val] : names_seen)
				{
					if (!val)
					{
						m_pSkeleton.root = key;
						int index = containsNodeName(key);
						RootNode = model.nodes[index];
					}
				}

				for (int mat_index = 0; mat_index < matricesAccesor.count; mat_index++)
				{
					m_pSkeleton.inverse_matrices.at(mat_index) = *(reinterpret_cast<const glm::mat4 *>(dataAddress + mat_index * byteStride));
					m_pSkeleton.Joints.at(local_nodes[model.skins[0].joints[mat_index]].name).inverseMatrix = m_pSkeleton.inverse_matrices.at(mat_index);
					m_pSkeleton.Joints.at(local_nodes[model.skins[0].joints[mat_index]].name).index = mat_index;
				}
				m_pSkeleton.globalTransform = getTransformFromNode(RootNode);
				m_pSkeleton.globalInverseTransform = glm::inverse(m_pSkeleton.globalTransform);

				int skeleton_index = model.skins[0].skeleton;
				if (skeleton_index < 0)
					skeleton_index = 0;

				parse_skeleton(model, RootNode, m_pSkeleton.Joints[local_nodes[skin.joints[0]].name], glm::mat4(1.0f));

				for (int anim_index = 0; anim_index < model.animations.size(); anim_index++)
				{
					// parse_animation(model, model.animations[anim_index], m_pSkeleton);
					parse_samplers(model, model.animations[anim_index], m_pSkeleton, anim_index);
				}
			}

			if (model.skins.size() == 0)
			{
				for (int j = 0; j < model.scenes.size(); j++)
				{
					auto &currentScene = model.scenes[j];
					for (int i = 0; i < currentScene.nodes.size(); i++)
					{
						int currentNode = currentScene.nodes[i];
						if (!seen_nodes.contains(currentNode))
						{
							seen_nodes.insert(currentNode);
							parse_nodes(model, model.nodes[currentNode], glm::mat4(1.0f));
						}
						// parse_nodes(model, model.nodes[model.scenes[model.defaultScene].nodes[0]], glm::mat4(1.0f));
					}
				}
			}
			else
			{
				parse_nodes(model, RootNode, glm::mat4(1.0f));
			}
			glm::mat4 parent_transform = glm::mat4(1.0f);

			for (const auto &tmpMesh : meshes)
			{
				if (!tmpMesh.hasMesh)
					continue;
				const auto &gltfMesh = tmpMesh.mesh;
				for (const auto &meshPrimitive : gltfMesh.primitives)
				{
					parent_transform = tmpMesh.transform;
					const auto meshTransform = tmpMesh.transform;
					auto &currentMesh = this->mMeshes.emplace_back(); // at(index);
					currentMesh.setName(tmpMesh.mesh.name);
					mAnimatedMeshes.emplace_back();
					unsigned currentvOffset = 0;
					unsigned currentiOffset = 0;
					int thisVOffset = 0;
					int thisIOffset = 0;

					{
						bool convertedToTriangleList = false;
						const auto &indicesAccessor = model.accessors[meshPrimitive.indices];
						const auto &bufferView = model.bufferViews[indicesAccessor.bufferView];
						const auto &buffer = model.buffers[bufferView.buffer];
						const auto dataAddress = buffer.data.data() + bufferView.byteOffset + indicesAccessor.byteOffset;
						const auto byteStride = indicesAccessor.ByteStride(bufferView);
						const auto count = indicesAccessor.count;

						if (meshPrimitive.material != -1)
						{
							const auto primitiveMaterial = model.materials[meshPrimitive.material];

							thisIOffset += indicesAccessor.count;
							auto baseColor = primitiveMaterial.pbrMetallicRoughness.baseColorFactor;
							currentMesh.Material().color = glm::vec4(baseColor[0], baseColor[1], baseColor[2], baseColor[3]);
							currentMesh.Material().roughness_color = primitiveMaterial.pbrMetallicRoughness.roughnessFactor;
							currentMesh.Material().metallic_color = primitiveMaterial.pbrMetallicRoughness.metallicFactor;
							currentMesh.Material().transmission = currentMesh.Material().color.w;

							if (primitiveMaterial.pbrMetallicRoughness.baseColorTexture.index != -1)
								currentMesh.Material().albedo = "textures/" + model.images[model.textures[primitiveMaterial.pbrMetallicRoughness.baseColorTexture.index].source].uri;

							if (primitiveMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index != -1)
								currentMesh.Material().metallic = "textures/" + model.images[model.textures[primitiveMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index].source].uri;

							if (primitiveMaterial.normalTexture.index != -1)
								currentMesh.Material().normal = "textures/" + model.images[model.textures[primitiveMaterial.normalTexture.index].source].uri;
						}
						std::unique_ptr<IndexArrayPtrBase> indexArrayPtr = nullptr;

						switch (indicesAccessor.componentType)
						{
						case TINYGLTF_COMPONENT_TYPE_BYTE:
							indexArrayPtr =
								std::unique_ptr<IndexArrayPtr<char>>(new IndexArrayPtr<char>(IndexArrayStorage<char>(dataAddress, count, byteStride)));
							break;

						case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
							indexArrayPtr = std::unique_ptr<IndexArrayPtr<unsigned char>>(new IndexArrayPtr<unsigned char>(IndexArrayStorage<unsigned char>(dataAddress, count, byteStride)));
							break;

						case TINYGLTF_COMPONENT_TYPE_SHORT:
							indexArrayPtr = std::unique_ptr<IndexArrayPtr<short>>(new IndexArrayPtr<short>(IndexArrayStorage<short>(dataAddress, count, byteStride)));
							break;

						case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
							indexArrayPtr = std::unique_ptr<IndexArrayPtr<unsigned short>>(new IndexArrayPtr<unsigned short>(IndexArrayStorage<unsigned short>(dataAddress, count, byteStride)));
							break;

						case TINYGLTF_COMPONENT_TYPE_INT:
							indexArrayPtr = std::unique_ptr<IndexArrayPtr<int>>(new IndexArrayPtr<int>(IndexArrayStorage<int>(dataAddress, count, byteStride)));
							break;

						case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
							indexArrayPtr = std::unique_ptr<IndexArrayPtr<unsigned int>>(new IndexArrayPtr<unsigned int>(IndexArrayStorage<unsigned int>(dataAddress, count, byteStride)));
							break;
						default:
							break;
						}
						indices.clear();
						for (int i = 0; i < count; i++)
						{
							currentMesh.data().mesh.Indices.push_back((*indexArrayPtr)[i] + currentvOffset);
							indices.push_back((*indexArrayPtr)[i]);

							if (HasAnimation())
							{
								mAnimatedMeshes.at(index).Indices.push_back((*indexArrayPtr)[i] + currentvOffset);
							}
						}

						std::cout << "TRIANGLES\n";
						auto pos_accesor = model.accessors[meshPrimitive.attributes.at("POSITION")];

						currentMesh.data().mesh.Vertices.resize(pos_accesor.count + currentvOffset);

						if (HasAnimation())
						{

							mAnimatedMeshes.at(index).Vertices.resize(pos_accesor.count + currentvOffset);
						}
						switch (meshPrimitive.mode)
						{
							// We re-arrange the indices so that it describe a simple list of
							// triangles
						case TINYGLTF_MODE_TRIANGLE_FAN:
							if (!convertedToTriangleList)
							{
								std::cout << "TRIANGLE_FAN\n";
								// This only has to be done once per primitive
								convertedToTriangleList = true;

								// We steal the guts of the vector
								auto triangleFan = std::move(indices);
								// currentMesh.data().mesh.Indices.clear();

								// Push back the indices that describe just one triangle one by one
								for (size_t i{currentvOffset + 2}; i < triangleFan.size() + currentvOffset; ++i)
								{
									currentMesh.data().mesh.Indices[i] = triangleFan[0] + currentvOffset;
									currentMesh.data().mesh.Indices[i + 1] = triangleFan[i - 1] + currentvOffset;
									currentMesh.data().mesh.Indices[i + 2] = triangleFan[i] + currentvOffset;

									currentMesh.data().mesh.Vertices[i].color = glm::vec4(1.0f);
									currentMesh.data().mesh.Vertices[i + 1].color = glm::vec4(1.0f);
									currentMesh.data().mesh.Vertices[i + 2].color = glm::vec4(1.0f);
								}
							}
						case TINYGLTF_MODE_TRIANGLE_STRIP:
							if (!convertedToTriangleList)
							{
								std::cout << "TRIANGLE_STRIP\n";
								// This only has to be done once per primitive
								convertedToTriangleList = true;

								auto triangleStrip = std::move(indices);
								// currentMesh.data().mesh.Indices.clear();

								for (size_t i{currentvOffset + 2}; i < triangleStrip.size() + currentvOffset; ++i)
								{

									currentMesh.data().mesh.Indices[i] = triangleStrip[i - 2] + currentvOffset;
									currentMesh.data().mesh.Indices[i + 1] = triangleStrip[i - 1] + currentvOffset;
									currentMesh.data().mesh.Indices[i + 2] = triangleStrip[i] + currentvOffset;

									currentMesh.data().mesh.Vertices[i].color = glm::vec4(1.0f);
									currentMesh.data().mesh.Vertices[i + 1].color = glm::vec4(1.0f);
									currentMesh.data().mesh.Vertices[i + 2].color = glm::vec4(1.0f);
								}
							}
						case TINYGLTF_MODE_TRIANGLES: // this is the simpliest case to handle

						{

							for (const auto &attribute : meshPrimitive.attributes)
							{
								const auto attribAccessor = model.accessors[attribute.second];
								const auto &bufferView =
									model.bufferViews[attribAccessor.bufferView];
								const auto &buffer = model.buffers[bufferView.buffer];
								const auto dataPtr = buffer.data.data() + bufferView.byteOffset +
													 attribAccessor.byteOffset;
								const auto byte_stride = attribAccessor.ByteStride(bufferView);
								const auto count = attribAccessor.count;

								// std::cout << "current attribute has count " << count
								//	<< " and stride " << byte_stride << " bytes\n";

								// std::cout << "attribute string is : " << attribute.first << '\n';
								if (attribute.first == "POSITION")
								{
									// std::cout << "found position attribute\n";

									// get the position min/max for computing the boundingbox
									/*pMin.x = attribAccessor.minValues[0];
									pMin.y = attribAccessor.minValues[1];
									pMin.z = attribAccessor.minValues[2];
									pMax.x = attribAccessor.maxValues[0];
									pMax.y = attribAccessor.maxValues[1];
									pMax.z = attribAccessor.maxValues[2];*/

									switch (attribAccessor.type)
									{
									case TINYGLTF_TYPE_VEC3:
									{
										switch (attribAccessor.componentType)
										{
										case TINYGLTF_COMPONENT_TYPE_FLOAT:
											std::cout << "Type is FLOAT\n";
											// 3D vector of float
											auto positions = std::unique_ptr<Vertex3DArrayPtr<glm::vec3>>(new Vertex3DArrayPtr<glm::vec3>(VertexArrayStorage<glm::vec3>(dataPtr, count, byte_stride)));

											std::cout << "positions's size : " << positions->size()
													  << '\n';

											for (size_t i{0}; i < positions->size(); ++i)
											{
												const auto v = (*positions)[i];
												// std::cout << "positions[" << i << "]: (" << v.x << ", "
												//	<< v.y << ", " << v.z << ")\n";

												currentMesh.data().mesh.Vertices[i + currentvOffset].position = parent_transform * (glm::vec4(v.x, v.y, v.z, 1.0f));
												if (v.x < m_pMin.x)
													m_pMin.x = v.x;
												if (v.y < m_pMin.y)
													m_pMin.y = v.y;
												if (v.z < m_pMin.z)
													m_pMin.z = v.z;

												if (v.x > m_pMax.x)
													m_pMax.x = v.x;
												if (v.y > m_pMax.y)
													m_pMax.y = v.y;
												if (v.z > m_pMax.z)
													m_pMax.z = v.z;
												// currentMesh.data().mesh.Vertices[i + currentvOffset].color = glm::vec4(1.0f);
												if (HasAnimation())
												{
													mAnimatedMeshes.at(index).Vertices[i + currentvOffset].position = (glm::vec4(v.x, v.y, v.z, 1.0f));
													// mAnimatedMeshes.at(index).Vertices[i + currentvOffset].color = glm::vec4(1.0f);
												}
											}

											thisVOffset += positions->size();
										}
										break;
									case TINYGLTF_COMPONENT_TYPE_DOUBLE:
									{
										std::cout << "Type is DOUBLE\n";
										switch (attribAccessor.type)
										{
										case TINYGLTF_TYPE_VEC3:
										{
											auto positions = std::unique_ptr<Vertex3DArrayPtr<glm::vec3>>(new Vertex3DArrayPtr<glm::vec3>(VertexArrayStorage<glm::vec3>(dataPtr, count, byte_stride)));

											for (size_t i{0}; i < positions->size(); ++i)
											{
												const auto v = (*positions)[i];
												// std::cout << "positions[" << i << "]: (" << v.x
												//	<< ", " << v.y << ", " << v.z << ")\n";

												currentMesh.data().mesh.Vertices[i + currentvOffset].position = parent_transform * (glm::vec4(v.x, v.y, v.z, 1.0f));
												if (v.x < m_pMin.x)
													m_pMin.x = v.x;
												if (v.y < m_pMin.y)
													m_pMin.y = v.y;
												if (v.z < m_pMin.z)
													m_pMin.z = v.z;

												if (v.x > m_pMax.x)
													m_pMax.x = v.x;
												if (v.y > m_pMax.y)
													m_pMax.y = v.y;
												if (v.z > m_pMax.z)
													m_pMax.z = v.z;
												if (HasAnimation())
												{

													mAnimatedMeshes.at(index).Vertices[i + currentvOffset].position = (glm::vec4(v.x, v.y, v.z, 1.0f));
												}
											}
											thisVOffset += positions->size();
										}
										break;
										default:
											// TODO Handle error
											break;
										}
										break;
									default:
										break;
									}
									}
									break;
									}
								}

								if (attribute.first == "NORMAL")
								{
									std::cout << "found normal attribute\n";

									switch (attribAccessor.type)
									{
									case TINYGLTF_TYPE_VEC3:
									{
										std::cout << "Normal is VEC3\n";
										switch (attribAccessor.componentType)
										{
										case TINYGLTF_COMPONENT_TYPE_FLOAT:
										{
											std::cout << "Normal is FLOAT\n";
											auto normals = std::unique_ptr<Vertex3DArrayPtr<glm::vec3>>(new Vertex3DArrayPtr<glm::vec3>(VertexArrayStorage<glm::vec3>(dataPtr, count, byte_stride)));

											// IMPORTANT: We need to reorder normals (and texture
											// coordinates into "facevarying" order) for each face

											// For each triangle :

											for (size_t i{0}; i < indices.size() / 3; ++i)
											{
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
												currentMesh.data().mesh.Vertices[f0 + currentvOffset].normal = glm::vec3(glm::mat3(parent_transform) * glm::vec4(n0, 1.0f));
												currentMesh.data().mesh.Vertices[f1 + currentvOffset].normal = glm::vec3(glm::mat3(parent_transform) * glm::vec4(n1, 1.0f));
												currentMesh.data().mesh.Vertices[f2 + currentvOffset].normal = glm::vec3(glm::mat3(parent_transform) * glm::vec4(n2, 1.0f));
												if (HasAnimation())
												{

													mAnimatedMeshes.at(index).Vertices[f0 + currentvOffset].normal = glm::vec3(glm::mat3(parent_transform) * glm::vec4(n0, 1.0f));
													mAnimatedMeshes.at(index).Vertices[f1 + currentvOffset].normal = glm::vec3(glm::mat3(parent_transform) * glm::vec4(n1, 1.0f));
													mAnimatedMeshes.at(index).Vertices[f2 + currentvOffset].normal = glm::vec3(glm::mat3(parent_transform) * glm::vec4(n2, 1.0f));
												}
											}
										}
										break;
										case TINYGLTF_COMPONENT_TYPE_DOUBLE:
										{
											std::cout << "Normal is DOUBLE\n";
											auto normals = std::unique_ptr<Vertex3DArrayPtr<glm::vec3>>(new Vertex3DArrayPtr<glm::vec3>(VertexArrayStorage<glm::vec3>(dataPtr, count, byte_stride)));

											// IMPORTANT: We need to reorder normals (and texture
											// coordinates into "facevarying" order) for each face

											// For each triangle :
											for (size_t i{0}; i < indices.size() / 3; ++i)
											{
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

												currentMesh.data().mesh.Vertices[f0 + currentvOffset].normal = glm::vec3(glm::mat3(parent_transform) * glm::vec4(n0, 1.0f));
												currentMesh.data().mesh.Vertices[f1 + currentvOffset].normal = glm::vec3(glm::mat3(parent_transform) * glm::vec4(n1, 1.0f));
												currentMesh.data().mesh.Vertices[f2 + currentvOffset].normal = glm::vec3(glm::mat3(parent_transform) * glm::vec4(n2, 1.0f));

												if (HasAnimation())
												{

													mAnimatedMeshes.at(index).Vertices[f0 + currentvOffset].normal = glm::vec3(glm::mat3(parent_transform) * glm::vec4(n0, 1.0f));
													mAnimatedMeshes.at(index).Vertices[f1 + currentvOffset].normal = glm::vec3(glm::mat3(parent_transform) * glm::vec4(n1, 1.0f));
													mAnimatedMeshes.at(index).Vertices[f2 + currentvOffset].normal = glm::vec3(glm::mat3(parent_transform) * glm::vec4(n2, 1.0f));
												}
											}
										}
										break;
										default:
											std::cerr << "Unhandeled componant type for normal\n";
										}
									}
									break;
									default:
										std::cerr << "Unhandeled vector type for normal\n";
									}
								}

								// Face varying comment on the normals is also true for the UVs
								if (attribute.first == "TEXCOORD_0")
								{
									std::cout << "Found texture coordinates\n";

									switch (attribAccessor.type)
									{
									case TINYGLTF_TYPE_VEC2:
									{
										std::cout << "TEXTCOORD is VEC2\n";
										switch (attribAccessor.componentType)
										{
										case TINYGLTF_COMPONENT_TYPE_FLOAT:
										{
											std::cout << "TEXTCOORD is FLOAT\n";

											auto uvs = std::unique_ptr<Vertex2DArrayPtr<glm::vec2>>(new Vertex2DArrayPtr<glm::vec2>(VertexArrayStorage<glm::vec2>(dataPtr, count, byte_stride)));

											for (size_t i{0}; i < indices.size() / 3; ++i)
											{
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

												if (HasAnimation())
												{

													mAnimatedMeshes.at(index).Vertices[f0 + currentvOffset].texCoords = glm::vec2(uv0);
													mAnimatedMeshes.at(index).Vertices[f1 + currentvOffset].texCoords = glm::vec2(uv1);
													mAnimatedMeshes.at(index).Vertices[f2 + currentvOffset].texCoords = glm::vec2(uv2);
												}
											}
										}
										break;
										case TINYGLTF_COMPONENT_TYPE_DOUBLE:
										{
											std::cout << "TEXTCOORD is DOUBLE\n";

											auto uvs = std::unique_ptr<Vertex2DArrayPtr<glm::dvec2>>(new Vertex2DArrayPtr<glm::dvec2>(VertexArrayStorage<glm::dvec2>(dataPtr, count, byte_stride)));

											for (size_t i{0}; i < indices.size() / 3; ++i)
											{
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

												if (HasAnimation())
												{

													mAnimatedMeshes.at(index).Vertices[f0 + currentvOffset].texCoords = glm::dvec2(uv0);
													mAnimatedMeshes.at(index).Vertices[f1 + currentvOffset].texCoords = glm::dvec2(uv1);
													mAnimatedMeshes.at(index).Vertices[f2 + currentvOffset].texCoords = glm::dvec2(uv2);
												}
											}
										}
										break;
										default:
											std::cerr << "unrecognized vector type for UV";
										}
									}
									break;
									default:
										std::cerr << "unreconized componant type for UV";
									}
								}

								// Face varying comment on the normals is also true for the UVs
								if (attribute.first == "COLOR_0")
								{
									std::cout << "Found color\n";

									switch (attribAccessor.type)
									{
									case TINYGLTF_TYPE_VEC4:
									{
										std::cout << "COLOR_0 is VEC4\n";
										switch (attribAccessor.componentType)
										{
										case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
										{
											std::cout << "COLOR_0 is TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT\n";

											auto color = std::unique_ptr<UnsignedVertex4DArrayPtr<glm::lowp_u16vec4>>(new UnsignedVertex4DArrayPtr<glm::lowp_u16vec4>(VertexArrayStorage<glm::lowp_u16vec4>(dataPtr, count, byte_stride)));

											for (size_t i{0}; i < indices.size() / 3; ++i)
											{
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

												if (HasAnimation())
												{

													mAnimatedMeshes.at(index).Vertices[f0 + currentvOffset].color = glm::vec4(color0);
													mAnimatedMeshes.at(index).Vertices[f1 + currentvOffset].color = glm::vec4(color1);
													mAnimatedMeshes.at(index).Vertices[f2 + currentvOffset].color = glm::vec4(color2);
												}
											}
										}
										break;
										case TINYGLTF_COMPONENT_TYPE_DOUBLE:
										{
											std::cout << "COLOR_0 is DOUBLE\n";

											auto color = std::unique_ptr<Vertex4DArrayPtr<glm::dvec4>>(new Vertex4DArrayPtr<glm::dvec4>(VertexArrayStorage<glm::dvec4>(dataPtr, count, byte_stride)));

											for (size_t i{0}; i < indices.size() / 3; ++i)
											{
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

												if (HasAnimation())
												{

													mAnimatedMeshes.at(index).Vertices[f0 + currentvOffset].color = glm::dvec4(color0);
													mAnimatedMeshes.at(index).Vertices[f1 + currentvOffset].color = glm::dvec4(color1);
													mAnimatedMeshes.at(index).Vertices[f2 + currentvOffset].color = glm::dvec4(color2);
												}
											}
										}
										break;
										case TINYGLTF_COMPONENT_TYPE_FLOAT:
										{
											std::cout << "COLOR_0 is FLOAT\n";

											auto color = std::unique_ptr<Vertex4DArrayPtr<glm::vec4>>(new Vertex4DArrayPtr<glm::vec4>(VertexArrayStorage<glm::vec4>(dataPtr, count, byte_stride)));

											for (size_t i{0}; i < indices.size() / 3; ++i)
											{
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
												currentMesh.data().mesh.Vertices[f0 + currentvOffset].color = glm::vec4(color0);
												currentMesh.data().mesh.Vertices[f1 + currentvOffset].color = glm::vec4(color1);
												currentMesh.data().mesh.Vertices[f2 + currentvOffset].color = glm::vec4(color2);

												if (HasAnimation())
												{

													mAnimatedMeshes.at(index).Vertices[f0 + currentvOffset].color = glm::vec4(color0);
													mAnimatedMeshes.at(index).Vertices[f1 + currentvOffset].color = glm::vec4(color1);
													mAnimatedMeshes.at(index).Vertices[f2 + currentvOffset].color = glm::vec4(color2);
												}
											}
										}
										break;
										default:
											std::cerr << "unrecognized vector type for color\n";
										}
									}
									break;
									default:
										std::cerr << "unreconized componant type for color\n";
									}
								}
								if (attribute.first == "JOINTS_0")
								{
									std::cout << "Found joints\n";

									switch (attribAccessor.type)
									{
									case TINYGLTF_TYPE_VEC4:
									{
										std::cout << "JOINTS_0 is VEC4\n";
										switch (attribAccessor.componentType)
										{
										case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
										{
											std::cout << "JOINTS_0 is TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT\n";

											auto joint = std::unique_ptr<UnsignedVertex4DArrayPtr<glm::lowp_u16vec4>>(new UnsignedVertex4DArrayPtr<glm::lowp_u16vec4>(VertexArrayStorage<glm::lowp_u16vec4>(dataPtr, count, byte_stride)));

											for (size_t i{0}; i < indices.size() / 3; ++i)
											{
												// get the i'th triange's indexes
												auto f0 = indices[3 * i + 0];
												auto f1 = indices[3 * i + 1];
												auto f2 = indices[3 * i + 2];

												// get the joint ids for each triangle's
												// vertices
												glm::lowp_u16vec4 joint0, joint1, joint2;

												joint0 = (*joint)[f0];
												joint1 = (*joint)[f1];
												joint2 = (*joint)[f2];

												// push them in order into the mesh data
												mAnimatedMeshes.at(index).Vertices[f0 + currentvOffset].boneIDs = (glm::ivec4)joint0;
												mAnimatedMeshes.at(index).Vertices[f1 + currentvOffset].boneIDs = (glm::ivec4)joint1;
												mAnimatedMeshes.at(index).Vertices[f2 + currentvOffset].boneIDs = (glm::ivec4)joint2;
											}
										}
										break;

										case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
										{
											std::cout << "JOINTS_0 is TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE\n";

											auto joint = std::unique_ptr<UnsignedVertex4DArrayPtr<glm::lowp_u8vec4>>(new UnsignedVertex4DArrayPtr<glm::lowp_u8vec4>(VertexArrayStorage<glm::lowp_u8vec4>(dataPtr, count, byte_stride)));

											for (size_t i{0}; i < indices.size() / 3; ++i)
											{
												// get the i'th triange's indexes
												auto f0 = indices[3 * i + 0];
												auto f1 = indices[3 * i + 1];
												auto f2 = indices[3 * i + 2];

												// get the joint ids for each triangle's
												// vertices
												glm::lowp_u16vec4 joint0, joint1, joint2;

												joint0 = (*joint)[f0];
												joint1 = (*joint)[f1];
												joint2 = (*joint)[f2];

												// push them in order into the mesh data
												mAnimatedMeshes.at(index).Vertices[f0 + currentvOffset].boneIDs = (glm::ivec4)joint0;
												mAnimatedMeshes.at(index).Vertices[f1 + currentvOffset].boneIDs = (glm::ivec4)joint1;
												mAnimatedMeshes.at(index).Vertices[f2 + currentvOffset].boneIDs = (glm::ivec4)joint2;
											}
										}
										break;
										default:
											std::cerr << "unrecognized vector type for joint\n";
										}
									}
									break;
									default:
										std::cerr << "unreconized componant type for joint\n";
									}
								}

								if (attribute.first == "WEIGHTS_0")
								{
									std::cout << "found weight attribute\n";

									switch (attribAccessor.type)
									{
									case TINYGLTF_TYPE_VEC4:
									{
										std::cout << "weight is VEC4\n";
										switch (attribAccessor.componentType)
										{
										case TINYGLTF_COMPONENT_TYPE_FLOAT:
										{
											std::cout << "weight is FLOAT\n";
											auto weights = std::unique_ptr<Vertex4DArrayPtr<glm::vec4>>(new Vertex4DArrayPtr<glm::vec4>(VertexArrayStorage<glm::vec4>(dataPtr, count, byte_stride)));

											// IMPORTANT: We need to reorder normals (and texture
											// coordinates into "facevarying" order) for each face

											// For each triangle :

											for (size_t i{0}; i < indices.size() / 3; ++i)
											{
												// get the i'th triange's indexes
												auto f0 = indices[3 * i + 0];
												auto f1 = indices[3 * i + 1];
												auto f2 = indices[3 * i + 2];

												// get the 4 weight vectors for that face
												glm::vec4 w0, w1, w2;
												w0 = (*weights)[f0];
												w1 = (*weights)[f1];
												w2 = (*weights)[f2];

												// push them in order into the mesh data
												mAnimatedMeshes.at(index).Vertices[f0 + currentvOffset].weights = w0;
												mAnimatedMeshes.at(index).Vertices[f1 + currentvOffset].weights = w1;
												mAnimatedMeshes.at(index).Vertices[f2 + currentvOffset].weights = w2;
											}
										}
										break;

										default:
											std::cerr << "Unhandeled componant type for normal\n";
										}
									}
									break;
									default:
										std::cerr << "Unhandeled vector type for normal\n";
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

					currentiOffset = thisIOffset;
					currentvOffset = thisVOffset;
					currentMesh.generateTangentSpaceVectors();

					if (HasAnimation())
					{

						int tmp_index = 0;
						for (auto &vertex : mAnimatedMeshes.at(index).Vertices)
						{
							vertex.tangent = currentMesh.data().mesh.Vertices.at(tmp_index).tangent;
							vertex.bitangent = currentMesh.data().mesh.Vertices.at(tmp_index).bitangent;
							tmp_index++;
						}
					}
					auto &meshData = currentMesh.data();
					meshData = {
						.numVertices = meshData.mesh.Vertices.size(),
						.numIndices = meshData.mesh.Indices.size(),
						.vertexOffset = currentvOffset,
						.indexOffset = currentiOffset,
						.mesh = {meshData.mesh.Vertices, meshData.mesh.Indices}};

					index++;
				}
			}

			MinMaxPoints.MAX_X = m_pMax.x;
			MinMaxPoints.MAX_Y = m_pMax.y;
			MinMaxPoints.MAX_Z = m_pMax.z;

			MinMaxPoints.MIN_X = m_pMin.x;
			MinMaxPoints.MIN_Y = m_pMin.y;
			MinMaxPoints.MIN_Z = m_pMin.z;

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

			return true;
		}

		std::vector<unsigned int> indices{};
	};
}