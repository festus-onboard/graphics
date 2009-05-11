#include "RenderWidget0.h"

#include "Camera.h"
#include "Object.h"

#include "SWWidget.h"
#include "GLRenderContext.h"

#include "GLWidget.h"
#include "GLRenderContext.h"
#include "VertexData.h"

#include <QtOpenGL>
#include "GeometryFactory.h"
#include "ColorFactory.h"
#include "Vector4.h"
#include "BasicMath.h"

#include "PGMReader.h"
#include "Shader.h"

#include "Materials.h"
#include "SceneManager.h"
#include "Material.h"

#include "scenegraph/LightNode.h"

#include "scenegraph/Shape3D.h"
#include "scenegraph/TransformGroup.h"
#include "scenegraph/CameraNode.h"
#include "spline/BezierCurve.h"
#include "spline/Circle.h"
#include "spline/Helix.h"
#include "spline/Morpher.h"
#include "spline/Square.h"
#include "spline/PiecewiseSpline.h"





#include "Vector3.h"
#include "Texture.h"

/**
* This class is an OpenGL window, with support for mouse dragging
* and key presses.
* @modified Nicholas Basis
* @date   February 21, 2009
*/


using namespace RE167;
using std::cout;
using std::endl;
RenderWidget0::RenderWidget0()
{
    // Store a reference for later
    this->rs = new GLRenderContext();
        
    sceneManager = 0;
	counter = 0;
	QWidget::grabKeyboard();
}

RenderWidget0::~RenderWidget0()
{
	if(sceneManager)
	{
		delete sceneManager;
	}
}

void RenderWidget0::initSceneEvent()
{
    
	sceneManager = new SceneManager();

    
	initCamera();
	//initLights();
    //initStillLife();

    test();
	
	// Trigger timer event every 5ms.
	timerId = startTimer(5);
}

void RenderWidget0::initMaterials()
{}

void RenderWidget0::initCamera() {
    Vector3 cameraCenter = Vector3(0,0,10);
	Vector3 lookAtPoint = Vector3(0,0,-1);
	Vector3 upVector = Vector3(0,1,0);

//	camera = sceneManager->createCamera();
	
	// TODO: the camera stuff will only work if setFrustum is called first!!
	
	Camera* camera = new Camera();
	camera->setFrustum(1, 100, 1, BasicMath::radians(60));
	camera->changeSettings(cameraCenter, lookAtPoint, upVector);
	
	
    stillCamera = new CameraNode(camera);
    
    sceneManager->getRoot()->addChild(stillCamera);
	
    
}

void RenderWidget0::initLights()
{
    
    
    // Make a blue spotlight coming from the left
    Light * blue = sceneManager->createLight();
    blue->setType(Light::SPOT);
    blue->setAmbientColor(Vector3(.2,.2,.2));
    blue->setDiffuseColor(Vector3(0,0,1));
    blue->setSpecularColor(Vector3(1,1,1));
    blue->setSpotDirection(Vector3(0,0,1));
    blue->setPosition(Vector3(0,1,0));
	blue->setSpotCutoff(90.0);
	blue->setSpotExponent(1.0);
    
    
    LightNode * blueLight = new LightNode(blue);

/*    
    // Create a white light
    Light * white = sceneManager->createLight();
	white->setType(Light::SPOT);
    white->setSpotDirection(Vector3(1,1,1));
    white->setDiffuseColor(Vector3(1,1,1));
    white->setAmbientColor(Vector3(.2,.2,.2));
    white->setSpecularColor(Vector3(1,1,1));
	white->setSpotCutoff(90.0);
	white->setSpotExponent(1.0);
    
    LightNode * whiteLight = new LightNode(white);
*/    
    
    sceneManager->getRoot()->addChild(blueLight);
//	sceneManager->getRoot()->addChild(whiteLight);
}


void RenderWidget0::renderSceneEvent()
{
	sceneManager->renderScene();
}

void RenderWidget0::resizeRenderWidgetEvent(const QSize &s)
{
}

