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

const int INF = 1000;

const int camera_speed = 5, rotation_speed = 2;
const double big_sphere_radius = 30, sml_sphere_radius = 10;
const double plane_vertex = 500;
const double MAX_ANGLE = 30, MIN_ANGLE = -MAX_ANGLE ;


double sphere_hor_rotation_angle, sphere_ver_rotation_angle;
double cylinder_ver_rotation_angle, cylinder_spin_angle;


const int sphere_slices = 100, sphere_stacks = 100;
const int cylinder_slices = 50, cylinder_stacks = 100;

time_t init_time;
int debug ;




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

vector<point> shots;


void print(vector< vector<double> > vec) {
    for(auto x : vec) {
        for(auto xx: x) cout<<xx<<" ";
        cout<<endl;
    }
    cout<<endl;
    cout<<endl;
}

point pos, u, r, l;

double dot(point p1, point p2) {
    return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}

void u_rotation(double angle) {
    angle *= pi/180.0;
    point rr = r * cos(angle) - l * sin(angle);
    point ll = r * sin(angle) + l * cos(angle);
    r = rr;
    l = ll;
}

void r_roation(double angle) {
    angle *= pi/180.0;
    point uu = u * cos(angle) + l * sin(angle);
    point ll = u * sin(angle)*-1 + l * cos(angle);
    u = uu;
    l = ll;
}

void l_rotation(double angle) {
    angle *= pi/180.0;
    point uu = u * cos(angle) - r * sin(angle);
    point rr = u * sin(angle) + r * cos(angle);
    u = uu;
    r = rr;
}


void look_left() { u_rotation(-camera_speed); }
void look_right() { u_rotation(camera_speed); }

void look_up() { r_roation(-camera_speed); }
void look_down() { r_roation(camera_speed); }

void tilt_clockwise() { l_rotation(camera_speed); }
void tilt_counterclockwise() { l_rotation(-camera_speed); }



void keyboardListener(unsigned char key, int x,int y){
    debug = 0;
	switch(key){
		case '1':
            look_left();
			break;
        case '2':
            look_right();
            break;
        case '3':
            look_up();
            break;
        case '4':
            look_down();
            break;
        case '5':
            tilt_clockwise();
            break;
        case '6':
            tilt_counterclockwise();
            break;
        case 'q':
            sphere_hor_rotation_angle += rotation_speed;
            break;
        case 'w':
            sphere_hor_rotation_angle -= rotation_speed;
            break;
        case 'e':
            sphere_ver_rotation_angle += rotation_speed;
            break;
        case 'r':
            sphere_ver_rotation_angle -= rotation_speed;
            break;
        case 'a':
            cylinder_ver_rotation_angle += rotation_speed;
            break;
        case 's':
            cylinder_ver_rotation_angle -= rotation_speed;
            break;
        case 'd':
            cylinder_spin_angle += rotation_speed;
            break;
        case 'f':
            cylinder_spin_angle -= rotation_speed;
            break;
		default:
			break;
	}
}



void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
//			cameraHeight -= 3.0;
            pos = pos - l*camera_speed;
			break;
		case GLUT_KEY_UP:		// up arrow key
//			cameraHeight += 3.0;
            pos = pos + l*camera_speed;
			break;

		case GLUT_KEY_RIGHT:
//			cameraAngle += 0.03;
            pos = pos + r*camera_speed;
			break;
		case GLUT_KEY_LEFT:
//			cameraAngle -= 0.03;
            pos = pos - r*camera_speed;
			break;

		case GLUT_KEY_PAGE_UP:
		    pos = pos + u*camera_speed;
			break;

		case GLUT_KEY_PAGE_DOWN:
		    pos = pos - u*camera_speed;
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

void lazer_shoot() {

    double alpha = (sphere_hor_rotation_angle + 90)*pi/180.0;
    double beta = (sphere_ver_rotation_angle)*pi/180.0;

    double angle = cylinder_ver_rotation_angle* pi/180.0;

    point gun_dir(cos(alpha)*cos(beta), sin(alpha)*cos(beta), sin(beta ));
    gun_dir.normalize();


    point gun_pos = gun_dir * big_sphere_radius;
    gun_dir = gun_dir + gun_pos;


    double yy = gun_dir.y + (gun_dir.z - gun_pos.z) * tan(angle);
    double zz = gun_dir.z - (gun_pos.y - gun_dir.y) * tan(angle);
    gun_dir.z = zz;
    gun_dir.y = yy;

    gun_dir = gun_dir - gun_pos;
    gun_dir = gun_dir * 1000;
    gun_dir = gun_dir + gun_pos;

//    if(debug == 0) {
//        cout<<beta*180/pi<<" "<<angle*180/pi<<" -- "<<gun_pos<<" "<<gun_dir<<" "<<endl;
//        debug = 1;
//    }
//
//
//    glColor3f(0, 1, 0);
//    glBegin(GL_LINES); {
//        glVertex3f(gun_pos.x, gun_pos.y, gun_pos.z);
//        glVertex3f(gun_dir.x, gun_dir.y, gun_dir.z);
//    }glEnd();
}



