//
// Created by Stefan on 07-Dec-20.
//
#pragma once
#include "Common.h"
using namespace std;

typedef struct Vertex {
    glm::vec3 position, normal;
    glm::vec2 texture_coord;
    glm::vec4 weights, bonesIDs;
}Vertex;

typedef struct Mesh {
    string name;
    vector<uint32_t> indices;
    vector<Vertex> vertices;
}Mesh;

typedef struct Model {
    string name;
    vector<Mesh*> meshes;
}Model;


class glTFImporter {
public:
    explicit glTFImporter(const string& path) {
        if(filesystem::exists(path)) {
            ifstream reader(path.c_str());
            reader >> j;
            current_dir = path.substr(0, path.find_last_of('/')).append("/");
            readGeometry();
        }
    }

private:
    nlohmann::json j;
    string current_dir;

     Model* readGeometry() {
         auto* model = new Model;
         for(const auto& mesh : j["meshes"]) {
            model->meshes.push_back(readMesh(mesh));
         }
         return model;
     }

    Mesh* readMesh(nlohmann::json mesh_field) {
         cout << mesh_field << "\n";
         Mesh* mesh = new Mesh;
         mesh->name = mesh_field["name"];
         
         return mesh;
    }
};


