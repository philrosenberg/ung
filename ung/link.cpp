#include"link.h"
#include"cell.h"
#include<memory>

void inviscidLinkUpdate( void *linkVoid, UNG_FLT timeStep)
{
	InviscidLink *link=(InviscidLink*)linkVoid;
	UNG_FLT pressureDif=(link->m_cell2->getPressure()-link->m_cell1->getPressure());
	UNG_FLT pressureImpulse=-link->m_area*pressureDif*timeStep/UNG_FLT(2.0);
	link->m_parallelImpulse=pressureImpulse;
	UNG_FLT transferSpeed=(*link->m_speed1+*link->m_speed2)/UNG_FLT(2.0);
	UNG_FLT transferDistance=transferSpeed*timeStep;
	link->m_transferVolume=transferDistance*link->m_area;
	link->m_cell1->averageTracerDensities(link->m_tracerDensities, link->m_cell1->getTracerDensities(), link->m_cell2->getTracerDensities());
}
void solidBoundaryLinkUpdate( void *solidBoundaryLink, UNG_FLT timestep)
{
}

Link::Link(Cell *cell1, Cell*cell2, UNG_FLT area, const UNG_FLT *speed1, const UNG_FLT *speed2, void (*updateFunc)( void *, UNG_FLT ))
	:m_cell1(cell1), m_cell2(cell2), m_area(area), m_speed1(speed1), m_speed2(speed2), m_updateFunc(updateFunc)
{
	m_tracerDensities=(void*)new char[cell1->getTracerDensitySize()];
}

Link::~Link()
{
	delete m_tracerDensities;
}

//this function's sole job is to set the parameters such that it can return the correct
//values when getLinkProperties is called
void Link::update(UNG_FLT timeStep)
{
	m_updateFunc(this, timeStep);
	/*UNG_FLT pressureDif=(m_cell2->getPressure()-m_cell1->getPressure());
	UNG_FLT pressureImpulse=-m_area*pressureDif*timeStep/UNG_FLT(2.0);
	m_parallelImpulse=pressureImpulse;
	UNG_FLT transferSpeed=(*m_speed1+*m_speed2)/UNG_FLT(2.0);
	UNG_FLT transferDistance=transferSpeed*timeStep;
	m_transferVolume=transferDistance*m_area;
	m_cell1->averageTracerDensities(m_tracerDensities, m_cell1->getTracerDensities(), m_cell2->getTracerDensities());*/
}

void Link::getLinkProperties( const void *&sourceTracerDensities, UNG_FLT &transferVolume, UNG_FLT &parallelImpulse, UNG_FLT &perpendicularImpulse)
{
	sourceTracerDensities=m_tracerDensities;
	transferVolume=m_transferVolume;
	parallelImpulse=m_parallelImpulse;
	perpendicularImpulse=m_perpendicularImpulse;
}

InviscidLink::InviscidLink(Cell *cell1, Cell*cell2, UNG_FLT area, const UNG_FLT *speed1, const UNG_FLT *speed2)
	:Link(cell1, cell2, area, speed1, speed2, inviscidLinkUpdate)
{
}

BoundaryLink::BoundaryLink(Cell *cell, UNG_FLT area, const UNG_FLT *speed, void (*updateFunc)( void *, UNG_FLT ))
	:Link(cell, nullptr, area, speed, nullptr, updateFunc)
{
	size_t size=cell->getTracerDensitySize();
	char* dest=(char*)m_tracerDensities;
	for(size_t i=0; i<size; ++i)
		dest[i]=0;
}

SolidBoundaryLink::SolidBoundaryLink(Cell *cell, UNG_FLT area, const UNG_FLT *speed)
	:BoundaryLink(cell, area, speed, solidBoundaryLinkUpdate)
{
	m_transferVolume=0;
	m_parallelImpulse=0;
	m_perpendicularImpulse=0;
}

//void SolidBoundaryLink::update(UNG_FLT timeStep)
//{
//}
