#include <osg/MatrixTransform>
#include <osg/Quat>
#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/PositionAttitudeTransform>
#include <osgGA/TrackballManipulator>
#include <osgViewer/ViewerEventHandlers>


//Para que funcione correlo como g++ -o cube_rotation2 cube3.cpp -losg -losgViewer -losgDB (ejemplo)
osg::AnimationPath* createAnimationPath(const osg::Vec3& center,float radius,double looptime, const osg::Vec3& axis)
{
    // set up the animation path
    osg::AnimationPath* animationPath = new osg::AnimationPath;
    animationPath->setLoopMode(osg::AnimationPath::LOOP);

    int numSamples = 40;
    float yaw = 0.0f;
    float yaw_delta = 2.0f*osg::PI/((float)numSamples-1.0f);
    float roll = osg::inDegrees(30.0f);

    double time=0.0f;
    double time_delta = looptime/(double)numSamples;
    for(int i=0;i<numSamples;++i)
    {
        osg::Vec3 position(center+osg::Vec3(sinf(yaw)*radius,cosf(yaw)*radius,0.0f));
        osg::Quat rotation(osg::Quat(roll,osg::Vec3(0.0,1.0,0.0))*osg::Quat(-(yaw+osg::inDegrees(90.0f)),axis));

        animationPath->insert(time,osg::AnimationPath::ControlPoint(position,rotation));

        yaw += yaw_delta;
        time += time_delta;

    }
    return animationPath;
}



int main()
{

    float animationLength = 10.0f;



    osg::Node* cube = osgDB::readNodeFile("/cube.osg");
    if(cube == nullptr) {
        return -1;
    }


    const osg::BoundingSphere& bs = cube->getBound();
    float size = 10.0f/bs.radius()*0.3f;
    osg::AnimationPath* centerRotation = createAnimationPath(bs.center(),0.0f,3.0f,osg::Vec3(0.0,1.0,0.0));
    osg::AnimationPath* rotationHorizontal = createAnimationPath(osg::Vec3(0.0,1.0,0.0),5.0f,animationLength,osg::Vec3(1.0,0.0,0.0));


    osg::ref_ptr<osg::Geode> geode = new osg::Geode;


    osg::PositionAttitudeTransform* xform = new osg::PositionAttitudeTransform;


    osg::PositionAttitudeTransform* xform1 = new osg::PositionAttitudeTransform;
    osg::PositionAttitudeTransform* xform2 = new osg::PositionAttitudeTransform;

    xform1->setUpdateCallback(new osg::AnimationPathCallback(centerRotation,0.0,1.0));
    xform2->setUpdateCallback(new osg::AnimationPathCallback(rotationHorizontal,0.0,1.0));

    xform1->addChild(cube);
    xform2->addChild(xform1);





    geode->addChild(xform2);

    // Create a root node and add the rotation transform
    osg::ref_ptr<osg::Group> root = new osg::Group;
    root->addChild(geode);

    // Create a viewer and set the scene data
    osgViewer::Viewer viewer;
    viewer.setSceneData(root);

    // Run the viewer
    return viewer.run();
}
