// Example to display multiple OpenNI device in point cloud with OpenGL
//
// 		by Heresy
// 		http://kheresy.wordpress.com
//
// version 1.00 @2014/11/18

// Standard Header
#include <iostream>
#include <vector>

// glut Header
#include <GL/glut.h>

// OpenNI Header
#include <OpenNI.h>

// helper header
#include "../../common/OpenGLCamera.h"

// namespace
using namespace std;
using namespace openni;

// class mapping to one device
class CDevice
{
public:
	Device			mDevice;
	VideoStream		mDepthStream;
	VideoStream		mColorStream;
	double			aTransformation[16];

	CDevice(const char* uri)
	{
		// initial matrix
		for (int y = 0; y < 4; ++y)
			for (int x = 0; x < 4; ++x)
			{
				int idx = x + y * 4;
				if (x == y)
					aTransformation[idx] = 1;
				else
					aTransformation[idx] = 0;
			}

		// initial OpenNI device
		if (mDevice.open(uri) == STATUS_OK)
		{
			if (mDepthStream.create(mDevice, SENSOR_DEPTH) == STATUS_OK)
			{
				// start
				mDepthStream.start();
			}
			else
			{
				cerr << "Can't create depth stream of device: " << uri << endl;
			}

			if (mColorStream.create(mDevice, SENSOR_COLOR) == STATUS_OK)
			{
				// start
				mColorStream.start();
			}
			else
			{
				cerr << "Can't create color stream of device: " << uri << endl;
			}
		}
		else
		{
			cerr << "Can't open device at: " << uri << endl;
		}
	}

	~CDevice()
	{
		if (mDepthStream.isValid())
		{
			mDepthStream.stop();
			mDepthStream.destroy();
		}

		if (mColorStream.isValid())
		{
			mColorStream.stop();
			mColorStream.destroy();
		}

		if (mDevice.isValid())
		{
			mDevice.close();
		}
	}
};

// global OpenNI object
vector<CDevice*>	g_vDevices;

// global object
SimpleCamera g_Camera;

// shutdown OpenNI when exit program
void exit_program()
{
	for (vector<CDevice*>::iterator itDevice = g_vDevices.begin(); itDevice != g_vDevices.end(); ++itDevice)
	{
		delete *itDevice;
	}
	g_vDevices.clear();
	OpenNI::shutdown();
}

// glut display function(draw)
void display()
{
	// clear previous screen
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// draw points
	glPointSize(1.0f);

	// for each device
	for (vector<CDevice*>::iterator itDevice = g_vDevices.begin(); itDevice != g_vDevices.end(); ++itDevice)
	{
		CDevice& rDevice = **itDevice;

		// get depth frame
		VideoFrameRef	vfDepthFrame;
		if (rDevice.mDepthStream.readFrame(&vfDepthFrame) == STATUS_OK)
		{
			const DepthPixel* pDepthArray = static_cast<const DepthPixel*>(vfDepthFrame.getData());

			// get color frame
			VideoFrameRef	vfColorFrame;
			if (rDevice.mColorStream.readFrame(&vfColorFrame) == STATUS_OK)
			{
				const RGB888Pixel* pColorArray = static_cast<const RGB888Pixel*>(vfColorFrame.getData());

				// apply transform matrix
				glPushMatrix();
				glMultMatrixd(rDevice.aTransformation);

				// draw points with color
				glBegin(GL_POINTS);
				int w = vfDepthFrame.getWidth(),
					h = vfDepthFrame.getHeight();
				for (int y = 0; y < h; ++y)
				{
					for (int x = 0; x < w; ++x)
					{
						int idx = x + y * w;
						const DepthPixel&	rDepth = pDepthArray[idx];
						if (rDepth != 0)
						{
							const RGB888Pixel&	rColor = pColorArray[idx];
							glColor3ub(rColor.r, rColor.g, rColor.b);

							float fX, fY, fZ;
							CoordinateConverter::convertDepthToWorld(rDevice.mDepthStream, x, y, rDepth, &fX, &fY, &fZ);
							glVertex3f(fX, fY, fZ);
						}
					}
				}
				glEnd();

				glPopMatrix();

				vfColorFrame.release();
			}
		}
		vfDepthFrame.release();
	}

	// Coordinate
	glLineWidth( 5.0f );
	glBegin( GL_LINES );
		glColor3ub( 255, 0, 0 );
		glVertex3f( 0, 0, 0 );
		glVertex3f( 100, 0, 0 );

		glColor3ub( 0, 255, 0 );
		glVertex3f( 0, 0, 0 );
		glVertex3f( 0, 100, 0 );

		glColor3ub( 0, 0, 255 );
		glVertex3f( 0, 0, 0 );
		glVertex3f( 0, 0, 100 );
	glEnd();

	// swap buffer
	glutSwapBuffers();
}

