#pragma once

class CTankTread
{
public:
	CTankTread(float fMaxForce);
	~CTankTread();

	float GetCurrentForce();
	void SetThrottle(float fThrottle);
private:
	float m_fThrottle;
	float m_fMaxForce;
};