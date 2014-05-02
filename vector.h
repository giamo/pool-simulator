#ifndef _VECTOR_
#define _VECTOR_

#include <math.h>

class Vector {
public:
	float x, y, z;

	Vector(){
		x = 0; 
		y = 0; 
		z = 0;
	}

	Vector(float _x, float _y, float _z)	{ 
		x = _x; 
		y = _y; 
		z = _z;
	}

	float getX() const {
		return x;
	}

	float getY() const {
		return y;
	}

	float getZ() const {
		return z;
	}

};

#endif
