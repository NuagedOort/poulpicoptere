#include <Viewer.hpp>
#include <ShaderProgram.hpp>

#include <GeometricTransformation.hpp>

#include <lighting/DirectionalLightRenderable.hpp>
#include <lighting/SpotLightRenderable.hpp>
#include <texturing/TexturedLightedMeshRenderable.hpp>
#include <texturing/UltimateMeshRenderable.hpp>

#include <FrameRenderable.hpp>

#include <iostream>

#define ANITIME 40.0f
#define FPS 24.0f
#define PI 3.1415926535897f

void bladesRotation(UltimateMeshRenderablePtr blades, float start, float end, float acc_threshold) {
    glm::vec3 translation = glm::vec3{0,0,0};
    glm::quat orientation = glm::quat{1,0,0,0};
    glm::vec3 scale = glm::vec3{1,1,1};
    float constRel = 0.0f;

    for(float i = start; i < end; i = i+0.1f){
        float rel = i - start;
        if(rel <= acc_threshold) { //Acceleration
            orientation = glm::quat(glm::vec3(0,rel*rel,0));
            constRel = rel*rel;
         } else {   // Constant part
            orientation = glm::quat(glm::vec3(0,(acc_threshold*acc_threshold)*rel + constRel,0));
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

void addLightDirSwitch(Viewer & viewer, SpotLightRenderablePtr spot, float time, std::string val)
{
    if(val=="on"){
        spot->add_switch(time,true);
    }
    if(val=="off"){
        spot->add_switch(time,false);
    }
}

void buildWarehouse ( Viewer& viewer, ShaderProgramPtr shader ){
    MaterialPtr concrete = std::make_shared<Material>(
        glm::vec3{0.0f,0.0f,0.0f}, glm::vec3{0.6f,0.6f,0.6f}, glm::vec3{0.2f,0.2f,0.2f}, 0.2f);

    MaterialPtr floor = std::make_shared<Material>(
        glm::vec3{0.0f,0.0f,0.0f}, glm::vec3{1.0f,1.0f,1.0f}, glm::vec3{0.2f,0.2f,0.2f}, 0.2f);

    UltimateMeshRenderablePtr warehouse_colums = std::make_shared<UltimateMeshRenderable>(
        shader,
        "./../../sfmlGraphicsPipeline/meshes/warehouse_mono/wh_columns.obj",
        "./../../sfmlGraphicsPipeline/meshes/warehouse_mono/concrete_color.png");
    warehouse_colums->setMaterial(concrete);

    UltimateMeshRenderablePtr warehouse_floor = std::make_shared<UltimateMeshRenderable>(
        shader,
        "./../../sfmlGraphicsPipeline/meshes/warehouse_mono/wh_floor.obj",
        "./../../sfmlGraphicsPipeline/meshes/warehouse_mono/Floor_Color.png");
    warehouse_floor->setMaterial(floor);

    UltimateMeshRenderablePtr warehouse_gates = std::make_shared<UltimateMeshRenderable>(
        shader,
        "./../../sfmlGraphicsPipeline/meshes/warehouse_mono/wh_gates.obj",
        "./../../sfmlGraphicsPipeline/meshes/warehouse_mono/gates_color.png");

    // UltimateMeshRenderablePtr warehouse_glass = std::make_shared<UltimateMeshRenderable>(
    //     texShader,
    //     "./../../sfmlGraphicsPipeline/meshes/warehouse_mono/wh_glass.obj",
    //     "./../../sfmlGraphicsPipeline/meshes/warehouse_mono/Floor_Color.png");

    UltimateMeshRenderablePtr warehouse_metallic = std::make_shared<UltimateMeshRenderable>(
        shader,
        "./../../sfmlGraphicsPipeline/meshes/warehouse_mono/wh_metallic.obj",
        "./../../sfmlGraphicsPipeline/meshes/warehouse_mono/Metal_color.png");

    UltimateMeshRenderablePtr warehouse_roof = std::make_shared<UltimateMeshRenderable>(
        shader,
        "./../../sfmlGraphicsPipeline/meshes/warehouse_mono/wh_roof.obj",
        "./../../sfmlGraphicsPipeline/meshes/warehouse_mono/roof_color.png");

    UltimateMeshRenderablePtr warehouse_walls = std::make_shared<UltimateMeshRenderable>(
        shader,
        "./../../sfmlGraphicsPipeline/meshes/warehouse_mono/wh_wall_concrete.obj",
        "./../../sfmlGraphicsPipeline/meshes/warehouse_mono/walls_color.png");
    warehouse_walls->setMaterial(concrete);

    HierarchicalRenderable::addChild(warehouse_floor, warehouse_gates);
    // HierarchicalRenderable::addChild(warehouse_floor, warehouse_glass);
    HierarchicalRenderable::addChild(warehouse_floor, warehouse_metallic);
    HierarchicalRenderable::addChild(warehouse_floor, warehouse_colums);
    HierarchicalRenderable::addChild(warehouse_floor, warehouse_roof);
    HierarchicalRenderable::addChild(warehouse_floor, warehouse_walls);

    glm::vec3 translation = glm::vec3{0,8,2};
    glm::quat orientation = glm::quat{1,0,0,0};
    glm::vec3 scale = glm::vec3{2.2f,2.2f,2.2f};

    warehouse_floor->setParentTransform(GeometricTransformation(translation, orientation, scale).toMatrix());

    viewer.addRenderable(warehouse_colums);
    viewer.addRenderable(warehouse_floor);
    viewer.addRenderable(warehouse_gates);
    // viewer.addRenderable(warehouse_glass);
    viewer.addRenderable(warehouse_metallic);
    viewer.addRenderable(warehouse_roof);
    viewer.addRenderable(warehouse_walls);

    /********************************** Lighting ***********************************/

    glm::mat4 localTransformation = glm::scale(glm::mat4(1.0), glm::vec3(0.5,0.5,0.5));

    SpotLightPtr spotLight = std::make_shared<SpotLight>(
        glm::vec3 {.0, 12.0, .0}, //Location
        glm::normalize(glm::vec3 {.0,-1.0, .0}), //Direction
        glm::vec3 {.0, .0, .0}, //Ambient
        glm::vec3 {1.0, 0.90, .7}, //Diffuse
        glm::vec3 {0.12, 0.11, 0.1}, //Specular
        1.0, 0.0, 0.0,
        std::cos(glm::radians(15.0f)),
        std::cos(glm::radians(30.0f)));
    SpotLightRenderablePtr spotLightRenderable = std::make_shared<SpotLightRenderable>(shader, spotLight);
    spotLightRenderable->setLocalTransform(localTransformation);

    SpotLightPtr spotLight2 = std::make_shared<SpotLight>(
        glm::vec3 {20.0, 12.0, .0}, //Location
        glm::normalize(glm::vec3 {.0,-1.0, .0}), //Direction
        glm::vec3 {.0, .0, .0}, //Ambient
        glm::vec3 {1.0, 0.90, .7}, //Diffuse
        glm::vec3 {0.12, 0.11, 0.1}, //Specular
        1.0, 0.0, 0.0,
        std::cos(glm::radians(15.0f)),
        std::cos(glm::radians(30.0f)));
    SpotLightRenderablePtr spotLightRenderable2 = std::make_shared<SpotLightRenderable>(shader, spotLight2);
    spotLightRenderable2->setLocalTransform(localTransformation);

    SpotLightPtr spotLight3 = std::make_shared<SpotLight>(
        glm::vec3 {40.0, 12.0, .0}, //Location
        glm::normalize(glm::vec3 {.0,-1.0, .0}), //Direction
        glm::vec3 {.0, .0, .0}, //Ambient
        glm::vec3 {1.0, 0.90, .7}, //Diffuse
        glm::vec3 {0.12, 0.11, 0.1}, //Specular
        1.0, 0.0, 0.0,
        std::cos(glm::radians(15.0f)),
        std::cos(glm::radians(30.0f)));
    SpotLightRenderablePtr spotLightRenderable3 = std::make_shared<SpotLightRenderable>(shader, spotLight3);
    spotLightRenderable3->setLocalTransform(localTransformation);

    SpotLightPtr spotLight4 = std::make_shared<SpotLight>(
        glm::vec3 {-20.0, 12.0, .0}, //Location
        glm::normalize(glm::vec3 {.0,-1.0, .0}), //Direction
        glm::vec3 {.0, .0, .0}, //Ambient
        glm::vec3 {1.0, 0.90, .7}, //Diffuse
        glm::vec3 {0.12, 0.11, 0.1}, //Specular
        1.0, 0.0, 0.0,
        std::cos(glm::radians(15.0f)),
        std::cos(glm::radians(30.0f)));
    SpotLightRenderablePtr spotLightRenderable4 = std::make_shared<SpotLightRenderable>(shader, spotLight4);
    spotLightRenderable4->setLocalTransform(localTransformation);

    SpotLightPtr spotLight5 = std::make_shared<SpotLight>(
        glm::vec3 {-40.0, 12.0, .0}, //Location
        glm::normalize(glm::vec3 {.0,-1.0, .0}), //Direction
        glm::vec3 {.0, .0, .0}, //Ambient
        glm::vec3 {1.0, 0.90, .7}, //Diffuse
        glm::vec3 {0.12, 0.11, 0.1}, //Specular
        1.0, 0.0, 0.0,
        std::cos(glm::radians(15.0f)),
        std::cos(glm::radians(30.0f)));
    SpotLightRenderablePtr spotLightRenderable5 = std::make_shared<SpotLightRenderable>(shader, spotLight5);
    spotLightRenderable5->setLocalTransform(localTransformation);

    viewer.addSpotLight(spotLight);
    viewer.addRenderable(spotLightRenderable);
    viewer.addSpotLight(spotLight2);
    viewer.addRenderable(spotLightRenderable2);
    viewer.addSpotLight(spotLight3);
    viewer.addRenderable(spotLightRenderable3);
    viewer.addSpotLight(spotLight4);
    viewer.addRenderable(spotLightRenderable4);
    viewer.addSpotLight(spotLight5);
    viewer.addRenderable(spotLightRenderable5);

    //turning the lights on at n seconds
    addLightDirSwitch(viewer,spotLightRenderable3,1.5,"on");
    addLightDirSwitch(viewer,spotLightRenderable2,2.0,"on");
    addLightDirSwitch(viewer,spotLightRenderable,2.5,"on");
    addLightDirSwitch(viewer,spotLightRenderable4,3.0,"on");
    addLightDirSwitch(viewer,spotLightRenderable5,3.5,"on");
}


void initialize_scene( Viewer& viewer )
{
    //Position the camera
    viewer.getCamera().setViewMatrix( glm::lookAt( glm::vec3(0, 0, 8 ), glm::vec3(0, 0, 0), glm::vec3( 0, 1, 0 ) ) );
    viewer.getCamera().setZfar(500.0f);

    //Default shader
    ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/flatVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");
    viewer.addShaderProgram( flatShader );

    //Add a 3D frame to the viewer
    // FrameRenderablePtr frame = std::make_shared<FrameRenderable>(flatShader);
    // viewer.addRenderable(frame);

    //Temporary variables
    glm::mat4 parentTransformation(1.0), localTransformation(1.0);

    //Simply lighte textured shader
    ShaderProgramPtr simpleTexShader = std::make_shared<ShaderProgram>(   "../../sfmlGraphicsPipeline/shaders/simpleTextureVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/simpleTextureFragment.glsl");
    viewer.addShaderProgram( simpleTexShader );

    //Textured shader
    ShaderProgramPtr texShader = std::make_shared<ShaderProgram>(   "../../sfmlGraphicsPipeline/shaders/textureVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/textureFragment.glsl");
    viewer.addShaderProgram( texShader );

    //Multitextured shader
    ShaderProgramPtr multiShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/multiTextureVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/multiTextureFragment.glsl");
    viewer.addShaderProgram( multiShader );

    //Textures
    std::string texMipMap, texBun, texHalf, texFlower, texMetal, texSky, texWater;
    texMipMap = "./../../sfmlGraphicsPipeline/textures/mipmap1.png";
    texFlower = "./../../sfmlGraphicsPipeline/textures/billboardredflowers.png";
    texHalf = "./../../sfmlGraphicsPipeline/textures/halflife.png";
    texBun = "./../../sfmlGraphicsPipeline/textures/TexturedBunny.png";
    texMetal = "./../../sfmlGraphicsPipeline/textures/poulpi/MetalBare.jpg";
    texSky = "./../../sfmlGraphicsPipeline/textures/cubemap.png";
    texWater = "./../../sfmlGraphicsPipeline/textures/waterbox.png";

    /********************************** Scene ***********************************/
    buildWarehouse(viewer, texShader);

    UltimateMeshRenderablePtr skybox = std::make_shared<UltimateMeshRenderable>(
         simpleTexShader,
         "./../../sfmlGraphicsPipeline/meshes/skybox.obj",
         texWater);
    // skybox->setLocalTransform(glm::scale(glm::mat4(1.0), glm::vec3(100,100,100)));
    skybox->addParentTransformKeyframe(GeometricTransformation(glm::vec3{0,0,0}, glm::quat(glm::vec3(0,-0.25,-0.2)), glm::vec3{110,110,110}), 0);
    skybox->addParentTransformKeyframe(GeometricTransformation(glm::vec3{0,0,0}, glm::quat(glm::vec3(0,0.25,0.2)), glm::vec3{110,110,110}), ANITIME);
    viewer.addRenderable(skybox);

    DirectionalLightPtr directional = std::make_shared<DirectionalLight>(glm::vec3(-3, -1, 2),
            glm::vec3(0.0, 0.0, 0.0),
            glm::vec3(0.5, 0.52, 0.56), 
            glm::vec3(0.1, 0.13, 0.15));

    viewer.setDirectionalLight(directional);

    /********************************** Poulpicoptere ***********************************/


    UltimateMeshRenderablePtr PoulpicoptereCorps = std::make_shared<UltimateMeshRenderable>(
        texShader,
        "./../../sfmlGraphicsPipeline/meshes/Poulpicoptere2-Corps.obj",
        texMetal);
    UltimateMeshRenderablePtr PoulpicopterePales = std::make_shared<UltimateMeshRenderable>(
        texShader,
        "./../../sfmlGraphicsPipeline/meshes/Poulpicoptere2-Pales.obj",
        texMetal);
    HierarchicalRenderable::addChild(PoulpicoptereCorps, PoulpicopterePales);
    
    UltimateMeshRenderablePtr tempPales = std::make_shared<UltimateMeshRenderable>(
        texShader,
        "./../../sfmlGraphicsPipeline/meshes/Poulpicoptere_Animation/Poulpicoptere2_Animation_0.obj",
        texMetal);

    //Parent Transform
    glm::vec3 translation = glm::vec3{0,0,0};
    glm::quat orientation = glm::quat{1,0,0,0};
    glm::vec3 scale = glm::vec3{0.85,0.85,0.85};
    //Local Transform
    glm::vec3 localTranslation = glm::vec3{-1000,0,0};
    glm::quat localOrientation = glm::quat{1,0,0,0};
    glm::vec3 localScale = glm::vec3{1,1,1};
    
    float offset = 6.5f;
    std::vector<float> segments{0.0f};

    PoulpicoptereCorps->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), 0);
    PoulpicopterePales->addLocalTransformKeyframe(GeometricTransformation(localTranslation, localOrientation, localScale), 0);
    tempPales->addParentTransformKeyframe(GeometricTransformation(translation, orientation, glm::vec3{1,1,1}), 0);
    tempPales->addParentTransformKeyframe(GeometricTransformation(translation, orientation, glm::vec3{1,1,1}), 3.499f);
    tempPales->addParentTransformKeyframe(GeometricTransformation(translation, orientation, glm::vec3{0,0,0}), 3.5f);

    PoulpicoptereCorps->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), -0.01 + offset);
    PoulpicopterePales->addLocalTransformKeyframe(GeometricTransformation(localTranslation, localOrientation, localScale), -0.01 + offset);

    /* Start of the flying animation */
    translation = glm::vec3{0,0,0};
    localTranslation = glm::vec3{0,0,0};
    PoulpicoptereCorps->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), 0.0 + offset);
    PoulpicopterePales->addLocalTransformKeyframe(GeometricTransformation(localTranslation, localOrientation, localScale), 0.0 + offset);
    segments.push_back(0.0+offset);

    /* Blades rotation acceleration is maximal. Poulpicoptere starts to lift */
    translation = glm::vec3{0,0,0};
    PoulpicoptereCorps->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), 5.0 + offset);

    /* Highest height, the Poulpicoptere starts a gentle inclination */
    translation = glm::vec3{-0.5f,6,0};
    orientation = glm::quat(glm::vec3(0,0,0));
    PoulpicoptereCorps->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), 10.0 + offset);

    translation = glm::vec3{-1,5,0};
    PoulpicoptereCorps->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), 12.0 + offset);
    
    /* The Poulpicoptere starts to slowly move forward */
    translation = glm::vec3{-8,6,0};
    PoulpicoptereCorps->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), 15.0 + offset); 

    /* The Poulpicoptere accelerate forward */
    translation = glm::vec3{-24,6,0};
    PoulpicoptereCorps->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), 17.0 + offset);

    /* The Poulpicoptère quickly reaches the end on the hangar*/
    translation = glm::vec3{-60,5,0};
    orientation = glm::quat(glm::vec3(0,0,0.2));
    PoulpicoptereCorps->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), 20.0 + offset);
    

    /* The Poulpicoptere leans backward to brake and starts his turn*/
    translation = glm::vec3{-70,5,-15};
    //orientation = glm::quat(glm::vec3(0,0,0));
    orientation = glm::quat(glm::vec3(0,-PI/2,0));
    PoulpicoptereCorps->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), 21.0 + offset);
    segments.push_back(20.0+offset);

    /* The Poullpicoptère swings back a bit and tries to right itself */
    translation = glm::vec3{-75,5,-20};
    //orientation = glm::quat(glm::vec3(0,-PI/2,0));
    orientation = glm::quat(glm::vec3(0,PI,-0.2));
    PoulpicoptereCorps->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), 22.0 + offset);


    /* The poulpicoptère turns around to face a new direction*/
    translation = glm::vec3{-60,5,-20};
    orientation = glm::quat(glm::vec3(0,PI,-0.2));
    PoulpicoptereCorps->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), 23.0 + offset);
    //segments.push_back(23.0+offset);

    /* Nyoooom to the hangar middle*/
    translation = glm::vec3{5,5,-20};
    orientation = glm::quat(glm::vec3(0,PI,-0.2));
    PoulpicoptereCorps->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), 26.0 + offset);
    segments.push_back(26.0+offset);

    /* First half of the drift turn */
    translation = glm::vec3{10,5,-10};
    orientation = glm::quat(glm::vec3(0.2,PI/2,-0.2));
    PoulpicoptereCorps->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), 26.5 + offset);

    /* Second half of the turn */
    translation = glm::vec3{20,5,5};
    orientation = glm::quat(glm::vec3(0.2,0,-0.2));
    PoulpicoptereCorps->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), 27.0 + offset);
    //segments.push_back(27.0+offset);

    // /* Recenter and move forward a bit */
    // translation = glm::vec3{0,5,0};
    // orientation = glm::quat(glm::vec3(0.2,0,0.2));
    // PoulpicoptereCorps->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), 27.5 + offset);

    // translation = glm::vec3{-10,5,0};
    // orientation = glm::quat(glm::vec3(0.2,0,0.2));
    // PoulpicoptereCorps->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), 28.0 + offset);
    //segments.push_back(28.0+offset);

    /* First half of the drift turn */
    translation = glm::vec3{-15,5,10};
    orientation = glm::quat(glm::vec3(0.2,PI/2,-0.2));
    PoulpicoptereCorps->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), 28.0 + offset);

    /* Second half of the turn */
    translation = glm::vec3{-25,5,25};
    orientation = glm::quat(glm::vec3(0,PI,-0.2));
    PoulpicoptereCorps->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), 29.0 + offset);
    segments.push_back(29.0+offset);

    /* Move to the door */
    translation = glm::vec3{-7,2,25};
    orientation = glm::quat(glm::vec3(0,PI,-0.1));
    PoulpicoptereCorps->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), 29.5 + offset);

    translation = glm::vec3{-5,1,25};
    orientation = glm::quat(glm::vec3(0,PI,0));
    PoulpicoptereCorps->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), 30.0 + offset);
    //segments.push_back(30.0+offset);

    /* Turn to face the door */
    translation = glm::vec3{-5,1,25};
    orientation = glm::quat(glm::vec3(0,PI/2,0));
    PoulpicoptereCorps->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), 31.0 + offset);

    /* Lean to prepare the nyyom */
    translation = glm::vec3{-5,1,25};
    orientation = glm::quat(glm::vec3(0,PI/2,0));
    PoulpicoptereCorps->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), 32.0 + offset);
    segments.push_back(32.0+offset);

    /* Nyyoooom */
    translation = glm::vec3{-5,1,150};
    orientation = glm::quat(glm::vec3(0,PI/2,0));
    PoulpicoptereCorps->addParentTransformKeyframe(GeometricTransformation(translation, orientation, scale), 32.5 + offset);
    segments.push_back(ANITIME);

    animationObj(viewer, texShader, "./../../sfmlGraphicsPipeline/meshes/Poulpicoptere_Animation/Poulpicoptere2_Animation_", 72, 2, texMetal, 3.5f);
    // float half = 20.0f + offset;
    PoulpicoptereCorps->setBezierSegment(segments);

    bladesRotation(PoulpicopterePales, offset, ANITIME, 3.0f); 
    
    viewer.addRenderable(PoulpicoptereCorps);
    viewer.addRenderable(PoulpicopterePales);
    viewer.addRenderable(tempPales);

    viewer.getCamera().setTarget(PoulpicoptereCorps);
    
    translation = glm::vec3{40,1,0};
    orientation = glm::quat{1,0,0,0};
    scale = glm::vec3{1,1,1};
    viewer.getCamera().addKeyframe(GeometricTransformation(translation, orientation, scale), 0.0);

    translation = glm::vec3{0,1,5};
    viewer.getCamera().addKeyframe(GeometricTransformation(translation, orientation, scale), 0.75f);

    translation = glm::vec3{-40,1,0};
    viewer.getCamera().addKeyframe(GeometricTransformation(translation, orientation, scale), 1.25f);

    translation = glm::vec3{-40,10,0};
    viewer.getCamera().addKeyframe(GeometricTransformation(translation, orientation, scale), 8.0);

    translation = glm::vec3{-40,10,0};
    viewer.getCamera().addKeyframe(GeometricTransformation(translation, orientation, scale), 19.999+offset);
    translation = glm::vec3{-80,10,25};
    viewer.getCamera().addKeyframe(GeometricTransformation(translation, orientation, scale), 20+offset);

    translation = glm::vec3{-80,10,0};
    viewer.getCamera().addKeyframe(GeometricTransformation(translation, orientation, scale), 24.999+offset);
    translation = glm::vec3{40,8,0};
    viewer.getCamera().addKeyframe(GeometricTransformation(translation, orientation, scale), 25.0+offset);

    translation = glm::vec3{40,10,0};
    viewer.getCamera().addKeyframe(GeometricTransformation(translation, orientation, scale), 26.49999+offset);
    translation = glm::vec3{-20,10,0};
    viewer.getCamera().addKeyframe(GeometricTransformation(translation, orientation, scale), 26.5+offset);

    
    translation = glm::vec3{-30,10,-25};
    viewer.getCamera().addKeyframe(GeometricTransformation(translation, orientation, scale), 28+offset);

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
