#include "MeshReader.h"

#include "TetgenAdapter.h"
#include "polyhedralGravity/util/UtilityString.h"

namespace polyhedralGravity {

    std::tuple<std::vector<Array3>, std::vector<IndexArray3>> MeshReader::getPolyhedralSource(const std::vector<std::string> &fileNames) {
        switch (fileNames.size()) {
            case 0:
                throw std::runtime_error("No mesh file given");
            case 1:
                if (util::ends_with(fileNames[0], ".obj")) {
                    return MeshReader::readObj(fileNames[0]);
                } else {
                    return MeshReader::readTetgenFormat(fileNames);
                }
            case 2:
                return MeshReader::readTetgenFormat(fileNames);
            default:
                throw std::runtime_error("More than two mesh files given. There is no known mesh-format consisting of three files. "
                                         "The polyhedron will be over-specified!");
        }
    }

    std::tuple<std::vector<Array3>, std::vector<IndexArray3>> MeshReader::readTetgenFormat(const std::vector<std::string> &fileNames) {
        return TetgenAdapter{fileNames}.getPolyhedralSource();
    }

    std::tuple<std::vector<Array3>, std::vector<IndexArray3>> MeshReader::readObj(const std::string &filename) {
        std::vector<Array3> vertices{};
        std::vector<IndexArray3> faces{};
        POLYHEDRAL_GRAVITY_LOG_DEBUG("Reading the file {}", filename);
        std::ifstream file(filename);
        if (!file) {
            throw std::runtime_error("Could not open file " + filename + " for reading.");
        }
        std::string line;
        char prefix;
        std::array<double, 3> vertex{};
        std::array<size_t, 3> face{};
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            if (line.empty() || line[0] == '#') {
                continue;
            }

            if (line[0] == 'v') {
                ss >> prefix >> vertex[0] >> vertex[1] >> vertex[2];
                vertices.push_back(vertex);
            } else if (line[0] == 'f') {
                ss >> prefix >> face[0] >> face[1] >> face[2];
                faces.push_back(face);
            }
        }
        file.close();
        return {vertices, faces};
    }
}