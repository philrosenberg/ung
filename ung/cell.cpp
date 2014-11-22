#include"cell.h"
#include"link.h"
#include<math.h>

Cell::Cell(UNG_FLT volume, UNG_FLT density, UNG_FLT potentialTemperature, UNG_FLT lrSpeed, UNG_FLT udSpeed, UNG_FLT ioSpeed)
	: m_volume(volume), m_mass(volume*density), m_potentialEnergy(volume*density*potentialTemperature), m_lrSpeed(lrSpeed),
	m_udSpeed(udSpeed), m_ioSpeed(ioSpeed) 
{
	m_lrMom=m_lrSpeed*m_mass;
	m_udMom=m_udSpeed*m_mass;
	m_ioMom=m_ioSpeed*m_mass;
	m_lrSpeedStep=0;
	m_udSpeedStep=0;
	m_ioSpeedStep=0;
	m_densities.potentialEnergyDensity=m_potentialEnergy/m_volume;
	m_densities.density=density;
	m_densities.lrMomentumDensity=m_lrMom/m_volume;
	m_densities.udMomentumDensity=m_udMom/m_volume;
	m_densities.ioMomentumDensity=m_ioMom/m_volume;

	setPressure();
}

void Cell::applyAdvectionAndAcceleration()
{
	const CellTracerDensities *densities;
	UNG_FLT volume;
	UNG_FLT parallelImpulse;
	UNG_FLT perpendicularImpulse;
	const void *densitiesVoid;

	UNG_FLT originalMass=m_mass;

	m_leftLink->getLinkProperties(densitiesVoid, volume, parallelImpulse, perpendicularImpulse);
	densities=(const CellTracerDensities *)densitiesVoid;
	m_lrSpeedStep+=parallelImpulse/originalMass;
	m_udSpeedStep+=perpendicularImpulse/originalMass;
	applyAdvection(densities, volume);

	m_rightLink->getLinkProperties(densitiesVoid, volume, parallelImpulse, perpendicularImpulse);
	densities=(const CellTracerDensities *)densitiesVoid;
	m_lrSpeedStep+=parallelImpulse/originalMass;
	m_udSpeedStep+=perpendicularImpulse/originalMass;
	applyAdvection(densities, -volume);

	m_topLink->getLinkProperties(densitiesVoid, volume, parallelImpulse, perpendicularImpulse);
	densities=(const CellTracerDensities *)densitiesVoid;
	m_lrSpeedStep+=perpendicularImpulse/originalMass;
	m_udSpeedStep+=parallelImpulse/originalMass;
	applyAdvection(densities, -volume);

	m_bottomLink->getLinkProperties(densitiesVoid, volume, parallelImpulse, perpendicularImpulse);
	densities=(const CellTracerDensities *)densitiesVoid;
	m_lrSpeedStep+=perpendicularImpulse/originalMass;
	m_udSpeedStep+=parallelImpulse/originalMass;
	applyAdvection(densities, volume);
}

void Cell::applyAdvection(const CellTracerDensities *densities, UNG_FLT volume)
{
	m_mass+=densities->density*volume;
	m_potentialEnergy+=densities->potentialEnergyDensity*volume;
	m_lrMom+=densities->lrMomentumDensity*volume;
	m_udMom+=densities->udMomentumDensity*volume;
	m_ioMom+=densities->ioMomentumDensity*volume;
}

void Cell::completeStep()
{
	m_lrSpeed=m_lrMom/m_mass+m_lrSpeedStep;
	m_udSpeed=m_udMom/m_mass+m_udSpeedStep;
	m_ioSpeed=m_ioMom/m_mass+m_ioSpeedStep;

	m_lrMom=m_lrSpeed*m_mass;
	m_udMom=m_udSpeed*m_mass;
	m_ioMom=m_ioSpeed*m_mass;

	m_lrSpeedStep=0;
	m_udSpeedStep=0;
	m_ioSpeedStep=0;

	m_densities.density=m_mass/m_volume;
	m_densities.potentialEnergyDensity=m_potentialEnergy/m_volume;
	m_densities.lrMomentumDensity=m_lrMom/m_volume;
	m_densities.udMomentumDensity=m_udMom/m_volume;
	m_densities.ioMomentumDensity=m_ioMom/m_volume;

	setPressure();
}


void Cell::setPressure()
{
	//to do When using data from Wikipedia I get pressure of 1018, not 1013.5 hPa
	//solve PV=nRT and theta=T(P0/P)^(R/cp)
	//gives P=((theta*n*R*P0^(-R/Cp))/V)^(1/(1-R/cp))
	const UNG_FLT power=UNG_FLT(1.40056);
	const UNG_FLT P0Term=UNG_FLT(0.0371535);
	UNG_FLT nMolesPerUnitVol=m_densities.density/UNG_FLT(0.02897);
	UNG_FLT potentialTemperature=m_potentialEnergy/m_mass;
	m_pressure=pow(potentialTemperature*nMolesPerUnitVol*UNG_FLT(8.3144621)*P0Term,power);
}

void Cell::setLinks(Link *leftLink, Link *rightLink, Link *topLink, Link *bottomLink)
{
	m_leftLink=leftLink;
	m_rightLink=rightLink;
	m_topLink=topLink;
	m_bottomLink=bottomLink;
}

void Cell::averageTracerDensities( void *dest, void *source1, void *source2 ) const
{
	CellTracerDensities *destCast=(CellTracerDensities*)dest;
	CellTracerDensities *source1Cast=(CellTracerDensities*)source1;
	CellTracerDensities *source2Cast=(CellTracerDensities*)source2;
	destCast->density=(source1Cast->density+source2Cast->density)/UNG_FLT(2.0);
	destCast->ioMomentumDensity=(source1Cast->ioMomentumDensity+source2Cast->ioMomentumDensity)/UNG_FLT(2.0);
	destCast->lrMomentumDensity=(source1Cast->lrMomentumDensity+source2Cast->lrMomentumDensity)/UNG_FLT(2.0);
	destCast->udMomentumDensity=(source1Cast->udMomentumDensity+source2Cast->udMomentumDensity)/UNG_FLT(2.0);
	destCast->potentialEnergyDensity=(source1Cast->potentialEnergyDensity+source2Cast->potentialEnergyDensity)/UNG_FLT(2.0);
}