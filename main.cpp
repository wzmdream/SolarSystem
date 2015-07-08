#include <GL/glut.h>
#include <windows.h>
#include <mmsystem.h>
#include <math.h>
#include "rf.h"
#include "mf.h"
#include "draw.h"

#define  DEG_TO_RAD 0.017453

static GLuint texName;
bool   g_bOrbitOn = true;
int show_orbit = 1;
int count = 0;
float  g_fSpeedmodifier = 1.0f;
float  g_fElpasedTime;
double g_dCurrentTime;
double g_dLastTime;

static GLdouble viewer[]= {-sin(15*DEG_TO_RAD),15*cos(15*DEG_TO_RAD), -15}; /* initial viewer location */
static int year =0 ,day = 0;
float angle=0.0;

char str[] = "'h': 查看帮助\r\n' ': 暂停\r\n'+': 加快所有星体的运行速度\r\n'-': 减慢所有星体的运行速度\r\n'a': 视图左倾斜\r\n'd': 视图右倾斜\r\n's': 增大视野\r\n'w': 减小视野\r\n'q': 增加俯瞰角度\r\n'e': 降低俯瞰角度\r\n'z': 是否显示运行轨道";

TEXTUREIMAGE  spaceImg;
TEXTUREIMAGE  sunImg;
TEXTUREIMAGE  mercuImg;
TEXTUREIMAGE  venusImg;
TEXTUREIMAGE  earthImg;
TEXTUREIMAGE  moonImg;;
TEXTUREIMAGE  marsImg;
TEXTUREIMAGE  jupiterImg;
TEXTUREIMAGE  saturnImg;


#define NUM_TEXTURES    12
GLuint  textureObjects[NUM_TEXTURES];

MATERIAL material[NUM_TEXTURES];
// 光照
LIGHT light =
{
	{0.0, 0.0, 0.0, 1.0},// 环境光参数
	{1.0, 1.0, 1.0, 1.0},// 漫射光参数
	{1.0, 1.0, 1.0, 1.0},// 镜面光参数
	{0.0, 0.0, 0.0, 1.0}	// 光源位置
};


MATERIAL materialSpace =  
{
    {0.4, 0.4, 0.4, 1.0},
    {0.0, 0.0, 0.0, 1.0},
    {0.0, 0.0, 0.0, 1.0},
    0.0
};

MATERIAL materialSun = 
{
    {100.0, 100.0, 100.0, 20.0},
    {0.8, 0.8, 0.8, 1.0},
    {0.0, 0.0, 0.0, 1.0},
    0.0
};
MATERIAL materialEarth = 
{
    {0.1, 0.1, 0.1, 1.0},
    {1.0, 1.0, 1.0, 1.0},
    {0.0, 0.0, 0.0, 1.0},
    100.0
};
MATERIAL materialMoon = 
{
    {0.1, 0.1, 0.1, 1.0},
    {1.0, 1.0, 1.0, 1.0},
    {0.0, 0.0, 0.0, 1.0},
    0.0
};


//create a disk
GLUquadricObj *myDisk;

void myInit()
{ 
	glClearColor(0.0,0.0,0.0,0.0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_FLAT);

	material[0] = materialSun;
	material[1] = materialEarth;
	material[2] = materialMoon;
	material[3] = materialSpace;

	GLfloat light_ambient[]={1.0, 1.0, 1.0, 1.0};
	GLfloat light_diffuse[]={1.0, 1.0, 1.0, 1.0};
	GLfloat light_specular[]={1.0, 1.0, 1.0, 1.0};

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}  

