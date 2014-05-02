#ifndef _PALLA_
#define _PALLA_

#include <assert.h>
#include <math.h>
#include <string>
#include "vector.h"
#include "texture.h"

#define SEMI_LUNGHEZZA 4.8
#define SEMI_LARGHEZZA 3.3
#define RAGGIO 0.12
using namespace std;

class Palla{

private:
	float raggio, spinning_angle_x, spinning_angle_z;
	float x, z, v0, vel, velx, velz, x0, z0, tempo, angle;
	int a, b; //variabili per mantenere in quale verso (lungo x e z) avviene il moto
	int fine;
	bool buca, bianca;

	texture _map;
	GLUquadricObj *quadric;

public:
	Palla() {
		x = z = 0;	
		Init();
	}
	
	Palla(float xx, float yy, float zz) {
		x = xx;
		z = zz;
		Init();
	}

	Palla(Vector v) {
		x = v.getX();
		z = v.getZ();	
		Init();
	}

	Vector getPosizione() {
		return Vector(x, 0, z);
	}

	float getVel() {
		return vel;
	}

	float getV0() {
		return v0;
	}

	float getAngle() {
		return angle;
	}

	int getA() {
		return a;
	}

	int getB() {
		return b;
	}

	int getFine() {
		return fine;
	}
	
	void setBianca() {
		bianca = true;
	}

	void setInBuca() {
		buca = true;
	}

	void setPosizione(float xx, float y, float zz) {
		x = x0 = xx;
		z = z0 = zz;
	}

	void setPosIniziale(float xx, float y, float zz) {
		x0 = xx;
		z0 = zz;
	}

	void setVel(float v) {
		vel = v;
		velx = vel*cos(angle);
		velz = vel*sin(angle);
	}

	void setV0(float vzero) {
		v0 = vzero;
	}

	void setAngle(float a) {
		angle = a;
	}

	void setA(float aa) {
		a = aa;
	}
	
	void setB(float bb) {
		b = bb;
	}

	void setTempo(float t) {
		tempo = t;
	}

	void setFine(float f) {
		fine  = f;
	}

	int inBuca() {
		if(buca)
			return 1;
		else return 0;
	}

	void Init() {
		buca = false;
		bianca = false;

		raggio = RAGGIO;
		spinning_angle_x = 0; spinning_angle_z = 0;

		quadric = gluNewQuadric();
		gluQuadricNormals(quadric, GLU_SMOOTH);
		gluQuadricTexture(quadric, GL_TRUE);
		
	}

	void Draw() {
		glBindTexture(GL_TEXTURE_2D, _map.ID);

		glPushMatrix();
			glTranslatef(x, 0, z);
			glRotatef(spinning_angle_x, 0, 0, 1);
			glRotatef(spinning_angle_z, 1, 0, 0);
			gluSphere(quadric, raggio, 32, 32 );
		glPopMatrix();

	}

	int LoadTextureTGA(string texture_filename) {
		return LoadTGA((char*)texture_filename.c_str(), GL_LINEAR, GL_LINEAR, &_map);
	}

	int in_buca() {
		if ((x <= -(SEMI_LUNGHEZZA - 0.1) || x >= (SEMI_LUNGHEZZA - 0.1)) && (z >= (SEMI_LARGHEZZA - 0.05) || z <= -(SEMI_LARGHEZZA - 0.05)))
		return 1;
	
		if (x <= 0.2 && x >= -0.2 && (z >= 3.25 || z <= -3.25))
			return 1;
		return 0;
	}

