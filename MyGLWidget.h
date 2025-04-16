#include "LL2GLWidget.h"

#include <vector>

#include <QTimer>

class MyGLWidget : public LL2GLWidget {
  Q_OBJECT

  public:
    MyGLWidget(QWidget *parent);
    ~MyGLWidget();

  protected:
  
    virtual void keyPressEvent (QKeyEvent *event);

    // per al moviment automàtic
    QTimer timer;

    // // initializeGL - Aqui incluim les inicialitzacions del context gràfic.
    virtual void initializeGL ();
    // // paintGL - Mètode cridat cada cop que cal refrescar la finestra.
    // // Tot el que es dibuixa es dibuixa aqui.
    virtual void paintGL() override;
    // // resizeGL - És cridat quan canvia la mida del widget
    virtual void resizeGL (int width, int height);
    // // keyPressEvent - És cridat quan es prem una tecla
    // virtual void keyPressEvent (QKeyEvent *event);
    // // mouse{Press/Release/Move}Event - Són cridades quan es realitza l'event
    // // corresponent de ratolí
    // virtual void mousePressEvent (QMouseEvent *event) override;
    // virtual void mouseReleaseEvent (QMouseEvent *event) override;
    virtual void mouseMoveEvent (QMouseEvent *event) override;

    // // iniEscena - Aqui incluim les inicialitzacions de l'escena
    virtual void iniEscena () override;
    // // iniCamera - Aqui incluim les inicialitzacions de la camera
    virtual void iniCamera () override;    
    
    // // viewTransform i projecTransform - Es fan servir per construir i enviar als shader les matrius de càmera (View i Projection respectivament).
    virtual void viewTransform () override;
    virtual void projectTransform () override;
    
    virtual void modelTransformRick (glm::vec3 pos) override;
    virtual void modelTransformVideoCam () override;
    virtual void modelTransformTerra () override;
    // virtual void modelTransformCub ();

    //  // creaBuffersTerra - Inicialitzem el VAO i els VBO d'un model fet a mà que representa un terra
    //  void creaBuffersTerra ();
    //  // creaBuffersCub - Inicialitzem el VAO i els VBO d'un model fet a mà que representa un cub
    //  void creaBuffersCub ();
    //  // creaBuffersModels - Carreguem els fitxers obj i fem la inicialització dels diferents VAOS i VBOs
    //  void creaBuffersModels ();
    //  // carregaShaders - Carreguem els shaders, els compilem i els linkem. També busquem els uniform locations que fem servir.
    void carregaShaders2 () ;

    void modelTransformParet1 (glm::vec3 pos);
    void modelTransformParet2 (glm::vec3 pos);
    void modelTransformPorta (glm::vec3 pos);

    void iniCameraVideoCam();

    glm::vec3 posParet1 = glm::vec3(0,0,-2.5);
    glm::vec3 posParet2 = glm::vec3(0,0,2.5);
    glm::vec3 posPorta = glm::vec3(0,0,0);

    float posRickX;
    float dirRick;
    float posPortaZ = 0.0f;

    bool useVideoCam = false;
 
    struct Camera {
      float FOV;
      float FOVini;
      float raw;
      float zNear;
      float zFar;
      float d;
      glm::vec3 OBS;
      glm::vec3 VRP;
      glm::vec3 UP;
      float psi;    // Ψ
      float theta;  // θ
      float phi;    // φ
    };

    Camera camera;
    Camera cameraVideoCam;

    struct Escena {
      glm::vec3 minim;
      glm::vec3 maxim;
      glm::vec3 centre;
      
      float radi;
    };

    Escena escena;

  public slots:
    void updatePosition();

  private:
  
    int printOglError(const char file[], int line, const char func[]);
   
    // Aquí tots els atributs privats que necessitis

    GLuint portaLoc;    
};
