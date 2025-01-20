
#include <GL/glut.h>  
#include <cmath>
#include <iostream>
#include "Task_1.h"


#define STEP_SIZE 32
//camera
Camera base_camera;
// octahedron
Point_Vector base_triangle_center,base_triangle_centroid;
GLfloat triangle_scale, rotation;

// sphere
GLfloat sphere_scale;
Point_Vector sphere_translate;

//cylinder

Point_Vector Max_info_cylinder, curr_info_cylinder; // x height , y theta, z translation
GLfloat cylinder_scale;

/* Initialize OpenGL Graphics */
void initGL() {
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
//    glClearDepth(1.0f);                   // Set background depth to farthest
   glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
//    glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
   glShadeModel(GL_SMOOTH);   // Enable smooth shading
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
}


/* Handler for window re-size event. Called back when the window first appears and
   whenever the window is re-sized with its new width and height */
void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
   // Compute aspect ratio of the new window
   if (height == 0) height = 1;                // To prevent divide by 0
   GLfloat aspect = (GLfloat)width / (GLfloat)height;
 
   // Set the viewport to cover the new window
   glViewport(0, 0, width, height);
 
   // Set the aspect ratio of the clipping volume to match the viewport
   glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
   glLoadIdentity();             // Reset
   // Enable perspective projection with fovy, aspect, zNear and zFar
   gluPerspective(45.0f, aspect, 0.1f, 100.0f);
}

void transition_to_octahedron()
{
    if(triangle_scale < 1.0)
    {
        triangle_scale+= 1.0/STEP_SIZE;
        base_triangle_center = base_triangle_center - base_triangle_centroid / STEP_SIZE;

        //sphere
        sphere_scale-= 1.0/STEP_SIZE;
        sphere_translate.x += 1.0/STEP_SIZE;

        //cylinder
        curr_info_cylinder.x+= Max_info_cylinder.x/STEP_SIZE;
        curr_info_cylinder.z+= Max_info_cylinder.z/STEP_SIZE;

    }
}

void transition_to_sphere()
{
    if(triangle_scale > 0.0)
    {
        triangle_scale-=1.0/STEP_SIZE;
        base_triangle_center = base_triangle_center + base_triangle_centroid / STEP_SIZE;

        //sphere
        sphere_scale+= 1.0/STEP_SIZE;
        sphere_translate.x -= 1.0/STEP_SIZE;

        //cylinder
        curr_info_cylinder.x-= Max_info_cylinder.x/STEP_SIZE;
        curr_info_cylinder.z-= Max_info_cylinder.z/STEP_SIZE;
    }
}

void clockwiserotate_y()
{
    rotation += 10;
}

void anticlockwiserotate_y()
{
    rotation -= 10;
}

void draw_axes()
{
    glLineWidth(3);
    glBegin(GL_LINES);

    glColor3f(1, 1, 0); // x //yelow
    glVertex3f(-10, 0, 0);
    glVertex3f(10, 0, 0);

    glColor3f(0, 1, 1); // y //cyan
    glVertex3f(0, -10, 0);
    glVertex3f(0, 10, 0);

    glColor3f(1, 0, 1); // z //purple
    glVertex3f(0, 0, -10);
    glVertex3f(0, 0, 10);
    glEnd();
    
}


void drawbasetriangle() {

    glBegin(GL_TRIANGLES);
    glVertex3f(1, 0, 0);
    glVertex3f(0, 1, 0);
    glVertex3f(0, 0, 1);
    glEnd();
}


void drawtetrahedron()
{
    GLdouble a = 90;
    int i = 0;
    while (i<4)
    {
        if(i%2) glColor3d(0,0.8,0.8);
        else glColor3d(0.53,0.15,0.34);

        glPushMatrix();
        glTranslated(base_triangle_center.x,base_triangle_center.y,base_triangle_center.z);
        glScaled(triangle_scale,triangle_scale,triangle_scale);
        
        drawbasetriangle();
        glPopMatrix();
        glRotated(a,0,0,1);
        

        i++;
    }
    
}

void drawoctahedron()
{
    glPushMatrix();
    drawtetrahedron();
    glPopMatrix();

    glPushMatrix();
    glRotated(180,1,0,0);
    drawtetrahedron();
    glPopMatrix();

}

