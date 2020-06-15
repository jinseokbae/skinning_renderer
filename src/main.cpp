#define GLM_ENABLE_EXPERIMENTAL
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "opengl_utils.h"
#include "geometry_primitives.h"
#include <iostream>
#include <vector>
#include "camera.h"
#include "texture.h"
#include "texture_cube.h"
#include "model.h"
#include "mesh.h"
#include "scene.h"
#include "math_utils.h"
#include "light.h"

#include <glm/gtx/string_cast.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window, DirectionalLight* sun);

bool isWindowed = true;
bool isKeyboardDone[1024] = { 0 };

// setting
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;
const unsigned int MAX_BONES = 80;
const float MAX_ERROR_DISTANCE = 1.0f;
const float planeSize = 100.f;

// camera
Camera camera(glm::vec3(0.0f, 4.0f, 20.0f),glm::vec3(0.0,0.995,0.1));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

bool useLighting = true;
bool useNormalMap = true;
bool useSpecularMap = true;
bool useShadow = true;

bool changeFlag1 = false;
bool changeFlag2 = false;
bool changeFlag3 = false;
bool changeFlagSpace = false;

bool startFlag = false;
float startTime;


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
    float bone_color[MAX_BONES][3] =
            {
                    {0.5041465021495181,0.05779669434712942,0.6072212430690112},//color of bone[0]
                    {0.7498160795867457,0.588761379237871,0.039692492291306225},//color of bone[1]
                    {0.8304916673453047,0.8639274860456219,0.9039456799618084},//color of bone[2]
                    {0.9882589491650949,0.5791571515649542,0.17546337137041434},//color of bone[3]
                    {0.5079970295130052,0.3316793511984759,0.46300528737001523},//color of bone[4]
                    {0.6613290401527622,0.7926456200470111,0.967915619494014},//color of bone[5]
                    {0.9569567523372782,0.19351155812307475,0.829262665230657},//color of bone[6]
                    {0.7515479696532832,0.7593753889240491,0.5596947459562461},//color of bone[7]
                    {0.2049567180467562,0.3338852516389955,0.4352552383388518},//color of bone[8]
                    {0.8389909542877028,0.7983354769024739,0.4801809223904202},//color of bone[9]
                    {0.4164621848770216,0.368600012358854,0.6509668369381921},//color of bone[10]
                    {0.9023430143864234,0.8002398666733638,0.22873454253548342},//color of bone[11]
                    {0.4872493610577241,0.7474051978667612,0.3453332079303213},//color of bone[12]
                    {0.31165841385800863,0.37862434558908453,0.46728482548831596},//color of bone[13]
                    {0.8906883781934888,0.23148787936150694,0.17312173954903354},//color of bone[14]
                    {0.07937979238969783,0.7131507665437904,0.1169130444215919},//color of bone[15]
                    {0.4237726669748565,0.2498831546611162,0.7344202080060918},//color of bone[16]
                    {0.5612352139395802,0.5571127275035153,0.7664162087800133},//color of bone[17]
                    {0.4579503089928497,0.00295950130172995,0.0285302804794082},//color of bone[18]
                    {0.16231979218695725,0.7890966645210871,0.027150335730478092},//color of bone[19]
                    {0.6039844756368989,0.5624431515225472,0.6465656614465409},//color of bone[20]
                    {0.8521743448209264,0.130675727199969,0.646483117890974},//color of bone[21]
                    {0.9472965968459415,0.6536872912010017,0.5462643455574},//color of bone[22]
                    {0.9618095476131986,0.513868729556284,0.050912366823909205},//color of bone[23]
                    {0.4498130371720487,0.9180061063938476,0.2919885325212025},//color of bone[24]
                    {0.26286864129804277,0.973693672461893,0.32152551414179564},//color of bone[25]
                    {0.615562085705357,0.24298338523568697,0.30973371003506533},//color of bone[26]
                    {0.8236243719774858,0.7004072613466702,0.8545561283005417},//color of bone[27]
                    {0.38421244202092886,0.5960616554166225,0.9774217076847482},//color of bone[28]
                    {0.6068526573393066,0.6877236945347214,0.20293869331440173},//color of bone[29]
                    {0.41451750227404227,0.3036902810203136,0.015874825371109957},//color of bone[30]
                    {0.4249876090254501,0.7935844953586217,0.6675332109082437},//color of bone[31]
                    {0.3207396318264075,0.302969972076093,0.5182802340692152},//color of bone[32]
                    {0.10336778603420793,0.11834020550961932,0.12555601246193226},//color of bone[33]
                    {0.9404764746585421,0.8096879638520224,0.42949104208880784},//color of bone[34]
                    {0.9951024224144386,0.8569472313024978,0.3312346959788145},//color of bone[35]
                    {0.5669322079760787,0.45238271072637126,0.298547796046557},//color of bone[36]
                    {0.4461007737048799,0.8730592177981796,0.17068035037118479},//color of bone[37]
                    {0.12466138740834942,0.6788232634754972,0.06093833016968664},//color of bone[38]
                    {0.9982298124323844,0.9041276002916446,0.6123321979067254},//color of bone[39]
                    {0.06762200953799324,0.4049596760521652,0.4275560430127552},//color of bone[40]
                    {0.19560367001593348,0.09892050824468823,0.8665835003169878},//color of bone[41]
                    {0.6454081228139661,0.16333087947705338,0.8072597203961076},//color of bone[42]
                    {0.5825263504765595,0.9010251142701226,0.9791041509451773},//color of bone[43]
                    {0.45009443244707315,0.2415510562607287,0.4679166492015966},//color of bone[44]
                    {0.8898271704407896,0.48976692475556394,0.5594833433688882},//color of bone[45]
                    {0.753709825248267,0.605116007872228,0.5078379699449476},//color of bone[46]
                    {0.9708820681040096,0.3045410546302276,0.19648832214423761},//color of bone[47]
                    {0.8286143506467476,0.3614366340500409,0.7719009471490867},//color of bone[48]
                    {0.5324425792626014,0.10830527883222762,0.5893456460575505},//color of bone[49]
                    {0.7403300605259622,0.9491821054916636,0.19651406392280446},//color of bone[50]
                    {0.11899793843843887,0.07305379544215529,0.5742681870250719},//color of bone[51]
                    {0.6991573463733993,0.0811139006911551,0.28948623153735786},//color of bone[52]
                    {0.06193843165309054,0.7011187461471619,0.11092570823306369},//color of bone[53]
                    {0.5504920478710731,0.09528652897104972,0.7093260169921479},//color of bone[54]
                    {0.44319491993087357,0.13698915398515032,0.6872680203341193},//color of bone[55]
                    {0.19712392693924086,0.49600717528694205,0.5253215037808608},//color of bone[56]
                    {0.10331947878173475,0.1626312624519658,0.12224407131395731},//color of bone[57]
                    {0.5380563264402888,0.2702114987168327,0.2738229656439146},//color of bone[58]
                    {0.3952353956063701,0.4006033379758649,0.4328181583526063},//color of bone[59]
                    {0.2774245915504828,0.28562757070155465,0.9222607945355403},//color of bone[60]
                    {0.5664147830677392,0.6004376358402006,0.14551789729557618},//color of bone[61]
                    {0.8756133915917831,0.724102090313211,0.6135062284053358},//color of bone[62]
                    {0.010742917698759125,0.14765119533998483,0.3309276354987759},//color of bone[63]
                    {0.6496187825875601,0.8924568520468124,0.50380756757505},//color of bone[64]
                    {0.6689469777584661,0.23608878937670774,0.3087346990525939},//color of bone[65]
                    {0.20194647632605722,0.4813566207933323,0.3935264808013107},//color of bone[66]
                    {0.7444477256322757,0.3509512348257747,0.9842326307073695},//color of bone[67]
                    {0.5874419389341679,0.6646499526131426,0.8756753032938952},//color of bone[68]
                    {0.21783622845988326,0.3665511877163813,0.6473416187054075},//color of bone[69]
                    {0.7340049604304864,0.45706587983774116,0.9897161140826575},//color of bone[70]
                    {0.4606637612176133,0.07791161603412466,0.6092844193989205},//color of bone[71]
                    {0.5158318683595947,0.7339553908777775,0.07897795446524991},//color of bone[72]
                    {0.9076002631650293,0.027001760388104534,0.14835663540552024},//color of bone[73]
                    {0.00414273259330511,0.6187526432399494,0.11881609997259868},//color of bone[74]
                    {0.6271493246414279,0.5081633182572042,0.40854201581905614},//color of bone[75]
                    {0.09760667392863931,0.02366940897307468,0.8669595566560963},//color of bone[76]
                    {0.018914189613411847,0.2624812917891065,0.37857442680573306},//color of bone[77]
                    {0.9712365333427908,0.7296991388945634,0.47731543381343566},//color of bone[78]
                    {0.6084177474725762,0.38978196245143315,0.33992282544201924}//color of bone[79]
            };
    glm::vec3 bone_color_vec3[MAX_BONES];
    for (int i = 0; i< MAX_BONES; i++){
        glm::vec3 temp;
        temp.x = bone_color[i][0];
        temp.y = bone_color[i][1];
        temp.z = bone_color[i][2];
        bone_color_vec3[i] = temp;
    }

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "SKINNING RENDERER", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader program
    // ------------------------------------
    Shader lightingShader("../shaders/shader_lighting.vs", "../shaders/shader_lighting.fs"); // you can name your shader files however you like
    Shader shadowShader("../shaders/shadow.vs", "../shaders/shadow.fs");
    Shader skyboxShader("../shaders/shader_skybox.vs", "../shaders/shader_skybox.fs");


    Model GTModel = Model("../resources/Samba Dancing/Samba Dancing.dae", 0.02,0,1);
    GTModel.diffuse = new Texture("../resources/Samba Dancing/textures/Ch14_1001_Diffuse.png");
    GTModel.specular = new Texture("../resources/Samba Dancing/textures/Ch14_1001_Specular.png");
    GTModel.normal = new Texture("../resources/Samba Dancing/textures/Ch14_1001_Normal.png");

    Model OURModel = Model("../resources/Samba Dancing2/Samba Dancing2.dae", 0.02,0,0);
    OURModel.diffuse = new Texture("../resources/Samba Dancing2/textures/Ch14_1001_Diffuse.png");
    OURModel.specular = new Texture("../resources/Samba Dancing2/textures/Ch14_1001_Specular.png");
    OURModel.normal = new Texture("../resources/Samba Dancing2/textures/Ch14_1001_Normal.png");

