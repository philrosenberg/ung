#ifndef UNG_LINK_H
#define UNG_LINK_H

#include"ung.h"

class Cell;

void inviscidLinkUpdate( void *linkVoid, UNG_FLT timestep);
void solidBoundaryLinkUpdate( void *solidBoundaryLink, UNG_FLT timestep);

class Link
{
public:
	Link(Cell *cell1, Cell*cell2, UNG_FLT area, const UNG_FLT *speed1, const UNG_FLT *speed2, void (*updateFunc)( void *, UNG_FLT ));
	~Link();
	void update(UNG_FLT timeStep);
	void getLinkProperties( const void *&sourceTracerDensities, UNG_FLT &transferVolume, UNG_FLT &parallelImpulse, UNG_FLT &perpendicularImpulse);
protected:
	UNG_FLT m_area;
	Cell *m_cell1;
	Cell *m_cell2;
	const UNG_FLT * m_speed1; 
	const UNG_FLT * m_speed2;
	UNG_FLT m_transferVolume;
	void *m_tracerDensities;

	UNG_FLT m_perpendicularImpulse;
	UNG_FLT m_parallelImpulse;
	void (*const m_updateFunc)( void *, UNG_FLT );


	Link();
};

class InviscidLink : public Link
{
	friend void inviscidLinkUpdate(void *, UNG_FLT);
public:
	InviscidLink(Cell *cell1, Cell*cell2, UNG_FLT area, const UNG_FLT *speed1, const UNG_FLT *speed2);
};

class BoundaryLink: public Link
{
public:
	BoundaryLink(Cell *cell1, UNG_FLT area, const UNG_FLT *speed, void (*updateFunc)( void *, UNG_FLT ));
};

class SolidBoundaryLink: public BoundaryLink
{
	friend void solidBoundaryLinkUpdate(void *, UNG_FLT);
public:
	SolidBoundaryLink(Cell *cell1, UNG_FLT area, const UNG_FLT *speed);
};
#endif