void RenderWidget0::timerEvent(QTimerEvent *t)
{
    static int segment = 0;
    static const int numSegments = 3000;
    
        
    static const std::vector<Vector3> positions = track->uniformPointSample(numSegments);
    

    Vector3 loc = positions[segment % numSegments];
    segment++;
    
    
    minecart->setTransformation(Matrix4::translate(loc.getX(), loc.getY(), loc.getZ()));
    
	// we now add code to make the camera follow the mine cart
	// We do not need to worry about changing the center of projection because in the scene graph, the camera is a child
	// of the minecart so that center should follow the minecart.
	// We do have to change the lookAtPoint and the lookUpVector and for that we need the referenceFrames for the track
	
	static const std::vector<Basis> referenceFrames = track->getReferenceFrames(numSegments);
	
	// now that we have the referenceFrames, we need the Basis for this location:
	// (the -1 is because segment has been incremented)
	
	// now the tangent is v and the normal is u
	Vector3 newLookAt = movingCamera->getCenterOfProjection() + referenceFrames[(segment-1) % numSegments].getV();
	Vector3 newLookUp = referenceFrames[(segment-1) % numSegments].getU();
	
	// now we update the camera
	movingCamera->updateProjection(movingCamera->getCenterOfProjection(), newLookAt, newLookUp);
	whiteLight->getLight()->setSpotDirection(referenceFrames[(segment-1) % numSegments].getV());
    
    updateScene();
	counter++;
}

void RenderWidget0::mousePressEvent(QMouseEvent *e)
{
	lastX = e->x();
	lastY = e->y();
}

/**
* Handles mouse drags by repositioning camera on virtual
* trackball
*/
void RenderWidget0::mouseMoveEvent(QMouseEvent *e)
{
	int x = e->x();
	int y = e->y();

    
	int canvasWidth = this->width();
	int canvasHeight = this->height();

	// Calculate where (x,y) maps to in the unit sphere
	Vector3 curVector = mapToUnitSphere(x, y, canvasWidth, canvasHeight);

	// Calculate where (lastX, lastY) maps to in the unit sphere
	Vector3 lastVector = mapToUnitSphere(lastX, lastY, canvasWidth, canvasHeight);


	// Calculate the vector around which we need to rotate to take us from lastPoint to curPoint along
	// the sphere
	Vector3 axis = lastVector.crossProduct(curVector);

	// Check whether the cross product is 0.  If it is, then normalizing this
	// will cause a crash.  Just change the vector slightly
	if (axis.magnitude() == 0.0f) {
		axis = Vector3(axis.getX() + .01f, axis.getY() + .01f, axis.getZ() + .01f);
	}

	Vector4 axis4 = Vector4(axis.getX(), axis.getY(), axis.getZ(), 0).normalize();
	float angle = Vector3::angleBetween(lastVector, curVector);


	// Calculate the angle we would need
	// Calculate the transformation needed to rotate
	// the camera
	Matrix4 rotation = Matrix4::rotate(axis4, angle);

	
	// Apply transformation to each object in scene
	

	// Apply transformation to the objects in the scene
    TransformGroup * root = sceneManager->getRoot();
    root->setTransformation(rotation * root->getTransformation());
	
//	camera->setViewMatrix(camera->getViewMatrix() * rotation);
    
    
    /*
    int dx = lastX - x;
    int dy = lastY - y;
    
    static const float scale = 0.5;
    
    float oldYaw = camera->getYaw();
    float oldPitch = camera->getPitch();
    
    camera->setYaw(oldYaw + (dx * scale));
    camera->setPitch(oldPitch + (dy * scale));
    */
    
    
	// Current points are now old points.
	lastX = e->x();
	lastY = e->y();
}


