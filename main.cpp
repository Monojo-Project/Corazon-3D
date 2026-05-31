#include <QApplication>
#include <QTimer>
#include <QKeyEvent>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <cstdio>
#include <vector>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    #include <QOpenGLWidget>
    #include <QOpenGLFunctions>
    class BaseGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    public:
        BaseGLWidget(QWidget *parent = nullptr) : QOpenGLWidget(parent) {}
    };
#else
    #include <QGLWidget>
    class BaseGLWidget : public QGLWidget {
    public:
        BaseGLWidget(QWidget *parent = nullptr) : QGLWidget(parent) {}
    };
#endif

struct Vertex {
    float x, y, z;
    float nx, ny, nz;
};

class CorazonWidget : public BaseGLWidget {
    Q_OBJECT

public:
    CorazonWidget(QWidget *parent = nullptr) : BaseGLWidget(parent), frame(0) {
        setWindowTitle("❤️ CORAZÓN 3D ROMÁNTICO ESPECTACULAR - F11 para pantalla completa");
        setFixedSize(1400, 900);
        
        generateHeartGeometry();
        
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, QOverload<>::of(&QWidget::update));
        timer->start(16);
    }

protected:
    void initializeGL() override {
        #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            initializeOpenGLFunctions();
        #endif
        
        // Fondo con gradiente oscuro y misterioso
        glClearColor(0.01f, 0.01f, 0.05f, 1.0f);
        
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        
        // Niebla para efecto cinematográfico
        glEnable(GL_FOG);
        GLfloat fogColor[] = { 0.02f, 0.02f, 0.08f, 1.0f };
        glFogfv(GL_FOG_COLOR, fogColor);
        glFogi(GL_FOG_MODE, GL_LINEAR);
        glFogf(GL_FOG_START, 5.0f);
        glFogf(GL_FOG_END, 25.0f);
        glFogf(GL_FOG_DENSITY, 0.05f);
        
        // Iluminación avanzada
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHT1);
        glEnable(GL_LIGHT2);
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        
        // Luz principal - roja cálida
        GLfloat light0_position[] = { 4.0f, 5.0f, 6.0f, 1.0f };
        GLfloat light0_ambient[] = { 0.4f, 0.2f, 0.3f, 1.0f };
        GLfloat light0_diffuse[] = { 1.0f, 0.7f, 0.6f, 1.0f };
        GLfloat light0_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        GLfloat light0_shine[] = { 128.0f };
        
        glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
        glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
        glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.05f);
        
        // Luz secundaria - azul frío para contraste
        GLfloat light1_position[] = { -5.0f, 3.0f, 4.0f, 1.0f };
        GLfloat light1_ambient[] = { 0.2f, 0.2f, 0.3f, 1.0f };
        GLfloat light1_diffuse[] = { 0.6f, 0.4f, 0.7f, 1.0f };
        GLfloat light1_specular[] = { 0.8f, 0.8f, 1.0f, 1.0f };
        
        glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
        glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
        glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
        glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.05f);
        
        // Luz trasera - halo dorado
        GLfloat light2_position[] = { 0.0f, -2.0f, -8.0f, 1.0f };
        GLfloat light2_ambient[] = { 0.2f, 0.15f, 0.1f, 1.0f };
        GLfloat light2_diffuse[] = { 0.8f, 0.7f, 0.3f, 1.0f };
        
        glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
        glLightfv(GL_LIGHT2, GL_AMBIENT, light2_ambient);
        glLightfv(GL_LIGHT2, GL_DIFFUSE, light2_diffuse);
        
        // Material ultra brillante
        GLfloat mat_specular[] = { 1.0f, 0.9f, 0.9f, 1.0f };
        GLfloat mat_shininess[] = { 200.0f };
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
        
        GLfloat lmodel_ambient[] = { 0.25f, 0.15f, 0.2f, 1.0f };
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
        
        glShadeModel(GL_SMOOTH);
    }

    void resizeGL(int w, int h) override {
        if (h == 0) h = 1;
        glViewport(0, 0, w, h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        float aspect = (float)w / (float)h;
        gluPerspective(50.0f, aspect, 0.1f, 100.0f);
        glMatrixMode(GL_MODELVIEW);
    }

    void paintGL() override {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        
        // Cámara cinematográfica que se mueve ligeramente
        float camSway = 0.3f * sinf(frame * 0.005f);
        float camBob = 0.2f * sinf(frame * 0.003f);
        glTranslatef(camSway, camBob - 0.5f, -16.0f);
        
        // Animaciones cinematográficas
        float rotationY = frame * 0.6f;
        float rotationX = 18.0f * sinf(frame * 0.0065f);
        float rotationZ = 5.0f * sinf(frame * 0.004f);
        float pulse = 1.0f + 0.35f * sinf(frame * 0.035f);
        
        glRotatef(rotationX, 1.0f, 0.0f, 0.0f);
        glRotatef(rotationY, 0.0f, 1.0f, 0.0f);
        glRotatef(rotationZ, 0.0f, 0.0f, 1.0f);
        glScalef(pulse, pulse, pulse);
        
        drawHeart();
        
        // Aura brillante alrededor del corazón
        drawHeartAura();
        
        // Resetear para partículas
        glLoadIdentity();
        glTranslatef(camSway, camBob - 0.5f, -16.0f);
        glRotatef(rotationX, 1.0f, 0.0f, 0.0f);
        glRotatef(rotationY, 0.0f, 1.0f, 0.0f);
        glRotatef(rotationZ, 0.0f, 0.0f, 1.0f);
        
        glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        
        drawInnerGlow();
        drawFloatingRosePetals();
        drawMagicParticles();
        drawStarBurst();
        drawTrails();
        
        glEnable(GL_LIGHTING);
        
        frame++;
    }

    void keyPressEvent(QKeyEvent *event) override {
        if (event->key() == Qt::Key_Escape) {
            if (isFullScreen()) {
                showNormal();
            } else {
                close();
            }
        } else if (event->key() == Qt::Key_F11) {
            if (isFullScreen()) {
                showNormal();
            } else {
                showFullScreen();
            }
        } else if (event->key() == Qt::Key_F && event->modifiers() & Qt::AltModifier) {
            // Alt+F también para pantalla completa
            if (isFullScreen()) {
                showNormal();
            } else {
                showFullScreen();
            }
        }
    }

private:
    void generateHeartGeometry() {
        vertices.clear();
        indices.clear();
        
        const int segments = 150;
        const int rings = 100;
        
        for (int i = 0; i <= segments; i++) {
            for (int j = 0; j <= rings; j++) {
                float theta = 2.0f * M_PI * i / segments;
                float z = (float)j / rings;
                
                float R = 0.32f * (1.0f - z * 0.2f);
                
                float sinTheta = sinf(theta);
                float cosTheta = cosf(theta);
                float sin2Theta = sinf(2.0f * theta);
                float cos2Theta = cosf(2.0f * theta);
                float sin3Theta = sinf(3.0f * theta);
                float cos3Theta = cosf(3.0f * theta);
                float cos4Theta = cosf(4.0f * theta);
                
                float x = R * 16.0f * sinTheta * sinTheta * sinTheta;
                float y = R * (13.0f * cosTheta - 5.0f * cos2Theta - 2.0f * cos3Theta - cos4Theta);
                float vz = z * 0.6f - 0.3f;
                
                float epsilon = 0.008f;
                float xp = R * 16.0f * sinf(theta + epsilon) * sinf(theta + epsilon) * sinf(theta + epsilon);
                float yp = R * (13.0f * cosf(theta + epsilon) - 5.0f * cosf(2.0f * (theta + epsilon)) - 
                           2.0f * cosf(3.0f * (theta + epsilon)) - cosf(4.0f * (theta + epsilon)));
                
                float nx = xp - x;
                float ny = yp - y;
                float nz = 0.15f;
                
                float len = sqrtf(nx*nx + ny*ny + nz*nz);
                if (len > 0.0001f) {
                    nx /= len;
                    ny /= len;
                    nz /= len;
                }
                
                Vertex v;
                v.x = x;
                v.y = y;
                v.z = vz;
                v.nx = nx;
                v.ny = ny;
                v.nz = nz;
                
                vertices.push_back(v);
            }
        }
        
        for (int i = 0; i < segments; i++) {
            for (int j = 0; j < rings; j++) {
                int a = i * (rings + 1) + j;
                int b = a + rings + 1;
                int c = a + 1;
                int d = b + 1;
                
                indices.push_back(a);
                indices.push_back(b);
                indices.push_back(c);
                
                indices.push_back(b);
                indices.push_back(d);
                indices.push_back(c);
            }
        }
    }

    void drawHeart() {
        glBegin(GL_TRIANGLES);
        
        for (size_t i = 0; i < indices.size(); i += 3) {
            int idx0 = indices[i];
            int idx1 = indices[i + 1];
            int idx2 = indices[i + 2];
            
            if (idx0 < (int)vertices.size() && idx1 < (int)vertices.size() && idx2 < (int)vertices.size()) {
                const Vertex &v0 = vertices[idx0];
                const Vertex &v1 = vertices[idx1];
                const Vertex &v2 = vertices[idx2];
                
                // Color dinámico: rojo intenso con pulsación
                float pulse = 0.3f + 0.2f * sinf(frame * 0.04f);
                float colorVar = 0.2f * sinf(frame * 0.02f + idx0 * 0.01f);
                
                glColor4f(1.0f, 0.05f + pulse * 0.3f + colorVar * 0.1f, 0.35f + colorVar * 0.15f, 1.0f);
                
                glNormal3f(v0.nx, v0.ny, v0.nz);
                glVertex3f(v0.x, v0.y, v0.z);
                glNormal3f(v1.nx, v1.ny, v1.nz);
                glVertex3f(v1.x, v1.y, v1.z);
                glNormal3f(v2.nx, v2.ny, v2.nz);
                glVertex3f(v2.x, v2.y, v2.z);
            }
        }
        
        glEnd();
    }

    void drawHeartAura() {
        glDisable(GL_LIGHTING);
        glColor4f(1.0f, 0.3f, 0.5f, 0.4f);
        
        for (int layer = 0; layer < 3; layer++) {
            float scale = 1.05f + layer * 0.08f;
            float alpha = 0.3f / (layer + 1);
            glColor4f(1.0f, 0.4f - layer * 0.1f, 0.5f - layer * 0.1f, alpha);
            
            glPushMatrix();
            glScalef(scale, scale, scale);
            
            glBegin(GL_TRIANGLES);
            for (size_t i = 0; i < indices.size(); i += 3) {
                int idx0 = indices[i];
                int idx1 = indices[i + 1];
                int idx2 = indices[i + 2];
                
                if (idx0 < (int)vertices.size() && idx1 < (int)vertices.size() && idx2 < (int)vertices.size()) {
                    const Vertex &v0 = vertices[idx0];
                    const Vertex &v1 = vertices[idx1];
                    const Vertex &v2 = vertices[idx2];
                    
                    glVertex3f(v0.x, v0.y, v0.z);
                    glVertex3f(v1.x, v1.y, v1.z);
                    glVertex3f(v2.x, v2.y, v2.z);
                }
            }
            glEnd();
            
            glPopMatrix();
        }
        
        glEnable(GL_LIGHTING);
    }

    void drawInnerGlow() {
        glPointSize(20.0f);
        glBegin(GL_POINTS);
        
        int numGlows = 15;
        for (int i = 0; i < numGlows; i++) {
            float angle = (float)i / numGlows * 2.0f * M_PI;
            float r = 0.4f + 0.2f * sinf(frame * 0.03f);
            float x = r * cosf(angle);
            float y = r * sinf(angle);
            float z = 0.1f * sinf(angle + frame * 0.02f);
            
            float brightness = 0.5f + 0.5f * sinf(frame * 0.05f + i);
            glColor4f(1.0f, 0.6f + brightness * 0.3f, 0.5f + brightness * 0.2f, 0.7f * brightness);
            
            glVertex3f(x, y, z);
        }
        glEnd();
    }

    void drawFloatingRosePetals() {
        glPointSize(14.0f);
        glBegin(GL_POINTS);
        
        int numPetals = 40;
        for (int i = 0; i < numPetals; i++) {
            float angle = (float)i / numPetals * 2.0f * M_PI + frame * 0.04f;
            float time = frame * 0.01f + i * 0.5f;
            float radius = 1.2f + 0.6f * sinf(time);
            float x = radius * cosf(angle);
            float y = radius * sinf(angle) + 0.3f * sinf(time * 0.7f);
            float z = 1.0f * sinf(angle + time) + 0.5f * cosf(time * 0.3f);
            
            float brightness = 0.4f + 0.6f * sinf(frame * 0.035f + i * 0.2f);
            glColor4f(1.0f, 0.2f + brightness * 0.3f, 0.4f + brightness * 0.2f, 0.75f * brightness);
            
            glVertex3f(x, y, z);
        }
        glEnd();
    }

    void drawMagicParticles() {
        glPointSize(8.0f);
        glBegin(GL_POINTS);
        
        int numMagic = 80;
        for (int i = 0; i < numMagic; i++) {
            float angle = (float)i / numMagic * 2.0f * M_PI + frame * 0.055f;
            float radius = 0.8f + 0.7f * sinf(frame * 0.025f + i * 0.1f);
            float x = radius * cosf(angle) + 0.2f * sinf(frame * 0.015f + i);
            float y = radius * sinf(angle) + 0.4f * cosf(frame * 0.015f + i);
            float z = 0.5f * sinf(angle + frame * 0.03f);
            
            float brightness = 0.3f + 0.7f * sinf(frame * 0.045f + i * 0.3f);
            glColor4f(1.0f, 0.95f, 0.5f + brightness * 0.3f, 0.8f * brightness);
            
            glVertex3f(x, y, z);
        }
        glEnd();
    }

    void drawStarBurst() {
        glPointSize(6.0f);
        glBegin(GL_POINTS);
        
        int numStars = 120;
        for (int i = 0; i < numStars; i++) {
            float angle = (float)i / numStars * 2.0f * M_PI;
            float time = frame * 0.02f;
            float radius = 0.3f + 2.0f * sinf(time + i * 0.05f) * cosf(time * 0.5f);
            float x = radius * cosf(angle);
            float y = radius * sinf(angle);
            float z = 0.4f * sinf(angle + time);
            
            float brightness = 0.5f + 0.5f * sinf(frame * 0.06f + i);
            glColor4f(1.0f, 1.0f, 0.7f + brightness * 0.2f, 0.6f * brightness);
            
            glVertex3f(x, y, z);
        }
        glEnd();
    }

    void drawTrails() {
        glLineWidth(1.5f);
        glBegin(GL_LINE_STRIP);
        
        int numTrails = 60;
        for (int i = 0; i < numTrails; i++) {
            float t = (float)i / numTrails * 2.0f * M_PI + frame * 0.03f;
            float radius = 1.5f;
            float x = radius * cosf(t);
            float y = radius * sinf(t);
            float z = 0.6f * sinf(t + frame * 0.02f);
            
            float brightness = (float)i / numTrails;
            glColor4f(1.0f, 0.6f - brightness * 0.3f, 0.4f + brightness * 0.2f, brightness * 0.5f);
            
            glVertex3f(x, y, z);
        }
        glEnd();
        glLineWidth(1.0f);
    }

    std::vector<Vertex> vertices;
    std::vector<int> indices;
    QTimer *timer;
    int frame;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    printf("\n");
    printf("╔══════════════════════════════════════════════════╗\n");
    printf("║                                                  ║\n");
    printf("║     ❤️  CORAZÓN 3D ROMÁNTICO ESPECTACULAR  ❤️    ║\n");
    printf("║                                                  ║\n");
    printf("║  CONTROLES:                                      ║\n");
    printf("║  • F11 - Pantalla Completa / Ventana            ║\n");
    printf("║  • ESC - Salir (o volver a ventana si fullscreen)║\n");
    printf("║  • Alt+F - Pantalla Completa (alternativa)      ║\n");
    printf("║                                                  ║\n");
    printf("║  Un screensaver mágico con efectos cinematográ-  ║\n");
    printf("║  ficos, luces dinámicas, partículas flotantes,   ║\n");
    printf("║  y animaciones suaves para momentos románticos   ║\n");
    printf("║                                                  ║\n");
    printf("╚══════════════════════════════════════════════════╝\n");
    printf("\n");
    
    CorazonWidget widget;
    widget.show();
    
    return app.exec();
}

#include "main.moc"
