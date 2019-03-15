#pragma once
#include <math.h>

enum SolarSystem {
	SUN, MERCURY, VENUS, EARTH, MARS, JUPITER, SATURN, URANUS, NEPTUNE,
	MOON, 
	NUM_ELEMENTS
};
class AstronomicalObject
{
public:
	AstronomicalObject(SolarSystem elementIndex, AstronomicalObject *revoluteObject);
	~AstronomicalObject();
	void setRotation(double angleRotation) { this->angleRotation = angleRotation; }
	void setRevolution(double angleRevolution) { this->angleRevolution = angleRevolution; }
	void setRealDistanceMode(bool realDistanceMode) { this->realDistanceMode = realDistanceMode; }
	double getRadius() { return rescaleKm(radius); }
	double getDistanceRevolution(); 
	AstronomicalObject& getRevoluteObject() { return *pRevoluteObject; }
	double getAngleRotation() { return angleRotation; }
	double getRadianRotation() { return degree2radian(angleRotation); }
	double getAngleRevolution() { return angleRevolution; }
	double getRadianRevolution() { return degree2radian(angleRevolution); }
	double getAngleAxialTilt() { return angleAxialTilt; }
	double getHoursOfRotation() { return hoursOfRotation; }
	double gethoursOfRevolution() { return hoursOfRevolution; }
	double getDeltaAngleRotation() { if (hoursOfRotation != 0) return (timeScale / hoursOfRotation); else return 0; }
	double getDeltaAngleRevolution() { if (hoursOfRevolution) return (timeScale / hoursOfRevolution); else return 0; }
	double getX();
	double getY();
	double getZ();
	void increaseRotation();
	void increaseRevolution();
private:
	double radius; // km
	double distanceRevolution; // km
	double distanceRevolutionClose;
	AstronomicalObject *pRevoluteObject;
	double hoursOfRotation; // hours
	double hoursOfRevolution; // hours
	double angleAxialTilt; // degree
	double angleRotation = 0; // degree
	double angleRevolution = 0; // degree
	double deltaRotation;
	double deltaRevolution;
	const double timeScale = 100.0; // big: fast, small: slow.
	bool realDistanceMode = false;
	// converting functions
	double day2hour(double day) { return day * 24; }
	double year2hour(double year) { return year * 356 * 24; }
	double minute2hour(double minute) { return minute / 60.0; }
	double rescaleKm(double kilometer) { return kilometer / 6378;  } // radius of the earth
	const double PI = 3.141593;
	double degree2radian(double degree) { return PI / 180.0 * degree; }
};