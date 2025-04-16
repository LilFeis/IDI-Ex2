#include "MyGLWidget.h"
#include <iostream>
#include <stdio.h>

#define printOpenGLError() printOglError(__FILE__, __LINE__)
#define CHECK() printOglError(__FILE__, __LINE__,__FUNCTION__)
#define DEBUG() std::cout << __FILE__ << " " << __LINE__ << " " << __FUNCTION__ << std::endl;

MyGLWidget::MyGLWidget(QWidget *parent=0) : LL2GLWidget(parent) 
{
  connect(&timer, SIGNAL(timeout()), this, SLOT(updatePosition()));
  timer.setInterval(100);
}

int MyGLWidget::printOglError(const char file[], int line, const char func[]) 
{
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    const char * error = 0;
    switch (glErr)
    {
        case 0x0500:
            error = "GL_INVALID_ENUM";
            break;
        case 0x501:
            error = "GL_INVALID_VALUE";
            break;
        case 0x502: 
            error = "GL_INVALID_OPERATION";
            break;
        case 0x503:
            error = "GL_STACK_OVERFLOW";
            break;
        case 0x504:
            error = "GL_STACK_UNDERFLOW";
            break;
        case 0x505:
            error = "GL_OUT_OF_MEMORY";
            break;
        default:
            error = "unknown error!";
    }
    if (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s function: %s\n",
                             file, line, error, func);
        retCode = 1;
    }
    return retCode;
}

MyGLWidget::~MyGLWidget()
{
}

void MyGLWidget::resizeGL (int width, int height) {
  LL2GLWidget::resizeGL(width, height);

  camera.raw = ra;   
  cameraVideoCam.raw = ra;
     
  if (ra < 1) {
    camera.FOV = 2*atan(tan(camera.FOVini/2)/ra);
    cameraVideoCam.FOV = 2*atan(tan(cameraVideoCam.FOVini/2)/ra);
  }

}

void MyGLWidget::keyPressEvent(QKeyEvent* event) 
{
  makeCurrent();
  switch (event->key()) {
    case Qt::Key_Up: { 
      if (posRickX < 5.5f) {
        posRickX += 0.25;
        if (dirRick < 0.0f) dirRick = 1.0f; 
      }
      break;
    }
    case Qt::Key_Down: { 
      if (posRickX > -5.5f) {
        posRickX -= 0.25;
        if (dirRick > 0.0f) dirRick = -1.0f; 
      } 
      break;
        }           
    case Qt::Key_C: { 
        useVideoCam = !useVideoCam;
        projectTransform();
      break;
        }           
    case Qt::Key_A: { 
      if (timer.isActive()) {
        timer.stop(); // Atura el temporitzador
      } else {
        timer.start(); // Engega el temporitzador
      }
      break;
        }  
    case Qt::Key_R: { 
      camera.psi = -M_PI/4.0f;  
      camera.theta = M_PI/4.0f; 
      camera.phi = 0;
      useVideoCam = false;
      posRickX = -5.5f;
      dirRick = 1.0f;
      posPortaZ = 0.0f;
      timer.stop();
      projectTransform();
      break;
        }  
    default: event->ignore(); break;
  }

  if (abs(posRickX) <= 2.0f) posPortaZ = 2 - abs(posRickX);

  viewTransform();

  update();
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *e)
{
  makeCurrent();
  if (DoingInteractive == ROTATE && !useVideoCam) {
    // Calcula el desplaçament del ratolí
    float deltaX = e->x() - xClick;
    float deltaY = e->y() - yClick;

    // Actualitza els angles d’Euler de la càmera
    camera.psi += deltaX * factorAngleX;   // Inverteix el moviment horitzontal
    camera.theta += deltaY * factorAngleY; // Inverteix el moviment vertical

    //Actualitza la transformació de la vista
    viewTransform();
  }

  // Actualitza la posició del clic
  xClick = e->x();
  yClick = e->y();

  update ();
}

