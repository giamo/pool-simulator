#include <stdlib.h>
#include <iostream>
#include <GL/freeglut.h>
#include <math.h>
#include <string>
#include "vector.h"
#include "ball.h"
#include "texture.h"

#define rad 0.017453292519943;
#define numpalle 10

int width = 800;
int height = 600;

GLint FPS = 30;		// Frame per Secondo. Vengono usati nella funzione di animazione
					// per controllare il massimo numero di frame al secondo dell'animazione

int spin_x = 0, old_x = 0;		// spin_ indica di quanto si deve ruotare la scena
int spin_y = 0, old_y = 0;		// old_  indica la vecchia posizione del cursore

float px, py , pz;
float sux, suy;
float strx, stry, strz;

int fase = 0;		// specifica lo stato dell' animazione

Palla p[numpalle];

float pot = 70, attrito = -1.5;
float line_angle, stecca_angle, tiro_x = 0, tiro_z = 0, angle;

int tiro = 0, laterale = 0, istruzioni = 0;

char str[20] = "POWER = ";

GLUquadricObj *quadratic;

/*
* processa l'azione selezionata dal menu
*
* op - intero che rappresenta l'azione selezionata nel menu
*/
void menu(int op) {
	
	switch(op) {
	case 'Q':
	case 'q':
		exit(0);
	}
}

/*
* invocata da GLUT quando un tasto viene premuto
*
* key - tasto premuto
* x, y - coordinate del cursore del mouse
*/
void keyboardDown(unsigned char key, int x, int y) {
	
	switch(key) {
	case ' ': 
	  //impostiamo i valori per mettere in moto la bianca
	  if (!tiro) {
		  int i;
		  angle = p[0].getAngle();
		  angle*=rad;
		  p[0].setV0(pot/10);
		  p[0].setTempo(0);
		  p[0].setVel(pot/10);
		  p[0].setFine(0);

		  for (i = 1; i < numpalle; i++) {
			p[i].setTempo(0);
			p[i].setVel(0);
			p[i].setV0(0);
			p[i].setFine(0);
		  }
		  //stabiliamo le direzioni del moto	
		  if (p[0].getPosizione().getX() < 0)
			p[0].setA(0);
		  else p[0].setA(1);

		  if (p[0].getPosizione().getX() >= 0)
			p[0].setB(0);
		  else p[0].setB(1);

		  fase = 1;
	  }
	break;
  case 'v': if (!tiro) fase = 4; break;
  case 'x': if (!tiro) p[0].setAngle(p[0].getAngle() - 1); glutPostRedisplay(); break;
  case 'z': if (!tiro) p[0].setAngle(p[0].getAngle() + 1); glutPostRedisplay(); break;
  case 's': if (!tiro) p[0].setAngle(p[0].getAngle() - 90); glutPostRedisplay(); break;
  case 'a': if (!tiro) p[0].setAngle(p[0].getAngle() + 90); glutPostRedisplay(); break;
  case '+': if (!tiro && pot < 100) pot++; glutPostRedisplay(); break;
  case '-': if (!tiro && pot > 0) pot--; glutPostRedisplay(); break;
  case 'i': if (!tiro) istruzioni = (istruzioni) ? 0 : 1; glutPostRedisplay(); break;
  case 'Q':
  case 'q':
  case  27:   // ESC
    exit(0);
  }
}

/*
* invocata da GLUT quando un tasto viene rilasciato
*
* key - tasto premuto
* x, y - coordinate del cursore del mouse
*/
void keyboardUp(unsigned char key, int x, int y) {}