void draw_unit_cube_sphere(int subdivision)
{
    const float DEG2RAD = acos(-1) / 180.0f;

    Point_Vector n1,n2,v,a;

    int pointsPerRow = (int)pow(2, subdivision) + 1;

    Point_Vector points[pointsPerRow][pointsPerRow];

    for (unsigned int i = 0; i < pointsPerRow; i++)
    {
        a.z = DEG2RAD * (45.0f - 90.0f * i / (pointsPerRow - 1));
        n2.x = - sin(a.z);
        n2.y = cos(a.z);
        n2.z = 0;

        for (unsigned int j = 0; j < pointsPerRow; j++)
        {
            a.y = DEG2RAD * (-45.0f + 90.0f * j / (pointsPerRow - 1)); 
            n1.x = - sin(a.y);
            n1.y = 0;
            n1.z = - cos(a.y);

            //cross product
            v.x = n1.y * n2.z - n1.z * n2.y;
            v.y = n1.z * n2.x - n1.x * n2.z;
            v.z = n1.x * n2.y - n1.y * n2.x;

            float scale = 0.577 / length(v);
            v = v*scale;

            points[i][j] = v;



        }
        
        
    }

    glBegin(GL_QUADS);
    for (int j = 0; j < pointsPerRow-1; j++)
    {
        for (int i = 0; i < pointsPerRow-1; i++)
        {
            glVertex3f(points[j][i].x, points[j][i].y, points[j][i].z);
            glVertex3f(points[j][i + 1].x, points[j][i + 1].y, points[j][i + 1].z);

            glVertex3f(points[j + 1][i + 1].x, points[j + 1][i + 1].y, points[j + 1][i + 1].z);
            glVertex3f(points[j + 1][i].x, points[j + 1][i].y, points[j + 1][i].z);
        }
    }
    glEnd();
    






}

void draw_unit_cube_sphere_with_translation()
{
    glPushMatrix();
    glTranslated(sphere_translate.x,sphere_translate.y,sphere_translate.z);
    glScaled(sphere_scale,sphere_scale,sphere_scale);
    draw_unit_cube_sphere(6);
    glPopMatrix();
}


void draw_sphere()
{

    GLfloat a = 90;

    glPushMatrix();
    int i = 0;
    while (i<4)
    {
        if(i%2)
        {
            glColor3d(0.2, 0.5, 0.8);
        }
        else
        {
            glColor3d(0.8,0.5,0.2);
        }

        draw_unit_cube_sphere_with_translation();
        glRotated(a,0,0,1);


        i++;
    }
    glPopMatrix();

    glPushMatrix();
    glColor3d(0.5, 0.3, 0.9); 
    glRotated(a,0,1,0);
    draw_unit_cube_sphere_with_translation();
    glPopMatrix();

    glPushMatrix();
    glColor3d(0.5, 0.3, 0.9); 
    glRotated(-a,0,1,0);
    draw_unit_cube_sphere_with_translation();
    glPopMatrix();

    

}

void draw_base_cylinder(int slices)
{
    Point_Vector temp,curr;
    temp.x = 0.577, temp.y = 0;
    double angle = - curr_info_cylinder.y/2.0;
    glPushMatrix();
    glBegin(GL_QUADS);
    int i = 0;
    while (i<slices)
    {       
        angle += curr_info_cylinder.y/ slices;
        curr.x = 0.577 * cos(angle);
        curr.y = 0.577 * sin(angle);

        glVertex3f(curr.x,curr.y,curr_info_cylinder.x/2.0);
        glVertex3f(curr.x,curr.y,- curr_info_cylinder.x/2.0);

        glVertex3f(temp.x, temp.y, - curr_info_cylinder.x/2.0);
        glVertex3f(temp.x, temp.y, curr_info_cylinder.x/2.0);

        temp.x = curr.x;
        temp.y = curr.y;

        i++;
    }
    glEnd();
    glPopMatrix();
    


}
void draw_base_cylinder_with_translation()
{
    glPushMatrix();
    glRotated(45,0,1,0);
    glTranslated(curr_info_cylinder.z,0,0);
    glScaled(sphere_scale,sphere_scale,1);
    draw_base_cylinder(120);
    glPopMatrix();
}

void draw_quad_cylinder()
{
    glPushMatrix();
    int i = 0;
    while (i<4)
    {
        draw_base_cylinder_with_translation();
        glRotated(90,0,1,0);
        i++;
    }

    glPopMatrix();

    
}

void draw_cylinder_frame()
{
    glPushMatrix();
    glColor3d(0.9, 0.6, 0.2);
    draw_quad_cylinder();

    glRotated(90, 1, 0, 0);
    draw_quad_cylinder();
    glColor3d(0.2, 0.9, 0.6);

    glRotated(90, 0, 0, 1);
    draw_quad_cylinder();
    glColor3d(0.2, 0.6, 0.9);
    
    glPopMatrix();
}



