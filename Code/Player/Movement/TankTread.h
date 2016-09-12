#pragma once

class CTankTread
{
public:
    CTankTread(float fMaxForce, Vec2 localPos);
    ~CTankTread();

	float GetCurrentForce();
	void SetThrottle(float fThrottle);
    Vec2 GetLocalPosition() { return m_vecLocalPosition; }
    float GetThrottle() { return m_fThrottle; }
private:
	float m_fThrottle;
	float m_fMaxForce;
    Vec2 m_vecLocalPosition;
};