/**
* Given a point in screen coordinates, relative to upper left corner
* of window, returns a vector that represents the point's location in
* the unit sphere.  Does this by first calculating where the point lies
* in the unit square, and then calculating what the z value must be to lie
* on the unit sphere.  Note: If the point could not lie on the unit sphere,
* this method will force it to lie on the sphere by normalizing the (x,y) vector.
*/
Vector3 RenderWidget0::mapToUnitSphere(int x, int y, int width, int height) 
{
	int smallerSide = std::min(width, height);

	// Since we want the trackball to remain a sphere, we make
	// our sphere have radius min(width, height).

	// Calculate our x and y position relative to the center of
	// the screen.
	int dx = x - (width / 2);
	int dy = y - (height / 2);

	// Divide through by the smaller side.  Now all of the points along
	// the smaller side axis are in the range [-1, 1].  Those on the
	// bigger side might be outside of this range, but they'll be clamped
	// to lie on the sphere anyways.

	float x1 = static_cast<float>(dx) / (smallerSide / 2);
	// y value must be flipped
	float y1 = - static_cast<float>(dy) / (smallerSide / 2);
	float z1;

	float distFromCenter = (x1 * x1) + (y1 * y1);

	// Point does not lie in unit circle; therefore it cannot lie
	// in unit sphere.  Force the point to lie on unit circle with
	// z = 0
	if (distFromCenter > 1) {
		z1 = 0;
		// Normalize x1 and y1 so that the vector (x1, y1, z) has unit
		// length
		x1 = x1 / distFromCenter;
		y1 = y1 / distFromCenter;
	}
	else {
		// Use the formula for a sphere, x^2 + y^2 + z^2 = r^2
		// to solve for z
		z1 = sqrt(1 - (x1 * x1) - (y1 * y1));
	}
	return Vector3(x1, y1, z1);
}


void RenderWidget0::mouseReleaseEvent(QMouseEvent *e)
{
}


void RenderWidget0::startAnimation()
{
	if(!timerId)
	{
		timerId = startTimer(5);
	}
}

void RenderWidget0::stopAnimation()
{
	if(timerId)
	{
		killTimer(timerId);
		timerId = 0;
	}
}


void RenderWidget0::toggleCulling()
{
    static bool objectLevelCulling = true;
    objectLevelCulling = !objectLevelCulling;
    std::cout << "Object level culling: " << (objectLevelCulling ? "true" : "false") << std::endl;
    sceneManager->setObjectLevelCulling(objectLevelCulling);
}


// TODO: Clean all this code up
void RenderWidget0::keyPressEvent ( QKeyEvent * k )
{
    
    TransformGroup * root = sceneManager->getRoot();
         
	switch ( k->key() )  {
    // reload
    case Qt::Key_R:                               
        stillCamera->resetTransformation();
        break;
    // move forward
    case Qt::Key_Up: // Qt::Key_W
        stillCamera->setTransformation(Matrix4::translate(0,0,1) * stillCamera->getTransformation());
        break;
    // Move camera backwards
    case Qt::Key_Down: //Qt::Key_S:
        stillCamera->setTransformation(Matrix4::translate(0,0,-1) * stillCamera->getTransformation());
        break;
    // Move camera left
    case Qt::Key_Left: //Key_A:
        root->setTransformation(Matrix4::translate(1,0,0) * root->getTransformation());
    
        //camera->setViewMatrix(Matrix4::translate(1,0,0) * camera->getViewMatrix());
        break;
    // Move camera right
    case Qt::Key_Right: //D:
        root->setTransformation(Matrix4::translate(-1,0,0) * root->getTransformation());
    
        //camera->setViewMatrix(Matrix4::translate(-1,0,0) * camera->getViewMatrix());
        break;
    
    
    // Toggle object level culling
    case Qt::Key_C:
        toggleCulling();
        break;
    
    // Move camera up
    case Qt::Key_Q:
        stillCamera->setTransformation(Matrix4::translate(0,-1,0) * stillCamera->getTransformation());
        break;
    // Move camera down
    case Qt::Key_Z:
        stillCamera->setTransformation(Matrix4::translate(0,1,0) * stillCamera->getTransformation());
        break;
			
	// switch between moving camera and still camera
		case Qt::Key_M:
			if (stillCamera->inUse()) {
				stillCamera->disable();
				movingCamera->use();
			}
			else {
				movingCamera->disable();
				stillCamera->use();
			}
			break;
	}
}
void RenderWidget0::keyReleaseEvent ( QKeyEvent * e)
{

}