//    Model GTModel = Model("../resources/Flying Knee Punch Combo2/Flying Knee Punch Combo.dae", 0.02,0,1);
//    GTModel.diffuse = new Texture("../resources/Flying Knee Punch Combo2/textures/Ch11_1001_Diffuse.png");
//    GTModel.specular = new Texture("../resources/Flying Knee Punch Combo2/textures/Ch11_1001_Specular.png");
//    GTModel.normal = new Texture("../resources/Flying Knee Punch Combo2/textures/Ch11_1001_Normal.png");
//
//    Model OURModel = Model("../resources/Flying Knee Punch Combo3/Flying Knee Punch Combo.dae", 0.02,0,0);
//    OURModel.diffuse = new Texture("../resources/Flying Knee Punch Combo3/textures/Ch11_1001_Diffuse.png");
//    OURModel.specular = new Texture("../resources/Flying Knee Punch Combo3/textures/Ch11_1001_Specular.png");
//    OURModel.normal = new Texture("../resources/Flying Knee Punch Combo3/textures/Ch11_1001_Normal.png");


    Model GTModel_weights = GTModel;
    GTModel_weights.mode = 1;
    Model OURModel_weights = OURModel;
    OURModel_weights.mode = 1;
    Model OURModel_deformation = OURModel;
    OURModel_deformation.mode = 2;

    Model grassGroundModel = Model("../resources/plane.obj",1.0, 0, 0 );
    grassGroundModel.diffuse = new Texture("../resources/grid_plane.jpg");
    grassGroundModel.ignoreShadow = true;


    Scene scene;


    scene.addEntity(new Entity(&GTModel,glm::vec3(-6.5,-0.5,-3),0.0f, 0.0f, 0.0f, GTModel.model_scale));
    scene.addEntity(new Entity(&GTModel_weights,glm::vec3(-3.25,-0.5,-3),0.0f, 0.0f, 0.0f, GTModel.model_scale));
    scene.addEntity(new Entity(&OURModel,glm::vec3(-0.0,-0.5,-3),0.0f, 0.0f, 0.0f, OURModel.model_scale));
    scene.addEntity(new Entity(&OURModel_weights,glm::vec3(3.25,-0.5,-3),0.0f, 0.0f, 0.0f, OURModel.model_scale));
    scene.addEntity(new Entity(&OURModel_deformation,glm::vec3(6.5,-0.5,-3),0.0f, 0.0f, 0.0f, OURModel.model_scale));

    glm::mat4 planeWorldTransform = glm::mat4(1.0f);
    planeWorldTransform = glm::scale(planeWorldTransform, glm::vec3(planeSize));
    planeWorldTransform = glm::translate(glm::vec3(0.0f, -0.5f, 0.0f)) * planeWorldTransform;
    scene.addEntity(new Entity(&grassGroundModel, planeWorldTransform));
    // define depth texture


    DepthMapTexture depth = DepthMapTexture(SHADOW_WIDTH, SHADOW_HEIGHT);



    std::vector<std::string> faces
            {
                    "../resources/default_skybox_2.jpg",
                    "../resources/default_skybox_2.jpg",
                    "../resources/default_skybox_2.jpg",
                    "../resources/default_skybox_2.jpg",
                    "../resources/default_skybox_2.jpg",
                    "../resources/default_skybox_2_back.jpg"
            };

    CubemapTexture skyboxTexture = CubemapTexture(faces);
    unsigned int VAOskybox, VBOskybox;
    getPositionVAO(skybox_positions, sizeof(skybox_positions), VAOskybox, VBOskybox);


    shadowShader.use();


    lightingShader.use();
    lightingShader.setInt("material.diffuseSampler", 0);
    lightingShader.setInt("material.specularSampler", 1);
    lightingShader.setInt("material.normalSampler", 2);
    lightingShader.setInt("depthMapSampler", 3);
    lightingShader.setFloat("material.shininess", 64.f);    // set shininess to constant value.
    lightingShader.setFloat("MAX_ERROR_DISTANCE", MAX_ERROR_DISTANCE);
    skyboxShader.use();
    skyboxShader.setInt("skyboxTexture1", 0);


    DirectionalLight sun(0.0f, 20.0f, glm::vec3(1.0f));
    float animationTime;
    float bufferTime = 0;
    float bufferStartTime;
    bool firstFlag =true;
    float oldTime = 0;
    while (!glfwWindowShouldClose(window))// render loop
    {
        if(!startFlag){
            startFlag = true;
            startTime = glfwGetTime();
        }
        float currentTime = glfwGetTime();
        float dt = currentTime - oldTime;
        deltaTime = dt;
        oldTime = currentTime;
        if(!changeFlagSpace) {
            if(!firstFlag){
                bufferTime += currentTime - bufferStartTime;
                firstFlag = true;
            }
            animationTime = currentTime - bufferTime - startTime;
        }
        else{
            if(firstFlag){
                bufferStartTime= currentTime;
                firstFlag = false;
            }
        }


        // input
        processInput(window, &sun);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        map<Model*, vector<Entity*>>::iterator it;


        glm::mat4 lightProjection = glm::mat4(1.0f);
        glm::mat4 lightView = glm::mat4(1.0f);
        glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);
        if (useShadow){
            //lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
            lightProjection = sun.getProjectionMatrix();
            lightView = sun.getViewMatrix(glm::vec3(0.0f,0.0f,0.0f));
            lightSpaceMatrix = lightProjection * lightView;
            shadowShader.use();
            shadowShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

            //cout << glm::to_string(lightSpaceMatrix) << endl;

            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, depth.depthMapFBO);
            //glClear(GL_DEPTH_BUFFER_BIT);
            glClear(GL_DEPTH_BUFFER_BIT);


            for(it = scene.entities.begin(); it != scene.entities.end(); it++ ) {
                if(it -> first -> ignoreShadow) continue;
                glBindVertexArray(it -> first -> VAO);
                //glActiveTexture(GL_TEXTURE0);
                glCullFace(GL_FRONT);

                if(it -> first -> hasBone){
                    glm::mat4 bones[MAX_BONES] ={glm::mat4(1.0f)};
                    vector<glm::mat4> temp = it -> first -> extractBoneTransforms(animationTime, 0);

                    int NumBones = it -> first -> mNumBones;

                    for (int i = 0; i < NumBones; i++){
                        bones[i] = temp[i];
                    }

                    shadowShader.setMat4Array("bones", bones, MAX_BONES);
                }
                for(auto& entity : it -> second){
                    glm::mat4 world = entity -> getModelMatrix();
//                    cout << entity -> model -> mDuration << endl;
//                    cout << entity -> model -> mNumBones << endl;
                    shadowShader.setMat4("world", world);
                    glDrawElements(GL_TRIANGLES,(it -> first -> mesh).indices.size(), GL_UNSIGNED_INT, 0 );

                }
                glBindVertexArray(0);
                glCullFace(GL_BACK);
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        // reset viewport
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // render objects


        lightingShader.use();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        lightingShader.setVec3("light.dir",sun.lightDir);
        lightingShader.setVec3("light.color", sun.lightColor);
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setFloat("useLighting",useLighting);
        lightingShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        lightingShader.setVec3Array("bone_color",bone_color_vec3,MAX_BONES);

        glm::mat4 all_bones[2*MAX_BONES] = {glm::mat4(1.0f)};
        for(it = scene.entities.begin(); it != scene.entities.end(); it++ ){
            Model* it_model = it -> first;
            it -> first -> bind();
            bool modelNormal = (it_model -> diffuse && it_model -> normal)? true: false;
            bool modelSpecular = (it_model -> diffuse && it_model -> specular)? true : false;

            lightingShader.setFloat("useNormalMap",useNormalMap && modelNormal);
            lightingShader.setFloat("useSpecularMap",useSpecularMap && modelSpecular);
            lightingShader.setFloat("useShadow", useShadow);

            //cout << float(useShadow && (it_model -> ignoreShadow) ) << endl;
            lightingShader.setInt("mode",it->first->mode);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, depth.ID);

            if(it -> first -> hasBone){
                glm::mat4 bones[2*MAX_BONES] ={glm::mat4(1.0f)};
                vector<glm::mat4> temp = it -> first -> extractBoneTransforms(animationTime, 0);

                int NumBones = it -> first -> mNumBones;

                for (int i = 0; i < NumBones; i++){
                    bones[i] = temp[i];
                    if(!(it -> first -> GTFlag)){//OUR Model
                        all_bones[i] = bones[i];
                    }
                    else{
                        all_bones[i+MAX_BONES] = bones[i];
                    }
                }
                if (it -> first -> mode == 2){
                    lightingShader.setMat4Array("bones", all_bones, 2*MAX_BONES);
                }
                else{
                    lightingShader.setMat4Array("bones", bones, 2* MAX_BONES);
                }

            }

            for(auto& entity : it -> second){
                glm::mat4 world = entity -> getModelMatrix();
                lightingShader.setMat4("world", world);
                glDrawElements(GL_TRIANGLES,(it -> first -> mesh).indices.size(), GL_UNSIGNED_INT, 0 );


            }

            glBindVertexArray(0);
        }


        // use skybox Shader
        skyboxShader.use();
        glDepthFunc(GL_LEQUAL);
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);

        // render a skybox
        glBindVertexArray(VAOskybox);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture.textureID);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(0);
        glDepthFunc(GL_LESS);




        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window, DirectionalLight* sun)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    // toggle direction of sun
    float t = 20.0f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        sun->ProcessKeyboard(ELEVATION_UP, t);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        sun->ProcessKeyboard(ELEVATION_DOWN, t);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        sun->ProcessKeyboard(AZIMUTH_UP, t);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        sun->ProcessKeyboard(AZIMUTH_DOWN, t);

    // toggle useNormalMap
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !changeFlag1){
        useNormalMap = !useNormalMap;
        useSpecularMap = !useSpecularMap;
        changeFlag1 = true;
    }
    else if(glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && changeFlag1){
        // key is pressed but changed at last call
        // nothing
    }
    else if (glfwGetKey(window, GLFW_KEY_1) != GLFW_PRESS && changeFlag1){
        changeFlag1 = false;
    }

    // toggle useShadow
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !changeFlag2){
        useShadow = !useShadow;
        changeFlag2 = true;
    }
    else if(glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && changeFlag2){
        // key is pressed but changed at last call
        // nothing
    }
    else if (glfwGetKey(window, GLFW_KEY_2) != GLFW_PRESS && changeFlag2){
        changeFlag2 = false;
    }

    // toggle useLighting
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && !changeFlag3){
        useLighting = !useLighting;
        changeFlag3 = true;
    }
    else if(glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && changeFlag3){
        // key is pressed but changed at last call
        // nothing
    }
    else if (glfwGetKey(window, GLFW_KEY_3) != GLFW_PRESS && changeFlag3){
        changeFlag3 = false;
    }

    // toggle useLighting
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !changeFlagSpace){
        changeFlagSpace = true;
    }
    else if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && changeFlagSpace){
        // key is pressed but changed at last call
        // nothing
    }
    else if (glfwGetKey(window, GLFW_KEY_SPACE) != GLFW_PRESS && changeFlagSpace){
        changeFlagSpace = false;
    }

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}