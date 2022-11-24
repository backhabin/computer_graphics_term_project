#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include<stdbool.h>


GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void make_vertexShaders(); //���ؽ� ���̴� �����
void make_fragmentShaders(); //�����׸�Ʈ ���̴� �����
void make_shaderProgram(); //���̴� ���α׷� ����
void InitBuffer();
char* filetobuf(const char* file);
GLint height, width; //������â ũ�� 
GLuint shaderProgram; //���̴� ���α׷� �̸�
GLuint vertexShader;//���ؽ� ���̴� ��ü
GLuint fragmentShader;//�����׸�Ʈ ���̴� ��ü
GLuint VAO, VBO_pos, VBO_normal, VBO_color;
GLuint VAO_axis, VBO_axis;
bool loadOBJ(const char* path);



std::vector< glm::vec3 > out_vertices;
std::vector< glm::vec2 > out_uvs;
std::vector< glm::vec3 > out_normals; // Won't be used at the moment.
bool res = loadOBJ("cube.obj");
GLint object = out_vertices.size();



float axis[] = {
	0.0,0.0,0.0,
	2.0,0.0,0.0,
	-2.0,0.0,0.0,
	0.0,0.0,0.0,
	0.0,2.0,0.0,
	0.0,-2.0,0.0,
	0.0,0.0,0.0,
	0.0,0.0,2.0,
	0.0,0.0,-2.0

};

void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{ //--- ������ �����ϱ�
	height = 500;
	width = 500;
	glutInit(&argc, argv); // glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // ���÷��� ��� ����
	glutInitWindowPosition(100, 100); // �������� ��ġ ����
	glutInitWindowSize(width, height); // �������� ũ�� ����
	glutCreateWindow("cube");


	glewExperimental = GL_TRUE;
	glewInit();


	make_vertexShaders(); //--- ���ؽ� ���̴� �����
	make_fragmentShaders(); //--- �����׸�Ʈ ���̴� �����
	make_shaderProgram(); //--- ���̴� ���α׷� �����


	InitBuffer();



	glutDisplayFunc(drawScene); // ��� �Լ��� ����
	glutReshapeFunc(Reshape); // �ٽ� �׸��� �Լ� ����


	glutMainLoop(); // �̺�Ʈ ó�� ���� 


}




GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	glClearColor(0.0,1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




	glEnable(GL_DEPTH_TEST);





	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		//�𵨺�ȯ
	glm::mat4 Sz = glm::mat4(1.0);
	Sz = glm::scale(Sz, glm::vec3(0.5, 0.5, 0.5));
	int modeltrans = glGetUniformLocation(shaderProgram, "Smatrix");
	glUniformMatrix4fv(modeltrans, 1, GL_FALSE, glm::value_ptr(Sz));


	//�� ��ȯ
	glm::vec3 cameraPos = glm::vec3(-0.8f, 1.0f, 1.0f); //--- ī�޶� ��ġ
	glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- ī�޶� �ٶ󺸴� ����
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- ī�޶� ���� ����
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	unsigned int viewLocation = glGetUniformLocation(shaderProgram, "viewTransform"); //--- ���� ��ȯ ����
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);


	//������ȯ
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
	projection = glm::translate(projection, glm::vec3(0.0, 0.0, -3.0));//--- ������ �ణ �ڷ� �̷���
	unsigned int projectionLocation = glGetUniformLocation(shaderProgram, "projectionTransform"); //--- ���� ��ȯ �� ����
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);















	//ť��
	int vColorLocation = glGetUniformLocation(shaderProgram, "outColor");
	glUniform4f(vColorLocation, 1.0f, 1.0f, 1.0f, 1.0f);

	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(GL_TRIANGLES, 0, object);

	//��ǥ��
	/*int aColorLocation = glGetUniformLocation(shaderProgram, "outColor");
	glUniform4f(aColorLocation, 0.0f, 1.0f, 0.0f, 1.0f);


	glBindVertexArray(VAO_axis);
	glDrawArrays(GL_LINES, 0, 9);*/




	glutSwapBuffers(); // ȭ�鿡 ����ϱ�


}







GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ� 
{

	glViewport(0, 0, w, h);
}





GLvoid InitBuffer() {

	//��ǥ��

	glGenVertexArrays(1, &VAO_axis);
	glGenBuffers(1, &VBO_axis);


	glUseProgram(shaderProgram);
	glBindVertexArray(VAO_axis);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_axis);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axis), axis, GL_STATIC_DRAW);
	GLint aAttribute = glGetAttribLocation(shaderProgram, "aPos");
	glVertexAttribPointer(aAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	glEnableVertexAttribArray(aAttribute);




	//ť��
	//-----------------------------------------------------------------------------------------

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO_pos);
	glGenBuffers(1, &VBO_normal);

	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
	glBufferData(GL_ARRAY_BUFFER, out_vertices.size() * sizeof(glm::vec3), &out_vertices[0], GL_STATIC_DRAW);
	GLint pAttribute = glGetAttribLocation(shaderProgram, "aPos");
	glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(pAttribute);


	glBindBuffer(GL_ARRAY_BUFFER, VBO_normal);
	glBufferData(GL_ARRAY_BUFFER, out_normals.size() * sizeof(glm::vec3), &out_normals[0], GL_STATIC_DRAW);
	GLint nAttribute = glGetAttribLocation(shaderProgram, "aNormal");
	glVertexAttribPointer(nAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(nAttribute);


	/*glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
	GLint cAttribute = glGetAttribLocation(shaderProgram, "aColor");
	glVertexAttribPointer(cAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(cAttribute);*/


}


