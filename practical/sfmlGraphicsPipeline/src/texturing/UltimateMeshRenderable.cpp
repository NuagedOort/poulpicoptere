#include "./../../include/texturing/UltimateMeshRenderable.hpp"
#include "./../../include/GeometricTransformation.hpp"
#include "./../../include/gl_helper.hpp"
#include "./../../include/log.hpp"
#include "./../../include/Io.hpp"
#include "./../../include/Utils.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
# include <iostream>

UltimateMeshRenderable::~UltimateMeshRenderable()
{
    glcheck(glDeleteBuffers(1, &m_pBuffer));
    glcheck(glDeleteBuffers(1, &m_cBuffer));
    glcheck(glDeleteBuffers(1, &m_nBuffer));
    glcheck(glDeleteBuffers(1, &m_iBuffer));
    glcheck(glDeleteBuffers(1, &m_tBuffer));
    glcheck(glDeleteTextures(1, &m_texId));
}

UltimateMeshRenderable::UltimateMeshRenderable(
    ShaderProgramPtr shaderProgram, 
    const std::string& mesh_filename, 
    const std::string& texture_filename,
    const float endAnimation 
    ) : HierarchicalRenderable(shaderProgram), m_pBuffer(0), m_cBuffer(0), m_nBuffer(0), m_iBuffer(0), m_tBuffer(0), m_texId( 0 )
{
    setMaterial( std::make_shared<Material>(glm::vec3{1.0f,1.0f,1.0f}, glm::vec3{1.0f,1.0f,1.0f}, glm::vec3{1.0f,1.0f,1.0f}, 1.0f) );
    read_obj(mesh_filename, m_positions, m_indices, m_normals, m_texCoords);
    m_colors.resize( m_positions.size(), glm::vec4(1.0,1.0,1.0,1.0) );

    // Check if an animation time has been set to a coherent value, else, use bezier interpolation mode
    endAnimation <= -1.0 ? isBezier = false : isBezier = true;
    bezier_segmentation.push_back(0.0f);
    bezier_segmentation.push_back(endAnimation);

    //Create buffers
    glcheck(glGenBuffers(1, &m_pBuffer)); //vertices
    glcheck(glGenBuffers(1, &m_cBuffer)); //colors
    glcheck(glGenBuffers(1, &m_nBuffer)); //normals
    glcheck(glGenBuffers(1, &m_iBuffer)); //indices
    glcheck(glGenBuffers(1, &m_tBuffer)); //texture coordinates

    //Activate buffer and send data to the graphics card
    glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_pBuffer));
    glcheck(glBufferData(GL_ARRAY_BUFFER, m_positions.size()*sizeof(glm::vec3), m_positions.data(), GL_STATIC_DRAW));
    glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_cBuffer));
    glcheck(glBufferData(GL_ARRAY_BUFFER, m_colors.size()*sizeof(glm::vec4), m_colors.data(), GL_STATIC_DRAW));
    glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_nBuffer));
    glcheck(glBufferData(GL_ARRAY_BUFFER, m_normals.size()*sizeof(glm::vec3), m_normals.data(), GL_STATIC_DRAW));
    glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_tBuffer));
    glcheck(glBufferData(GL_ARRAY_BUFFER, m_texCoords.size()*sizeof(glm::vec2), m_texCoords.data(), GL_STATIC_DRAW));
    glcheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iBuffer));
    glcheck(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size()*sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW));

    // create and setup the texture
    glcheck(glGenTextures(1, &m_texId));
    glcheck(glBindTexture(GL_TEXTURE_2D, m_texId));
    glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

    // load the texture
    sf::Image image;
    image.loadFromFile(texture_filename);
    image.flipVertically(); // sfml inverts the v axis... put the image in OpenGL convention: lower left corner is (0,0)
    glcheck(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, image.getSize().x, image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, (const GLvoid*)image.getPixelsPtr()));

    //Release the texture
    glcheck(glBindTexture(GL_TEXTURE_2D, 0));
}

void UltimateMeshRenderable::addLocalTransformKeyframe( const GeometricTransformation& transformation, float time )
{
    if (isBezier) {
        m_BlocalKeyframes.add( transformation, time );
    } else {
        m_localKeyframes.add( transformation, time );
    }
}

void UltimateMeshRenderable::addParentTransformKeyframe( const GeometricTransformation& transformation, float time )
{
    if(isBezier) { 
        m_BparentKeyframes.add( transformation, time );
    } else {
        m_parentKeyframes.add( transformation, time );
    }
}

