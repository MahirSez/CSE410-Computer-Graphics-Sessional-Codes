#include<bits/stdc++.h>
#include "bitmap_image.hpp"
using namespace std;

const int N = 4;
const int RGB = 256;
const double PI = acos(-1);
const double INF = 1e18;
const double EPS = 1e-12;

ifstream scene_in;
ofstream stage1_out;

int triangle_cnt;

class Point {
    vector<double>pt;
public:
    Point() {
        pt = vector<double>(N);
    }
    Point(double x, double y, double z, double w = 0) {
        pt = vector<double>(N);
        pt[0] = x; pt[1] = y; pt[2] = z, pt[3] = w;
    }
    static double dot(Point& v1, Point& v2) {
        double ret = 0;
        for(int i = 0 ; i < N ; i++ ) {
            ret += v1[i] * v2[i];
        }
        return ret;
    }
    static Point cross(Point& v1, Point &v2) {
        Point ret;
        ret[0] = v1[1] * v2[2] - v1[2] * v2[1];
        ret[1] = -v1[0] * v2[2] + v1[2] * v2[0];
        ret[2] = v1[0] * v2[1] - v1[1] * v2[0];
        return ret;
    }
    void normalize() {
        double down = 0;
        for(double x : pt) down += x*x;
        down = sqrt(down);
        for(double &x: pt) x /= down;
    }
    void weigh_down() {
        assert(pt[N-1] != 0);
        for(double &x: pt) x /= pt[N-1];
    }

    //operator overloads
    double& operator[] (int id) {
        return pt[id];
    }
    friend ostream& operator<<(ostream& os, Point& v) {
        for(int i = 0 ; i < N - 1; i++ ) os << v[i] <<" ";
        return os;
    }
    friend istream& operator>>(istream& is, Point& v) {
        for(int i = 0 ; i < N - 1; i++ ) is >> v[i] ;
        return is;
    }
    Point operator* (double val) {
        Point ret = *this;
        for(double &x: ret.pt) x *= val;
        return ret;
    }
    Point operator+ (Point &v) {
        Point ret;
        for(int i = 0 ; i < N ; i++) ret[i] = (*this)[i] + v[i];
        return ret;
    }
    Point operator- (Point &v) {
        Point ret;
        for(int i = 0 ; i < N ; i++) ret[i] = (*this)[i] - v[i];
        return ret;
    }
};

class Matrix {
    vector< vector< double> > mat;
public:
    Matrix() {
        mat = vector< vector<double> > (N, vector<double> (N));
    }

    // returns NxN identity matrix
    static Matrix I() {
        Matrix ret;
        for(int i = 0 ; i < N ; i++) ret[i][i] = 1;
        return ret;
    }

    //operator overloads

    vector<double>& operator[] (int id) {
        return mat[id];
    }
    friend ostream& operator<<(ostream& os, Matrix& mat) {
        for(int i = 0 ; i < N ; i++ ) {
            for(int j = 0 ; j < N ; j++ ) os << mat[i][j] << " ";
            os<<'\n';
        }
        return os;
    }
    Point operator*(Point& pt) {
        Point ret;
        for(int i = 0 ; i < N ; i++ ) {
            for(int j = 0 ; j < N ; j++) {
                ret[i] += mat[i][j] * pt[j];
            }
        }
        return ret;
    }
    Matrix operator*(Matrix& mul) {
        Matrix ret;
        for(int i = 0 ; i < N ; i++ ) {
            for(int j = 0; j < N ; j++) {
                for(int k = 0 ; k < N ; k++) {
                    ret[i][j] += mat[i][k] * mul[k][j];
                }
            }
        }
        return ret;
    }
};

class Triangle {
    vector<Point>pts;
public:
    vector<int>rgb;
    Triangle() {
        pts = vector<Point>(3);
        rgb = vector<int>(3);
        rgb[0] = rand() % RGB;
        rgb[1] = rand() % RGB;
        rgb[2] = rand() % RGB;
    }
    Triangle(Point p1, Point p2, Point p3) {
        Triangle();
        pts[0] = p1;
        pts[1] = p2;
        pts[2] = p3;
    }
    double get_top_y() {
        double ret = -INF;
        for(Point pt: pts) ret = max(ret, pt[1]);
        return ret;
    }
    double get_bottom_y() {
        double ret = INF;
        for(Point pt: pts) ret = min(ret, pt[1]);
        return ret;        
    }
    double get_left_x() {
        double ret = INF;
        for(Point pt: pts) ret = min(ret, pt[0]);
        return ret;
    }
    double get_right_x() {
        double ret = -INF;
        for(Point pt: pts) ret = max(ret, pt[0]);
        return ret;
    }