// glut idle function
void idle()
{
	glutPostRedisplay();
}

// glut keyboard function
void keyboard( unsigned char key, int x, int y )
{
	float fSpeed = 10.0f;
	switch( key )
	{
	case 'q':
		exit(0);
		break;

	case 's':
		g_Camera.MoveForward( -fSpeed );
		break;

	case 'w':
		g_Camera.MoveForward( fSpeed );
		break;

	case 'a':
		g_Camera.MoveSide( -fSpeed );
		break;

	case 'd':
		g_Camera.MoveSide( fSpeed );
		break;

	case 'z':
		g_Camera.MoveUp( -fSpeed );
		break;

	case 'x':
		g_Camera.MoveUp( fSpeed );
		break;
	}
}

// glut special keyboard function
void specialKey( int key, int x, int y )
{
	float fRotateScale = 0.01f;
	switch( key )
	{
	case GLUT_KEY_DOWN:
		g_Camera.RotateUp( -fRotateScale );
		break;

	case GLUT_KEY_UP:
		g_Camera.RotateUp( fRotateScale );
		break;

	case GLUT_KEY_RIGHT:
		g_Camera.RotateSide( fRotateScale );
		break;

	case GLUT_KEY_LEFT:
		g_Camera.RotateSide( -fRotateScale );
		break;
	}
}

int main( int argc, char** argv )
{
	// Initial OpenNI
	if( OpenNI::initialize() != STATUS_OK )
	{
		cerr << "OpenNI Initial Error: " << OpenNI::getExtendedError() << endl;
		return -1;
	}

	// enumerate devices
	Array<DeviceInfo> aDeviceList;
	OpenNI::enumerateDevices(&aDeviceList);

	// output information
	cout << "There are " << aDeviceList.getSize() << " devices on this system." << endl;
	for (int i = 0; i < aDeviceList.getSize(); ++i)
	{
		cout << "Device " << i << "\n";
		const DeviceInfo& rDevInfo = aDeviceList[i];

		cout << " " << rDevInfo.getName() << " by " << rDevInfo.getVendor() << "\n";
		cout << " PID: " << rDevInfo.getUsbProductId() << "\n";
		cout << " VID: " << rDevInfo.getUsbVendorId() << "\n";
		cout << " URI: " << rDevInfo.getUri() << endl;

		// initialize
		CDevice* pDevWin = new CDevice(aDeviceList[i].getUri());
		if (pDevWin->mColorStream.isValid() && pDevWin->mDepthStream.isValid())
		{
			// appleytest matrix
			pDevWin->aTransformation[12] = i * -1000;
			g_vDevices.push_back(pDevWin);
		}
	}

	// initial glut
	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB );

	// create glut window
	glutInitWindowSize( 640, 480 );
	glutCreateWindow( "OpenNI 3D OpenGL 3D Point" );

	glEnable( GL_DEPTH_TEST );
	glDisable( GL_LIGHTING );

	glMatrixMode( GL_PROJECTION );
	gluPerspective( 40.0, 1.0, 300.0, 20000.0 );	// FOV, aspect ration, near, far
	g_Camera.vCenter	= Vector3( 0.0, 0.0, 1000.0 );
	g_Camera.vPosition	= Vector3( 0.0, 0.0, -2000.0 );
	g_Camera.vUpper		= Vector3( 0.0, 1.0, 0.0 );
	g_Camera.SetCamera();

	// register glut callback functions
	glutDisplayFunc( display );
	glutIdleFunc( idle );
	glutKeyboardFunc( keyboard );
	glutSpecialFunc( specialKey );
	
	std::atexit(exit_program);

	// start
	glutMainLoop();

	return 0;
}