void myReshape(int w,int h)
{
	glViewport(0,0,(GLfloat) w, (GLfloat) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0,(GLfloat)w/(GLfloat)h,1.0,200.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(viewer[0],viewer[1],viewer[2],0,0,0,0,0,1);
	
}


void myDisplay(void)
{

	glLoadIdentity();

	//设置观察点的位置和观察的方向
	gluLookAt(viewer[0],viewer[1],viewer[2],0,0,0,0,0,1);

	//获得系统时间使太阳系有动态效果
	g_dCurrentTime = timeGetTime();
	g_fElpasedTime = (float)((g_dCurrentTime - g_dLastTime) * 0.001);
	g_dLastTime    = g_dCurrentTime;
	GLfloat light_position[]={0.0, 0.0, 0.0, 1.0};

	static float fSunSpin    = 0.0f;//太阳自转速度

	static float fMercuSpin  = 0.0f;//水星自转速度
	static float fMercuOrbit = 0.0f;//水星公转速度
	
	static float fVenusSpin  = 0.0f;//金星自转速度
	static float fVenusOrbit = 0.0f;//金星公转速度

	static float fEarthSpin  = 0.0f;//地球自转速度    
	static float fEarthOrbit = 0.0f;//地球公转速度

	static float fMoonSpin   = 0.0f;//月亮自转速度
	static float fMoonOrbit  = 0.0f;//月亮公转速度

	static float fMarsSpin   = 0.0f;//火星自转速度
	static float fMarsOrbit  = 0.0f;//火星公转速度

	static float fJupiterSpin   = 0.0f;//木星自转速度
	static float fJupiterOrbit  = 0.0f;//木星公转速度

	static float fSaturnSpin   = 0.0f;//土星自转速度	
	static float fSaturnOrbit  = 0.0f;//土星公转速度


	float spaceWidth =18.50;
	float spacedepth =8.0;

	static float mercuToSun=-3.0;//水星离太阳的距离
	static float VenToSun =-5.0;//金星离太阳的距离
	static float earthToSun =-7.0;//地球离太阳的距离	
	static float moonToEarth = -0.9f;//月亮离地球的距离
	static float MarsToSun =-9.0;//火星离太阳的距离
	static float JupiterToSun =-12.0;//木星离太阳的距离
    static float SaturnToSun =-14.0;//土星离太阳的距离


	
	 if( g_bOrbitOn == true )
		{
			fSunSpin += g_fSpeedmodifier * (g_fElpasedTime * 10.0f);
       
			fEarthSpin  += g_fSpeedmodifier * (g_fElpasedTime * 10000.0f);
			fEarthOrbit += g_fSpeedmodifier * (g_fElpasedTime * 20.5f);

			fMoonSpin  += g_fSpeedmodifier * (g_fElpasedTime * 50.0f);
			fMoonOrbit += g_fSpeedmodifier * (g_fElpasedTime * 200.0f);

			fVenusSpin += g_fSpeedmodifier * (g_fElpasedTime * 40.0f);
			fVenusOrbit += g_fSpeedmodifier * (g_fElpasedTime * 20.0f);

			fMercuSpin+= g_fSpeedmodifier * (g_fElpasedTime * 5.0f);
			fMercuOrbit+= g_fSpeedmodifier * (g_fElpasedTime * 10.0f);

			fMarsSpin+= g_fSpeedmodifier * (g_fElpasedTime * 30.0f);
			fMarsOrbit+= g_fSpeedmodifier * (g_fElpasedTime * 15.0f);

			fJupiterSpin+= g_fSpeedmodifier * (g_fElpasedTime * 23.0f);
			fJupiterOrbit+= g_fSpeedmodifier * (g_fElpasedTime * 13.0f);

			fSaturnSpin+= g_fSpeedmodifier * (g_fElpasedTime * 21.0f);
			fSaturnOrbit+= g_fSpeedmodifier * (g_fElpasedTime * 15.0f);

			
	 }

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMatrixMode( GL_MODELVIEW ); 


	// 设置纹理映射
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//光源位置

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);


	/************************space背景*******************************/
	//space材质
	glMaterialfv(GL_FRONT, GL_SPECULAR,material[3].specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, material[3].ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material[3].diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, material[3].shininess);

	//sapce的位置，大小，转速，和纹理。
	glPushMatrix();
	//glRotatef(fSpace,0.0,0.0,1.0);
	mf(spaceImg,& textureObjects[0]);
	glRotatef(90,1,1.0,0.0);
	gltDrawSphere(40,100,100);
	glPopMatrix();
	glDeleteTextures(1, &textureObjects[0]);

   
	/************************太阳*******************************/
    //设置太阳材质
	glMaterialfv(GL_FRONT, GL_SPECULAR,material[0].specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, material[0].ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material[0].diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, material[0].shininess);
	glColor3f(1,1,1);

	//太阳的位置，大小，转速，和纹理
	glPushMatrix();
	glRotatef(10,1,0.0,0.0);
	glRotatef(fSunSpin,0.0,1.0,0.0);
	glRotatef(90,1,0.0,0.0);
	mf(sunImg,& textureObjects[0]);
   	gltDrawSphere(2,200,200); 
	glPopMatrix();
	glDeleteTextures(1, &textureObjects[0]);
	

    /************************地球*******************************/
    //设置地球材质
	glMaterialfv(GL_FRONT, GL_SPECULAR,material[1].specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, material[1].ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material[1].diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, material[1].shininess);

	//地球的位置，大小，转速，和纹理
	glPushMatrix();
	glRotatef(fEarthOrbit,0.0,1.0,0.0);
	glTranslatef(0.0,0.0,earthToSun);
	glRotatef(fEarthSpin,0.0,1.0,0.0);
	glRotatef(10,0,0.0,1.0);
	glRotatef(90,1,0.0,0.0);
	mf(earthImg,& textureObjects[0]);
   	gltDrawSphere(0.6,100,100); //earth
	glPopMatrix();

	//地球的轨道
	 if(show_orbit)
	{
		 glBegin(GL_LINE_LOOP);
		for(angle=0;angle<=360;angle++)
		{	
			glVertex3f(earthToSun*sin(DEG_TO_RAD*angle),
			0,earthToSun*cos(DEG_TO_RAD*angle));		
		}
		glEnd();
		glDeleteTextures(1, &textureObjects[0]);
	}
	
	
	

	/************************月亮*******************************/
	
	glPushMatrix();
	glRotatef(fEarthOrbit, 0.0f, 1.0f, 0.0f );  
	glTranslatef( 0.0f, 0.0f, earthToSun);       
	glRotatef( fMoonOrbit, 0.0f, 1.0f, 0.0f ); 
	glTranslatef( 0.0f, 0.0f, moonToEarth );   
	glRotatef( fMoonSpin, 0.0f, 1.0f, 0.0f );   
	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
	glRotatef(90,1,0.0,0.0);
	mf(moonImg,& textureObjects[0]);
   	gltDrawSphere(0.2,50,20); //moon
   	glPopMatrix();
	glDeleteTextures(1, &textureObjects[0]);
	

	/************************金星*******************************/
	
	glColor3f(192,122,24);
	glPushMatrix();
	glRotatef(fVenusOrbit,0.0,1.0,0.0);
	glTranslatef(0.0,0.0,VenToSun);
	glRotatef(fVenusSpin,0.0,1.0,0.0);
	glRotatef(90,1,0.0,0.0);
	mf(venusImg,& textureObjects[0]);
   	gltDrawSphere(0.4,150,200); 
	glPopMatrix();	

	//金星的轨道
	if(show_orbit)
	{
		glBegin(GL_LINE_LOOP);
		for(angle=0;angle<=360;angle++)
            glVertex3f(VenToSun*sin(DEG_TO_RAD*angle),0,VenToSun*cos(DEG_TO_RAD*angle));		
		glEnd();
		glDeleteTextures(1, &textureObjects[0]);
	}

    /************************水星*******************************/
	
	glColor3f(192,122,24);
	glPushMatrix();
	glRotatef(fMercuOrbit,0.0,1.0,0.0);
	glTranslatef(0.0,0.0,mercuToSun);
	glRotatef(fMercuSpin,0.0,1.0,0.0);
	glRotatef(90,1,0.0,0.0);
	mf(mercuImg,& textureObjects[0]);
   	gltDrawSphere(0.3,150,200); 
	glPopMatrix();	

	//轨道
	if(show_orbit)
	{
		glBegin(GL_LINE_LOOP);
		for(angle=0;angle<=360;angle++)
            glVertex3f(mercuToSun*sin(DEG_TO_RAD*angle),0,mercuToSun*cos(DEG_TO_RAD*angle));		
		glEnd();
		glDeleteTextures(1, &textureObjects[0]);
	}

	/************************火星*******************************/
	
	glColor3f(192,122,24);
	glPushMatrix();
	glRotatef(fMarsOrbit,0.0,1.0,0.0);
	glTranslatef(0.0,0.0,MarsToSun);
	glRotatef(fMarsSpin,0.0,1.0,0.0);
	glRotatef(90,1,0.0,0.0);
	mf(marsImg,& textureObjects[0]);
   	gltDrawSphere(0.5,150,200); 
	glPopMatrix();	

	//轨道
	if(show_orbit)
	{
		glBegin(GL_LINE_LOOP);
		for(angle=0;angle<=360;angle++)
            glVertex3f(MarsToSun*sin(DEG_TO_RAD*angle),0,MarsToSun*cos(DEG_TO_RAD*angle));		
		glEnd();
		glDeleteTextures(1, &textureObjects[0]);
	}

	/************************木星*******************************/
	
	glColor3f(192,122,24);
	glPushMatrix();
	glRotatef(fJupiterOrbit,0.0,1.0,0.0);
	glTranslatef(0.0,0.0,JupiterToSun);
	glRotatef(fJupiterSpin,0.0,1.0,0.0);
	glRotatef(90,1,0.0,0.0);
	mf(jupiterImg,& textureObjects[0]);
   	gltDrawSphere(0.8,150,200); 
	glPopMatrix();	

	if(show_orbit)
	{
		glBegin(GL_LINE_LOOP);
		for(angle=0;angle<=360;angle++)
            glVertex3f(JupiterToSun*sin(DEG_TO_RAD*angle),0,JupiterToSun*cos(DEG_TO_RAD*angle));		
		glEnd();
		glDeleteTextures(1, &textureObjects[0]);
	}


   /************************土星*******************************/
	
	glColor3f(192,122,24);
	glPushMatrix();
	glRotatef(fSaturnOrbit,0.0,1.0,0.0);
	glTranslatef(0.0,0.0,SaturnToSun);
	glRotatef(fSaturnSpin,0.0,1.0,0.0);
	glRotatef(90,1,0.0,0.0);
	mf(saturnImg,& textureObjects[0]);
   	gltDrawSphere(0.4,150,200); 
	glPopMatrix();	

	//土星的轨道
	if(show_orbit)
	{
		glBegin(GL_LINE_LOOP);
		for(angle=0;angle<=360;angle++)
            glVertex3f(SaturnToSun*sin(DEG_TO_RAD*angle),0,SaturnToSun*cos(DEG_TO_RAD*angle));		
		glEnd();
		glDeleteTextures(1, &textureObjects[0]);
	}

    /************************  土星的环******************************/

	glMaterialfv(GL_FRONT, GL_SPECULAR,material[1].specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, material[1].ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material[1].diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, material[1].shininess);

	
	glColor3f(192,122,24);
	glPushMatrix();
	glRotatef(fSaturnOrbit,0.0,1.0,0.0);
	glTranslatef(0.0,0.0,SaturnToSun);
	glRotatef(fSaturnSpin,0.0,1.0,0.0);
	glRotatef(90,1,0.0,0.0);
	glColor4f(0.5f,0.5f,0.5f,1.0f);
	myDisk = gluNewQuadric();
	gluQuadricDrawStyle(myDisk,GLU_FILL);
   	gluDisk(myDisk,0.5,0.7,80,30); 
	glPopMatrix();	
	/************************************************************************/

	glutSwapBuffers();	//交换前台和后台缓存
	glFlush();
		
}