/*
* invocata quando la finestra cambia dimensione, oppure è occlusa da un'altra finestra
* 
* width. height - dimensioni attuali della finestra
*/
void reshape(int width, int height) 
{
	GLfloat fieldOfView = 45.0f;
	glViewport (0, 0, (GLsizei) width, (GLsizei) height);
	
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fieldOfView, (GLfloat) width/(GLfloat) height, 0.1, 500.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/*
* invocata quando uno dei bottoni del mouse cambia stato (viene premuto o rilasciato)
*
* button - bottone che ha cambiato stato
* state - stato attuale del bottone
* x, y - coordinate del cursore del mouse
*/
void mouseClick(int button, int state, int x, int y) 
{
	old_x = x;
	old_y = y;
	
	glutPostRedisplay();
}

/*
* invocata ogni volta che il cursore del mouse viene mosso
*
* x, y - coordinate del cursore del mouse
*
* nota: sulla maggior parte dei sistemi, la posizione del cursore del mouse viene rilevata (campionata) 
*       con una frequenza di almeno 1/30 secondi
*/
void mouseMotion(int x, int y) 
{
	spin_x = x - old_x;
	spin_y = y - old_y;
	
	glutPostRedisplay();
}

void DrawTable() {
	float x1, z1, ang, raggio = 0.35;
	
	//tavolo
	glPushMatrix();
	glBegin(GL_POLYGON);
		glColor3f(0, 1, 0);
		glVertex3f(5.7, -0.15, 4.2);
		glVertex3f(5.7, -0.15, -4.2);
		glVertex3f(-5.7, -0.15, -4.2);
		glVertex3f(-5.7, -0.15, 4.2);
	glEnd();
	
	//6 buche
	glColor3f(0, 0, 0);

	glBegin(GL_TRIANGLE_FAN);
		x1 = 0; z1 = 3.75;
		glVertex3f(x1, -0.14, z1);
		for (ang = 0; ang <= 360; ang++)
			glVertex3f(x1 + sin(ang) * raggio, 0, z1 + cos(ang) * raggio);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
		x1 = 0; z1 = -3.75;
		glVertex3f(x1, -0.14, z1);
		for (ang = 0; ang <= 360; ang++)
			glVertex3f(x1 + sin(ang) * raggio, 0, z1 + cos(ang) * raggio);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
		x1 = 4.95; z1 = 3.45;
		glVertex3f(x1, -0.1, z1);
		for (ang = 0; ang <= 360; ang++)
			glVertex3f(x1 + sin(ang) * raggio, 0, z1 + cos(ang) * raggio);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
		x1 = -4.95; z1 = 3.45;
		glVertex3f(x1, -0.1, z1);
		for (ang = 0; ang <= 360; ang++)
			glVertex3f(x1 + sin(ang) * raggio, 0, z1 + cos(ang) * raggio);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
		x1 = 4.95; z1 = -3.45;
		glVertex3f(x1, -0.1, z1);
		for (ang = 0; ang <= 360; ang++)
			glVertex3f(x1 + sin(ang) * raggio, 0, z1 + cos(ang) * raggio);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
		x1 = -4.95; z1 = -3.45;
		glVertex3f(x1, -0.1, z1);
		for (ang = 0; ang <= 360; ang++)
			glVertex3f(x1 + sin(ang) * raggio, 0, z1 + cos(ang) * raggio);
	glEnd();

	// bordi interni
	glColor3f(0.06, 0.6, 0.05);
	glPushMatrix();
		glTranslatef(2.45, -0.05, 3.5);
		glScalef(1, 0.024, 0.024);
		glutSolidCube(4.3);
	glPopMatrix();
	
	glPushMatrix();
		glTranslatef(-2.45, -0.05, 3.5);
		glScalef(1, 0.024, 0.024);
		glutSolidCube(4.3);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(2.45, -0.05, -3.5);
		glScalef(1, 0.024, 0.024);
		glutSolidCube(4.3);
	glPopMatrix();
	
	glPushMatrix();
		glTranslatef(-2.45, -0.05, -3.5);
		glScalef(1, 0.024, 0.024);
		glutSolidCube(4.3);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(5, -0.05, 0);
		glScalef(0.016, 0.016, 1);
		glutSolidCube(6.2);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-5, -0.05, 0);
		glScalef(0.016, 0.016, 1);
		glutSolidCube(6.2);
	glPopMatrix();

	//bordi esterni
	glColor3f(0.45, 0.23, 0.07);
	glPushMatrix();
		glTranslatef(3, -0.05, 3.9);
		glScalef(1, 0.039, 0.117);
		glutSolidCube(5.4);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-3, -0.05, 3.9);
		glScalef(1, 0.039, 0.117);
		glutSolidCube(5.4);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(3, -0.05, -3.9);
		glScalef(1, 0.039, 0.117);
		glutSolidCube(5.4);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-3, -0.05, -3.9);
		glScalef(1, 0.039, 0.117);
		glutSolidCube(5.4);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(5.38, -0.05, 0);
		glScalef(0.102, 0.035, 1);
		glutSolidCube(6.1);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-5.38, -0.05, 0);
		glScalef(0.102, 0.035, 1);
		glutSolidCube(6.1);
	glPopMatrix();
	
	//"zeppetti" per completare i bordo esterno
	glPushMatrix();
		glTranslatef(0, -0.05, 4);
		glScalef(1, 0.3, 0.6);
		glutSolidCube(0.7);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(0, -0.05, -4);
		glScalef(1, 0.3, 0.6);
		glutSolidCube(0.7);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-5.37, -0.05, 3.32);
		glScalef(1, 0.324, 0.81);
		glutSolidCube(0.65);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(5.37, -0.05, 3.32);
		glScalef(1, 0.324, 0.81);
		glutSolidCube(0.65);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(5.37, -0.05, -3.32);
		glScalef(1, 0.324, 0.81);
		glutSolidCube(0.65);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-5.37, -0.05, -3.32);
		glScalef(1, 0.324, 0.81);
		glutSolidCube(0.65);
	glPopMatrix();

	glPopMatrix();
}

