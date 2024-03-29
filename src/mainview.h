#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QKeyEvent>
#include <QMouseEvent>
#include <QOpenGLDebugLogger>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QTimer>
#include <QVector3D>


/**
 * @brief The MainView class is resonsible for the actual content of the main
 * window.
 */
class MainView : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
  Q_OBJECT

 public:
  MainView(QWidget *parent = nullptr);
  ~MainView() override;

  // Functions for widget input events
  void setRotation(int rotateX, int rotateY, int rotateZ);
  void setScale(float scale);

 protected:
  void initializeGL() override;
  void resizeGL(int newWidth, int newHeight) override;
  void paintGL() override;

  // Functions for keyboard input events
  void keyPressEvent(QKeyEvent *ev) override;
  void keyReleaseEvent(QKeyEvent *ev) override;

  // Function for mouse input events
  void mouseDoubleClickEvent(QMouseEvent *ev) override;
  void mouseMoveEvent(QMouseEvent *ev) override;
  void mousePressEvent(QMouseEvent *ev) override;
  void mouseReleaseEvent(QMouseEvent *ev) override;
  void wheelEvent(QWheelEvent *ev) override;

 private slots:
  void onMessageLogged(QOpenGLDebugMessage Message);

 private:
     GLuint vbo;

 private:
     GLuint vao;
     GLuint meshVao, meshVbo;


 // private:


 private:
     QVector3D angle = QVector3D(0, 0, 0);
     int angleX= 0;
     int angleY= 0;
     int angleZ= 0;
     struct Vertex {
         float x;
         float y;
         float z;
         float red;
         float green;
         float blue;
     };





 private:
  QOpenGLDebugLogger debugLogger;
  QTimer timer;  // timer used for animation
  QOpenGLShaderProgram shaderProgram;
  QMatrix4x4 transformationModel;
  QMatrix4x4 transformationProjection;
  QMatrix4x4 transformationModelMatrix;
  int nrOfTriangles;

  void createShaderProgram();
};

#endif  // MAINVIEW_H
