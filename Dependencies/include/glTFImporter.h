
#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>

#include <json.hpp>

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "gtx/string_cast.hpp"
#include "gtc/quaternion.hpp"
#include "gtx/quaternion.hpp"

using namespace std;
using namespace nlohmann;

typedef struct Vertex {
    glm::vec3 position, normal;
    glm::vec2 texture_coord;
    glm::vec4 weights, bonesIDs;
}Vertex;

typedef struct Mesh {
    string name;
    vector<uint16_t> indices;
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
            current_dir = path.substr(0, path.find_last_of('/'));
            current_dir == path ? (current_dir = "./") : current_dir.append("/");
            readGeometry();
        }
    }

    Model *model{};
private:
    json j;
    string current_dir;

    Model* readGeometry() {
         model = new Model;
         for(const auto& mesh : j["meshes"]) {
             vector<Mesh*> res = readMesh(mesh);
             model->meshes.insert(model->meshes.end(), res.size(), reinterpret_cast<Mesh *const>(readMesh(mesh).data()));
         }
         return model;
     }
    vector<Mesh*> readMesh(json mesh_field) {
        vector<Mesh*> res;
        for(const auto& attrib : mesh_field["primitives"]) {
            res.push_back(readSubMeshes(attrib, mesh_field));
        }
        return res;
    }

    // read vertices, normals, texture coordinates, tangents, weights and bone IDs
    Mesh* readSubMeshes(json mesh_attrib, json mesh_field) {
        Mesh *mesh = new Mesh;
        mesh->name = mesh_field["name"];

        /* To get the vertex coordinates you need to look up
         * for atoi(mesh_attrib["POSITION"].dump().c_str()) in j["accessors"]
         * and extract vertex data from .bin
         * */

        // POSITIONS
        vector<glm::vec3> pos;
        if(mesh_attrib["attributes"]["POSITION"] != NULL) {
            json bufferViewT = getBufferView(mesh_attrib, "POSITION");
            json bufferT = getBuffer(bufferViewT);
            string buffer_filenameT = getBinaryFilePath(bufferT);

            pos = getVec3Buffer(buffer_filenameT, bufferViewT);
        }
        // NORMALS
        vector<glm::vec3> norm;
        if(mesh_attrib["attributes"]["NORMAL"] != NULL) {
            json bufferViewN = getBufferView(mesh_attrib, "NORMAL");
            json bufferN = getBuffer(bufferViewN);
            string buffer_filenameN = getBinaryFilePath(bufferN);

            norm = getVec3Buffer(buffer_filenameN, bufferViewN);
        }

        // INDICES
        json bufferViewI = getIndicesBufferView(mesh_attrib);
        json bufferI = getBuffer(bufferViewI);
        string buffer_filenameI = getBinaryFilePath(bufferI);

        vector<uint16_t> indices = getUint16Buffer(buffer_filenameI, bufferViewI);


        /* SUM UP EVERYTHING !!!
         * TODO: do a fact checking
         * The sizes of normal, positions and textures vectors should be the same (I think)
         * */
        for(int i=0; i<pos.size(); i++) {
            Vertex v;

            pos.size() > 0 ? (v.position = pos[i]) : (v.position = glm::vec3(0));
            norm.size() > 0 ? (v.normal = norm[i]) : (v.normal = glm::vec3(0));

            mesh->vertices.push_back(v);
        }
        mesh->indices = indices;

        return mesh;
    }

    json getIndicesBufferView(json mesh_attrib) {
        return j["bufferViews"].at(atoi(j["accessors"].at(atoi(mesh_attrib["indices"].dump().c_str()))["bufferView"].dump().c_str()));
    }
    json getBufferView(json mesh_attrib, char* type) {
        return j["bufferViews"].at(atoi(j["accessors"].at(atoi(mesh_attrib["attributes"][type].dump().c_str()))["bufferView"].dump().c_str()));
    }
    json getBuffer(json bufferView) {
        return j["buffers"].at(atoi(bufferView["buffer"].dump().c_str()));
    }
    string getBinaryFilePath(json buffer) {
        string buffer_path = current_dir;
        string buffer_filename = buffer["uri"].dump();
        buffer_filename = buffer_filename.substr(1, buffer_filename.size() - 2);
        buffer_path.append(buffer_filename);
        return buffer_path;
    }

    // Use whatever your heart desires for indices. I chose uint_16
    static vector<uint16_t> getUint16Buffer(const string& binary_file_path, json bufferView) {

        std::ifstream is (binary_file_path.c_str(), std::ifstream::binary);

        if (is) {
            // get length and offset:
            int length = atoi(bufferView["byteLength"].dump().c_str());
            int offset = atoi(bufferView["byteOffset"].dump().c_str());

            // read data as a block:
            is.seekg(offset, is.beg);
            vector<uint16_t> vec = vector<uint16_t>(length/sizeof(uint16_t));
            is.read (reinterpret_cast<char*>(&vec[0]), length);

            return vec;
        }
        else cout << "Binary file " <<binary_file_path << " doesn't exist or cannot be opened\n\n";
        throw runtime_error("No buffer\n");
    }
    // for texture coords
    static vector<glm::vec2> getVec2Buffer(const string& binary_file_path, json bufferView) {

        std::ifstream is (binary_file_path.c_str(), std::ifstream::binary);

        if (is) {
            // get length and offset:
            int length = atoi(bufferView["byteLength"].dump().c_str());
            int offset = atoi(bufferView["byteOffset"].dump().c_str());

            // read data as a block:
            is.seekg(offset, is.beg);
            vector<glm::vec2> vec = vector<glm::vec2>(length/sizeof(float)/2);
            is.read (reinterpret_cast<char*>(&vec[0]), length);

            return vec;
        }
        else cout << "Binary file " <<binary_file_path << " doesn't exist or cannot be opened\n\n";
        throw runtime_error("No buffer\n");
    }
    // for vertex coords and normals
    static vector<glm::vec3> getVec3Buffer(const string& binary_file_path, json bufferView) {

        std::ifstream is (binary_file_path.c_str(), std::ifstream::binary);

        if (is) {
            // get length and offset:
            int length = atoi(bufferView["byteLength"].dump().c_str());
            int offset = atoi(bufferView["byteOffset"].dump().c_str());

            // read data as a block:
            is.seekg(offset, is.beg);
            vector<glm::vec3> vec = vector<glm::vec3>(length/sizeof(float)/3);
            is.read (reinterpret_cast<char*>(&vec[0]), length);

            return vec;
        }
        else cout << "Binary file " <<binary_file_path << " doesn't exist or cannot be opened\n\n";
        throw runtime_error("No buffer\n");
    }
    // for boneIDs and bone weights
    static vector<glm::vec4> getVec4Buffer(const string& binary_file_path, json bufferView) {

        std::ifstream is (binary_file_path.c_str(), std::ifstream::binary);

        if (is) {
            // get length and offset:
            int length = atoi(bufferView["byteLength"].dump().c_str());
            int offset = atoi(bufferView["byteOffset"].dump().c_str());

            // read data as a block:
            is.seekg(offset, is.beg);
            vector<glm::vec4> vec = vector<glm::vec4>(length/sizeof(float)/4);
            is.read (reinterpret_cast<char*>(&vec[0]), length);

            return vec;
        }
        else cout << "Binary file " <<binary_file_path << " doesn't exist or cannot be opened\n\n";
        throw runtime_error("No buffer\n");
    }


};


