#ifndef UNG_CELL_H
#define UNG_CELL_H

#include"ung.h"

class Link;

struct CellTracerDensities
{
	UNG_FLT density;
	UNG_FLT potentialEnergyDensity;
	UNG_FLT lrMomentumDensity;
	UNG_FLT udMomentumDensity;
	UNG_FLT ioMomentumDensity;
};

class Cell
{
public:
	Cell(UNG_FLT volume, UNG_FLT density, UNG_FLT potentialTemperature, UNG_FLT lrSpeed, UNG_FLT udSpeed, UNG_FLT ioSpeed);
	void setLinks(Link *leftLink, Link *rightLink, Link *topLink, Link *bottomLink);
	inline void setTopLink(Link *link);
	inline void setBottomLink(Link *link);
	inline void setLeftLink(Link *link);
	inline void setRightLink(Link *link);
	inline virtual void * const getTracerDensities() const;
	void applyAdvectionAndAcceleration();
	void completeStep();
	inline UNG_FLT getMass() const;
	inline UNG_FLT getVolume() const;
	inline UNG_FLT getDensity() const;
	inline UNG_FLT getPotentialTemperature() const;
	inline UNG_FLT getLrMomentumDensity() const;
	inline UNG_FLT getUdMomentumDensity() const;
	inline UNG_FLT getPressure() const;
	inline UNG_FLT getLrSpeed() const;
	inline UNG_FLT getUdSpeed() const;
	inline UNG_FLT getIoSpeed() const;
	inline const UNG_FLT * getLrSpeedPointer() const;
	inline const UNG_FLT * getUdSpeedPointer() const;
	inline const UNG_FLT * getIoSpeedPointer() const;
	inline UNG_FLT * getLrSpeedStepPointer() const;
	inline UNG_FLT * getUdSpeedStepPointer() const;
	inline UNG_FLT * getIoSpeedStepPointer() const;
private:
	//these are the fundamental things we need to do the simulation
	UNG_FLT m_volume;
	UNG_FLT m_mass;
	UNG_FLT m_potentialEnergy; //potential_temperature*mass
	UNG_FLT m_lrSpeed;
	UNG_FLT m_udSpeed;
	UNG_FLT m_ioSpeed;
	
	UNG_FLT m_lrMom;
	UNG_FLT m_udMom;
	UNG_FLT m_ioMom;

	UNG_FLT m_lrSpeedStep;
	UNG_FLT m_udSpeedStep;
	UNG_FLT m_ioSpeedStep;

	CellTracerDensities m_densities;

	Link *m_leftLink;
	Link *m_rightLink;
	Link *m_topLink;
	Link *m_bottomLink;

	void applyAdvection(const CellTracerDensities *densities, UNG_FLT volume);

protected:
	//We need to derive pressure too
	UNG_FLT m_pressure;

	virtual void setPressure();

	Cell();
	
};

inline void * const Cell::getTracerDensities() const
{
	return (void*) &m_densities;
}

inline const UNG_FLT * Cell::getLrSpeedPointer() const
{
	return &m_lrSpeed;
}
inline const UNG_FLT * Cell::getUdSpeedPointer() const
{
	return &m_udSpeed;
}
inline const UNG_FLT * Cell::getIoSpeedPointer() const
{
	return &m_ioSpeed;
}
inline UNG_FLT Cell::getPressure() const
{
	return m_pressure;
}

inline UNG_FLT Cell::getLrSpeed() const
{
	return m_lrSpeed;
}
inline UNG_FLT Cell::getUdSpeed() const
{
	return m_udSpeed;
}
inline UNG_FLT Cell::getIoSpeed() const
{
	return m_ioSpeed;
}
inline UNG_FLT Cell::getDensity() const
{
	return m_densities.density;
}
inline UNG_FLT Cell::getPotentialTemperature() const
{
	return m_potentialEnergy/m_mass;
}
inline void Cell::setTopLink(Link *link)
{
	m_topLink=link;
}
inline void Cell::setBottomLink(Link *link)
{
	m_bottomLink=link;
}
inline void Cell::setLeftLink(Link *link)
{
	m_leftLink=link;
}
inline void Cell::setRightLink(Link *link)
{
	m_rightLink=link;
}
#endif