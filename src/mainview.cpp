#include "mainview.h"

#include <QDateTime>
struct Vertex {
    float x;
    float y;
    float z;
    float red;
    float green;
    float blue;
};
QOpenGLShaderProgram prog;

/**
 * @brief MainView::MainView Constructs a new main view.
 *
 * @param parent Parent widget.
 */
MainView::MainView(QWidget *parent) : QOpenGLWidget(parent) {
  qDebug() << "MainView constructor";

  connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
}

/**
 * @brief MainView::~MainView
 *
 * Destructor of MainView
 * This is the last function called, before exit of the program
 * Use this to clean up your variables, buffers etc.
 *
 */
MainView::~MainView() {
  qDebug() << "MainView destructor";

  makeCurrent();
  glDeleteBuffers(1, &vbo); // Clean up VBO
  glDeleteVertexArrays(1, &vao); // Clean up VAO
}

// --- OpenGL initialization

/**
 * @brief MainView::initializeGL Called upon OpenGL initialization
 * Attaches a debugger and calls other init functions.
 */
void MainView::initializeGL() {
  qDebug() << ":: Initializing OpenGL";
  initializeOpenGLFunctions();

  connect(&debugLogger, SIGNAL(messageLogged(QOpenGLDebugMessage)), this,
          SLOT(onMessageLogged(QOpenGLDebugMessage)), Qt::DirectConnection);

  if (debugLogger.initialize()) {
    qDebug() << ":: Logging initialized";
    debugLogger.startLogging(QOpenGLDebugLogger::SynchronousLogging);
  }

  QString glVersion{reinterpret_cast<const char *>(glGetString(GL_VERSION))};
  qDebug() << ":: Using OpenGL" << qPrintable(glVersion);
  Vertex vertices[18]; // 6 vertices for the pyramid

  // Define vertices of the pyramid
  vertices[0] = {-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f}; // BLUE
  vertices[1] = {1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f};  // YELLOW
  vertices[2] = {0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f};  // PURPLE

  vertices[3] = {1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f};  // YELLOW
  vertices[4] = {1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f};   // GREEN
  vertices[5] = {0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f};  // PURPLE

  vertices[6] = {1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f};   // GREEN
  vertices[7] = {-1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f};  // RED
  vertices[8] = {0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f};  // PURPLE

  vertices[9] = {-1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f};  // RED
  vertices[10] = {-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f}; // BLUE
  vertices[11] = {0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f};  // PURPLE

  vertices[12] = {-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f}; // BLUE
  vertices[13] = {1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f};  // YELLOW
  vertices[14] = {-1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f};  // RED

  vertices[15] = {1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f};  // YELLOW
  vertices[16] = {1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f};   // GREEN
  vertices[17] = {-1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f};  // RED

  glGenBuffers(1, &vbo);
  glGenVertexArrays(1, &vao);



  prog.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader.glsl");

  prog.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader.glsl");


  prog.link();
  prog.bind();


  glBindVertexArray(vao); // Bind the VAO
  glBindBuffer(GL_ARRAY_BUFFER, vbo); // Bind the VBO
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Specify vertex attribute pointers
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, x)));
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, red)));
  glEnableVertexAttribArray(0); // Vertex position
  glEnableVertexAttribArray(1); // Vertex color


  // Unbind VBO and VAO
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  // // Enable depth buffer
  // glEnable(GL_DEPTH_TEST);

  // // Enable backface culling
  // glEnable(GL_CULL_FACE);

  // // Default is GL_LESS
  // glDepthFunc(GL_LEQUAL);

  // Set the color to be used by glClear. This is, effectively, the background
  // color.
glClearColor(0.37f, 0.42f, 0.45f, 0.0f);
  createShaderProgram();
}

/**
 * @brief MainView::createShaderProgram Creates a new shader program with a
 * vertex and fragment shader.
 */
void MainView::createShaderProgram() {
  shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                        ":/shaders/vertshader.glsl");
  shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                        ":/shaders/fragshader.glsl");
  shaderProgram.link();
}

/**
 * @brief MainView::paintGL Actual function used for drawing to the screen.
 *
 */
void MainView::paintGL() {
  // Clear the screen before rendering
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  shaderProgram.bind();

  // Draw here
  // Clear the color buffer
  glClear(GL_COLOR_BUFFER_BIT);

  // Bind the VAO
  glBindVertexArray(vao);

  // Draw the triangle
  glDrawArrays(GL_TRIANGLES, 0, 18);

  // Unbind the VAO
  glBindVertexArray(0);
  shaderProgram.release();
}

/**
 * @brief MainView::resizeGL Called upon resizing of the screen.
 *
 * @param newWidth The new width of the screen in pixels.
 * @param newHeight The new height of the screen in pixels.
 */
void MainView::resizeGL(int newWidth, int newHeight) {
  // TODO: Update projection to fit the new aspect ratio
  // Q_UNUSED(newWidth)
  // Q_UNUSED(newHeight)
  glViewport(0, 0, newWidth, newHeight);

}

/**
 * @brief MainView::setRotation Changes the rotation of the displayed objects.
 * @param rotateX Number of degrees to rotate around the x axis.
 * @param rotateY Number of degrees to rotate around the y axis.
 * @param rotateZ Number of degrees to rotate around the z axis.
 */
void MainView::setRotation(int rotateX, int rotateY, int rotateZ) {
  qDebug() << "Rotation changed to (" << rotateX << "," << rotateY << ","
           << rotateZ << ")";
  Q_UNIMPLEMENTED();
}

/**
 * @brief MainView::setScale Changes the scale of the displayed objects.
 * @param scale The new scale factor. A scale factor of 1.0 should scale the
 * mesh to its original size.
 */
void MainView::setScale(float scale) {
  qDebug() << "Scale changed to " << scale;
  Q_UNIMPLEMENTED();
}

/**
 * @brief MainView::onMessageLogged OpenGL logging function, do not change.
 *
 * @param Message The message to be logged.
 */
void MainView::onMessageLogged(QOpenGLDebugMessage Message) {
  qDebug() << " → Log:" << Message;
}