/* Funzione che stampa la stringa string a partire dalle coordinate x, y, z */
void stampa(float x, float y, float z, string string) {
	glDisable(GL_LIGHTING);
	glPushMatrix();
		int i;
		glColor3f(1,0, 0);

		glRotatef(-spin_x, 0.0, 1.0, 0.0);

		glRasterPos3f(x, y, z);
		int len = (int) string.length();

		for (i = 0; i < len; i++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
		}
	glPopMatrix();
	
	glEnable(GL_LIGHTING);
}

/* Funzione che stampa la barra di potenza (con colore variabile in base al valore di potenza) */
void stampabarra (float x, float y, float z) {
	glDisable(GL_LIGHTING);
	glPushMatrix();
		int i;
		glRotatef(-spin_x, 0.0, 1.0, 0.0);

		glColor3f(1, 1 - pot/100, 0);
		
		if (laterale) 
			glRasterPos3f(x , y - 0.5, z);
		else glRasterPos3f(x + 0.5, y, z - 0.1);
		
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, '|');
		
		for (i = 0; i < 100; i+=10) {
			if (i < pot)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, '=');
			else 	
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ' ');
		}
		
		//coordinate diverse a seconda della visuale ce si sta usando
		if (laterale)
			glRasterPos3f(x + 6.25 , y + 0.78, z);
		else glRasterPos3f(x + 0.5 , y, z - 2.5);
		
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, '|');

	glPopMatrix();
	glEnable(GL_LIGHTING);
}