void MyGLWidget::initializeGL () {
  camera.psi = -M_PI/4.0f;  
  camera.theta = M_PI/4.0f; 
  camera.phi = 0;
  posRickX = -5.0f;
  dirRick = 1.0f;
  LL2GLWidget::initializeGL();
  iniCameraVideoCam();
  carregaShaders2();
}

void MyGLWidget::iniEscena () {
  escena.minim = glm::vec3(-6.0f, 0.0f, -4.0f);     
  escena.maxim = glm::vec3(6.0f, 2.0f, 4.0f);  
  escena.centre = (escena.maxim+escena.minim)/2.0f;   // (min+max)/2.f

  escena.radi = glm::distance(escena.centre, escena.maxim);
}

void MyGLWidget::iniCamera () {
  camera.d =  2*escena.radi;  // d = 2*R
  camera.FOVini = 2*(asin(float(escena.radi/(camera.d)))); // FOV = 2*alfa  //sin(alfa) = R/2*R  
  camera.FOV   = camera.FOVini;
  camera.raw   = 1.0f;

  camera.zNear = escena.radi;
  camera.zFar  = 3*escena.radi;

  camera.OBS   = escena.centre + camera.d*glm::vec3(0.0f, 0.0f, 1.0f);  // VRP + d*v  (v vec normalitzat en la direcció)
  camera.VRP   = escena.centre;
  camera.UP    = glm::vec3(0, 1, 0);

  projectTransform();
  viewTransform();
}

void MyGLWidget::iniCameraVideoCam() {
  cameraVideoCam.OBS   = glm::vec3(0.0f, 2.25f, -1.0f); 
  cameraVideoCam.VRP   = glm::vec3(posRickX, 0.0f, 0.0f);
  cameraVideoCam.UP    = glm::vec3(0, 1, 0);

  cameraVideoCam.d = glm::distance(cameraVideoCam.OBS, cameraVideoCam.VRP);
  cameraVideoCam.FOVini = M_PI/2.0f;
  cameraVideoCam.FOV   = cameraVideoCam.FOVini;
  cameraVideoCam.raw   = 1.0f;

  cameraVideoCam.zNear = 0.1f;
  cameraVideoCam.zFar  = 12.0f;
}
 
void MyGLWidget::updatePosition()
{
  // mètode que implementa el que ha de fer l'aplicació cada cop que el timer
 // fa timeout.
  
 // Actualitza la posició de Rick
  posRickX += 0.25f * dirRick;

  // Comprovem si Rick ha arribat al final del terra
  if (posRickX >= 5.5f) {
    posRickX = 5.5f;
    dirRick = -1.0f; // Canvia de direcció
  } else if (posRickX <= -5.5f) {
    posRickX = -5.5f;
    dirRick = 1.0f; // Canvia de direcció
  }

  // Actualitza la posició de la porta
  if (abs(posRickX) <= 2.0f) {
    posPortaZ = 2.0f - abs(posRickX);
  } else {
    posPortaZ = 0.0f;
  }

  // if (useVideoCam) {
  //   viewTransform(); 
  //   //std::cerr << "Entrant al bloc if (useVideoCam)" << std::endl;
  // }

  // Força l'actualització de la vista  
  update();
}

void MyGLWidget::paintGL () {
  // descomentar per canviar paràmetres
  // glViewport (0, 0, ample, alt);

  // Esborrem el frame-buffer
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glUniform1i(portaLoc, 0);

  // Rick
  glBindVertexArray (VAO_Rick);
  modelTransformRick (posRick);
  glDrawArrays(GL_TRIANGLES, 0, rick.faces().size()*3);

  // Video Camera
  if (!useVideoCam) {
    glBindVertexArray (VAO_VideoCam);
    modelTransformVideoCam ();
    glDrawArrays(GL_TRIANGLES, 0, videoCam.faces().size()*3);
  }
  
  // Paret1
  glBindVertexArray (VAO_Cub);
  modelTransformParet1 (posParet1);
  glDrawArrays(GL_TRIANGLES, 0, 36);

  // Paret2
  glBindVertexArray (VAO_Cub);
  modelTransformParet2 (posParet2);
  glDrawArrays(GL_TRIANGLES, 0, 36);

  // Porta
  glUniform1i(portaLoc, 1);
  glBindVertexArray (VAO_Cub);
  modelTransformPorta (posPorta);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glUniform1i(portaLoc, 0);

  // Terra
  glBindVertexArray (VAO_Terra);
  modelTransformTerra ();
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  if (useVideoCam) viewTransform();

  glBindVertexArray (0);
}

