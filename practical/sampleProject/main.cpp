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

#define ANITIME 23.0f
#define FPS 24.0f

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
            orientation = glm::quat(glm::vec3(0,(acc_threshold*acc_threshold)*rel + rel*rel,0));
         }
        blades->addLocalTransformKeyframe(GeometricTransformation(translation, orientation, scale), i);
    }
}

void animationObj(
    Viewer& viewer,
    ShaderProgramPtr texShader,
    const std::string& basepath,
    const int fileNumber,
    const int step,
    const std::string texture,
    const float start ) 
{            
    glm::vec3 translation = glm::vec3{0,-0.5,0};
    glm::quat orientation = glm::quat{1,0,0,0};
    glm::vec3 scale = glm::vec3{1,1,1};

    UltimateMeshRenderablePtr arr[fileNumber];

    for (int i = 0; i<fileNumber; i = i+step) {
        arr[i] = std::make_shared<UltimateMeshRenderable>(
            texShader,
            basepath + std::to_string(i) +".obj",
            basepath + std::to_string(i) +".mtl",
            texture);

        float inst = start + (i/FPS);
        // Size = 0 : Model not visible
        arr[i]->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), start);
        arr[i]->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), inst-.001f);
        // Scale to the right size at appropriate moment
        scale = glm::vec3{1,1,1};
        arr[i]->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), inst);
        arr[i]->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), inst + (step/FPS));
        // Scale back to 0 one keyframe later (according to given FPS rate)
        scale = glm::vec3{0,0,0};
        arr[i]->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), inst + (step/FPS)+.001f);

        viewer.addRenderable(arr[i]);
    }
}

