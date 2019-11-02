#include <Viewer.hpp>
#include <ShaderProgram.hpp>

#include <GeometricTransformation.hpp>

#include <texturing/TexturedPlaneRenderable.hpp>
#include <texturing/TexturedCubeRenderable.hpp>
#include <texturing/MultiTexturedCubeRenderable.hpp>
#include <texturing/MipMapCubeRenderable.hpp>
#include <texturing/BillBoardPlaneRenderable.hpp>
#include <lighting/DirectionalLightRenderable.hpp>
#include <texturing/TexturedTriangleRenderable.hpp>
#include <texturing/TexturedLightedMeshRenderable.hpp>
#include <texturing/UltimateMeshRenderable.hpp>

#include <FrameRenderable.hpp>

#include <iostream>

#define ANITIME 25.0f
#define FPS 1.0f

void bladesRotation(UltimateMeshRenderablePtr blades, float start, float end, float acc_threshold) {
    glm::vec3 translation = glm::vec3{0,0,0};
    glm::quat orientation = glm::quat{1,0,0,0};
    glm::vec3 scale = glm::vec3{1,1,1};

    for(float i = start; i < end; i = i+0.1f){
        float rel = i - start;
        if(rel <= acc_threshold) {
            orientation = glm::quat(glm::vec3(0,rel*rel,0));
         } else {
            //orientation = glm::quat(glm::vec3(0,(rel-acc_threshold)*9.0f+25.0f,0));
            orientation = glm::quat(glm::vec3(0,(acc_threshold*acc_threshold)*rel,0));
         }
        blades->addLocalTransformKeyframe(GeometricTransformation(translation, orientation, scale), i);
    }
}

void initialize_scene( Viewer& viewer )
{
    //Position the camera
    viewer.getCamera().setViewMatrix( glm::lookAt( glm::vec3(0, 0, 8 ), glm::vec3(0, 0, 0), glm::vec3( 0, 1, 0 ) ) );

    //Default shader
    ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/flatVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");
    viewer.addShaderProgram( flatShader );

    //Add a 3D frame to the viewer
    FrameRenderablePtr frame = std::make_shared<FrameRenderable>(flatShader);
    viewer.addRenderable(frame);

    //Temporary variables
    glm::mat4 parentTransformation(1.0), localTransformation(1.0);

    //Textured shader
    //    ShaderProgramPtr texShader = std::make_shared<ShaderProgram>("../shaders/textureVertex.glsl","../shaders/textureFragment.glsl");
    ShaderProgramPtr texShader = std::make_shared<ShaderProgram>(   "../../sfmlGraphicsPipeline/shaders/simpleTextureVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/simpleTextureFragment.glsl");
    viewer.addShaderProgram( texShader );

    //Multitextured shader
    ShaderProgramPtr multiShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/multiTextureVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/multiTextureFragment.glsl");
    viewer.addShaderProgram( flatShader );

    //Textures
    std::string texMipMap, texBun, texHalf, texFlower, texMetal;
    texMipMap = "./../../sfmlGraphicsPipeline/textures/mipmap1.png";
    texFlower = "./../../sfmlGraphicsPipeline/textures/billboardredflowers.png";
    texHalf = "./../../sfmlGraphicsPipeline/textures/halflife.png";
    texBun = "./../../sfmlGraphicsPipeline/textures/TexturedBunny.png";
    texMetal = "./../../sfmlGraphicsPipeline/textures/poulpi/MetalBare.jpg";

    PointLightPtr pointLight1 = std::make_shared<PointLight>(glm::vec3(5.0, 5.0, 3.0),
            glm::vec3(0.0,0.0,0.0),
            glm::vec3(1.0,0.0,0.0), 
            glm::vec3(1.0,0.0,0.0),
            1.0,
            5e-1,
            0);

    /********************************** Poulpicoptere***********************************/


    UltimateMeshRenderablePtr PoulpicoptereCorps = std::make_shared<UltimateMeshRenderable>(
        texShader,
        "./../../sfmlGraphicsPipeline/meshes/Poulpicoptere2-Corps.obj",
        "./../../sfmlGraphicsPipeline/meshes/Poulpicoptere2-Corps.mtl",
        texMetal);
    UltimateMeshRenderablePtr PoulpicopterePales = std::make_shared<UltimateMeshRenderable>(
        texShader,
        "./../../sfmlGraphicsPipeline/meshes/Poulpicoptere2-Pales.obj",
        "./../../sfmlGraphicsPipeline/meshes/Poulpicoptere2-Pales.mtl",
        texMetal);
    HierarchicalRenderable::addChild(PoulpicoptereCorps, PoulpicopterePales);

    glm::vec3 translation = glm::vec3{0,0,0};
    glm::quat orientation = glm::quat{1,0,0,0};
    glm::vec3 scale = glm::vec3{0,0,0};
    PoulpicoptereCorps->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), 0);

    float offset = 10.0f;

    PoulpicoptereCorps->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), -0.01 + offset);

    scale = glm::vec3{1,1,1};
    PoulpicoptereCorps->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), 0.0 + offset);
    
    translation = glm::vec3{0,0,0};
    orientation = glm::quat(glm::vec3(0,0,0));
    scale = glm::vec3{1,1,1}; 
    PoulpicoptereCorps->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), 5.0 + offset);

    translation = glm::vec3{0,6,0};
    orientation = glm::quat(glm::vec3(0,0,0));
    scale = glm::vec3{1,1,1};
    PoulpicoptereCorps->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), 10.0 + offset);
        
    translation = glm::vec3{-8,6,0};
    orientation = glm::quat(glm::vec3(0,0,0));
    scale = glm::vec3{1,1,1};
    PoulpicoptereCorps->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), 15.0 + offset); 

    translation = glm::vec3{-24,6,0};
    orientation = glm::quat(glm::vec3(0,0,0));
    scale = glm::vec3{1,1,1};
    PoulpicoptereCorps->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), 20.0 + offset);

    /* translation = glm::vec3{-2,0,0};
    orientation = glm::quat(glm::vec3(0,0,0));
    scale = glm::vec3{1,1,1};
    PoulpicoptereCorps->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), 10.0); */

    bladesRotation(PoulpicopterePales, offset, ANITIME, 0.7f); 

    viewer.addPointLight(pointLight1);
    viewer.addRenderable(PoulpicoptereCorps);
    viewer.addRenderable(PoulpicopterePales);
    viewer.startAnimation();
    viewer.setAnimationLoop(true, ANITIME);
}

int main() 
{
	Viewer viewer(1280,720);
	initialize_scene(viewer);

	while( viewer.isRunning() )
	{
		viewer.handleEvent();
		viewer.animate();
		viewer.draw();
		viewer.display();
	}	

	return EXIT_SUCCESS;
}
