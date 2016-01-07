#ifdef _H_AVR_OBJECT
#else
#define _H_AVR_OBJECT

#include "avr_gl.hh"

#include <vector>

class AVRObject
{
protected:
    AVRObject();
    ~AVRObject();
    // Triangle vertices
    std::vector<GLfloat> vertices;    

    // OpenGL handles
    GLuint vertexArrayObject;    
    GLuint vertexBuffer;
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint shaderProgram;


    void push_triangle(glm::vec3 &p1, glm::vec3 &p2, glm::vec3 &p3, glm::vec4 &col, GLfloat scale);
    void sendMatrix(const char * name, glm::mat4 &M);
    GLuint compileShader(const char * filename, GLuint shaderType);

public:
    bool enabled;
    
	void compileVertexShader(const char * filename);
	void compileFragmentShader(const char * filename);
    void createProgram(const char * vertexShaderFilename, const char * fragmentShaderFilename);

    virtual void useProgram();
    virtual void draw(glm::mat4 projection)=0;
};

void checkGLerror(const char * where);    



#endif