    vector<pair< double, double> >get_xz_intersections(double y) {
        vector<pair< double, double>>ret;
        for(int i = 0; i < 3 ; i++) {
            Point pt1 = pts[i];
            Point pt2 = pts[(i+1)%3];

            if(pt1[0] > pt2[0]) swap(pt1, pt2);

            double x = pt1[0] + (y - pt1[1]) * (pt2[0] - pt1[0]) / (pt2[1] - pt1[1]);
            double z = pt1[2] + (pt2[2] - pt1[2]) * (y - pt1[1]) / (pt2[1] - pt1[1]);
            
            // cout<<" -- "<<pt1[0]<<" "<<pt2[0]<<" "<<x<<'\n';
            if(x >= pt1[0] && x <= pt2[0]) ret.push_back({x,z});
        }
        return ret;
    }

    Point& operator[] (int id) {
        return pts[id];
    }
    friend ostream& operator<<(ostream& os, Triangle& triangle) {
        for(int i = 0 ; i < 3; i++ ) os << triangle[i] <<'\n';
        return os;
    }
    friend istream& operator>>(istream& is, Triangle& triangle) {
        for(int i = 0 ; i < 3; i++ ) is >> triangle[i] ;
        return is;
    }
    

};

stack<Matrix>transformation_stack;

void handle_triangle() {    
    for(int i = 0 ; i < 3; i++ ) {
        Point pt;
        scene_in>>pt;
        pt[3] = 1;

        pt = transformation_stack.top() * pt ;
        stage1_out<<pt<<'\n';
    }
    stage1_out<<'\n';
}


void apply_transformation(Matrix& mat) {
    mat = transformation_stack.top() * mat;
    transformation_stack.pop();
    transformation_stack.push(mat);
}

void handle_scaling() {
    Matrix mat;
    scene_in>>mat[0][0]>>mat[1][1]>>mat[2][2];
    mat[3][3] = 1;
    apply_transformation(mat);
}

void handle_translate() {
    Matrix mat = Matrix::I();
    scene_in >>mat[0][N-1]>>mat[1][N-1]>>mat[2][N-1];
    apply_transformation(mat);
}

Point apply_rodrigues(Point main_axis, Point axis_of_rotation, double theta) {

    theta *= PI / 180.0;
    Point v1 = main_axis * cos(theta);
    Point v2 = axis_of_rotation * (1.0 - cos(theta)) * Point::dot(axis_of_rotation, main_axis);
    Point v3 = Point::cross(axis_of_rotation, main_axis) * sin(theta);
    return (v1 + v2 + v3);
}

void handle_rotate() {

    double angle ;
    scene_in>>angle;
    Point rot_vec;
    scene_in>>rot_vec;
    rot_vec.normalize();
    Point c1 = apply_rodrigues(Point(1, 0, 0), rot_vec, angle);
    Point c2 = apply_rodrigues(Point(0, 1, 0), rot_vec, angle);
    Point c3 = apply_rodrigues(Point(0, 0, 1), rot_vec, angle);

    Matrix mat;
    for(int i = 0 ; i < N ; i++ ) {
        mat[i][0] = c1[i], mat[i][1] = c2[i] , mat[i][2] = c3[i];
    }
    mat[3][3] = 1;

    apply_transformation(mat);
}

Point eye, look, up;
double fovY, aspect_ratio, near, far;


void apply_view_transformation() {

    
    Point l = look - eye;
    l.normalize();
    Point r = Point::cross(l, up);
    r.normalize();
    Point u = Point::cross(r, l);
    u.normalize();

    Matrix R;

    for(int i = 0 ; i < N ; i++ ) {
        R[0][i] = r[i];
        R[1][i] = u[i];
        R[2][i] = - l[i];
    }
    R[3][3] = 1;

    Matrix T = Matrix::I();
    for(int i = 0 ; i < 3 ; i++ ) T[i][3] = -eye[i];

    Matrix V = R * T;

    ifstream stage1_in;
    ofstream stage2_out;
    stage1_in.open("stage1.txt");
    stage2_out.open("stage2.txt");
    stage2_out<<setprecision(7)<<fixed;

    
    int cnt = 0;

    for(int i = 0 ; i < 3 * triangle_cnt ; i++) {
        Point pt;
        stage1_in>>pt;
        pt[3] = 1;
        pt = V * pt;
        pt.weigh_down();
        cnt++;
        stage2_out<<pt<<'\n';
        if(cnt % 3 == 0) stage2_out<<'\n';
    }

    stage1_in.close();
    stage2_out.close();
}

void apply_projection_transformation() {
    double fovX = fovY * aspect_ratio;
    double t = near * tan( (fovY*PI) / (2.0 * 180));
    double r = near * tan( (fovX*PI) / (2.0*180));
    
    Matrix P;
    P[0][0] = near / r;
    P[1][1] = near / t;
    P[2][2] = -(far+ near) / (far - near);
    P[2][3] = -(2*far*near) / (far - near);
    P[3][2] = -1;

    ifstream stage2_in;
    ofstream stage3_out;

    stage2_in.open("stage2.txt");
    stage3_out.open("stage3.txt");
    stage3_out<<setprecision(7)<<fixed;

    int cnt = 0;

    for(int i = 0 ; i < 3 * triangle_cnt ; i++) {
        Point pt;
        stage2_in>>pt;
        pt[3] = 1;
        pt = P * pt;
        pt.weigh_down();
        cnt++;
        stage3_out<<pt<<'\n';
        if(cnt % 3 == 0) stage3_out<<'\n';
    }

    stage2_in.close();
    stage3_out.close();
}