//键盘回调函数
void myKeyboard(unsigned char key ,int x,int y)
{
	switch(key)
	{
	case ' ':  //暂停
		g_bOrbitOn = !g_bOrbitOn;
		glutPostRedisplay();
		break;
	
    case'+':  //增加运转速度
		++g_fSpeedmodifier;
		glutPostRedisplay();
		break;


	case'-':  //降低运转速度
		--g_fSpeedmodifier;
		glutPostRedisplay();
		break;

	case'a':  //视图左倾斜
		viewer[0]+= 0.5;
		break;

	case'd':  //视图右倾斜
		viewer[0]-= 0.5;
		break;

	case's':  //增大视野
		viewer[2]+=0.5;
		break;

	case'w':  //减小视野
		viewer[2]-=0.5;
		break;

	case'q':  //增加俯瞰角度
		viewer[1]+=0.5;
		break;

	case'e':  //降低俯瞰角度
		viewer[1]-=0.5;
		break;

	case'z':  //是否显示运转轨道
			{
				show_orbit = count%2;
				count++;
			}
			break;
	case 'h':
		::MessageBox(NULL,str,"帮助",0);
		break;
	default:
		break;

	}
	myDisplay();
}


//鼠标回调函数
void myMouse(int button, int state, int x, int y)
{

/* mouse callback, selects an axis about which to rotate */

	static int axis=0;

	if(button==GLUT_LEFT_BUTTON && state == GLUT_DOWN) 
	{
		axis++;
		axis=axis%5;
	}
	
	if(button==GLUT_RIGHT_BUTTON && state == GLUT_DOWN) 
	{
		axis--;
		if(axis==-1)
			axis=4;
	}
		switch(axis)
		{
		case 0:
			viewer[0]=-sin(10*DEG_TO_RAD);
			viewer[1]=cos(10*DEG_TO_RAD);
			viewer[2]=-15;
			break;

		case 1:
			viewer[0]=1;
			viewer[1]=20;
			viewer[2]=1;
			break;
			
		case 2:
			viewer[0]=-sin(10*DEG_TO_RAD);
			viewer[1]=10*cos(10*DEG_TO_RAD);
			viewer[2]=-13;
			break;

		case 3:
			viewer[0]=-5;
			viewer[1]=15;
			viewer[2]=-15;
			break;


		case 4:
			viewer[0]=-sin(10*DEG_TO_RAD);
			viewer[1]=10*cos(10*DEG_TO_RAD);
			viewer[2]=-25;
			break;

		default:
			break;
		}
		myDisplay();
	
}


int main(int argc, char**argv)
{
	//读取图片
	LoadBmp("Sun.bmp" , &sunImg);
	LoadBmp("Earth.bmp", &earthImg);
	LoadBmp("moon.bmp" , &moonImg);
	LoadBmp("venus.bmp" , &venusImg);
	LoadBmp("space.bmp", &spaceImg);
	LoadBmp("mercu.bmp", &mercuImg);
	LoadBmp("mars.bmp"  , &marsImg);
	LoadBmp("jupiter.bmp", &jupiterImg);
	LoadBmp("saturn.bmp" , &saturnImg);
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(800,600);
	glutCreateWindow("徐杰（0843042026）");
	myInit();
	glutDisplayFunc(myDisplay);
	glutIdleFunc(myDisplay);
	glutReshapeFunc(myReshape);
	glutKeyboardFunc(myKeyboard);
	glutMouseFunc(myMouse);
	::MessageBox(NULL,str,"帮助",0);
	glutMainLoop();
	glDeleteTextures(NUM_TEXTURES, textureObjects);
	return 0;
}

