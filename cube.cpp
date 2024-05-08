#include <osg/MatrixTransform>
#include <osg/Quat>
#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/PositionAttitudeTransform>
#include <osgGA/TrackballManipulator>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Texture2D>


//Para que funcione correlo como g++ -o cube_rotation2 cube3.cpp -losg -losgViewer -losgDB (ejemplo)
//Function to create the animations
osg::AnimationPath *createAnimationPath(const osg::Vec3 &center, float radius, double looptime, const osg::Vec3 &axis, bool y) {
    // set up the animation path
    osg::AnimationPath *animationPath = new osg::AnimationPath;
    animationPath->setLoopMode(osg::AnimationPath::LOOP);

    int numSamples = 40;
    float yaw = 0.0f;
    float yaw_delta = 2.0f * osg::PI / ((float) numSamples - 1.0f);
    float roll = osg::inDegrees(30.0f);

    double time = 0.0f;
    double time_delta = looptime / (double) numSamples;
    for (int i = 0; i < numSamples; ++i) {
        if(y){
            osg::Vec3 position(center+osg::Vec3(sinf(yaw)*radius,0.0f,cosf(yaw)*radius));
            osg::Quat rotation(osg::Quat(roll, osg::Vec3(1.0, 0.0, 0.0)) * osg::Quat(-(yaw + osg::inDegrees(90.0f)), axis));

            animationPath->insert(time, osg::AnimationPath::ControlPoint(position, rotation));

            yaw += yaw_delta;
            time += time_delta;
        }
        else{
            osg::Vec3 position(center+osg::Vec3(sinf(yaw)*radius,cosf(yaw)*radius,0.0f));
                    osg::Quat rotation(osg::Quat(roll, osg::Vec3(1.0, 0.0, 0.0)) * osg::Quat(-(yaw + osg::inDegrees(90.0f)), axis));

        animationPath->insert(time, osg::AnimationPath::ControlPoint(position, rotation));

        yaw += yaw_delta;
        time += time_delta;
        }


    }
    return animationPath;
}


int main() {

    //Create the "lamp", a small cube with a yellow light
    osg::ref_ptr<osg::Group> sgRoot(new osg::Group());

    osg::ref_ptr<osg::Node> lampModel = osgDB::readNodeFile("/cube.osg");
    if (!lampModel)
    {
      std::cerr << "Problem opening the cube model.\n";
      exit(1);
    }

    osg::MatrixTransform* transform = new osg::MatrixTransform();
    transform->setMatrix(osg::Matrix::scale(0.25f, 0.25f, 0.25f));

    transform->addChild(lampModel);

    osg::ref_ptr<osg::PositionAttitudeTransform> lightPAT(
      new osg::PositionAttitudeTransform());

    lightPAT->setPosition(osg::Vec3(-3.0, 0.0, 3.0));
    sgRoot->addChild(lightPAT);

    osg::ref_ptr<osg::LightSource> lightSource(new osg::LightSource());  
    lightSource->addChild(transform);                                    
    lightSource->getLight()->setLightNum(1);                             
    lightSource->getLight()->setPosition(osg::Vec4(0.0, 0.0, 0.0, 1.0)); 
    lightSource->getLight()->setDiffuse(osg::Vec4(1.0, 1.0, 0.0, 1.0));  

    lightPAT->addChild(lightSource);

    osg::ref_ptr<osg::StateSet> ss = sgRoot->getOrCreateStateSet();
    ss->setMode(GL_LIGHT1, osg::StateAttribute::ON);



    //Create the cubes, the textures and the animations
    float animationLength = 10.0f;


    osg::Node *cube = osgDB::readNodeFile("/cube.osg");
    if (cube == nullptr) {
        return -1;
    }
    osg::Node *cube2 = osgDB::readNodeFile("/cube.osg");
    if (cube2 == nullptr) {
        return -1;
    }

    osg::Texture2D* texture = new osg::Texture2D;
    osg::Texture2D* texture2 = new osg::Texture2D;

    texture->setImage(osgDB::readRefImageFile("/Brick-Norman-Brown.TGA"));
    texture2->setImage(osgDB::readRefImageFile("/Brick-Std-Orange.TGA"));


    if (!texture | !texture2) {
        std::cerr << "Problem opening the texture image.\n";
        exit(1);
    }

    //Apply textures
    osg::StateSet* stateset = cube->getOrCreateStateSet();
    stateset->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);

    osg::StateSet* stateset2 = cube2->getOrCreateStateSet();
    stateset2->setTextureAttributeAndModes(0,texture2,osg::StateAttribute::ON);


    //Create animations
    const osg::BoundingSphere &bs = cube->getBound();
    float size = 10.0f / bs.radius() * 0.3f;
    osg::AnimationPath *centerRotation = createAnimationPath(bs.center(), 0.0f, 3.0f, osg::Vec3(0.0, 1.0, 0.0),false);
    osg::AnimationPath *rotationHorizontal = createAnimationPath(osg::Vec3(0.0, 1.0, 0.0), 5.0f, animationLength,
                                                                 osg::Vec3(0.0, 1.0, 0.0),false);

    osg::AnimationPath *centerRotation2 = createAnimationPath(bs.center(), 0.0f, 5.0f, osg::Vec3(0.0, 0.0, 1.0),true);
    osg::AnimationPath *rotationVertical = createAnimationPath(osg::Vec3(1.0, 0.0, 0.0), 3.0f, animationLength,
                                                                  osg::Vec3(0.0, 1.0, 0.0),true);

    osg::ref_ptr <osg::Geode> geode = new osg::Geode;

    //Apply animations to PATs and the cubes to the PATs
    osg::PositionAttitudeTransform *xform = new osg::PositionAttitudeTransform;


    osg::PositionAttitudeTransform *xform1_1 = new osg::PositionAttitudeTransform;
    osg::PositionAttitudeTransform *xform1_2 = new osg::PositionAttitudeTransform;
    osg::PositionAttitudeTransform *xform2_1 = new osg::PositionAttitudeTransform;
    osg::PositionAttitudeTransform *xform2_2 = new osg::PositionAttitudeTransform;

    xform1_1->setUpdateCallback(new osg::AnimationPathCallback(centerRotation, 0.0, 1.0));
    xform1_2->setUpdateCallback(new osg::AnimationPathCallback(rotationHorizontal, 0.0, 1.0));

    xform2_1->setUpdateCallback(new osg::AnimationPathCallback(centerRotation2, 0.0, 1.0));
    xform2_2->setUpdateCallback(new osg::AnimationPathCallback(rotationVertical, 0.0, 1.0));
    xform1_1->addChild(cube);
    xform1_2->addChild(xform1_1);
    xform2_1->addChild(cube2);
    xform2_2->addChild(xform2_1);


    xform->addChild(xform1_2);
    xform->addChild(xform2_2);


    geode->addChild(xform);


    osg::ref_ptr <osg::Group> root = new osg::Group;
    sgRoot->addChild(geode);
    // Create a viewer and set the scene data
    osgViewer::Viewer viewer;
    viewer.setSceneData(sgRoot);

    // Run the viewer
    return viewer.run();
}