void MyGLWidget::viewTransform () {
  glm::mat4 View(1.0f);
  
  if (!useVideoCam) {
    View = glm::translate(View, glm::vec3(0.0f, 0.0f, -camera.d));
    View = glm::rotate(View, camera.theta, glm::vec3(1.0, 0.0, 0.0));
    View = glm::rotate(View, camera.psi, glm::vec3(0.0, 1.0, 0.0));
    View = glm::translate(View, glm::vec3(-escena.centre[0], -escena.centre[1], -escena.centre[2]));
  } 
  else {
    View = glm::lookAt(cameraVideoCam.OBS, glm::vec3(posRickX, 0.0f, 0.0f), cameraVideoCam.UP);
  }
  
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &View[0][0]);
}

void MyGLWidget::projectTransform () {
  glm::mat4 Proj;
  if (!useVideoCam) Proj = glm::perspective(camera.FOV, camera.raw, camera.zNear, camera.zFar);
  else Proj = glm::perspective(cameraVideoCam.FOV, cameraVideoCam.raw, cameraVideoCam.zNear, cameraVideoCam.zFar);
  glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Proj[0][0]);
}
 
void MyGLWidget::modelTransformTerra () {
  glm::mat4 TG(1.0f);
  TG = glm::scale(TG, glm::vec3(1.5f, 1.0f, 1.0f));
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformRick (glm::vec3 pos) {
  glm::mat4 TG(1.0f);
  float alçadaRick = 2.0f/escalaRick;
  float escala = 1.5f/alçadaRick;
  TG = glm::translate(TG, glm::vec3(posRickX, 0.0f, 0.0f));
  TG = glm::rotate(TG, glm::radians(dirRick*90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  TG = glm::scale(TG, glm::vec3(escala, escala, escala));
  TG = glm::translate(TG, -centreBaseRick);
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformVideoCam () {
  glm::mat4 TG(1.0f);
  float alçadaVideoCam = 1.0f/escalaVideoCam;
  float escala = 0.4f/alçadaVideoCam;
  TG = glm::translate(TG, glm::vec3(0.0f, 2.0f, -1.0f));
  TG = glm::rotate(TG, glm::radians(-80.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  TG = glm::rotate(TG, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  TG = glm::scale(TG, glm::vec3(escala, escala, escala));
  TG = glm::translate(TG, -centreBaseVideoCam);
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformParet1 (glm::vec3 pos) {
  glm::mat4 TG(1.0f);
  TG = glm::translate(TG, pos);
  TG = glm::scale(TG, glm::vec3(0.5f, 2.0f, 3.0f));
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformParet2 (glm::vec3 pos) {
  glm::mat4 TG(1.0f);
  TG = glm::translate(TG, pos);
  TG = glm::scale(TG, glm::vec3(0.5f, 2.0f, 3.0f));
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformPorta (glm::vec3 pos) {
  glm::mat4 TG(1.0f);
  TG = glm::translate(TG, glm ::vec3(pos[0], pos[1], posPortaZ));
  TG = glm::scale(TG, glm::vec3(0.25f, 1.9f, 2.0f));
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::carregaShaders2 () {
  // Identificadors dels uniform locations
  portaLoc = glGetUniformLocation(program->programId(), "Porta");
}
