// Base de de: https://github.com/openscenegraph/OpenSceneGraph/blob/master/examples/osganimationsolid/osganimationsolid.cpp

#include <iostream>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Image>
#include <osg/Light>
#include <osg/LightSource>
#include <osg/MatrixTransform>
#include <osg/Node>
#include <osg/Plane>
#include <osg/PositionAttitudeTransform>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/Vec3d>
#include <osg/ref_ptr>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osgAnimation/BasicAnimationManager>
#include <osgAnimation/StackedRotateAxisElement>
#include <osgAnimation/UpdateMatrixTransform>

// Debuxa o cubo
osg::ref_ptr<osg::Node> drawCube() {
	// Cubo centrado en (0,0,0), de 0.5 de ancho
	osg::ref_ptr<osg::Geometry> cubeGeometry = new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f,0.0f,0.0f),0.5));
	osg::ref_ptr<osg::Geode> geodeCube = new osg::Geode;
	geodeCube->addDrawable(cubeGeometry);

	// Cores do cubo, 4 por cada vértice de cada cara.
	osg::ref_ptr<osg::Vec4Array> cubeColor (new osg::Vec4Array);
	// Vermello
	cubeColor->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	cubeColor->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	cubeColor->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	cubeColor->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	// Verde
	cubeColor->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
	cubeColor->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
	cubeColor->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
	cubeColor->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
	// Azul
	cubeColor->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
	cubeColor->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
	cubeColor->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
	cubeColor->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
	// Maxenta
	cubeColor->push_back(osg::Vec4(1.0f, 0.0f, 1.0f, 1.0f));
	cubeColor->push_back(osg::Vec4(1.0f, 0.0f, 1.0f, 1.0f));
	cubeColor->push_back(osg::Vec4(1.0f, 0.0f, 1.0f, 1.0f));
	cubeColor->push_back(osg::Vec4(1.0f, 0.0f, 1.0f, 1.0f));
	// Ciano
	cubeColor->push_back(osg::Vec4(0.0f, 1.0f, 1.0f, 1.0f));
	cubeColor->push_back(osg::Vec4(0.0f, 1.0f, 1.0f, 1.0f));
	cubeColor->push_back(osg::Vec4(0.0f, 1.0f, 1.0f, 1.0f));
	cubeColor->push_back(osg::Vec4(0.0f, 1.0f, 1.0f, 1.0f));
	// Branco
	cubeColor->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	cubeColor->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	cubeColor->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	cubeColor->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));

	cubeGeometry->setColorArray(cubeColor, osg::Array::BIND_PER_VERTEX);
	geodeCube->addDrawable(cubeGeometry);

	/// ANIMACIÓN DO CUBO
	//Transformation to be manipulated by the animation
	osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform();
	trans->setName("AnimatedNode");
	//Dynamic object, has to be updated during update traversal
	trans->setDataVariance(osg::Object::DYNAMIC);
	//Animation callback for Matrix transforms, name is targetName for Channels
	osg::ref_ptr<osgAnimation::UpdateMatrixTransform> updatecb (new osgAnimation::UpdateMatrixTransform("AnimatedCallback"));
	//add manipulator Stack, names must match with channel names
	//elements are applied in LIFO order
	//This element modifies the position component of the matrix
	updatecb->getStackedTransforms().push_back(
		new osgAnimation::StackedRotateAxisElement("euler", osg::Vec3(0, 0, 1), 0)
	);
	//connect the UpdateMatrixTransform callback to the MatrixTransform
	trans->setUpdateCallback(updatecb);
	//initialize MatrixTranform
	trans->setMatrix(osg::Matrix::identity());
	//append geometry node
	trans->addChild(geodeCube.get());

	return trans;
}

// Debuxa unha esfera
osg::ref_ptr<osg::Node> drawSphere() {
	// Centrada en (1,0,0), de 0.35 de radio
	osg::ref_ptr<osg::Geometry> sphereGeometry = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(-1.0f, 0.0f, 0.0f),0.35));
	osg::ref_ptr<osg::Geode> geodeSphere = new osg::Geode;
	geodeSphere->addDrawable(sphereGeometry);

	// Cor da esfera (negra)
	osg::ref_ptr<osg::Vec4Array> sphereColor (new osg::Vec4Array);
	sphereColor->push_back(osg::Vec4(1.0f, 0.5f, 0.0f, 1.0f));
	sphereGeometry->setColorArray(sphereColor, osg::Array::BIND_OVERALL);
	geodeSphere->addDrawable(sphereGeometry);

	return geodeSphere;
}

// Debuxa un cono
osg::ref_ptr<osg::Node> drawCone() {
	// Centrado en (-1, -1, 0), con 0.35 de radio e 0.5 de altura
	osg::ref_ptr<osg::Geometry> geometry = new osg::ShapeDrawable(new osg::Cone(osg::Vec3(1.0f,0.0f,0.0f),0.35, 0.5));
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(geometry);

	// Cor do cono (verde)
	osg::ref_ptr<osg::Vec4Array> color (new osg::Vec4Array);
	color->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
	geometry->setColorArray(color, osg::Array::BIND_OVERALL);
	geode->addDrawable(geometry);

	return geode;
}

// Crea unha fonte de luz
osg::ref_ptr<osg::Node> buildLightsource() {
	osg::ref_ptr<osg::PositionAttitudeTransform> lightPAT(new osg::PositionAttitudeTransform());
	osg::ref_ptr<osg::LightSource> luz (new osg::LightSource());

	luz->getLight()->setLightNum(1);
	luz->getLight()->setDiffuse(osg::Vec4(1.0, 1.0, 0.0, 1.0));
	luz->getLight()->setPosition(osg::Vec4(0.0, 0.0, 0.0, 1.0));

	lightPAT->setPosition(osg::Vec3(2, -2.0, 4.0));
	lightPAT->addChild(luz);

	return lightPAT;
}

