#include "TankTread.h"

CTankTread::CTankTread(float fMaxForce, Vec2 localPos) : m_fMaxForce(fMaxForce), m_vecLocalPosition(localPos)
{
}

CTankTread::~CTankTread()
{
}

float CTankTread::GetCurrentForce()
{
	return m_fMaxForce*m_fThrottle;
}

void CTankTread::SetThrottle(float fThrottle)
{
	m_fThrottle = CLAMP(fThrottle, -1.0f, 1.0f);
}