float angle_line=0;
float new_angle=0;
void draw_circle_with_moving_radius(float radius, float speed) {
    static float angle = 0.0f;

    glPushMatrix();
    glTranslatef(radius * cos(angle), radius * sin(angle), 0.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 360; i++) {
        float theta = i * 3.14159 / 180.0;
        glVertex3f(radius * cos(theta), 0.0f, radius * sin(theta));
    }
    glEnd();

    glLineWidth(2.0f);
    glColor3f(1.0f, 0.0f, 0.0f); // Red color
    glBegin(GL_LINES);
    glVertex3f(0,0,0);
    if(angle_line < 360)
    {
        angle_line += 1;
    }
    else
    {
        angle_line = 0;
    }
    glVertex3f(radius*cos(angle_line* 3.14159f / 180.0f) , 0.0f , radius*sin(angle_line* 3.14159f / 180.0f));

    new_angle=angle_line;
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) , 0.0f, radius*sin(new_angle* 3.14159f / 180.0f));
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) , -4.0f, radius*sin(new_angle* 3.14159f / 180.0f));
    //squares
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) +0.25 , -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)+0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) -0.25, -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)+0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) +0.25 , -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)+0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) +0.25, -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)-0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) -0.25 , -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)-0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) -0.25, -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)+0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) -0.25 , -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)-0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) +0.25, -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)-0.25);

    //startings
    if(angle_line+60 >= 360)    new_angle = angle_line-360;
    new_angle = new_angle + 60;
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) , 0.0f, radius*sin(new_angle* 3.14159f / 180.0f));
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) , -4.0f, radius*sin(new_angle* 3.14159f / 180.0f));
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) +0.25 , -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)+0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) -0.25, -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)+0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) +0.25 , -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)+0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) +0.25, -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)-0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) -0.25 , -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)-0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) -0.25, -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)+0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) -0.25 , -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)-0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) +0.25, -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)-0.25);

    if(angle_line+120 >= 360)    new_angle = angle_line-360;
    new_angle = new_angle + 120;
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) , 0.0f, radius*sin(new_angle* 3.14159f / 180.0f));
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) , -4.0f, radius*sin(new_angle* 3.14159f / 180.0f));  
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) +0.25 , -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)+0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) -0.25, -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)+0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) +0.25 , -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)+0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) +0.25, -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)-0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) -0.25 , -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)-0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) -0.25, -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)+0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) -0.25 , -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)-0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) +0.25, -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)-0.25);

    if(angle_line+180 >= 360)    new_angle = angle_line-360;
    new_angle = new_angle + 180;
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) , 0.0f, radius*sin(new_angle* 3.14159f / 180.0f));
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) , -4.0f, radius*sin(new_angle* 3.14159f / 180.0f));
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) +0.25 , -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)+0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) -0.25, -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)+0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) +0.25 , -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)+0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) +0.25, -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)-0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) -0.25 , -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)-0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) -0.25, -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)+0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) -0.25 , -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)-0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) +0.25, -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)-0.25);

    if(angle_line+240 >= 360)    new_angle = angle_line-360;
    new_angle = new_angle + 240;
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) , 0.0f, radius*sin(new_angle* 3.14159f / 180.0f));
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) , -4.0f, radius*sin(new_angle* 3.14159f / 180.0f));
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) +0.25 , -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)+0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) -0.25, -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)+0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) +0.25 , -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)+0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) +0.25, -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)-0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) -0.25 , -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)-0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) -0.25, -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)+0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) -0.25 , -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)-0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) +0.25, -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)-0.25);

    if(angle_line+300 >= 360)    new_angle = angle_line-360;
    new_angle = new_angle + 300;
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) , 0.0f, radius*sin(new_angle* 3.14159f / 180.0f));
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) , -4.0f, radius*sin(new_angle* 3.14159f / 180.0f));
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) +0.25 , -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)+0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) -0.25, -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)+0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) +0.25 , -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)+0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) +0.25, -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)-0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) -0.25 , -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)-0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) -0.25, -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)+0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) -0.25 , -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)-0.25);
    glVertex3f(radius*cos(new_angle* 3.14159f / 180.0f) +0.25, -4.0f, radius*sin(new_angle* 3.14159f / 180.0f)-0.25);

    glVertex3f(10.0f , 0.0f, 0.0f);
    glVertex3f(-10.0f , 0.0f, 0.0f);

    glVertex3f(0.0f , 10.0f, 0.0f);
    glVertex3f(0.0f , -10.0f, 0.0f);

    glVertex3f(0.0f , 0.0f, 10.0f);
    glVertex3f(0.0f , 0.0f, -10.0f);

    glEnd();
    glPopMatrix();

    // glPushMatrix();

    glPushMatrix();
    // glTranslatef(4,radius*sin(angle_line* 3.14159f / 180.0f),0);
        // glPushMatrix();
        // glTranslatef(19, radius*sin(angle_line* 3.14159f / 180.0f), -10);
        // glLineWidth(2.0f);
        // glColor3f(1.0f, 0.0f, 0.0f); // Red color
        // glBegin(GL_LINE_STRIP);
        // for (float x = -10.0f; x <= 10.0f; x += 0.1f) {
        //     float y = sin(x);
        //     glVertex3f(x, y, 0.0f);
        // }
        // glEnd();

    /*glBegin(GL_LINE_STRIP);
    for (int i = 0; i < 36000; i++) {
        glVertex2f((6+i*0.01), radius*sin((angle_line* 3.14159f / 180.0f)-i*0.01));
    }
    glEnd();
      */  

