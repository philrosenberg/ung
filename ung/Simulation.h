#ifndef UNG_SIMULATION_H
#define UNG_SIMULATION_H

#include"ung.h"
class Cell;
class Link;

class Simulation
{
public:
	Simulation( );
	void Step(UNG_FLT timeStep);
private:
protected:
	Link * linkHorizontal( Cell *leftCell, Cell * rightCell);
	Link * linkVertical( Cell *topCell, Cell * bottomCell);
	Link * linkTopBoundary( Cell * cell); //for BCs
	Link * linkBottomBoundary( Cell * cell); //for BCs
	Link * linkLeftBoundary( Cell * cell); //for BCs
	Link * linkRightBoundary( Cell * cell); //for BCs
	size_t m_nCells;
	size_t m_nLinks;
	Cell** m_cells;
	Link** m_links;
};

class RectangularSimulation : public Simulation
{
public:
	RectangularSimulation( size_t nHoriz, size_t nVert, const UNG_FLT *densities, const UNG_FLT *potentialTemperatures,
		const UNG_FLT* horizontalSpeeds, const UNG_FLT* verticalSpeeds);
	void getHorizontalSpeeds( UNG_FLT* horizontalSpeeds ) const;
	void getVerticalSpeeds( UNG_FLT* verticalSpeeds ) const;
	void getDensities( UNG_FLT* densities ) const;
	void getPotentialTemperatures( UNG_FLT* potentialTemperatures ) const;
	void getTemperatures( UNG_FLT* temperatures ) const;
private:
	virtual void setupLinks( size_t nHoriz, size_t nVert )=0;
protected:
};

class RectangularSimulationSolidEdges : public RectangularSimulation
{
public:
	RectangularSimulationSolidEdges( size_t nHoriz, size_t nVert, const UNG_FLT *densities, const UNG_FLT *potentialTemperatures,
		const UNG_FLT* horizontalSpeeds, const UNG_FLT* verticalSpeeds);
	virtual void setupLinks( size_t nHoriz, size_t nVert );
};

class RectangularSimulationWrappedEdges : public RectangularSimulation
{
public:
	RectangularSimulationWrappedEdges( size_t nHoriz, size_t nVert, const UNG_FLT *densities, const UNG_FLT *potentialTemperatures,
		const UNG_FLT* horizontalSpeeds, const UNG_FLT* verticalSpeeds);
	virtual void setupLinks( size_t nHoriz, size_t nVert );
};

#endif