void shoot() {
    double alpha = (sphere_hor_rotation_angle + 90)*pi/180.0;
    double beta = (sphere_ver_rotation_angle)*pi/180.0;

    double angle = cylinder_ver_rotation_angle* pi/180.0;

    point gun_dir(cos(alpha)*cos(beta), sin(alpha)*cos(beta), sin(beta ));
    gun_dir.normalize();


    point gun_pos = gun_dir * big_sphere_radius;
    gun_dir = gun_dir + gun_pos;
//    gun_dir.z += gun_dir.y * sin(angle);


    double yy = gun_dir.y + (gun_dir.z - gun_pos.z) * tan(angle);
    double zz = gun_dir.z - (gun_pos.y - gun_dir.y) * tan(angle);
    gun_dir.z = zz;
    gun_dir.y = yy;

    gun_dir = gun_dir - gun_pos;
    gun_dir = gun_dir * 990;
    gun_dir = gun_dir + gun_pos;


//    gun_dir = gun_dir * 990;


    double t = (990 - gun_pos.y)/gun_dir.y;


    point target_point = gun_pos + gun_dir * t;

    if(fabs(target_point.x) <= plane_vertex && fabs(target_point.z) <= plane_vertex) shots.push_back(target_point);
}


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
                shoot();
			}
			break;

		case GLUT_RIGHT_BUTTON:

			if(state == GLUT_DOWN) {
                drawaxes ^= 1;
			}
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


void drawAxes() {
    if(!drawaxes) return;
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINES);{

        glVertex3f( INF,0,0);
        glVertex3f(-INF,0,0);

        glVertex3f(0,-INF,0);
        glVertex3f(0, INF,0);

        glVertex3f(0,0, INF);
        glVertex3f(0,0,-INF);
    }glEnd();
}


vector< vector< point> > load_sphere_points(double radius, int slices, int stacks) {

    vector< vector< point> > points(stacks+1, vector<point>(slices+1));
    for(int i = 0 ; i <= stacks ; i++) {
        double angle = 1.0 * i / stacks * pi / 2;

		double h = radius * sin(angle);
		double r = radius * cos(angle);
		for(int j = 0 ; j <= slices ; j++) {

            angle = 1.0 * j / slices * pi * 2;
			points[i][j].x = r*cos(angle);
			points[i][j].y = h;
			points[i][j].z = r*sin(angle);
		}
	}
	return points;
}

void draw_half_sphere(int flg, const vector< vector< point> > &points) {
    int stacks = points.size()-1;
    int slices = points[0].size()-1;
    for(int i = 0 ; i < stacks ; i++) {
		for(int j=0 ; j < slices ;j++) {

		    if(j % 2 == 0) glColor3f(0, 0, 0);
            else glColor3f(1, 1, 1);
            glBegin(GL_QUADS);{
                glVertex3f(points[i][j].x, points[i][j].y * flg, points[i][j].z);
                glVertex3f(points[i][j+1].x, points[i][j+1].y * flg, points[i][j+1].z);
                glVertex3f(points[i+1][j+1].x, points[i+1][j+1].y * flg, points[i+1][j+1].z);
                glVertex3f(points[i+1][j].x, points[i+1][j].y * flg, points[i+1][j].z);

            }glEnd();
		}
	}
}


void draw_cylinder(double radius, int slices) {

    vector< vector< point> > points(2, vector<point>(slices+1));

    for(int i = 0 ; i < 2 ; i++) {
        for(int j = 0 ; j <= slices ; j++) {
            double angle = 1.0 * j / slices * pi * 2;
            points[i][j].x = radius * cos(angle);
            points[i][j].y = (i == 0 ? 0 : cylinder_stacks);
            points[i][j].z = radius * sin(angle);
        }
    }
    draw_half_sphere(1, points);
}