/*
* disegna la scena
*/
void draw() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	//variabili che vengono modificate a seconda di quale sia la visuale attuale
	gluLookAt(  px, py, pz,		// da dove si guarda
				0, 0, 0,		// dove si guarda
				sux, suy, 0);	// su

	glRotatef(spin_x, 0.0, 1.0, 0.0);

	int i, j = 0;
	
	if (!tiro) {
		//linea della traiettoria
		glBegin(GL_LINES);
			glColor3f(1, 0, 0);
			line_angle = p[0].getAngle()*rad;
			glVertex3f(p[0].getPosizione().getX(), 0, p[0].getPosizione().getZ());
			if (p[0].getPosizione().getX() >= 0)
				glVertex3f(p[0].getPosizione().getX()-5*cos(line_angle), 0, p[0].getPosizione().getZ()+5*sin(line_angle));
			else glVertex3f(p[0].getPosizione().getX()+5*cos(line_angle), 0, p[0].getPosizione().getZ()-5*sin(line_angle));
		
		glEnd();
		
		//stecca
		glPushMatrix();
			glColor3f(0.73, 0.3, 0);
			float r = (p[0].getPosizione().getX() >= 0) ? 90 : -90;
		
			stecca_angle = p[0].getAngle()*rad;

			if (p[0].getPosizione().getX() >= 0) {
				glTranslatef(tiro_x*cos(stecca_angle), 0.03, -tiro_z*sin(stecca_angle)); //per effettuare l'animazione del movimento della stecca
				glTranslatef(p[0].getPosizione().getX()+0.3*cos(stecca_angle), 0, p[0].getPosizione().getZ()-0.3*sin(stecca_angle));
			}
			else {
				glTranslatef(-tiro_x*cos(stecca_angle), 0.03, tiro_z*sin(stecca_angle));
				glTranslatef(p[0].getPosizione().getX()-0.3*cos(stecca_angle), 0, p[0].getPosizione().getZ()+0.3*sin(stecca_angle));
			}
			stecca_angle/=rad;
		
			glRotatef(r + stecca_angle, 0, 1, 0);
			glRotatef(-3, 1, 0, 0);//incliniamo leggermente la stecca verso l'alto
			gluCylinder(quadratic, 0.03, 0.06, 5, 30, 30);
		
		glPopMatrix();
	}

	sprintf(&str[8], "%0.f", pot);
	stampa(strx, stry, strz, str);
	stampabarra(strx, stry, strz);

	//tavolo
	DrawTable();
	
	//disegnamo la bianca...
	glPushMatrix();
		glColor3f (1.0, 1.0, 1.0);
		p[0].Draw();
	glPopMatrix();
	
	//... e tutte le altre
	for (i = 1; i < numpalle; i++) {
		glPushMatrix();
		if (!p[i].inBuca()) {
			p[i].Draw();
			j++;
		}
		glPopMatrix();
	}

	if (j <= 1) {
		tiro = 1;
		if (!laterale) {
			stampa(0, 1, 3, "THE GAME IS OVER! PRESS Q OR ESC TO QUIT");
		}
	}
	//stampa istruzioni
	if(istruzioni) {
		glColor3f(1, 0, 0);
		if (!laterale) {
			stampa(-4, 1, 1, "INSTRUCTIONS");
			stampa(-3.95, 1, 1, "___________");
			stampa(-3, 1, 3, "+/-: increase/decrease shot's power");
			stampa(-2.2, 1, 3, "z/x: increase/decrease shot's angle (a/s: angle +/- 90°)");
			stampa(-1.4, 1, 3, "SPACE: shoot!");
			stampa(-0.6, 1, 3, "v: switch from high and lateral camera");
			stampa(0.2, 1, 3, "i: show/hide instructions");
			stampa(1, 1, 3, "q, ESC: quit");
		}
	}
	
	glFlush();
	glutSwapBuffers();
}

/*
* invocata ogni volta che non ci sono eventi pendenti da processare
*/
void idle() { }

