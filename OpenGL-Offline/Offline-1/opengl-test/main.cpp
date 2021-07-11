#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<bits/stdc++.h>

#include <windows.h>
#include <GL/glut.h>

#define pi (2*acos(0.0))
using namespace std;
double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
int pause;
double angle;


const double MIN_SPEED = 0.002;
const double MAX_SPEED = 0.5;
const int SML_CIRCLES = 5;

time_t init_time;


struct point
{
	double x,y,z;
	point() {}
	point(double x, double y, double z) : x(x), y(y), z(z) {}
    point operator + (const point p) {
        return point(x + p.x , y + p.y , z + p.z);
	}
	point operator - (const point p) {
	    return point(x - p.x , y - p.y , z - p.z);
	}
	point operator * (double c) {
	    return point(x*c, y*c, z*c);
	}
	point operator / (double c) {
	    return point(x/c, y/c, z/c);
	}
    friend ostream& operator<<(ostream& os, const point& p) {
        os << "( "<< p.x <<", "<< p.y <<", "<< p.z <<" )";
        return os;
    }
    void normalize() {
        double down = sqrt(x * x + y * y + z * z);
        (*this) = (*this) / down;
    }
};

double dot(point p1, point p2) {
    return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}

point sml_circle[SML_CIRCLES+1], sml_circle_dir[SML_CIRCLES+1], big_circle;
double sml_radius, big_radius, circle_speed, circle_segemnts;
double square_side;
int circle_status[SML_CIRCLES+1];
int collision_status[SML_CIRCLES+1][SML_CIRCLES+1];

int color_status[SML_CIRCLES+1];


void keyboardListener(unsigned char key, int x,int y){
	switch(key){
        case 'p':
            pause ^=1;
            break;
		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			cameraHeight -= 3.0;
			circle_speed -= 0.01;
			circle_speed = max(circle_speed, MIN_SPEED);
			circle_speed = min(circle_speed, MAX_SPEED);
			break;
		case GLUT_KEY_UP:		// up arrow key
			cameraHeight += 3.0;
			circle_speed += 0.01;

            circle_speed = max(circle_speed, MIN_SPEED);
			circle_speed = min(circle_speed, MAX_SPEED);
			break;

		case GLUT_KEY_RIGHT:
			break;
		case GLUT_KEY_LEFT:
			break;

		case GLUT_KEY_PAGE_UP:
			break;
		case GLUT_KEY_PAGE_DOWN:
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
			break;
		case GLUT_KEY_END:
			break;

		default:
			break;
	}
}


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			break;

		case GLUT_RIGHT_BUTTON:
			//........
			break;

		case GLUT_MIDDLE_BUTTON:
			//........
			break;

		default:
			break;
	}
}

void draw_square(double x, double y, double a) {
	glBegin(GL_LINE_LOOP);
	{
		glVertex3f(x, y, 0);
		glVertex3f(x+a, y, 0);
		glVertex3f(x+a, y+a, 0);
		glVertex3f(x, y+a, 0);
	}
	glEnd();
}