void draw_plane() {

    glColor3f(0.4, 0.4, 0.4);
	glBegin(GL_QUADS);
	{
		glVertex3f(-plane_vertex, 0, plane_vertex);
		glVertex3f(plane_vertex, 0, plane_vertex);
		glVertex3f(plane_vertex, 0, -plane_vertex);
		glVertex3f(-plane_vertex, 0, -plane_vertex);
	}
	glEnd();
}


void draw_reverse_sphere(double radius, int slices, int stacks) {

    vector< vector< point> > points(stacks+1, vector<point>(slices+1));
    for(int i = 0 ; i <= stacks ; i++) {
        double angle = 1.0 * i / stacks * pi / 2;

		double h = radius * sin(angle);
		double r = radius * cos(angle);
		r = 2*radius - r;
		for(int j = 0 ; j <= slices ; j++) {

            angle = 1.0 * j / slices * pi * 2;
			points[i][j].x = r*cos(angle);
			points[i][j].y = h;
			points[i][j].z = r*sin(angle);
		}
	}
	draw_half_sphere(1, points);
}



void limit_angles() {

    sphere_hor_rotation_angle = max( min(sphere_hor_rotation_angle, MAX_ANGLE), MIN_ANGLE);
    sphere_ver_rotation_angle = max( min(sphere_ver_rotation_angle, MAX_ANGLE), MIN_ANGLE);
    cylinder_ver_rotation_angle = max( min(cylinder_ver_rotation_angle, MAX_ANGLE), MIN_ANGLE);
    cylinder_spin_angle = max( min(cylinder_spin_angle, MAX_ANGLE), MIN_ANGLE);
}

void show_shots() {

    double side = 15;
    for(auto xx : shots) {

        glColor3f(1, 0, 0);
        glBegin(GL_QUADS);{
            glVertex3f(xx.x + side, xx.y, xx.z + side);
            glVertex3f(xx.x - side, xx.y, xx.z + side);
            glVertex3f(xx.x - side, xx.y, xx.z - side);
            glVertex3f(xx.x + side, xx.y, xx.z - side);

        }glEnd();
    }
}

void display_task2() {
    limit_angles();
    lazer_shoot();
    drawAxes();



    glPushMatrix();
    glTranslatef(0, 1000, 0);
    draw_plane();
    glPopMatrix();

    show_shots();

    glRotatef(sphere_hor_rotation_angle, 0, 0, 1);

    vector< vector< point> > points = load_sphere_points(big_sphere_radius, sphere_slices, sphere_stacks);
    draw_half_sphere( -1, points);

    glRotatef(sphere_ver_rotation_angle, 1, 0, 0);
    draw_half_sphere( 1, points);


    glTranslatef(0, big_sphere_radius, 0);
    glRotatef(cylinder_ver_rotation_angle, 1, 0, 0);
    glTranslatef(0, sml_sphere_radius, 0);
    glRotatef(cylinder_spin_angle, 0, 1, 0);

    points = load_sphere_points(sml_sphere_radius, cylinder_slices, sphere_stacks);
    draw_half_sphere( -1, points);


    draw_cylinder(sml_sphere_radius, cylinder_slices);
    glTranslatef(0, cylinder_stacks, 0);
    draw_reverse_sphere(sml_sphere_radius, cylinder_slices, sphere_stacks);





}

double dist2(point p1, point p2) {
    return (p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y) + (p1.z - p2.z)*(p1.z - p2.z) ;
}

bool collide(point c1, point c2, double r1, double r2) {
    return dist2(c1, c2) <= (r1 + r2)*(r1+r2);
}

void animate_task2() {


}

void init_task2() {
    pos = point(100, 100, 0);
    u = point(0, 0, 1);
    r = point(-1.0/sqrt(2.0), 1.0/sqrt(2.0), 0);
    l = point(-1.0/sqrt(2.0), -1.0/sqrt(2.0), 0);
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
	gluLookAt(pos.x, pos.y, pos.z, pos.x + l.x, pos.y + l.y, pos.z + l.z, u.x, u.y, u.z);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects



    //glColor3f(1,0,0);
    //drawSquare(10);

    display_task2();

//    drawSS();

    //drawCircle(30,24);

    //drawCone(20,50,24);

	//drawSphere(30,24,20);

//    drawGrid();


	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){
//	angle+=0.05;
    animate_task2();
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=0;
	drawaxes=1;
	cameraHeight=150.0;
	cameraAngle=1.0;
//	angle=0;

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
	gluPerspective(80,	1,	1,	2000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance

	init_task2();
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
