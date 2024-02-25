#include "mainview.h"
#include "model.h"

#include <QDateTime>



 float startScale = 1.0f;
//QOpenGLShaderProgram shaderProgram;

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
  // Enable depth buffer
  glEnable(GL_DEPTH_TEST);

  // Enable backface culling
  glEnable(GL_CULL_FACE);


  // Default is GL_LESS
  glDepthFunc(GL_LEQUAL);
  // glBindBuffer(GL_ARRAY_BUFFER, 0);
  // glBindVertexArray(0);

  // Set the color to be used by glClear. This is, effectively, the background
  // color.
  glClearColor(0.37f, 0.42f, 0.45f, 0.0f);
  createShaderProgram();
  QString glVersion{reinterpret_cast<const char *>(glGetString(GL_VERSION))};
  qDebug() << ":: Using OpenGL" << qPrintable(glVersion);
  Vertex vertices[18]; // 6 vertices for the pyramid
  vertices[10] = {-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f}; // BLUE
  vertices[11] = {0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f};  // PURPLE
  vertices[9] = {1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f};  // YELLOW

  vertices[6] = {1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f};   // GREEN
  vertices[7] = {1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f};  // YELLOW
  vertices[8] = {0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f};  // PURPLE

  vertices[0] = {0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f};  // PURPLE
  vertices[1] = {-1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f};  // RED
  vertices[2] = {1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f};   // GREEN


  vertices[3] = {-1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f};  // RED
  vertices[4] = {0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f};  // PURPLE
  vertices[5] = {-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f}; // BLUE

  vertices[15] = {-1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f};  // RED
  vertices[16] = {-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f}; // BLUE
  vertices[17] = {1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f};  // YELLOW

  vertices[12] = {1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f};   // GREEN
  vertices[13] = {-1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f};  // RED
  vertices[14] = {1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f};  // YELLOW

  glGenBuffers(1, &vbo);
  glGenVertexArrays(1, &vao);

  glBindVertexArray(vao); // Bind the VAO
  glBindBuffer(GL_ARRAY_BUFFER, vbo); // Bind the VBO
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Specify vertex attribute pointers
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, x)));
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, red)));
  glEnableVertexAttribArray(0); // Vertex position
  glEnableVertexAttribArray(1); // Vertex color

  transformationModel.setToIdentity();
  transformationModel.translate(-2.0f, 0.0f, -6.0f);

  //transformationProjection.setToIdentity();



  Model knotModel(":/models/knot.obj");
  QVector<QVector3D> meshCoords = knotModel.getMeshCoords();
  Vertex verticesModel[meshCoords.size()];

  // Construct an array of Vertex objects
  for (int i =0 ; i< meshCoords.size(); i++) {

      verticesModel[i].x = meshCoords[i].x();
      verticesModel[i].y = meshCoords[i].y();
      verticesModel[i].z = meshCoords[i].z();

      verticesModel[i].red = abs(meshCoords[i].x());
      verticesModel[i].green = abs(meshCoords[i].y());
      verticesModel[i].blue = abs(meshCoords[i].z());

  }

  glGenBuffers(1, &meshVbo);

  glGenVertexArrays(1, &meshVao);

  // Bind VAO
  glBindVertexArray(meshVao);

  // Bind VBO and fill it with vertex data
  glBindBuffer(GL_ARRAY_BUFFER, meshVbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(verticesModel), verticesModel, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0); // Vertex position
  glEnableVertexAttribArray(1); // Vertex color
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, x)));
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, red)));


  transformationModelMatrix.setToIdentity();
  transformationModelMatrix.translate(2.0f, 0.0f, -6.0f);
  transformationProjection.setToIdentity();

  float aspectRatio = static_cast<float>(width()) / height();
  transformationProjection.perspective(60.0f, aspectRatio, 0.2f, 20.0f);
  nrOfTriangles = sizeof(verticesModel);



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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderProgram.bind();

    glBindVertexArray(vao);

    int temp = shaderProgram.uniformLocation("transformationModel");
    shaderProgram.setUniformValue(temp, transformationModel);
    int temp2 = shaderProgram.uniformLocation("transformationProjection");
    shaderProgram.setUniformValue(temp2, transformationProjection);

    // Draw the pyramid
    glDrawArrays(GL_TRIANGLES, 0, 18);



    glBindVertexArray(meshVao);

    int temp3 = shaderProgram.uniformLocation("transformationModelMatrix");
    shaderProgram.setUniformValue(temp3, transformationModelMatrix);

    glDrawArrays(GL_TRIANGLES, 0, nrOfTriangles);

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
    float aspectRatio = static_cast<float>(newWidth) / newHeight;

    // Update perspective projection
    transformationProjection.setToIdentity();
    transformationProjection.perspective(60.0f, aspectRatio, 0.2f, 20.0f);

}