void UltimateMeshRenderable::setBezierSegment( const std::vector< float > & segment ){
    bezier_segmentation = segment;
}

void UltimateMeshRenderable::do_draw()
{
    //Location
    int positionLocation = m_shaderProgram->getAttributeLocation("vPosition");
    int colorLocation = m_shaderProgram->getAttributeLocation("vColor");
    int normalLocation = m_shaderProgram->getAttributeLocation("vNormal");
    int texcoordLocation = m_shaderProgram->getAttributeLocation("vTexCoord");
    int modelLocation = m_shaderProgram->getUniformLocation("modelMat");
    int nitLocation = m_shaderProgram->getUniformLocation("NIT");
    int texsamplerLocation = m_shaderProgram->getUniformLocation("texSampler");

    //Send material uniform to GPU
    Material::sendToGPU(m_shaderProgram, m_material);

    //Send data to GPU
    if(modelLocation != ShaderProgram::null_location)
    {
        glcheck(glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(getModelMatrix())));
    }

    if(positionLocation != ShaderProgram::null_location)
    {
        //Activate location
        glcheck(glEnableVertexAttribArray(positionLocation));
        //Bind buffer
        glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_pBuffer));
        //Specify internal format
        glcheck(glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));
    }

    if(colorLocation != ShaderProgram::null_location)
    {
        glcheck(glEnableVertexAttribArray(colorLocation));
        glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_cBuffer));
        glcheck(glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE, 0, (void*)0));
    }

    if(normalLocation != ShaderProgram::null_location)
    {
        glcheck(glEnableVertexAttribArray(normalLocation));
        glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_nBuffer));
        glcheck(glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));
    }

    if( nitLocation != ShaderProgram::null_location )
      {
        glcheck(glUniformMatrix3fv( nitLocation, 1, GL_FALSE,
          glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(getModelMatrix()))))));
      }

    //Bind texture in Textured Unit 0
    if(texcoordLocation != ShaderProgram::null_location)
    {
        glcheck(glActiveTexture(GL_TEXTURE0));
        glcheck(glBindTexture(GL_TEXTURE_2D, m_texId));
        //Send "texSampler" to Textured Unit 0
        glcheck(glUniform1i(texsamplerLocation, 0));
        glcheck(glEnableVertexAttribArray(texcoordLocation));
        glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_tBuffer));
        glcheck(glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE, 0, (void*)0));
    }


    //Draw triangles elements
    glcheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iBuffer));
    glcheck(glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, (void*)0));

    if(positionLocation != ShaderProgram::null_location)
    {
        glcheck(glDisableVertexAttribArray(positionLocation));
    }

    if(colorLocation != ShaderProgram::null_location)
    {
        glcheck(glDisableVertexAttribArray(colorLocation));
    }

    if(normalLocation != ShaderProgram::null_location)
    {
        glcheck(glDisableVertexAttribArray(normalLocation));
    }
    if(texcoordLocation != ShaderProgram::null_location)
    {
        glcheck(glDisableVertexAttribArray(texcoordLocation ));
    }
}

void UltimateMeshRenderable::do_animate(float time) {
    //Assign the interpolated transformations from the keyframes to the local/parent transformations.
    if (isBezier) {     //Technically, this test could be removed, but we do not want collisions between the two modes of interpolation
        int i, j = 1;
        while ( time > bezier_segmentation[j] ) {
            j++;
        }
        i = j-1;
        if(!m_BlocalKeyframes.empty()) {
            setLocalTransform( m_BlocalKeyframes.interpolateTransformation( time, bezier_segmentation[i], bezier_segmentation[j] ));
        }
        if(!m_BparentKeyframes.empty()) {
            // std::cout << "Call parent keyframe | " << bezier_segmentation[i] << " :: " << bezier_segmentation[j] << "\n";
            setParentTransform( m_BparentKeyframes.interpolateTransformation( time, bezier_segmentation[i], bezier_segmentation[j] ));
        }
    } else {
        if(!m_localKeyframes.empty()) {
            setLocalTransform( m_localKeyframes.interpolateTransformation( time ) );
        }
        if(!m_parentKeyframes.empty()) {
            setParentTransform( m_parentKeyframes.interpolateTransformation( time ) );
        }
    }
}

void UltimateMeshRenderable::setMaterial(const MaterialPtr& material)
{
    m_material = material;
}