void draw_circle(double radius,int segments) {
    int i;
    struct point points[segments + 1];
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw segments using generated points
    for(i=0;i<segments;i++)
    {
        glBegin(GL_LINES);
        {
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}

void display_task1() {
    glColor3f(0,1,0);
    draw_square(0,0, square_side);


    for(int i =0; i < SML_CIRCLES ; i++) {
        if( (time(0) - init_time) < (i+1) ) continue;

        glPushMatrix();

//        int ok = 0;
//        for(int j =0 ; j < SML_CIRCLES ;j++ ) {
//            if(collision_status[i][j]) ok = 1;
//        }
//        if(ok) color_status[i] ^= 1;
//
        glColor3f(1,0.5,0);
//        if(color_status[i]) glColor3f(1,0.5,1);
//        else glColor3f(1,0.5,0.5);
        glTranslatef(sml_circle[i].x, sml_circle[i].y ,0);
        draw_circle(sml_radius, circle_segemnts);
        glPopMatrix();
    }

    glColor3f(1, 0, 0);
    glTranslatef(big_circle.x, big_circle.y ,0);
    draw_circle(big_radius, circle_segemnts);

}

double dist2(point p1, point p2) {
    return (p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y) + (p1.z - p2.z)*(p1.z - p2.z) ;
}

bool collide(point c1, point c2, double r1, double r2) {
    return dist2(c1, c2) <= (r1 + r2)*(r1+r2);
}

bool inside_big_circle(point p) {
    return dist2(p, big_circle) <= (big_radius - sml_radius)*(big_radius- sml_radius);
}


void animate_task1() {
    if(pause) return;

    for(int i = 0; i < SML_CIRCLES ; i++ ) {

        if( (time(0) - init_time) < (i+1) ) continue;


        if(inside_big_circle(sml_circle[i])) {
//            if(circle_status[i] == 0) printf("I am %d, going in\n",i);
            circle_status[i] = 1;
        }


        if(sml_circle[i].x < sml_radius || square_side - sml_circle[i].x < sml_radius) sml_circle_dir[i].x *= -1;   // checking if circle collides with vertical side
        if(sml_circle[i].y < sml_radius || square_side - sml_circle[i].y < sml_radius) sml_circle_dir[i].y *= -1;   // checking if circle collides with horizontal side

        for(int j = i+1 ;  j < SML_CIRCLES ; j++ ) {

            if( (time(0) - init_time) < (j+1) ) continue;

            if(!collide(sml_circle[i] , sml_circle[j], sml_radius, sml_radius) ) {
                collision_status[i][j] = collision_status[j][i]  = 0;
                continue;
            }
            if(collision_status[i][j]) continue;
            collision_status[i][j] = collision_status[j][i] =  1;
            if(circle_status[i] == 0 || circle_status[j] == 0) continue;



//            cout<<sml_circle[i]<<" "<<sml_circle[j]<<endl;
//            cout<<"before = ";
//
//            cout<<sml_circle_dir[i]<<" "<<sml_circle_dir[j]<<endl;

            point nn = sml_circle[i] - sml_circle[j];
            nn.normalize();
            sml_circle_dir[i] = sml_circle_dir[i] - nn * 2 * dot(sml_circle_dir[i], nn);
            sml_circle_dir[i].normalize();

            nn = sml_circle[j] - sml_circle[i];
            nn.normalize();
            sml_circle_dir[j] = sml_circle_dir[j] - nn * 2 * dot(sml_circle_dir[j], nn);
            sml_circle_dir[j].normalize();

            color_status[i] ^= 1;
            color_status[j] ^= 1;
//            cout<<"after = ";
//
//            cout<<sml_circle_dir[i]<<" "<<sml_circle_dir[j]<<endl;
//            cout<<endl;
        }

//
        if(circle_status[i] && !inside_big_circle(sml_circle[i])) {
            point nn = big_circle - sml_circle[i];
            nn.normalize();
            sml_circle_dir[i] = sml_circle_dir[i] - nn * 2 * dot(sml_circle_dir[i], nn);
        }


        sml_circle[i] = sml_circle[i] + sml_circle_dir[i] * circle_speed;
    }



}

void init_task1() {
    square_side = 200;
    circle_segemnts = 100;

    circle_speed = 0.003;
    sml_radius = 10;
    for(int i =0 ; i < SML_CIRCLES ; i++ ) {

        sml_circle_dir[i] = point(rand(), rand(), 0);
        sml_circle_dir[i].normalize();
        sml_circle[i] = point(sml_radius, sml_radius, 0);
    }

    big_circle.x = square_side/2;
    big_circle.y = square_side/2;
    big_radius = square_side/3;

    init_time = time(NULL);
}


void display(){

	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/********************
	/ set-up camera here
	********************/
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();

	//now give three info
	//1. where is the camera (viewer)?
	//2. where is the camera looking?
	//3. Which direction is the camera's UP direction?

//	gluLookAt(100,100,100,	0,0,0,	0,0,1);
//	gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	gluLookAt(100,100,150,	100,100,0,	0,1,0);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/

    display_task1();


	glutSwapBuffers();
}


void animate(){
    animate_task1();
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=0;
	drawaxes=0;
	cameraHeight=150.0;
	cameraAngle=1.0;
	angle=0;

	//clear the screen
	glClearColor(0,0,0,0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(80,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance

	init_task1();
}

int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("My OpenGL Program");

	init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}