glPopMatrix();
    


}


void updateScene() {
    // Update your scene here

    glutPostRedisplay(); // Request a redraw of the scene
}

// int main(int argc, char **argv) {
//     // ...

//     glutIdleFunc(updateScene); // Register the updateScene function as the idle function

//     // ...
// }


void display()
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW); 
    glLoadIdentity();    

    gluLookAt(base_camera.eye_vec.x,base_camera.eye_vec.y,base_camera.eye_vec.z,
            base_camera.eye_vec.x + base_camera.lookat_vec.x,base_camera.eye_vec.y + base_camera.lookat_vec.y,base_camera.eye_vec.z + base_camera.lookat_vec.z,
            base_camera.up_vec.x,base_camera.up_vec.y,base_camera.up_vec.z

    );

    // draw_axes();
    glPushMatrix();
    glTranslatef(0,0,-10);
    draw_circle_with_moving_radius(2.0, 5);
    glPopMatrix();
    // glScaled(1.5,1.5,1.5);
    // glRotated(rotation,0,1,0);

    // glPushMatrix();
    // drawoctahedron();
    // glPopMatrix();

    // glPushMatrix();
    // draw_sphere();
    // glPopMatrix();

    // glPushMatrix();
    // draw_cylinder_frame();
    // glPopMatrix();


    glutSwapBuffers();


}


void key_res(unsigned char key, int x , int y)
{
    if(key == 'a')
    {
        clockwiserotate_y();
    }
    else if(key == 'd')
    {
        anticlockwiserotate_y();
    }
    else if (key == ',')
    {
        transition_to_sphere();
    }
    else if(key == '.')
    {
        transition_to_octahedron();
    }
    else
    {
        base_camera.function_handler(key);
    }
    
    glutPostRedisplay();

}

void special_key_res(int key, int x , int y)
{
    base_camera.special_function_handler(key);

    glutPostRedisplay();
}

int main(int argc, char **argv)
{
    base_camera.Set_eye(2,-2,2);
    base_camera.Set_lookat(0,0,-10);
    base_camera.Set_up(0,1,0);
    base_camera.Set_right(1,0,0);
    // base_camera.Set_eye(2.962583, -2.767312, 2.941701);
    // base_camera.Set_lookat(-0.497674, 0.496355, -0.516514);
    // base_camera.Set_up(-0.346840, 0.309600, 0.611779);
    // base_camera.Set_right(0.662934, 0.497914, -0.047495);
    // base_camera.Set_movement(0.05);


    base_triangle_center.Set_val(0.0);
    base_triangle_centroid.Set_val(1.0/3.0);
    triangle_scale = 1.0;
    rotation = 0.0;

    sphere_scale = 0.0;
    sphere_translate.Set_val(0.0);
    sphere_translate.x = 1.0;

    Max_info_cylinder.x = sqrt(2.0); //height
    Max_info_cylinder.y = M_PI / 2.0 ; //theta
    Max_info_cylinder.z = 1 / Max_info_cylinder.x; // translation

    curr_info_cylinder.x = Max_info_cylinder.x;
    curr_info_cylinder.y = Max_info_cylinder.y;
    curr_info_cylinder.z = Max_info_cylinder.z;
    cylinder_scale = 0;



    glutInit(&argc, argv);
    glutInitWindowSize(800, 450); 
    glutInitWindowPosition(700,200);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
    glutCreateWindow("Task : Wheel"); 
    glutDisplayFunc(display); 
    glutReshapeFunc(reshape);
    glutKeyboardFunc(key_res);
    glutSpecialFunc(special_key_res);
    glutIdleFunc(updateScene); // Register the updateScene function as the idle function


    initGL();
    glutMainLoop();
    return 0;

}