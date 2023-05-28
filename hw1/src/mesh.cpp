#include <mesh.h>
#include <utils.h>
#include <fstream>
#include <vector>
#include <string>
#include <stdio.h>


Mesh::Mesh(const std::string &path) { loadDataFromFile(getPath(path)); }

/**
 * TODO: implement load mesh data from file here
 * you need to open file with [path]
 *
 * File format
 *
 *
 * For each line starting with 'v' contains 3 floats, which
 * represents the position of a vertex
 *
 * For each line starting with 'n' contains 3 floats, which
 * represents the normal of a vertex
 *
 * For each line starting with 'f' contains 6 integers
 * [v0, n0, v1, n1, v2, n2], which represents the triangle face
 * v0, n0 means the vertex index and normal index of the first vertex
 * v1, n1 means the vertex index and normal index of the second vertex
 * v2, n2 means the vertex index and normal index of the third vertex
 */
void Mesh::loadDataFromFile(const std::string &path) {
    /** path stands for the where the object is storaged **/
    std::ifstream infile(path);
    std::vector<vec3> vertex_set;
    std::vector<vec3> normal_set;
    std::vector<vec3> temp;
    std::vector<uvec3> vertex_indices;
    std::vector<uvec3> normal_indices;
    while (infile.good())
    {
        std::string choice_char;
        getline(infile, choice_char, '\n');
        if (choice_char[0] == '#')
            continue;
        else if (choice_char[0] == 'v')
        {
            vec3 vertex;
            sscanf_s(choice_char.c_str(), "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            vertex_set.push_back(vertex);
        }
        else if (choice_char[0] == 'n')
        {
            vec3 normal;
            sscanf_s(choice_char.c_str(), "n %f %f %f", &normal.x, &normal.y, &normal.z);
            normal_set.push_back(normal);
        }
        else if (choice_char[0] == 'f')
        {
            uvec3 face_vertex, face_normal;
            sscanf_s(choice_char.c_str(), "f %d %d %d %d %d %d",
                     &face_vertex.x, &face_normal.x,
                     &face_vertex.y, &face_normal.y,
                     &face_vertex.z, &face_normal.z);
            vertex_indices.push_back(face_vertex);
            normal_indices.push_back(face_normal);
        }
      
    }
    int vertex_capacity;
    vertex_capacity = vertex_set.size();
    int vertex_indices_capacity;
    vertex_indices_capacity = vertex_indices.size();
    for (int i = 0; i < vertex_capacity; i++)
    {
        Vertex v;
        v.position = vertex_set[i];
        if (i < normal_set.size())
        {
            v.normal = normal_set[i];
        }
        vertices.push_back(v);
    }
    for (int i = 0; i < vertex_indices_capacity; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            indices.push_back(vertex_indices[i][j]);
        }
    }
    unsigned int bunny_VBO;
    unsigned int bunny_EBO;
    glGenVertexArrays(1, &bunny_VAO);
    glGenBuffers(1, &bunny_VBO);
    glGenBuffers(1, &bunny_EBO);
    glBindVertexArray(bunny_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, bunny_VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bunny_EBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); 
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(glm::vec3)); 
    glEnableVertexAttribArray(1);
}

/**
 * TODO: implement your draw code here
 */
void Mesh::draw() const {
    glBindVertexArray(bunny_VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}