void buildWarehouse ( Viewer& viewer ){
    ShaderProgramPtr texShader = std::make_shared<ShaderProgram>(   "../../sfmlGraphicsPipeline/shaders/textureVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/textureFragment.glsl");
    viewer.addShaderProgram( texShader );
    UltimateMeshRenderablePtr warehouse_colums = std::make_shared<UltimateMeshRenderable>(
        texShader,
        "./../../sfmlGraphicsPipeline/meshes/warehouse_mono/wh_columns.obj",
        "./../../sfmlGraphicsPipeline/meshes/warehouse_mono/wh_columns.mtl",
        "./../../sfmlGraphicsPipeline/meshes/warehouse_mono/concrete_color.png");

    UltimateMeshRenderablePtr warehouse_floor = std::make_shared<UltimateMeshRenderable>(
        texShader,
        "./../../sfmlGraphicsPipeline/meshes/warehouse_mono/wh_floor.obj",
        "./../../sfmlGraphicsPipeline/meshes/warehouse_mono/wh_floor.mtl",
        "./../../sfmlGraphicsPipeline/meshes/warehouse_mono/Floor_Color.png");

    UltimateMeshRenderablePtr warehouse_gates = std::make_shared<UltimateMeshRenderable>(
        texShader,
        "./../../sfmlGraphicsPipeline/meshes/warehouse_mono/wh_gates.obj",
        " ",
        "./../../sfmlGraphicsPipeline/meshes/warehouse_mono/gates_color.png");

    UltimateMeshRenderablePtr warehouse_glass = std::make_shared<UltimateMeshRenderable>(
        texShader,
        "./../../sfmlGraphicsPipeline/meshes/warehouse_mono/wh_glass.obj",
        "./../../sfmlGraphicsPipeline/meshes/warehouse_mono/wh_glass.mtl",
        "./../../sfmlGraphicsPipeline/meshes/warehouse_mono/Floor_Color.png");

    UltimateMeshRenderablePtr warehouse_metallic = std::make_shared<UltimateMeshRenderable>(
        texShader,
        "./../../sfmlGraphicsPipeline/meshes/warehouse_mono/wh_metallic.obj",
        "./../../sfmlGraphicsPipeline/meshes/warehouse_mono/wh_metallic.mtl",
        "./../../sfmlGraphicsPipeline/meshes/warehouse_mono/Metal_color.png");

    UltimateMeshRenderablePtr warehouse_roof = std::make_shared<UltimateMeshRenderable>(
        texShader,
        "./../../sfmlGraphicsPipeline/meshes/warehouse_mono/wh_roof.obj",
        "./../../sfmlGraphicsPipeline/meshes/warehouse_mono/wh_roof.mtl",
        "./../../sfmlGraphicsPipeline/meshes/warehouse_mono/roof_color.png");

    UltimateMeshRenderablePtr warehouse_wall = std::make_shared<UltimateMeshRenderable>(
        texShader,
        "./../../sfmlGraphicsPipeline/meshes/warehouse_mono/wh_wall.obj",
        "./../../sfmlGraphicsPipeline/meshes/warehouse_mono/wh_wall.mtl",
        "./../../sfmlGraphicsPipeline/meshes/warehouse_mono/Bricks01_COL_VAR1_3K.jpg");

    HierarchicalRenderable::addChild(warehouse_floor, warehouse_gates);
    HierarchicalRenderable::addChild(warehouse_floor, warehouse_glass);
    HierarchicalRenderable::addChild(warehouse_floor, warehouse_metallic);
    HierarchicalRenderable::addChild(warehouse_floor, warehouse_colums);
    HierarchicalRenderable::addChild(warehouse_floor, warehouse_roof);
    HierarchicalRenderable::addChild(warehouse_floor, warehouse_wall);

    glm::vec3 translation = glm::vec3{0,0,0};
    glm::quat orientation = glm::quat{1,0,0,0};
    glm::vec3 scale = glm::vec3{1.5f,1.5f,1.5f};

    warehouse_floor->setParentTransform(GeometricTransformation(translation, orientation, scale).toMatrix());

    viewer.addRenderable(warehouse_colums);
    viewer.addRenderable(warehouse_floor);
    viewer.addRenderable(warehouse_gates);
    viewer.addRenderable(warehouse_glass);
    viewer.addRenderable(warehouse_metallic);
    viewer.addRenderable(warehouse_roof);
    viewer.addRenderable(warehouse_wall);
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
    ShaderProgramPtr texShader = std::make_shared<ShaderProgram>(   "../../sfmlGraphicsPipeline/shaders/textureVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/textureFragment.glsl");
    viewer.addShaderProgram( texShader );

    //Multitextured shader
    ShaderProgramPtr multiShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/multiTextureVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/multiTextureFragment.glsl");
    viewer.addShaderProgram( multiShader );

    //Textures
    std::string texMipMap, texBun, texHalf, texFlower, texMetal;
    texMipMap = "./../../sfmlGraphicsPipeline/textures/mipmap1.png";
    texFlower = "./../../sfmlGraphicsPipeline/textures/billboardredflowers.png";
    texHalf = "./../../sfmlGraphicsPipeline/textures/halflife.png";
    texBun = "./../../sfmlGraphicsPipeline/textures/TexturedBunny.png";
    texMetal = "./../../sfmlGraphicsPipeline/textures/poulpi/MetalBare.jpg";

    PointLightPtr pointLight1 = std::make_shared<PointLight>(glm::vec3(0, 10.0, 0),
            glm::vec3(0.0,0.0,0.0),
            glm::vec3(1.0,1.0,1.0), 
            glm::vec3(0.5,0.8,1.0),
            1.0,
            5e-1,
            0);

    /********************************** Poulpicoptere***********************************/


    UltimateMeshRenderablePtr PoulpicoptereCorps = std::make_shared<UltimateMeshRenderable>(
        texShader,
        "./../../sfmlGraphicsPipeline/meshes/Poulpicoptere2-Corps.obj",
        "./../../sfmlGraphicsPipeline/meshes/Poulpicoptere2-Corps.mtl",
        texMetal);
    //PoulpicoptereCorps->setMaterial(mat);
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

    float offset = 3.0f;
    //float offset = 0.0f;
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

    UltimateMeshRenderablePtr temp_Corps = std::make_shared<UltimateMeshRenderable>(
        texShader,
        "./../../sfmlGraphicsPipeline/meshes/Poulpicoptere2-Corps.obj",
        "./../../sfmlGraphicsPipeline/meshes/Poulpicoptere2-Corps.mtl",
        texMetal);

    translation = glm::vec3{0,0,0};
    orientation = glm::quat(glm::vec3(0,0,0));
    scale = glm::vec3{1,1,1};
    temp_Corps->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), 0);
    temp_Corps->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), -0.01f + offset);

    scale = glm::vec3{0,0,0};
    temp_Corps->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), 0.0f + offset);

    //animationObj(viewer, texShader, "./../../sfmlGraphicsPipeline/meshes/Poulpicoptere_Animation/Poulpicoptere2_Animation_", 72, 2, texMetal, 0.0f);

    bladesRotation(PoulpicopterePales, offset, ANITIME, 0.7f); 

    //buildWarehouse(viewer);

    viewer.addPointLight(pointLight1);
    viewer.addRenderable(PoulpicoptereCorps);
    viewer.addRenderable(PoulpicopterePales);
    viewer.addRenderable(temp_Corps);
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