void RenderWidget0::toggleWireframe()
{
    rs->toggleWireframe();
}


void RenderWidget0::test() 
{
    

    // this shader supports two spot lights
	Shader* twoSpotTexture = new Shader("src/Shaders/finalSpotLights.vert", "src/Shaders/finalSpotLights.frag");
	// this shader should support 8 lights - 2 spot lights and 6 point lights
//	Shader* lightingTexture = new Shader("src/Shaders/finalLight.vert", "src/Shaders/finalLight.frag");
	
    Vector3 t1(0,4,1);
    Vector3 t2(1,3,5);
    Vector3 t3(2,2,6);
    Vector3 t4(3,3,3);
    Vector3 t5(5,3,2);
    Vector3 t6(5.5,4,4);
    Vector3 t7(6,1,5);
    
    Vector3 p1(0,4,0);
    Vector3 p2(0,3,0);
    Vector3 p3(0,2,0);
    Vector3 p4(0,1,0);
    
    
    
    Vector3 track1(0, -7998, 0);
    Vector3 track2(0, -3895, 0);
    Vector3 track3(0, -793, 0);
    Vector3 track4(209, 11, 0);
    Vector3 track5(934, 446, 0);
    Vector3 track6(1777, 22, 0);
    Vector3 track7(2011, -934, -500);
    Vector3 track8(2011, -2458, -2536);
    Vector3 track9(1986, -4130, -4754);
    
    Vector3 track10(1986, -4100, -4800);
    Vector3 track11(3000, -8000, -5000);
    
    
    Vector3 track12(3166, -13013, -4700);
    Vector3 track13(0, -15000, -5869);
    
    Vector3 track14(-2000, -15000, -5869);
    Vector3 track15(-3000, -15000, -5869);
    Vector3 track16(-4000, -13000, -5869);

    

    //Vector3 trackArray[] = {t1,t2,t3,t4,t5,t6,t7};
    Vector3 pathArray[] = {p1,p2,p3,p4};
    Vector3 trackArray[] = {track1, track2, track3, track4, track5, track6, 
        track7, track8, track9, track10, track11, track12, track13, track14, track15, track16};

    BezierCurve path(pathArray, sizeof(pathArray)/ sizeof(Vector3));

    /*track = new Helix(10);
    track->setTransformation(Matrix4::scale(3,3,10));
*/

    track = new BezierCurve(trackArray, sizeof(trackArray) / sizeof(Vector3));
    track->setTransformation(Matrix4::scale(.001, .001, .001));

    
    //Helix path(10);

    Helix helix(2);
//    helix.setTransformation(Matrix4::scale(1,1,20));

    Vector3 c1(0,0,.5);
    Vector3 c2(.5,0,.5);
    Vector3 c3(.5,0,-.5);
    Vector3 c4(0,0,-.5);
    Vector3 c5(-.5,0,-.5);
    Vector3 c6(-.5,0,.5);
    Vector3 c7(0,0,.5);
    
    
    Vector3 s1(2,0,0);
    Vector3 s2(1,0,0);
    Vector3 s3(0,0,2);
    Vector3 s4(0,0,3);
    Vector3 s5(0,0,2);
    Vector3 s6(-1,0,0);
    Vector3 s7(-2,0,0);
    
    
    Vector3 torch1(.5,2,0);
    Vector3 torch2(.3,1.5,0);
    Vector3 torch3(.1,.1,0);
    Vector3 torch4(0,0,0);
    
    BezierCurve torchCurve(torch1, torch2, torch3, torch4);
    Object * torch = GeometryFactory::createSurfaceOfRevolution(torchCurve);
    sceneManager->getRoot()->addChild(new Shape3D(torch));
    
    
    Vector3 shapeArray[] = {c1,c2,c3,c4,c5,c6,c7};
    //Vector3 shapeArray[] = {s1,s2,s3,s4,s5,s6,s7};
    int numElements = sizeof(shapeArray) / sizeof(Vector3);
    
    BezierCurve curvedLine(shapeArray, numElements);
    curvedLine.setTransformation(Matrix4::scale(.2,.2,.2));
    
    BezierCurve nearCircle(shapeArray, numElements);
    
    /*
    PiecewiseSpline piecewise(&gobletCurve);


    piecewise.addSpline(&curvedLine);
    piecewise.addSpline(&nearCircle);
    */
        
    Vector3 str1(0,1,0);
    Vector3 str2(0,.5,0);
    Vector3 str3(0,-.5,0);
    Vector3 str4(0,-1,0);
    
    
    BezierCurve straightLine(str1, str2, str3, str4);
    
    Circle circle2;
    circle2.setTransformation(Matrix4::rotateX(BasicMath::radians(90)));
    
    Square square;
    square.setTransformation(Matrix4::scale(4,4,4) * Matrix4::rotateX(BasicMath::radians(90)));
    
    Object * loft = GeometryFactory::createLoft(square, *track , 5 ,50);
    
    Material * extrudedShapeMaterial = new Material(Brass);
	//extrudedShapeMaterial->setShader(twoSpotTexture);
    
    // Set up the textures

    //http://ryane.com/wp-content/uploads/2007/04/rock_02.jpg
    QImage *rockImg = new QImage("images/rock_02.jpg", "jpg");
    assert(rockImg != NULL);
    Texture *rockTexture = new Texture(rockImg);
    extrudedShapeMaterial->setTexture(rockTexture);

    loft->setMaterial(extrudedShapeMaterial);
    
    Circle circle;
    circle.setTransformation(Matrix4::scale(.5,.5,.5) * Matrix4::rotateX(BasicMath::radians(90)));
    
     
    track->setTransformation(Matrix4::translate(0,0,-3) * track->getTransformation());
    

    Object * trackLoft = GeometryFactory::createLoft(circle,  *track, 10 ,50);
    //trackLoft->setMaterial(new Material(Bronze));
    
    
    sceneManager->getRoot()->addChild(new Shape3D(loft));
    sceneManager->getRoot()->addChild(new Shape3D(trackLoft));
    
    Object * mineCartObj = sceneManager->createObject();
    GeometryFactory::createSphere(mineCartObj);
    mineCartObj->setTransformation(Matrix4::scale(.001,.001,.001));
    
    Shape3D * minecartShape = new Shape3D(mineCartObj);
    minecart = new TransformGroup();
    minecart->addChild(minecartShape);
	
	// first we have to create the camera and cameraNode:
	Camera* moveCamera = new Camera();
	movingCamera = new CameraNode(moveCamera);
	// now we modify the camera so that it sits where we want it to relative to the minecart
	movingCamera->updateProjection(Vector3(0,1,0),Vector3(0,1,-1),Vector3(0,1,0));
	// then we add it as a child of the minecart so that it follows the cart around
	minecart->addChild(movingCamera);
	
	// last thing to do is make sure that when the scene starts, we are using the moving camera, not the still one
	movingCamera->use();
	stillCamera->disable();
	
	
	// now we have to set up the lighting....
    // Create a white light
    Light * white = sceneManager->createLight();
	white->setType(Light::SPOT);
    white->setSpotDirection(Vector3(0,0,-1));
    white->setDiffuseColor(Vector3(1,1,1));
    white->setAmbientColor(Vector3(.2,.2,.2));
    white->setSpecularColor(Vector3(1,1,1));
	white->setSpotCutoff(90.0);
	white->setSpotExponent(1.0);
	white->setPosition(Vector3(0,0,-0.5));
    
    whiteLight = new LightNode(white);
	minecart->addChild(whiteLight);
	
	

    sceneManager->getRoot()->addChild(minecart);
    
    
}


