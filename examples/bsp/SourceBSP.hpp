//
// Created by solaire on 1/2/23.
//

#ifndef EPSILON_SOURCEBSP_HPP
#define EPSILON_SOURCEBSP_HPP


#include <Epsilon.hpp>
#include <beacon/beacon.hpp>

class SourceBSP : public Epsilon::Epsilon {

    struct BSPFace {
        engine::Ref<engine::Mesh> mesh;
        engine::Ref<engine::Texture> texture;
        engine::Ref<engine::Material> material;
    };

    struct BSPMap {
        BSPFace m_pFaces[];
    };

    std::vector<int> m_pVisibleFaces;

    explicit SourceBSP(const std::string &appname) : Epsilon::Epsilon(appname) {

    }

};

#endif //EPSILON_SOURCEBSP_HPP
