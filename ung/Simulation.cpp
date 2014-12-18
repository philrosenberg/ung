#include "simulation.h"
#include "cell.h"
#include "link.h"

Simulation::Simulation()
{
	m_nCells=0;
	m_nLinks=0;
	m_cells=nullptr;
	m_links=nullptr;
}

void Simulation::Step( UNG_FLT timeStep)
{
	for(size_t i=0; i<m_nLinks; ++i)
		m_links[i]->update(timeStep);
	for(size_t i=0; i<m_nCells; ++i)
		m_cells[i]->applyAdvectionAndAcceleration();
	for(size_t i=0; i<m_nCells; ++i)
		m_cells[i]->completeStep();
}

RectangularSimulation::RectangularSimulation(size_t nHoriz, size_t nVert, const UNG_FLT *densities, const UNG_FLT *potentialTemperatures, 
											 const UNG_FLT* horizontalSpeeds, const UNG_FLT* verticalSpeeds)
{
	m_nCells=nHoriz*nVert;
	m_cells=nullptr;
	m_links=nullptr;
	try
	{
		//create cell arrays
		m_cells=new Cell*[m_nCells];

		//initiallize all cell pointers to NULL
		for(size_t i=0; i< m_nCells; ++i)
			m_cells[i]=nullptr;
		
		//create the cells
		for(size_t i=0; i< m_nCells; ++i)
			m_cells[i]=new Cell(UNG_FLT(1.0), 
				densities ? densities[i] : UNG_FLT(1.18835),
				potentialTemperatures ? potentialTemperatures[i] : UNG_FLT(293.15),
				horizontalSpeeds ? horizontalSpeeds[i] : UNG_FLT(0.0),
				verticalSpeeds ? verticalSpeeds[i] : UNG_FLT(0.0), UNG_FLT(0.0));
	}
	catch(...)
	{
		if(m_cells)
		{
			for(size_t i=0; i<m_nCells; ++i)
				if(m_cells[i])
					delete m_cells[i];
			delete[] m_cells;
		}
		throw;
	}
}



void RectangularSimulation::getHorizontalSpeeds( UNG_FLT* horizontalSpeeds ) const
{
	for(size_t i=0; i<m_nCells; ++i)
		horizontalSpeeds[i]=m_cells[i]->getLrSpeed();
}

void RectangularSimulation::getVerticalSpeeds( UNG_FLT* verticalSpeeds ) const
{
	for(size_t i=0; i<m_nCells; ++i)
		verticalSpeeds[i]=m_cells[i]->getUdSpeed();
}


void RectangularSimulation::getDensities( UNG_FLT* densities ) const
{
	for(size_t i=0; i<m_nCells; ++i)
		densities[i]=m_cells[i]->getDensity();
}

void RectangularSimulation::getPotentialTemperatures( UNG_FLT* potentialTemperatures ) const
{
	for(size_t i=0; i<m_nCells; ++i)
		potentialTemperatures[i]=m_cells[i]->getPotentialTemperature();
}

void RectangularSimulation::getTemperatures( UNG_FLT* temperatures ) const
{
	for(size_t i=0; i<m_nCells; ++i)
		temperatures[i]=m_cells[i]->getTemperature();
}

RectangularSimulationSolidEdges::RectangularSimulationSolidEdges( size_t nHoriz, size_t nVert, const UNG_FLT *densities, const UNG_FLT *potentialTemperatures,
		const UNG_FLT* horizontalSpeeds, const UNG_FLT* verticalSpeeds)
		:RectangularSimulation(nHoriz, nVert, densities, potentialTemperatures, horizontalSpeeds, verticalSpeeds)
{
	setupLinks(nHoriz, nVert);
}

