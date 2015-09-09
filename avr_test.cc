#include "avr_test.hh"

AVRTest::AVRTest(float offset){

    glBindVertexArray(vertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);


    GLfloat triangleVertices[] = {
        0.0f+offset, 0.0f, 0.0f,
        1.0f+offset, 0.0f, 0.0f,
        0.0f+offset,  1.0f, 0.0f,
    };


    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, 
        GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);
    checkGLerror("Init Test Triangle");


}

void AVRTest::draw(glm::mat4 projection)
{
	glm::mat4 ID(1.0);
    useProgram();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    checkGLerror("Draw Triangle");

}