/**
 * @brief MainView::setRotation Changes the rotation of the displayed objects.
 * @param rotateX Number of degrees to rotate around the x axis.
 * @param rotateY Number of degrees to rotate around the y axis.
 * @param rotateZ Number of degrees to rotate around the z axis.
 */
void MainView::setRotation(int rotateX, int rotateY, int rotateZ) {
    qDebug() << "Rotation changed to (" << rotateX << "," << rotateY << "," << rotateZ << ")";

    angleX = rotateX;
    angleY = rotateY;
    angleZ = rotateZ;


    transformationModel.setToIdentity();
    transformationModel.translate(-2.0f, 0.0f, -6.0f);
    transformationModel.rotate(angleX, QVector3D(1.0f, 0.0f, 0.0f)); // Rotate around x-axis
    transformationModel.rotate(angleY, QVector3D(0.0f, 1.0f, 0.0f)); // Rotate around y-axis
    transformationModel.rotate(angleZ, QVector3D(0.0f, 0.0f, 1.0f)); // Rotate around z-axis
    transformationModel.scale(startScale);

    transformationModelMatrix.setToIdentity();
    transformationModelMatrix.translate(2.0f, 0.0f, -6.0f);

    transformationModelMatrix.rotate(angleX, QVector3D(1.0f, 0.0f, 0.0f)); // Rotate around x-axis
    transformationModelMatrix.rotate(angleY, QVector3D(0.0f, 1.0f, 0.0f)); // Rotate around y-axis
    transformationModelMatrix.rotate(angleZ, QVector3D(0.0f, 0.0f, 1.0f)); // Rotate around z-axis
    transformationModelMatrix.scale(startScale);


    update();

}

/**
 * @brief MainView::setScale Changes the scale of the displayed objects.
 * @param scale The new scale factor. A scale factor of 1.0 should scale the
 * mesh to its original size.
 */
void MainView::setScale(float scale) {
    qDebug() << "Scale changed to " << scale;
    startScale = scale;

    transformationModel.setToIdentity();
    transformationModel.translate(-2.0f, 0.0f, -6.0f);
    transformationModel.rotate(angleX, QVector3D(1.0f, 0.0f, 0.0f)); // Rotate around x-axis
    transformationModel.rotate(angleY, QVector3D(0.0f, 1.0f, 0.0f)); // Rotate around y-axis
    transformationModel.rotate(angleZ, QVector3D(0.0f, 0.0f, 1.0f)); // Rotate around z-axis
    transformationModel.scale(startScale);


    transformationModelMatrix.setToIdentity();
    transformationModelMatrix.translate(2.0f, 0.0f, -6.0f);
    transformationModelMatrix.rotate(angleX, QVector3D(1.0f, 0.0f, 0.0f)); // Rotate around x-axis
    transformationModelMatrix.rotate(angleY, QVector3D(0.0f, 1.0f, 0.0f)); // Rotate around y-axis
    transformationModelMatrix.rotate(angleZ, QVector3D(0.0f, 0.0f, 1.0f)); // Rotate around z-axis
    transformationModelMatrix.scale(startScale);

    update();

}

/**
 * @brief MainView::onMessageLogged OpenGL logging function, do not change.
 *
 * @param Message The message to be logged.
 */
void MainView::onMessageLogged(QOpenGLDebugMessage Message) {
  qDebug() << " → Log:" << Message;
}