void make_vertexShaders() {



	GLchar* vertexSource;

	//���ؽ� ���̴� �о�ͼ� ��ü ����� �������ϱ�
	vertexSource = filetobuf("vertex.glsl");
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader); //���ؽ� ������ 


	//�����ڵ� �ۼ�
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;
		return;
	}

}



void make_fragmentShaders() {

	GLchar* fragmentSource;

	//�����׸�Ʈ ���̴� �о�ͼ� ��ü ����� �������ϱ�
	fragmentSource = filetobuf("fragment.glsl");
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader); //���ؽ� ������ 


	//�����ڵ� �ۼ�
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;
		return;
	}

}




void make_shaderProgram() {

	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);


	//���α׷� ���� �� ��ü ���� ����
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


	//�����ڵ� �ۼ�
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(shaderProgram, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(shaderProgram, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;
		return;
	}
	glUseProgram(shaderProgram);

}



//������Ʈ ���� �б�
bool loadOBJ(const char* path) {
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< glm::vec3 > temp_vertices;
	std::vector< glm::vec2 > temp_uvs;
	std::vector< glm::vec3 > temp_normals;

	float sumX = 0.0, sumY = 0.0, sumZ = 0.0;
	float aveX, aveY, aveZ;
	float scaleX, scaleY, scaleZ;
	float minX = 0.0, minY = 0.0, minZ = 0.0;
	float maxX = 0.0, maxY = 0.0, maxZ = 0.0;
	float scaleAll;

	float sizeX, sizeY, sizeZ;

	FILE* file = fopen(path, "r");
	if (file == NULL) {
		printf("Impossible to open the file !\n");
		return false;
	}
	while (1) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader
		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);

			if (vertex.x < minX) minX = vertex.x;
			if (vertex.y < minY) minY = vertex.y;
			if (vertex.z < minZ) minZ = vertex.z;
			if (vertex.x > maxX) maxX = vertex.x;
			if (vertex.y > maxY) maxY = vertex.y;
			if (vertex.z > maxZ) maxZ = vertex.z;
			sumX += vertex.x;
			sumY += vertex.y;
			sumZ += vertex.z;



			temp_vertices.push_back(vertex);
		}
		//���� �ؽ��� �б�
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			temp_uvs.push_back(uv);
		} //���� ��ֺ��� �б� 
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);

		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("File can't be read by our simple parser : ( Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);

		}


		aveX = sumX / vertexIndices.size();
		aveY = sumY / vertexIndices.size();
		aveZ = sumZ / vertexIndices.size();
		scaleX = maxX - minX;
		scaleY = maxY - minY;
		scaleZ = maxZ - minZ;
	}
	//std::cout << "aveX: " << aveX << " aveY: " << aveY << " aveZ: " << aveZ << std::endl;

	//std::cout << "scaleX: " << scaleX << " scaleY: " << scaleY << " scaleZ: " << scaleZ << std::endl;

	for (unsigned int i = 0; i < vertexIndices.size(); i++) {
		glm::vec3 temp;
		unsigned int vertexIndex = vertexIndices[i];
		temp = temp_vertices[vertexIndex - 1];
		temp.x = temp.x - minX;
		temp.y = temp.y - minY;
		temp.z = temp.z - minZ;

		temp.x = ((temp.x * 2.0f) / scaleX) - 1.0f;
		temp.y = ((temp.y * 2.0f) / scaleY) - 1.0f;
		temp.z = ((temp.z * 2.0f) / scaleZ) - 1.0f;


		out_vertices.push_back(temp);
		//glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		//outvertex.push_back(vertex);
	}





	for (unsigned int i = 0; i < uvIndices.size(); i++) {
		unsigned int uvIndex = uvIndices[i];
		glm::vec2 vertex = temp_uvs[uvIndex - 1];
		out_uvs.push_back(vertex);
	}
	for (unsigned int i = 0; i < normalIndices.size(); i++) {
		unsigned int normalIndex = normalIndices[i];
		glm::vec3 vertex = temp_normals[normalIndex - 1];
		out_normals.push_back(vertex);
	}


	return true;
}













//���� �б�-----------------------
char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb"); // Open file for reading 
	if (!fptr) // Return NULL on failure 
		return NULL;
	fseek(fptr, 0, SEEK_END); // Seek to the end of the file 
	length = ftell(fptr); // Find out how many bytes into the file we are 
	buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator 
	fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file 
	fread(buf, length, 1, fptr);// Read the contents of the file in to the buffer 
	fclose(fptr); // Close the file 
	buf[length] = 0;
	// Null terminator 
	return buf; // Return the buffer 
}

