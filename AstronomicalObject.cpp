#include "AstronomicalObject.h"

AstronomicalObject::AstronomicalObject(SolarSystem elementIndex, AstronomicalObject *pRevoluteObject)
{
	// Source
	// radius(km): http://nineplanets.org/data1.htlm
	// distance from the sun(km): http://idahoptv.org/ntti/nttilessons/lessons2000/lau1.html
	// hoursOfDay(hours): http://www.universetoday.com/72305/order-of-the-planets-from-the-sun/

	switch (elementIndex)
	{
	case SolarSystem::SUN:
		radius = 695000; // km
		distanceRevolution = 0;
		distanceRevolutionClose = 0;
		hoursOfRotation = day2hour(24.47); // 24.47 earth days/rot //https://en.wikipedia.org/wiki/Solar_rotation 
		hoursOfRevolution = 0; // not revolute
		angleAxialTilt = 7.25;
		break;
	case SolarSystem::MERCURY:
		radius = 2440;
		distanceRevolution = 57.91E+6;
		distanceRevolutionClose = 695000 + 2440;
		hoursOfRotation = day2hour(59); // 59 earth days
		hoursOfRevolution = day2hour(87.97); // 87.97 Earth days
		angleAxialTilt = 0.03;
		break;
	case SolarSystem::VENUS:
		radius = 6052;
		distanceRevolution = 108.2E+6;
		distanceRevolutionClose = 695000 + 2*2440 + 6052;
		hoursOfRotation = day2hour(243); // 243 Earth days
		hoursOfRevolution = day2hour(225); // 225 days
		angleAxialTilt = 177.36;
		break;
	case SolarSystem::EARTH:
		radius = 6378;
		distanceRevolution = 149.6E+6;
		distanceRevolutionClose = 695000 + 2*(2440 + 6052) + 6378;
		hoursOfRotation = 24; // 24 hours
		hoursOfRevolution = day2hour(365); // 365 days
		angleAxialTilt = 23.44;
		break;
	case SolarSystem::MARS:
		radius = 3397;
		distanceRevolution = 227.94E+6;
		distanceRevolutionClose = 695000 + 2*(2440 + 6052 + 6378) + 3397;
		hoursOfRotation = 24 + day2hour(37.0); // 24 hours 37 minutes.
		hoursOfRevolution = day2hour(687); // 687 Earth days.
		angleAxialTilt = 25.19;
		break;
	case SolarSystem::JUPITER:
		radius = 71492;
		distanceRevolution = 778.33E+6;
		distanceRevolutionClose = 695000 + 2*(2440 + 6052 + 6378 + 3397) + 71492;
		hoursOfRotation = 9.8; // 9.8 Earth hours
		hoursOfRevolution = year2hour(11.9); // 11.9 Earth years
		angleAxialTilt = 3.13;
		break;
	case SolarSystem::SATURN:
		radius = 60268;
		distanceRevolution = 1424.6E+6;
		distanceRevolutionClose = 695000 + 2*(2440 + 6052 + 6378 + 3397 + 71492) + 60268;
		hoursOfRotation = 10.7; // 10.7 Earth hours
		hoursOfRevolution = year2hour(29.5); // 29.5 Earth years
		angleAxialTilt = 26.73;
		break;
	case SolarSystem::URANUS:
		radius = 25559;
		distanceRevolution = 2873.55E+6;
		distanceRevolutionClose = 695000 + 2*(2440 + 6052 + 6378 + 3397 + 71492 + 60268) + 25559;
		hoursOfRotation = 18; // 18 Earth hours
		hoursOfRevolution = year2hour(84); // 84 Earth years
		angleAxialTilt = 97.77;
		break;
	case SolarSystem::NEPTUNE:
		radius = 24766;
		distanceRevolution = 4501.0E+6;
		distanceRevolutionClose = 695000 + 2*(2440 + 6052 + 6378 + 3397 + 71492 + 60268 + 25559) + 24766;
		hoursOfRotation = 16; // 16 Earth hours
		hoursOfRevolution = year2hour(165); // 165 Earth years
		angleAxialTilt = 28.32;
		break;
	case SolarSystem::MOON:
		radius = 1738;
		distanceRevolution = 384.4E+3; 
		distanceRevolutionClose = 6378 + 1738 + 100;
		hoursOfRotation = day2hour(27.322); //  27.322 days
		hoursOfRevolution = day2hour(27.322); //  27.322 days
		angleAxialTilt = 6.68;
		break;
	}
	
	this->pRevoluteObject = pRevoluteObject;
	deltaRotation = hoursOfRotation / timeScale;
	deltaRevolution = hoursOfRevolution / timeScale;
}


AstronomicalObject::~AstronomicalObject()
{
}

double AstronomicalObject::getDistanceRevolution()
{ 
	if (realDistanceMode)
		return rescaleKm(distanceRevolution);
	else
		return rescaleKm(distanceRevolutionClose);
}

double AstronomicalObject::getX()
{
	if( pRevoluteObject == NULL)
		return  getDistanceRevolution() * sin(getRadianRevolution());
	else
		return  getDistanceRevolution() * sin(getRadianRevolution()) + pRevoluteObject->getX();
}

double AstronomicalObject::getY() 
{
	if (pRevoluteObject == NULL)
		return 0;
	else
		return pRevoluteObject->getY(); 
}

double AstronomicalObject::getZ() 
{ 
	if (pRevoluteObject == NULL)
		return getDistanceRevolution() * cos(getRadianRevolution());
	else
		return getDistanceRevolution() * cos(getRadianRevolution()) + pRevoluteObject->getZ(); 
}

void AstronomicalObject::increaseRotation()
{
	if (angleRotation > 360.0)
		angleRotation -= 360.0;
	angleRotation += getDeltaAngleRotation();
}

void AstronomicalObject::increaseRevolution()
{
	if (angleRevolution > 360.0)
		angleRevolution -= 360.0;
	angleRevolution += getDeltaAngleRevolution();
}