	void calcola() {
		float attrito = -1.5;
		
		//in base alla direzione la sfera ruota in un certo modo
		if (a) {
			if (b) {
				spinning_angle_x += velx*1.5;
				spinning_angle_z -= velz*1.5;
			}
			else {
				spinning_angle_x += velx*1.5;
				spinning_angle_z += velz*1.5;
			}
		}
		else if (b) {
			spinning_angle_x -= velx*1.5;
			spinning_angle_z -= velz*1.5;
		}
		else {
			spinning_angle_x -= velx*1.5;
			spinning_angle_z += velz*1.5;
		}
		

		if (vel > 0) {
			tempo += 0.033;
			vel = v0 + attrito*tempo;
			velx = vel*cos(angle);
			velz = vel*sin(angle);
			
			//direzione lungo x crescenti
			  if (!a) {
				x = x0 + (v0*cos(angle)*tempo + ((tempo*tempo*attrito*cos(angle))/2));
				
				if (x > SEMI_LUNGHEZZA) {//controlli che non si giunga alla fine del tavolo
				  a = 1;
				 x = (SEMI_LUNGHEZZA - 0.01);
				 x0 = (SEMI_LUNGHEZZA - 0.01) + (v0*cos(angle)*tempo + ((tempo*tempo*attrito*cos(angle))/2));
				}
				else if (x < -SEMI_LUNGHEZZA) {
				  a = 1;
				 x = -(SEMI_LUNGHEZZA - 0.01);
				 x0 = -(SEMI_LUNGHEZZA - 0.01) + (v0*cos(angle)*tempo + ((tempo*tempo*attrito*cos(angle))/2));
				}
			  }
			  else {
				x = x0 - (v0*cos(angle)*tempo + ((tempo*tempo*attrito*cos(angle))/2));
				
				if (x > SEMI_LUNGHEZZA) {
				  a = 0;
				 x = (SEMI_LUNGHEZZA - 0.01);
				 x0 = (SEMI_LUNGHEZZA - 0.01) - (v0*cos(angle)*tempo + ((tempo*tempo*attrito*cos(angle))/2));
				}
				else if (x < -SEMI_LUNGHEZZA) {
				  a = 0;
				 x = -(SEMI_LUNGHEZZA - 0.01);
				 x0 = -(SEMI_LUNGHEZZA - 0.01) - (v0*cos(angle)*tempo + ((tempo*tempo*attrito*cos(angle))/2));
				}
			  }
			  
			  //direzione lungo z crescenti
			  if (!b) {
				  z = z0 + (v0*sin(angle)*tempo + ((tempo*tempo*attrito*sin(angle))/2));
			  if (z > SEMI_LARGHEZZA) {
				  b = 1;
				  z = (SEMI_LARGHEZZA - 0.01);
				  z0 = (SEMI_LARGHEZZA - 0.01) + (v0*sin(angle)*tempo + ((tempo*tempo*attrito*sin(angle))/2));
			  }
			  else if (z < -SEMI_LARGHEZZA) {
				  b = 1;
				  z = -(SEMI_LARGHEZZA - 0.01);
				  z0 = -(SEMI_LARGHEZZA - 0.01) + (v0*sin(angle)*tempo + ((tempo*tempo*attrito*sin(angle))/2));
			  }
			}
			else {
				z = z0 - (v0*sin(angle)*tempo + ((tempo*tempo*attrito*sin(angle))/2));
				
				if (z > SEMI_LARGHEZZA) {
				  b = 0;
				  z = (SEMI_LARGHEZZA - 0.01);
				  z0 = (SEMI_LARGHEZZA - 0.01) - (v0*sin(angle)*tempo + ((tempo*tempo*attrito*sin(angle))/2));
				}
				else if (z < -SEMI_LARGHEZZA) {
				  b = 0;
				  z = -(SEMI_LARGHEZZA - 0.01);
				  z0 = -(SEMI_LARGHEZZA - 0.01) - (v0*sin(angle)*tempo + ((tempo*tempo*attrito*sin(angle))/2));
				}
			}
			//controllo che la palla non sia in una posizione considerata 'in buca'
			if (in_buca()) {
				x = 9;
				z = 9;
				  
				if (bianca) {
					x = x0 = (SEMI_LARGHEZZA + 0.2);
					z = z0 = 0;
				}
				else {
					setPosizione(x, 0, z);
					buca = true;
				}
				vel = 0;
				fine = 1;
				angle = 0;
			}
		}
		else {//se si arriva qui la velocità è <=0, la palla ha concluso il suo moto e settiamo il campo fine

			x0 = x;
			z0 = z;
		
			fine = 1;
			angle = 0;

		}
	}

};

# endif