/*
* inizializza lo stato di OpenGL
*
* width, height - larghezza e altezza della finestra OpenGL
*/
void initGL(int width, int height) 
{
	// colore e tipo delle luci	
	GLfloat light_ambient_0[] =	{0.65, 0.65, 0.65, 1.0};// colore ambiente della luce 0
	GLfloat light_diffuse_0[] = {0.0, 0.0, 0.0, 1.0};	// colore diffusione della luce 0
	GLfloat light_specular_0[] = {1.0, 1.0, 1.0, 1.0};	// colore speculare della luce 0
	GLfloat light_position_0[] = {5.0, 5.0, 0.0, 1.0};	// posizione della luce 0
	
	glLightfv (GL_LIGHT0, GL_AMBIENT,	light_ambient_0);
	glLightfv (GL_LIGHT0, GL_DIFFUSE,	light_diffuse_0);
	glLightfv (GL_LIGHT0, GL_SPECULAR,	light_specular_0);
	glLightfv (GL_LIGHT0, GL_POSITION,	light_position_0);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	
	reshape(width, height);
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	if (laterale) {
		px = 7;	py = 7; pz = 7;
		sux = 0; suy = 1;
		strx = -13; stry = 2; strz = 0;
	}
	else {
		px = 0; py = 15; pz = 0;
		sux = -1; suy = 0;
		strx = -5.5;
		stry = 0; strz = 8;
	}

	quadratic = gluNewQuadric();
	gluQuadricNormals(quadratic, GLU_SMOOTH);
	
	//posizione iniziale delle palle
	p[0].setPosizione(3.6, 0, 0);
	p[1].setPosizione(4.6, 0, 3.2);
	p[2].setPosizione(2.5, 0, 1);
	p[3].setPosizione(-2.5, 0, 1);
	p[4].setPosizione(-3, 0, -1.5);
	p[5].setPosizione(-2, 0, 3);
	p[6].setPosizione(4, 0, -2);
	p[7].setPosizione(0, 0, 2);
	p[8].setPosizione(1.5, 0, -1.5);
	p[9].setPosizione(20, 0, 20);
	p[0].setBianca();

	glEnable(GL_TEXTURE_2D);
	
	//caricamento delle texture
	if (!p[1].LoadTextureTGA("immagini/1gialla.tga")) {
		printf("An error occured while loading texture 1gialla, aborting...\n");
		exit(0);
	}

	if (!p[2].LoadTextureTGA("immagini/3rossa.tga")) {
		printf("An error occured while loading texture 3rossa, aborting...\n");
		exit(0);
	}

	if (!p[3].LoadTextureTGA("immagini/5arancione.tga")) {
		printf("An error occured while loading texture 5arancione, aborting...\n");
		exit(0);
	}

	if (!p[4].LoadTextureTGA("immagini/6verde.tga")) {
		printf("An error occured while loading texture 6verde, aborting...\n");
		exit(0);
	}

	if (!p[5].LoadTextureTGA("immagini/8nera.tga")) {
		printf("An error occured while loading texture 8nera, aborting...\n");
		exit(0);
	}

	if (!p[6].LoadTextureTGA("immagini/10azzurra.tga")) {
		printf("An error occured while loading texture 10azzurra, aborting...\n");
		exit(0);
	}

	if (!p[7].LoadTextureTGA("immagini/12blu.tga")) {
		printf("An error occured while loading texture 12blu, aborting...\n");
		exit(0);
	}

	if (!p[8].LoadTextureTGA("immagini/13arancione.tga")) {
		printf("An error occured while loading texture 13arancione, aborting...\n");
		exit(0);
	}

}

void urti(int i) {
	int j;
	float distx, distot, alpha, alpha2, angi, angj, angj2;
	for (j = 0; j < numpalle; j++) {
		if (i == j || p[j].inBuca()) continue;//per ogni palla saltiamo i confronti con se stessa e quelle in buca
		
		distot = sqrt((p[i].getPosizione().getX() - p[j].getPosizione().getX())*(p[i].getPosizione().getX() - p[j].getPosizione().getX()) 
			+ (p[i].getPosizione().getZ() - p[j].getPosizione().getZ())*(p[i].getPosizione().getZ() - p[j].getPosizione().getZ()));
		
		if (distot <= 0.25) {//e la distanza attuale è minore di due volte il raggio c'è collisione
			distx = sqrt((p[i].getPosizione().getX() - p[j].getPosizione().getX())*(p[i].getPosizione().getX() - p[j].getPosizione().getX()));
			alpha = acos(distx/distot);//angolo formato dalla congiungente dei due centri
			alpha2 = alpha;
					
			p[j].setV0(p[i].getVel());//imprimiamo alla pallina urtata velocità...
			if (cos(alpha) != cos(p[i].getAngle())) {
				angi =  p[i].getAngle()/rad;
				if (alpha > 0) {
					angj = (alpha + angi)*rad;
					angj2 = (alpha2 + angi)*rad;
				}
				else {
					angj = (alpha - angi)*rad;
					angj2 = (alpha2 - angi)*rad;
				}
			
				p[j].setAngle(angj);//... e angolo di movimento
				angi = (angj2 + 90);

				//settiamo i nuovi parametri della pallina urtante...
				p[i].setAngle(angi);	
				p[i].setV0(0.1*p[i].getVel());
				p[i].setTempo(0);
				p[i].setVel(p[i].getV0());
				p[i].setPosIniziale(p[i].getPosizione().getX(), 0, p[i].getPosizione().getZ());
			}
			else p[i].setVel(0);
			
			//... e quelli dell'urtata
			p[j].setA(p[i].getA()); p[j].setB(p[i].getB());
			p[j].setTempo(0);
			p[j].setVel(p[j].getV0());
			p[j].setPosIniziale(p[j].getPosizione().getX(), 0, p[j].getPosizione().getZ());

			p[j].setFine(0);
		}
	}
			
}

