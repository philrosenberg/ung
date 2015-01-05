#ifndef UNG_LINK_H
#define UNG_LINK_H

#include"ung.h"

class Cell;

class Link
{
public:
	Link(Cell *cell1, Cell*cell2, UNG_FLT area, const UNG_FLT *speed1, const UNG_FLT *speed2);
	~Link();
	virtual void update(UNG_FLT timeStep);
	virtual void getLinkProperties( const void *&sourceTracerDensities, UNG_FLT &transferVolume, UNG_FLT &parallelImpulse, UNG_FLT &perpendicularImpulse);
protected:
	UNG_FLT m_area;
	Cell *m_cell1;
	Cell *m_cell2;
	const UNG_FLT * m_speed1; 
	const UNG_FLT * m_speed2;
	UNG_FLT m_pressureImpulse;
	UNG_FLT m_transferVolume;
	void *m_tracerDensities;


	Link();
};

class BoundaryLink: public Link
{
public:
	BoundaryLink(Cell *cell1, UNG_FLT area, const UNG_FLT *speed);
	virtual void update(UNG_FLT timeStep)=0;
	virtual void getLinkProperties( const void *&sourceTracerDensities, UNG_FLT &transferVolume, UNG_FLT &parallelImpulse, UNG_FLT &perpendicularImpulse)=0;
};

class SolidBoundaryLink: public BoundaryLink
{
public:
	SolidBoundaryLink(Cell *cell1, UNG_FLT area, const UNG_FLT *speed);
	virtual void update(UNG_FLT timeStep);
	virtual void getLinkProperties( const void *&sourceTracerDensities, UNG_FLT &transferVolume, UNG_FLT &parallelImpulse, UNG_FLT &perpendicularImpulse);

};
#endif