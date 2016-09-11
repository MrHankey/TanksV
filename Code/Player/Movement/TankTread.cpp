#include "TankTread.h"

CTankTread::CTankTread(float fMaxForce) : m_fMaxForce(fMaxForce)
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
	m_fThrottle = fThrottle;
}