void RectangularSimulationSolidEdges::setupLinks( size_t nHoriz, size_t nVert )
{
	size_t nHorizLinks=(nHoriz+1)*nVert;
	size_t nVertLinks=nHoriz*(nVert+1);
	if(m_nCells==0)
		m_nLinks=0;
	else
		m_nLinks=nHorizLinks+nVertLinks; //for periodic x we have a link all the way round, for non periodic y we have a boundary link top/bottom

	try
	{
		m_links=new Link*[m_nLinks];

		//initiallize all link pointers to NULL
		for(size_t i=0; i< m_nLinks; ++i)
			m_links[i]=nullptr;
		
		//create the links
		for(size_t i=0; i< m_nCells; ++i)
		{
			//Vertical links
			if(i<nHoriz)
			{
				m_links[i]=new SolidBoundaryLink( m_cells[i], 1.0, m_cells[i]->getUdSpeedPointer());
				m_cells[i]->setBottomLink(m_links[i]);
			}
			else
			{
				m_links[i]=new InviscidLink(m_cells[i-nHoriz], m_cells[i], 1.0, m_cells[i-nHoriz]->getUdSpeedPointer(), 
					m_cells[i]->getUdSpeedPointer());
				m_cells[i-nHoriz]->setTopLink(m_links[i]);
				m_cells[i]->setBottomLink(m_links[i]);
			}
			if(i>=m_nCells-nHoriz)
			{
				m_links[i+nHoriz]=new SolidBoundaryLink( m_cells[i], 1.0, m_cells[i]->getUdSpeedPointer());
				m_cells[i]->setTopLink(m_links[i+nHoriz]);
			}

			//Horizontal Links
			if(i%nHoriz==0)
			{
				m_links[i+nVertLinks]=new SolidBoundaryLink(m_cells[i], 1.0, m_cells[i]->getLrSpeedPointer());
				m_cells[i]->setLeftLink(m_links[i+nVertLinks]);
			}
			else
			{
				m_links[i+nVertLinks]=new InviscidLink(m_cells[i-1],m_cells[i], 1.0, m_cells[i-1]->getLrSpeedPointer(), 
					m_cells[i]->getLrSpeedPointer());
				m_cells[i-1]->setRightLink(m_links[i+nVertLinks]);
				m_cells[i]->setLeftLink(m_links[i+nVertLinks]);
			}
			if((i+1)%nHoriz==0)
			{
				size_t row=i/nHoriz;
				m_links[m_nLinks-row-1]=new SolidBoundaryLink( m_cells[i], 1.0, m_cells[i]->getUdSpeedPointer());
				m_cells[i]->setRightLink(m_links[m_nLinks-row-1]);
			}
		}
	}
	catch(...)
	{
		if(m_links)
		{
			for(size_t i=0; i<m_nLinks; ++i)
				if(m_links[i])
					delete m_links[i];
			delete[] m_links;
		}
		throw;
	}
}
RectangularSimulationWrappedEdges::RectangularSimulationWrappedEdges( size_t nHoriz, size_t nVert, const UNG_FLT *densities, const UNG_FLT *potentialTemperatures,
		const UNG_FLT* horizontalSpeeds, const UNG_FLT* verticalSpeeds)
		:RectangularSimulation(nHoriz, nVert, densities, potentialTemperatures, horizontalSpeeds, verticalSpeeds)
{
	setupLinks(nHoriz, nVert);
}

void RectangularSimulationWrappedEdges::setupLinks( size_t nHoriz, size_t nVert )
{
	if(m_nCells==0)
		m_nLinks=0;
	else
		m_nLinks=nHoriz*nVert*2; //for periodic x we have a link all the way round, for non periodic y we have a boundary link top/bottom

	try
	{
		m_links=new Link*[m_nLinks];

		//initiallize all link pointers to NULL
		for(size_t i=0; i< m_nLinks; ++i)
			m_links[i]=nullptr;
		
		//create the links
		for(size_t i=0; i< m_nCells; ++i)
		{
			if(i<nHoriz)
			{
				//vertical links wrapped round bottom
				m_links[i]=new InviscidLink(m_cells[m_nCells-nHoriz+i], m_cells[i], 1.0, m_cells[m_nCells-nHoriz+i]->getUdSpeedPointer(), 
					m_cells[i]->getUdSpeedPointer());
				m_cells[m_nCells-nHoriz+i]->setTopLink(m_links[i]);
				m_cells[i]->setBottomLink(m_links[i]);
			}
			else
			{
				//vertical links
				m_links[i]=new InviscidLink(m_cells[i-nHoriz], m_cells[i], 1.0, m_cells[i-nHoriz]->getUdSpeedPointer(), 
					m_cells[i]->getUdSpeedPointer());
				m_cells[i-nHoriz]->setTopLink(m_links[i]);
				m_cells[i]->setBottomLink(m_links[i]);
			}
			if(i%nHoriz==0)
			{
				//horizontal links wrapped round end
				m_links[i+m_nCells]=new InviscidLink(m_cells[i+nHoriz-1],m_cells[i], 1.0, m_cells[i+nHoriz-1]->getLrSpeedPointer(), 
					m_cells[i]->getLrSpeedPointer());
				m_cells[i+nHoriz-1]->setRightLink(m_links[i+m_nCells]);
				m_cells[i]->setLeftLink(m_links[i+m_nCells]);
			}
			else
			{
				//horizontal links
				m_links[i+m_nCells]=new InviscidLink(m_cells[i-1],m_cells[i], 1.0, m_cells[i-1]->getLrSpeedPointer(), 
					m_cells[i]->getLrSpeedPointer());
				m_cells[i-1]->setRightLink(m_links[i+m_nCells]);
				m_cells[i]->setLeftLink(m_links[i+m_nCells]);
			}
		}
	}
	catch(...)
	{
		if(m_links)
		{
			for(size_t i=0; i<m_nLinks; ++i)
				if(m_links[i])
					delete m_links[i];
			delete[] m_links;
		}
		throw;
	}
}