/*
* invocata ogni 1000/FPS millisecondi.
* all' interno di questa funzione va cambiato lo stato degli oggetti della scena al fine, ad esempio, di animarli.
* quando lo stato cambia, si deve chiamare glutPostRedisplay() per aggiornare la visualizzazione il prima possibile
* (cioè quando tutti gli eventi pendenti sono stati processati).
*
* t - è il parametro definito dall' utente specificato per ultimo nella glutTimerFunc
*/
void animation(int t)
{
	switch (fase) {
	case 0: 
        break;
	
	case 1: //movimento all'indietro della stecca
		tiro_x += 0.1;
		tiro_z += 0.1;
		if (tiro_x >= 1)
			fase = 2;
		break;
	
	case 2: //movimento in avanti 
		tiro_x -= 0.2;
		tiro_z -= 0.2;
		if (tiro_x <= -0.2) {
			p[0].setAngle(angle);
			tiro_x = 0;
			tiro_z = 0;
			tiro = 1;
			fase = 3;
		}
		break;
	
	case 3: //per ogni palla calcoliamo la nuova posizione e verifichiamo la presenza di urti
		int i;
		for (i = 0; i < numpalle; i++) {
			p[i].calcola();
			urti(i);
		}
			
		fase = 0;
		tiro = 0;
		
		//finchè tutte le palle non hanno il flag fine a true rimaniamo in questa fase
		for (i = 0; i < numpalle; i++)
			if (p[i].getFine() == 0) {
				fase = 3;
				tiro = 1;
				break;
			}
		glutPostRedisplay();
		break;
	
	case 4: //cambio di visuale da alta a laterale e viceversa
		if (laterale) {
			px = 0; py = 15; pz = 0;
			sux = -1; suy = 0; 
			strx = -5.5; stry = 0; strz = 8;
			laterale = 0;
		}
		else {
			px = 7; py = 7; pz = 7;
			sux = 0; suy = 1;
			strx = -13; stry = 2; strz = 0;
			laterale = 1;
		}

		fase = 0;
		glutPostRedisplay();
		break;
	}
	
	glutTimerFunc((int) 1000/FPS, animation, 0);
	// La glutTimerFunc funziona come una sveglia che squilla una volta dopo x millisecondi. 
	// Se si vuole che suoni (si richiami) di nuovo bisogn ri-invocarla
	
	glutPostRedisplay();
}

/*
* inizializza GLUT, registra le callback, entra nel main loop
*
* argc : numero di parametri di ingresso
* argv : vettore degli argomenti di ingresso
*
* nota: argc e argv aiutano a capire quali sono i parametri di ingresso passati da linea di comando
*/
int main(int argc, char** argv) 
{
	glutInit(&argc, argv);
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Biliardo");
	
	// registra le callback
	glutKeyboardFunc(keyboardDown);
	glutKeyboardUpFunc(keyboardUp);
	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMotion);
	glutReshapeFunc(reshape);
	glutDisplayFunc(draw);  
	glutIdleFunc(idle);
	glutTimerFunc((int) 1000/FPS, animation, 0);
	glutIgnoreKeyRepeat(false); // process keys held down
	
	// crea un sottomenu
	int subMenu = glutCreateMenu(menu);
	glutAddMenuEntry("Do nothing", 0);
	glutAddMenuEntry("Really Quit", 'q');
	
	// crea il menu principale (apribile con click destro)
	glutCreateMenu(menu);
	glutAddSubMenu("Sub Menu", subMenu);
	glutAddMenuEntry("Quit", 'q');
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	
	// inizializza lo stato di OpenGL
	initGL(width, height);
	
	// entra nel rendering loop	
	glutMainLoop();
	return 0;
}
