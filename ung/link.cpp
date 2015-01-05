#include"link.h"
#include"cell.h"
#include<memory>

Link::Link(Cell *cell1, Cell*cell2, UNG_FLT area, const UNG_FLT *speed1, const UNG_FLT *speed2)
	:m_cell1(cell1), m_cell2(cell2), m_area(area), m_speed1(speed1), m_speed2(speed2)
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
	UNG_FLT pressureDif=(m_cell2->getPressure()-m_cell1->getPressure());
	m_pressureImpulse=-m_area*pressureDif*timeStep/UNG_FLT(2.0);
	UNG_FLT transferSpeed=(*m_speed1+*m_speed2)/UNG_FLT(2.0);
	UNG_FLT transferDistance=transferSpeed*timeStep;
	m_transferVolume=transferDistance*m_area;
	m_cell1->averageTracerDensities(m_tracerDensities, m_cell1->getTracerDensities(), m_cell2->getTracerDensities());
}

void Link::getLinkProperties( const void *&sourceTracerDensities, UNG_FLT &transferVolume, UNG_FLT &parallelImpulse, UNG_FLT &perpendicularImpulse)
{
	sourceTracerDensities=m_tracerDensities;
	transferVolume=m_transferVolume;
	parallelImpulse=m_pressureImpulse;
	perpendicularImpulse=UNG_FLT(0.0);
}


BoundaryLink::BoundaryLink(Cell *cell, UNG_FLT area, const UNG_FLT *speed)
	:Link(cell, nullptr, area, speed, nullptr)
{
	size_t size=cell->getTracerDensitySize();
	char* dest=(char*)m_tracerDensities;
	for(size_t i=0; i<size; ++i)
		dest[i]=0;
}

SolidBoundaryLink::SolidBoundaryLink(Cell *cell, UNG_FLT area, const UNG_FLT *speed)
	:BoundaryLink(cell, area, speed)
{
}

void SolidBoundaryLink::update(UNG_FLT timeStep)
{
}

void SolidBoundaryLink::getLinkProperties( const void *&sourceTracerDensities, UNG_FLT &transferVolume, UNG_FLT &parallelImpulse, UNG_FLT &perpendicularImpulse)
{
	sourceTracerDensities=m_tracerDensities;
	transferVolume=UNG_FLT(0.0);
	parallelImpulse=UNG_FLT(0.0);
	perpendicularImpulse=UNG_FLT(0.0);
}
