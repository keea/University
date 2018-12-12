#include "MyCharacter.h"
#include "CMyMath.h"
#include <math.h>

CMyCharacter::CMyCharacter()
{
	m_pSprite = NULL;
}

CMyCharacter::~CMyCharacter()
{
}

bool CMyCharacter::Create(char * filename)
{
	if (m_pSprite != NULL)
		delete m_pSprite;

	m_pSprite = new dsTexture(filename);

	return m_pSprite->LoadFromFilename(filename);
}

void CMyCharacter::OnDraw(float x, float y)
{
	m_pSprite->DrawRotation(m_vecPosition.x, m_vecPosition.y, m_angle);
}

void CMyCharacter::OnUpdate(DWORD tick)
{
	m_vecPosition.x += (m_vecDirection.x * m_speed);
	m_vecPosition.y += (m_vecDirection.y * m_speed);

	m_vecDirection = { 0,0 };
}

void CMyCharacter::Init(float x, float y)
{
	m_vecPosition.x = x;
	m_vecPosition.y = y;
	m_speed = 10;
}

void CMyCharacter::SetRotationAngle(POINT targetPos)
{
	//위로 향하는 기본 벡터
	Vector2 playerDirVec = { 0.0f, -1.0f };
	
	//점 - 점 = 벡터
	Vector2 vecTargetDirection = { targetPos.x - m_vecPosition.x,
		targetPos.y - m_vecPosition.y };

	//각도를 구함.
	m_angle = -RADIAN_DEGREE(atan2(vecTargetDirection.x, vecTargetDirection.y));

	m_vecAtkDirection = vecTargetDirection;
}

void CMyCharacter::SetMoveDirection(MOVEDIR dir)
{
	m_vecDirection = { 0,0 };
	if (dir == LEFT)
		m_vecDirection.x -= 1;
	if (dir == RIGHT)
		m_vecDirection.x += 1;
	if (dir == UP)
		m_vecDirection.y -= 1;
	if (dir == DOWN)
		m_vecDirection.y += 1;
}

Vector2 CMyCharacter::GetCenterPos()
{
	return Vector2();
}
