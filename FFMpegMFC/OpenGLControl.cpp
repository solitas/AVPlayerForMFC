#include "stdafx.h"
#include "OpenGLControl.h"


// COpenGLControl

IMPLEMENT_DYNAMIC(COpenGLControl, CWnd)

COpenGLControl::COpenGLControl()
: dc(NULL)
, rotation(0)
{

}

COpenGLControl::~COpenGLControl()
{
	if (dc)
	{
		delete dc;
	}
}


BEGIN_MESSAGE_MAP(COpenGLControl, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// COpenGLControl 메시지 처리기입니다.



void COpenGLControl::Create(CRect rect, CWnd* parent)
{
	CString className = AfxRegisterWndClass(
		CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
		NULL,
		(HBRUSH)GetStockObject(BLACK_BRUSH),
		NULL);

	CreateEx(
		0,
		className,
		"OpenGL",
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		rect,
		parent,
		0);

}

void COpenGLControl::InitGL(void)
{
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	//glEnable(GL_DEPTH_TEST);     
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

void COpenGLControl::DrawGLScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT
		| GL_DEPTH_BUFFER_BIT);


	glLoadIdentity();

	//***************************
	// DRAWING CODE
	//***************************

	// glTranslatef(0.0f,0.0f,-5.0f);
	glRotatef(rotation, 0.0f, 1.0f, 0.0f);
	if (pData)
	{
		GLuint* texName = new GLuint[1];
		texName[0] = 0;
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, texName);
		glBindTexture(GL_TEXTURE_2D, texName[0]);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pData);

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glBindTexture(GL_TEXTURE_2D, texName[0]);
		glBegin(GL_QUADS);
		glTexCoord2d(0.0f, 0.0f); glVertex3f(-50.0, -50.0f, 50.0f);
		glTexCoord2d(1.0f, 0.0f); glVertex3f(50.0, -50.0f, 50.0f);
		glTexCoord2d(1.0f, 1.0f); glVertex3f(50.0, 50.0f, 50.0f);
		glTexCoord2d(0.0f, 1.0f); glVertex3f(-50.0, 50.0f, 50.0f);
		glEnd();
	}
	else
	{
		glBegin(GL_TRIANGLES);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-1.0f, -1.0f, 0.0f);
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 1.0f, 0.0f);
		glEnd();
	}


	SwapBuffers(dc->m_hDC);
}

void COpenGLControl::OnPaint()
{
	rotation += 0.01f;

	if (rotation >= 360.0f)
	{
		rotation -= 360.0f;
	}

	/** OpenGL section **/

	openGLDevice.makeCurrent();

	DrawGLScene();
}

void COpenGLControl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (cy == 0)
	{
		cy = 1;
	}


	glViewport(0, 0, cx, cy);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();


	glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f);
	// gluPerspective(45.0f,cx/cy,0.1f,100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int COpenGLControl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	dc = new CClientDC(this);

	openGLDevice.create(dc->m_hDC);
	InitGL();
	return 0;
}

BOOL COpenGLControl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	// return CWnd::OnEraseBkgnd(pDC);
	return TRUE;
}

void COpenGLControl::decode(void* obj, void** ptr)
{
	if (ptr[0] != NULL)
	{
		//pVideoData = ptr[0];
		//EnterCriticalSection(&openGLControl.waveCriticalSection2);
		pData = ptr[0];
		//sLeaveCriticalSection(&openGLControl.waveCriticalSection2);
	}
	else
	{
		//EnterCriticalSection(&openGLControl.waveCriticalSection2);
		pData = NULL;
		//LeaveCriticalSection(&openGLControl.waveCriticalSection2);
	}
}
