#include "CMyEnemy.h"
#include "CMyEnemyAttack1.h"
#include "CMyEnemyAttack2.h"


CMyEnemy::CMyEnemy()
{
	m_pSprite = NULL;
	m_vecPostion.x = 512 >> 1;
	m_vecPostion.y = 18;

	m_rect = { (long)m_vecPostion.x , (long)m_vecPostion.y,
		(long)m_vecPostion.x + 512, (long)m_vecPostion.y + 150 };
}


CMyEnemy::~CMyEnemy()
{
	
	delete m_pAtkPatten[0];

	delete m_pHpSprite;
	m_pHpSprite = NULL;
}

bool CMyEnemy::Create(char * filename)
{
	if (m_pSprite != NULL)
		delete m_pSprite;

	m_pSprite = new dsTexture(filename);
	m_pHpSprite = new dsTexture("hp_bar.png");
	m_pHpSprite->LoadFromFilename("hp_bar.png");
	m_pLineSprite = new dsTexture("red_line.png");
	m_pLineSprite->LoadFromFilename("red_line.png");

	CMyEnemyAttack1 * atk1 = new CMyEnemyAttack1();
	atk1->Create();

	CMyEnemyAttack2 * atk2 = new CMyEnemyAttack2();
	atk2->Create();

	m_attackCount = 2;

	m_pAtkPatten[0] = atk1;
	m_pAtkPatten[1] = atk2;

	return m_pSprite->LoadFromFilename(filename);
}

void CMyEnemy::OnDraw()
{
	m_pLineSprite->Draw(0, (m_rect.top + 150) - 20, 0);

	m_pSprite->Draw(m_rect.left, m_rect.top, 0, 0, 512, 150, 0);
	m_pHpSprite->Draw(0, 0, 0, 0, m_hp, 15, 0);
	if(m_isAttackUpdate)
		m_pAtkPatten[m_currentAttack]->OnDraw();
}

void CMyEnemy::OnUpdate(DWORD tick)
{

	if (m_beforeAttackStatus == END) {
		m_isAttackUpdate = false;
		if (m_BtwTimeGap >= 1000)
			m_BtwTimeGap -= 500;

		m_BtwTimeAttack = m_BtwTimeGap;
		m_beforeAttackStatus = NOTHING;
	}

	if (m_beforeAttackStatus == NOTHING) {
		if (m_BtwTimeAttack <= 0) {
			m_currentAttack = rand() % m_attackCount;
			m_isAttackUpdate = true;
			m_pAtkPatten[m_currentAttack]->OnStart();
		}
		else {
			m_BtwTimeAttack -= tick;
		}
	}

	if(m_isAttackUpdate)
		m_beforeAttackStatus = m_pAtkPatten[m_currentAttack]->OnUpdate(tick);
}

void CMyEnemy::HaveDamage(int damage)
{
	m_hp -= damage;
}

bool CMyEnemy::IsCollision(RECT rect)
{
	if (rect.top <= (m_rect.top + 150) - 20)
		return true;

	return m_pAtkPatten[m_currentAttack]->IsCollision(rect);
}

void CMyEnemy::Init()
{
	m_hp = 1024;
	m_BtwTimeAttack = 1000;
	m_currentAttack = 0;
	m_BtwTimeGap = 3000;
	m_beforeAttackStatus = NOTHING;
	m_isAttackUpdate = false;
}

bool CMyEnemy::IsHpZero()
{
	return m_hp <= 0;
}


