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
#include <Windows.h>
using namespace std;
GLvoid drawScene(GLvoid);
void make_vertexShaders(); 
void make_fragmentShaders(); 
void make_shaderProgram(); 
void InitBuffer();
char* filetobuf(const char* file);
GLint height, width; 
GLuint shaderProgram; 
GLuint vertexShader;
GLuint fragmentShader;
GLuint VAO, VBO_pos, VBO_normal, VBO_color;
GLuint VAO_blank, VBO_blank;
bool loadOBJ(const char* path);
GLvoid keyboard(unsigned char key, int x, int y);


std::vector< glm::vec3 > out_vertices;
std::vector< glm::vec2 > out_uvs;
std::vector< glm::vec3 > out_normals;
bool res = loadOBJ("tree1..obj");
//bool res = loadOBJ("cube.obj");
GLint object = out_vertices.size();

typedef struct Hint
{
	int num = 3; //힌트 몇번 썼는지
	bool hint_ = true;
	bool blank = true;
	void hintsys() {
		if (hint_)
		{
			cout << "3" << endl;
			Sleep(1000);
			cout << "2" << endl;
			Sleep(1000);
			cout << "1" << endl;
		}
	}
};
Hint hint;

float blank[] = {
	2.0,-2.0,0.0,
	-2.0,-2.0,0.0,
	2.0,2.0,0.0,
	2.0,2.0,0.0,
	-2.0,2.0,0.0,
	-2.0,-2.0,0.0
	
};

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{ //--- 윈도우 생성하기
	height =600;
	width = 1300;
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // 디스플레이 모드 설정
	glutInitWindowPosition(100, 100); // 윈도우의 위치 지정
	glutInitWindowSize(width, height); // 윈도우의 크기 지정
	glutCreateWindow("cube");
	glewExperimental = GL_TRUE;
	glewInit();
	make_vertexShaders(); //--- 버텍스 세이더 만들기
	make_fragmentShaders(); //--- 프래그먼트 세이더 만들기
	make_shaderProgram(); //--- 세이더 프로그램 만들기
	InitBuffer();
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(drawScene); // 출력 함수의 지정
	glutMainLoop(); // 이벤트 처리 시작 


}




GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	glClearColor(1.0,1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




	glEnable(GL_DEPTH_TEST);

		//모델변환
	glm::mat4 Sz = glm::mat4(1.0);
	Sz = glm::scale(Sz, glm::vec3(0.5, 0.5, 0.5));
	int modeltrans = glGetUniformLocation(shaderProgram, "Smatrix");
	glUniformMatrix4fv(modeltrans, 1, GL_FALSE, glm::value_ptr(Sz));
	

	//뷰 변환
	glm::vec3 cameraPos = glm::vec3(-0.8f, 1.0f, 1.0f); //--- 카메라 위치
	glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- 카메라 바라보는 방향
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	unsigned int viewLocation = glGetUniformLocation(shaderProgram, "viewTransform"); //--- 뷰잉 변환 설정
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);


	//투영변환
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
	projection = glm::translate(projection, glm::vec3(0.0, 0.0, -3.0));//--- 공간을 약간 뒤로 미뤄줌
	unsigned int projectionLocation = glGetUniformLocation(shaderProgram, "projectionTransform"); //--- 투영 변환 값 설정
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);







		int objColorLocation = glGetUniformLocation(shaderProgram, "objectColor"); //--- object Color값 전달: (1.0, 0.5, 0.3)의 색
		glUniform3f(objColorLocation, 1.0, 1.0, 1.0);

		unsigned int lightPosLocation = glGetUniformLocation(shaderProgram, "lightPos"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
		glUniform3f(lightPosLocation,0.0,0.0,1.0);
		int lightColorLocation = glGetUniformLocation(shaderProgram, "lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
		glUniform3f(lightColorLocation,1.0,1.0,1.0);

		glUniform3f(objColorLocation, 1.0, 1.0, 1.0);
		unsigned int viewPosLocation = glGetUniformLocation(shaderProgram, "viewPos"); //--- viewPos 값 전달: 카메라 위치
		glUniform3f(viewPosLocation, 0.0, 0.0, 0.0);

    int vColorLocation = glGetUniformLocation(shaderProgram, "outColor");


     glViewport(-200, 0, width , height);
     glUniform4f(vColorLocation, 0.0f, 0.0f, 0.0f, 1.0f);
     glUseProgram(shaderProgram);
     glBindVertexArray(VAO);
     glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
     glDrawArrays(GL_TRIANGLES, 0, object);
     //메인(플레이)화면

	
	
     glViewport(width - 300, height -500, 200, 200);
     glUniform4f(vColorLocation, 0.0f, 0.0f, 0.0f, 1.0f);
     glUseProgram(shaderProgram);
     glBindVertexArray(VAO);
     glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
     glDrawArrays(GL_TRIANGLES, 0, object);
	
     //현재 진행상황 화면
	 glm::vec3 cameraPosh = glm::vec3(-0.8f, 1.0f, 1.0f); //--- 카메라 위치
	 glm::vec3 cameraDirectionh = glm::vec3(0.0f, 0.0f, 0.0f); //--- 카메라 바라보는 방향
	 glm::vec3 cameraUph = glm::vec3(0.0f, -1.0f, 0.0f); //--- 카메라 위쪽 방향
	 glm::mat4 viewh = glm::mat4(1.0f);
	 viewh = glm::lookAt(cameraPosh, cameraDirectionh, cameraUph);
      if (hint.hint_) {
		 glViewport(width - 300, height - 200, 200, 200);
		 glUniform4f(vColorLocation, 0.0f, 0.0f, 0.0f, 1.0f);
		 glUseProgram(shaderProgram);
		 glBindVertexArray(VAO);
		 glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		 glDrawArrays(GL_TRIANGLES, 0, object);
		 glBindVertexArray(VAO_blank);
		 glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		 glDrawArrays(GL_TRIANGLES, 0, object);
		 //예시(힌트)화면
	 }
	 hint.hint_ = false;
     glutSwapBuffers(); // 화면에 출력하기
     

}






GLvoid InitBuffer() {

	//좌표축

	glGenVertexArrays(1, &VAO_blank);
	glGenBuffers(1, &VBO_blank);


	glUseProgram(shaderProgram);
	glBindVertexArray(VAO_blank);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_blank);
	glBufferData(GL_ARRAY_BUFFER, sizeof(blank),blank, GL_STATIC_DRAW);
	GLint aAttribute = glGetAttribLocation(shaderProgram, "aPos");
	glVertexAttribPointer(aAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	glEnableVertexAttribArray(aAttribute);




	//큐브
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

}


void make_vertexShaders() {



	GLchar* vertexSource;

	//버텍스 세이더 읽어와서 객체 만들고 컴파일하기
	vertexSource = filetobuf("vertex.glsl");
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader); //버텍스 컴파일 


	//에러코드 작성
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}

}



void make_fragmentShaders() {

	GLchar* fragmentSource;

	//프래그먼트 세이더 읽어와서 객체 만들고 컴파일하기
	fragmentSource = filetobuf("fragment.glsl");
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader); //버텍스 컴파일 


	//에러코드 작성
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}

}




void make_shaderProgram() {

	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);


	//프로그램 생성 후 객체 삭제 가능
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


	//에러코드 작성
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(shaderProgram, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(shaderProgram, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
	glUseProgram(shaderProgram);

}



//오브젝트 파일 읽기
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
		//정점 텍스쳐 읽기
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			temp_uvs.push_back(uv);
		} //정점 노멀벡터 읽기 
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


//파일 읽기-----------------------
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

GLvoid keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	//힌트 
	case 'h':
		hint.hint_ = true;
		hint.hintsys();
		break;

	}
	glutPostRedisplay();
}