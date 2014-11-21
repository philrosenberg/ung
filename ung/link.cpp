#include"link.h"
#include"cell.h"


Link::Link(Cell *cell1, Cell*cell2, UNG_FLT area, const UNG_FLT *speed1, const UNG_FLT *speed2)
	:m_cell1(cell1), m_cell2(cell2), m_area(area), m_speed1(speed1), m_speed2(speed2)
{

}

//this function's sole job is to set the parameters such that it can return the correct
//values when getLinkProperties is called
void Link::update(UNG_FLT timeStep)
{
	UNG_FLT pressureDif=(m_cell2->getPressure()-m_cell1->getPressure());
	m_pressureImpulse=-m_area*pressureDif*timeStep;
	UNG_FLT transferSpeed=(*m_speed1+*m_speed2)/UNG_FLT(2.0);
	UNG_FLT transferDistance=transferSpeed*timeStep;
	m_transferVolume=transferDistance*m_area;
	if(transferSpeed>0)
		m_sourceTracerDensities=m_cell1->getTracerDensities();
	else
		m_sourceTracerDensities=m_cell2->getTracerDensities();
}

void Link::getLinkProperties( const void *&sourceTracerDensities, UNG_FLT &transferVolume, UNG_FLT &parallelImpulse, UNG_FLT &perpendicularImpulse)
{
	sourceTracerDensities=m_sourceTracerDensities;
	transferVolume=m_transferVolume;
	parallelImpulse=m_pressureImpulse;
	perpendicularImpulse=UNG_FLT(0.0);
}


BoundaryLink::BoundaryLink(Cell *cell, UNG_FLT area, const UNG_FLT *speed)
	:Link(cell, nullptr, area, speed, nullptr)
{
}

SolidBoundaryLink::SolidBoundaryLink(Cell *cell, UNG_FLT area, const UNG_FLT *speed)
	:BoundaryLink(cell, area, speed)
{
}

void SolidBoundaryLink::update(UNG_FLT timeStep)
{
	m_sourceTracerDensities=m_cell1->getTracerDensities();
}

void SolidBoundaryLink::getLinkProperties( const void *&sourceTracerDensities, UNG_FLT &transferVolume, UNG_FLT &parallelImpulse, UNG_FLT &perpendicularImpulse)
{
	sourceTracerDensities=m_sourceTracerDensities;
	transferVolume=UNG_FLT(0.0);
	parallelImpulse=UNG_FLT(0.0);
	perpendicularImpulse=UNG_FLT(0.0);
}