void generate_image(char config_file[]) {
    ifstream stage3_in, config_in;
    ofstream z_buffer_out;

    
    config_in.open(config_file);
    
    int screen_width, screen_height;
    double left_limit, right_limit;
    double bottom_limit, top_limit;
    double front_limit, rear_limit;

    config_in>>screen_width>>screen_height;
    config_in>>left_limit>>bottom_limit;
    config_in>>front_limit>>rear_limit;
    config_in.close();
    

    right_limit = - left_limit;
    top_limit = - bottom_limit;
    

    double dx = (right_limit - left_limit) / screen_width;
    double dy = (top_limit - bottom_limit) / screen_height;
    

    double top_y = top_limit - dy/2;
    double left_x = left_limit + dx/2;    
    
    vector< vector<double> > z_buffer(screen_width, vector<double>(screen_height, rear_limit));

    bitmap_image image(screen_width, screen_height);

    stage3_in.open("stage3.txt");

    for(int i = 0 ; i < triangle_cnt ; i++) {

        
        Triangle triangle;
        stage3_in>>triangle;

        int max_y = clamp( (int)round( (top_y - triangle.get_bottom_y() )/ dy) , 0, screen_height-1);
        int min_y = clamp( (int)round( (top_y - triangle.get_top_y() )/ dy) , 0, screen_height-1);

        for(int y = min_y ; y <= max_y; y++) {
            double yy = top_y - 1.0 * y * dy;

            if(yy < triangle.get_bottom_y() || yy >  triangle.get_top_y() ) continue;
            if(yy < bottom_limit || yy > top_limit) continue;

            vector< pair<double, double> >x_intersections = triangle.get_xz_intersections(yy);
            // assert(x_intersections.size());
            if(x_intersections.size() == 0) continue;
            
        
            sort(x_intersections.begin() , x_intersections.end() );
            double xa = x_intersections[0].first;
            double xb = x_intersections.back().first;

            int min_x = clamp( (int)round ((xa - left_x ) / dx), 0, screen_width-1);
            int max_x = clamp( (int)round ((xb - left_x ) / dx), 0, screen_width-1);


            double za = x_intersections[0].second;
            double zb = x_intersections.back().second;


            for(int x = min_x; x <= max_x ; x++) {

                double xx = left_x + 1.0 * x * dx;

                if(xx < triangle.get_left_x() || xx >  triangle.get_right_x() ) continue;
                if(xx < left_limit || xx > right_limit) continue;

                double zz;
                if(xx == xa) zz = za;
                else if(xx == xb) zz = zb;
                else zz = zb - (zb-za) * (xb - xx) / (xb - xa);

                if(zz >= front_limit && zz < z_buffer[x][y])  {                    
                    z_buffer[x][y] = zz;
                    image.set_pixel(x,y,triangle.rgb[0],triangle.rgb[1],triangle.rgb[2] );
                }
            }
        }
    }
    image.save_image("out.bmp");
    stage3_in.close();

    z_buffer_out.open("z_out.txt");
    z_buffer_out<<setprecision(6)<<fixed;
    for(int i = 0 ; i < screen_height ; i++) {
        for(int j = 0 ; j < screen_width ; j++ ) {
            if(z_buffer[j][i] < rear_limit) z_buffer_out<<z_buffer[j][i]<<"\t";
        }
        z_buffer_out<<'\n';
    }
    z_buffer_out.close();
}


int main(int argc, char *argv[]) {

    // scene_in.open("../tests/1/scene.txt");
    scene_in.open(argv[1]);
    stage1_out.open("stage1.txt");
    
    stage1_out<<setprecision(7)<<fixed;

    scene_in>>eye>>look>>up;

    scene_in>>fovY>>aspect_ratio>>near>>far;

    transformation_stack.push(Matrix::I());

    while(1) {
        
        string command;
        scene_in>>command;

        if(command == "triangle") {
            triangle_cnt++;
            handle_triangle();
        }
        else if(command == "push") transformation_stack.push(transformation_stack.top());
        else if(command == "pop") transformation_stack.pop();
        else if(command == "scale") handle_scaling();
        else if(command == "translate") handle_translate();
        else if(command == "rotate") handle_rotate();
        else if(command == "end") break;
    }
    scene_in.close();
    stage1_out.close();

    apply_view_transformation();
    apply_projection_transformation();

    generate_image(argv[2]);
    return 0;
}