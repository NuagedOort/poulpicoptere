#ifndef ULTIMATE_MESH_RENDERABLE_HPP
#define ULTIMATE_MESH_RENDERABLE_HPP

#include "./../HierarchicalRenderable.hpp"
#include "./../lighting/Material.hpp"
#include "./../lighting/Light.hpp"
#include "./../BezierKeyframeCollection.hpp"
#include "./../KeyframeCollection.hpp"

#include <string>
#include <vector>
#include <glm/glm.hpp>

class UltimateMeshRenderable : public HierarchicalRenderable
{
    public:
        ~UltimateMeshRenderable();
        UltimateMeshRenderable(
            ShaderProgramPtr program,
            const std::string& mesh_filename,
            const std::string& texture_filename,
            const float endAnimation = -1.0 );
        void setMaterial(const MaterialPtr& material);
        void addParentTransformKeyframe( const GeometricTransformation& transformation, float time );
        void addLocalTransformKeyframe( const GeometricTransformation& transformation, float time );
        void setBezierSegment( const std::vector< float > & segment );

    private:
        void do_draw();
        void do_animate( float time );

        bool isBezier;
        std::vector< float > bezier_segmentation;

        std::vector< glm::vec3 > m_positions;
        std::vector< glm::vec3 > m_normals;
        std::vector< glm::vec4 > m_colors;
        std::vector< glm::vec2 > m_texCoords;
        std::vector< unsigned int > m_indices;

        BezierKeyframeCollection m_BlocalKeyframes;     /*!< A collection of keyframes for the local transformation of renderable. */
        BezierKeyframeCollection m_BparentKeyframes;    /*!< A collection of keyframes for the parent transformation of renderable. */
        KeyframeCollection m_localKeyframes;            /*!< A collection of keyframes for the local transformation of renderable. */
        KeyframeCollection m_parentKeyframes;           /*!< A collection of keyframes for the parent transformation of renderable. */

        unsigned int m_pBuffer;
        unsigned int m_cBuffer;
        unsigned int m_nBuffer;
        unsigned int m_iBuffer;
        unsigned int m_tBuffer;
        unsigned int m_texId;

        MaterialPtr m_material;
};

typedef std::shared_ptr<UltimateMeshRenderable> UltimateMeshRenderablePtr;

#endif