// https://github.com/openscenegraph/OpenSceneGraph/blob/master/examples/osgkeyboard/osgkeyboard.cpp
class CameraChange : public osgGA::GUIEventHandler
{
public:
	CameraChange(osg::Camera* camera, osgViewer::Viewer* viewer, osgViewer::ScreenCaptureHandler* screenCaptureHandler) {
		this->camera = camera;
		this->viewer = viewer;
		this->screenCaptureHandler = screenCaptureHandler;
	}

	virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter&) {
		if(ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN) {
			switch(ea.getKey()) {
				case osgGA::GUIEventAdapter::KEY_1:
					camera->setViewMatrixAsLookAt(
						osg::Vec3d(1.5, -9.0, 0.5),
						osg::Vec3d(0.0,  0.0, 0.0),
						osg::Vec3d(0.0,  0.0, 1.0)
					);
					break;
				case osgGA::GUIEventAdapter::KEY_2:
					camera->setViewMatrixAsLookAt(
						osg::Vec3d(1.3, -2.0, 4.0),
						osg::Vec3d(0.0,  0.0, 0.0),
						osg::Vec3d(0.0,  0.0, 1.0)
					);
					break;
				case osgGA::GUIEventAdapter::KEY_3:
					camera->setViewMatrixAsLookAt(
						osg::Vec3d(6.0, -3.0, 1.0),
						osg::Vec3d(0.0,  0.0, 0.0),
						osg::Vec3d(0.0,  0.0, 1.0)
					);
					break;
				default:
					return false;
			}

			// Imaxe para sacar fotografías
			// https://narkive.com/esFiBYhR.1
			screenCaptureHandler->setFramesToCapture(1);
			screenCaptureHandler->captureNextFrame(*viewer);
		}

		return true;
	}
private:
	osg::Camera* camera;
	osgViewer::Viewer* viewer;
	osgViewer::ScreenCaptureHandler* screenCaptureHandler;
};

int main (int argc, char* argv[])
{
	// Crear a vista inicial
	osg::ArgumentParser arguments(&argc, argv);
	osgViewer::Viewer viewer(arguments);

	// https://narkive.com/esFiBYhR.1
	osg::ref_ptr<osgViewer::ScreenCaptureHandler> screenCaptureHandler (new osgViewer::ScreenCaptureHandler());
	screenCaptureHandler->setCaptureOperation(new osgViewer::ScreenCaptureHandler::WriteToFile("image", "png"));

	// Crear o grupo raíz e engadir os obxectos
	osg::ref_ptr<osg::Group> root (new osg::Group);
	root->addChild(drawSphere());
	root->addChild(drawCube());
	root->addChild(drawCone());
	root->addChild(buildLightsource());

	osg::ref_ptr<osg::StateSet> ss = root->getOrCreateStateSet();
	ss->setMode(GL_LIGHT1, osg::StateAttribute::ON);
	ss->setMode(GL_LIGHT0, osg::StateAttribute::OFF);

	/// XESTIONAR ANIMACIÓNS
		// Define a scheduler for our animations
		osg::ref_ptr<osg::Group> animationGroup (new osg::Group);
		//add the animation manager to the scene graph to get it called during update traversals
		osg::ref_ptr<osgAnimation::BasicAnimationManager> mng (new osgAnimation::BasicAnimationManager());
		animationGroup->setUpdateCallback(mng);
		//add the rest of the scene to the group node
		animationGroup->addChild(root);

		//define the channel for interpolation of a float angle value
		osg::ref_ptr<osgAnimation::FloatLinearChannel> channelAnimation (new osgAnimation::FloatLinearChannel);
		//name of the AnimationUpdateCallback
		channelAnimation->setTargetName("AnimatedCallback");
		//name of the StackedElementTransform for position modification
		channelAnimation->setName("euler");
		//Create keyframes for (in this case linear) interpolation of a osg::Vec3
		channelAnimation->getOrCreateSampler()->getOrCreateKeyframeContainer()->push_back(osgAnimation::FloatKeyframe(0, 0));
		channelAnimation->getOrCreateSampler()->getOrCreateKeyframeContainer()->push_back(osgAnimation::FloatKeyframe(1.5, 2*osg::PI));
		osg::ref_ptr<osgAnimation::Animation> anim (new osgAnimation::Animation);
		anim->addChannel(channelAnimation);
		anim->setPlayMode(osgAnimation::Animation::LOOP);

		// We register all animation inside the scheduler
		mng->registerAnimation(anim);

		//start the animation
		//mng->playAnimation(anim1);
		mng->playAnimation(anim);

		//set the Group with the scene and the AnimationManager as viewer's scene data
		viewer.setSceneData(animationGroup);

	// Xestionar a cámara, para poder cambiar premendo as teclas 1, 2 e 3.
	viewer.addEventHandler(new CameraChange(viewer.getCamera(), &viewer, screenCaptureHandler));
	// https://stackoverflow.com/a/21267807
	// Importante establecelo a NULL para que non sobrescriba a cámara
	viewer.setCameraManipulator(NULL);
	viewer.getCamera()->setViewMatrixAsLookAt(
		osg::Vec3d(1.5, -9.0, 0.5),
		osg::Vec3d(0.0,  0.0, 0.0),
		osg::Vec3d(0.0,  0.0, 1.0)
	);

	// Lanzar a aplicación
	// Non se pode usar viewer.run() por que sobrescribe o manipulador da cámara.
	viewer.realize();
	while(!viewer.done()) {
		viewer.frame();
	}

	return 0;
}
