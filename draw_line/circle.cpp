#include <GL/gl.h>
#include <GL/freeglut.h>
#include <math.h>

const float PI =3.141592653;
void init()
{
    glClearColor(1,1,1,0);
    glShadeModel(GLU_FLAT);
    //glPointSize( 6.0 );
}
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    // drawing

    float x,y;
    glColor3f(0,0,1);



    for ( float angle = 0; angle <= 2*PI; angle+=0.01)
    {
        x =( 0.3) * cos (angle);
        y =( 0.3) * sin (angle);
        glBegin(GL_POINTS);
        glVertex2f(x,y);
        glEnd();

    }




    glutSwapBuffers();
}
int main(int argc , char * argv[])
{
    glutInit(& argc , argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    // Window of drawing
    glutInitWindowSize(500,500);
    glutInitWindowPosition(100 , 100);
    glutCreateWindow("my first program");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
}
