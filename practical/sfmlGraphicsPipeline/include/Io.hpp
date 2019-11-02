#ifndef IO_HPP
#define IO_HPP

/**@file
 *@brief Input/Output functions.
 *
 * Currently, this file only contains I/O functions for OBJ meshes.*/

#include <vector>
#include <glm/glm.hpp>
#include <string>

/**@brief Collect mesh data from an OBJ file.
 *
 * This function opens an OBJ mesh file to collect information such
 * as vertex position, vertex indices of a face, vertex normals and vertex
 * texture coordinates.
 *
 * @param filename The path to the mesh file.
 * @param positions The vertex positions.
 * @param indices The vertex indices of faces.
 * @param normals The vertex normals.
 * @param texcoords The vertex texture coordinates.
 * @return False if import failed, true otherwise.
 */
bool read_obj(
        const std::string& filename, 
        std::vector<glm::vec3>& positions, 
        std::vector<unsigned int>& indices,
        std::vector<glm::vec3>& normals,
        std::vector<glm::vec2>& texcoords
        );

/**@brief Collect mesh data from an OBJ file, and add a custom material from .mtl
 *
 * This function is a alternate methode for read_obj that allows to called directly 
 * a .mtl file
 *
 * @param filename The path to the mesh file.
 * @param material_filename The path to the material file
 * @param positions The vertex positions.
 * @param indices The vertex indices of faces.
 * @param normals The vertex normals.
 * @param texcoords The vertex texture coordinates.
 * @return False if import failed, true otherwise.
 */

bool read_obj(const std::string& filename,
        const std::string& material_filename,
        std::vector<glm::vec3>& positions,
        std::vector<unsigned int>& triangles,
        std::vector<glm::vec3>& normals,
        std::vector<glm::vec2>& texcoords
        );

#endif //IO_HPP
