#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <fstream>
#include "Shader.h"
#include "Camera.h"

using std::cout;
using std::endl;

typedef unsigned char byte;

struct ModelTransform {
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	void setScale(float s) {
		scale.x = s;
		scale.y = s;
		scale.z = s;
	}
};

struct Color {
	float r, g, b, a;
};

Color background{ 0.f, 0.f, 0.f, 0.f };

struct Material {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

struct DirectionalLight { // Направленный свет
	glm::vec3 direction;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

struct PointLight { // Точечный свет
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	// Коэффициенты затухания света
	float constant;
	float linear;
	float quadratic;
};

Camera camera(glm::vec3(0.f, 0.f, -2.f));

bool wireframeMode = false; // Режим "каркаса" полигонов

void updatePolygonMode() { // Режим рендера полигона
	if (wireframeMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void onResize(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void onScroll(GLFWwindow* window, double x, double y) {
	camera.ChangeFOV(y);
}

void onKeyAction(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_SPACE:
			wireframeMode = !wireframeMode;
			updatePolygonMode();
			break;
		}
	}
}

void inputHandle(GLFWwindow* window, double dt) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		background = { 1.0f, 0.0f, 0.0f, 1.0f };
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		background = { 0.0f, 1.0f, 0.0f, 1.0f };
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		background = { 0.0f, 0.0f, 1.0f, 1.0f };
	uint32_t dir = 0;
	if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
		dir |= CAM_UP;
	if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
		dir |= CAM_DOWN;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		dir |= CAM_FORWARD;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		dir |= CAM_BACKWARD;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		dir |= CAM_LEFT;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		dir |= CAM_RIGHT;

	
	double newx = 0.f, newy = 0.f;
	glfwGetCursorPos(window, &newx, &newy); // Позиция курсора в оконных координатах
	static double x = newx, y = newy;
	double xoffset = newx - x;
	double yoffset = newy - y;
	x = newx;
	y = newy;

	camera.Move(dir, dt);
	camera.Rotate(xoffset, -yoffset);
}

int main() {
#pragma region WINDOW INITIALIZATION
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1280, 720, "Window", NULL, NULL);
	if (window == NULL) {
		cout << "Couldnt create window" << endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Couldnt load GLAD" << endl;
		glfwTerminate();
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, onResize);
	glfwSetScrollCallback(window, onScroll);
	glfwSetKeyCallback(window, onKeyAction);

	glViewport(0, 0, 1280, 720);
	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	updatePolygonMode();
	glEnable(GL_CULL_FACE); // Включить срезание граней
	glFrontFace(GL_CCW); // Определение передних полигонов; cw - clockwise - по часовой; ccw (по умолчанию) - counter clockwise - против часовой
#pragma endregion

	int box_width, box_height, channels;
	byte* data = stbi_load("images\\arbuzman.png", &box_width, &box_height, &channels, 0);


	const int verts = 36;

	float cube[] = {
	//position			normal					texture				color
	// y-координаты текстур перевёрнуты?
	// LEFT SIDE
	-1.0f,-1.0f,-1.0f,	-1.0f,  0.0f,  0.0f,	0.0f, 1.0f,		0.0f, 1.0f, 0.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,	-1.0f,  0.0f,  0.0f,	1.0f, 1.0f,		0.0f, 1.0f, 0.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,	-1.0f,  0.0f,  0.0f,	1.0f, 0.0f,		0.0f, 1.0f, 0.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,	-1.0f,  0.0f,  0.0f,	0.0f, 1.0f,		0.0f, 1.0f, 0.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,	-1.0f,  0.0f,  0.0f,	1.0f, 0.0f,		0.0f, 1.0f, 0.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,	-1.0f,  0.0f,  0.0f,	0.0f, 0.0f,		0.0f, 1.0f, 0.0f, 1.0f,
	// BACK SIDE																			
	1.0f, 1.0f,-1.0f,	0.0f,  0.0f, -1.0f, 	0.0f, 0.0f,		1.0f, 0.0f, 0.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,	0.0f,  0.0f, -1.0f, 	1.0f, 1.0f,		1.0f, 0.0f, 0.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,	0.0f,  0.0f, -1.0f, 	1.0f, 0.0f,		1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 1.0f,-1.0f,	0.0f,  0.0f, -1.0f,		0.0f, 0.0f,		1.0f, 0.0f, 0.0f, 1.0f,
	1.0f,-1.0f,-1.0f,	0.0f,  0.0f, -1.0f,		0.0f, 1.0f,		1.0f, 0.0f, 0.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,	0.0f,  0.0f, -1.0f,		1.0f, 1.0f,		1.0f, 0.0f, 0.0f, 1.0f,
	// DOWN SIDE																			
	1.0f,-1.0f, 1.0f,	0.0f, -1.0f,  0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,	0.0f, -1.0f,  0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 1.0f, 1.0f,
	1.0f,-1.0f,-1.0f,	0.0f, -1.0f,  0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 1.0f, 1.0f,
	1.0f,-1.0f, 1.0f,	0.0f, -1.0f,  0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,	0.0f, -1.0f,  0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,	0.0f, -1.0f,  0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 1.0f, 1.0f,
	// FRONT SIDE																			
	-1.0f, 1.0f, 1.0f,	0.0f,  0.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,	0.0f,  0.0f, 1.0f,		0.0f, 1.0f,		0.0f, 0.0f, 1.0f, 1.0f,
	1.0f,-1.0f, 1.0f,	0.0f,  0.0f, 1.0f,		1.0f, 1.0f,		0.0f, 0.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,	0.0f,  0.0f, 1.0f,		1.0f, 0.0f,		0.0f, 0.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,	0.0f,  0.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 1.0f, 1.0f,
	1.0f,-1.0f, 1.0f,	0.0f,  0.0f, 1.0f,		1.0f, 1.0f,		0.0f, 0.0f, 1.0f, 1.0f,
	// RIGHT SIDE																			
	1.0f, 1.0f, 1.0f,	1.0f,  0.0f,  0.0f,		0.0f, 0.0f,		1.0f, 0.0f, 0.0f, 1.0f,
	1.0f,-1.0f,-1.0f,	1.0f,  0.0f,  0.0f,		1.0f, 1.0f,		1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 1.0f,-1.0f,	1.0f,  0.0f,  0.0f,		1.0f, 0.0f,		1.0f, 0.0f, 0.0f, 1.0f,
	1.0f,-1.0f,-1.0f,	1.0f,  0.0f,  0.0f,		1.0f, 1.0f,		1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 1.0f,	1.0f,  0.0f,  0.0f,		0.0f, 0.0f,		1.0f, 0.0f, 0.0f, 1.0f,
	1.0f,-1.0f, 1.0f,	1.0f,  0.0f,  0.0f,		0.0f, 1.0f,		1.0f, 0.0f, 0.0f, 1.0f,
	// UP SIDE	(dont reverse texture y-coords)												
	1.0f, 1.0f, 1.0f,	0.0f,  1.0f,  0.0f,		1.0f, 0.0f,		0.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 1.0f,-1.0f,	0.0f,  1.0f,  0.0f,		1.0f, 1.0f,		0.0f, 1.0f, 0.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,	0.0f,  1.0f,  0.0f,		0.0f, 1.0f,		0.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 1.0f,	0.0f,  1.0f,  0.0f,		1.0f, 0.0f,		0.0f, 1.0f, 0.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,	0.0f,  1.0f,  0.0f,		0.0f, 1.0f,		0.0f, 1.0f, 0.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,	0.0f,  1.0f,  0.0f,		0.0f, 0.0f,		0.0f, 1.0f, 0.0f, 1.0f
	};

	Material cubeMaterials[3] = {
		{
			glm::vec3(0.25, 0.20725, 0.20725),
			glm::vec3(1, 0.829, 0.829),
			glm::vec3(0.296648, 0.296648, 0.296648),
			12.f
		}, // Pearl
		{
			glm::vec3(0.25, 0.25, 0.25),
			glm::vec3(0.4, 0.4, 0.4),
			glm::vec3(0.774597, 0.774597, 0.774597),
			77.f
		}, // Chrome
		{
			glm::vec3(0.1745, 0.01175, 0.01175),
			glm::vec3(0.61424, 0.04136, 0.04136),
			glm::vec3(0.727811, 0.626959, 0.626959),
			77.f
		}, // Ruby
	};

	const int cube_count = 50;

	int cubeMat[cube_count];
	ModelTransform cubeTrans[cube_count];
	for (int i = 0; i < cube_count; i++) {
		float scale = (rand() % 6 + 1) / 20.0f;
		cubeTrans[i] = {	
			glm::vec3((rand() % 201 - 100) / 50.0f, (rand() % 201 - 100) / 50.0f, (rand() % 201 - 100) / 50.0f),
			glm::vec3(rand() / 100.0f, rand() / 100.0f, rand() / 100.0f),
			glm::vec3(scale, scale, scale) 
		};
		cubeMat[i] = rand() % 3;
	}

	ModelTransform lightTrans = {	glm::vec3(0.f, 0.f, 0.f), // position 
									glm::vec3(0.f, 0.f, 0.f), // rotation
									glm::vec3(0.1f, 0.1f, 0.1f) }; // scale

#pragma region BUFFERS INITIALIZATION
	unsigned int box_texture;
	glGenTextures(1, &box_texture);

	glBindTexture(GL_TEXTURE_2D, box_texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if (channels == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, box_width, box_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	else 									   
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, box_width, box_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	//glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);

	// Vertex Buffer Object, Vertex Array Object
	unsigned int VBO_polygon, VAO_polygon;
	glGenBuffers(1, &VBO_polygon); // Генерируем буфер, в котором будут вершины
	glGenVertexArrays(1, &VAO_polygon); // Генерируем вертексный массив

	glBindVertexArray(VAO_polygon);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_polygon); // Привязываем буфер к таргету

	// GL_STREAM_DRAW - данные записываются один раз, редко используются ГП
	// GL_STATIC_DRAW - данные записываются один раз, используются часто
	// GL_DYNAMIC_DRAW - данные могут перезаписываться
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW); // Загружает в видеопамять данные из ОЗУ

	// Как читать данные из массива
	// 1 параметр - номер атрибута из шейдера (location = ?)
	// 2 - количество данных на 1 вершину (не менее 1 и не более 4)
	// 3 - тип данных
	// 4 - должны ли быть данные нормализованными 
	// 5 - шаг (через сколько байт начинается следующая область для второй вершины)
	// 6 - указатель для адреса смещения
	
	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// texture coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// color
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);

#pragma endregion

	Shader* polygon_shader = new Shader("shaders\\basic.vert", "shaders\\basic.frag");
	Shader* light_shader = new Shader("shaders\\light.vert", "shaders\\light.frag");

	double oldTime = glfwGetTime(), newTime, deltaTime;

	PointLight light1 = {	glm::vec3(0.0f, 0.0f, 0.0f),
						glm::vec3(0.2f, 0.2f, 0.2f),
						glm::vec3(0.5f, 0.5f, 0.5f),
						glm::vec3(1.0f, 1.0f, 1.0f),
						1.0f, 0.14f, 0.07f };

	while (!glfwWindowShouldClose(window)) {
		newTime = glfwGetTime();
		deltaTime = newTime - oldTime;
		oldTime = newTime;
		inputHandle(window, deltaTime);

		lightTrans.position = light1.position;

		/*light1.specular.r = sin(newTime * 2) + 1 / 2;
		light1.specular.g = sin(newTime * 2 + 2 * 3.14159 / 3) + 1;
		light1.specular.b = sin(newTime * 2 + 4 * 3.14159 / 3) + 1;

		light1.diffuse = light1.specular * 0.8f;
		light1.diffuse = light1.specular * 0.4f;*/

		glClearColor(background.r, background.g, background.b, background.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glm::mat4 p = camera.GetProjectionMatrix();
		glm::mat4 v = camera.GetViewMatrix();
		glm::mat4 pv = p * v;
		glm::mat4 model;

		for (int i = 0; i < cube_count; i++) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, cubeTrans[i].position);
			model = glm::rotate(model, glm::radians(cubeTrans[i].rotation.x), glm::vec3(1.f, 0.f, 0.f));
			model = glm::rotate(model, glm::radians(cubeTrans[i].rotation.y), glm::vec3(0.f, 1.f, 0.f));
			model = glm::rotate(model, glm::radians(cubeTrans[i].rotation.z), glm::vec3(0.f, 0.f, 1.f));
			model = glm::scale(model, cubeTrans[i].scale);

			polygon_shader->use();
			polygon_shader->setMatrix4F("pv", pv);
			polygon_shader->setMatrix4F("model", model);
			polygon_shader->setBool("wireframeMode", wireframeMode);
			polygon_shader->setVec3("viewPos", camera.Position);

			polygon_shader->setVec3("light.position", light1.position);
			polygon_shader->setVec3("light.ambient", light1.ambient);
			polygon_shader->setVec3("light.diffuse", light1.diffuse);
			polygon_shader->setVec3("light.specular", light1.specular);
			polygon_shader->setFloat("light.constant", light1.constant);
			polygon_shader->setFloat("light.linear", light1.linear);
			polygon_shader->setFloat("light.quadratic", light1.quadratic);

			polygon_shader->setVec3("material.ambient",		cubeMaterials[cubeMat[i]].ambient);
			polygon_shader->setVec3("material.diffuse",		cubeMaterials[cubeMat[i]].diffuse);
			polygon_shader->setVec3("material.specular",	cubeMaterials[cubeMat[i]].specular);
			polygon_shader->setFloat("material.shininess",	cubeMaterials[cubeMat[i]].shininess);

			glBindTexture(GL_TEXTURE_2D, box_texture);
			glBindVertexArray(VAO_polygon);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		
		// light
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightTrans.position);
		model = glm::scale(model, lightTrans.scale);

		light_shader->use();
		light_shader->setMatrix4F("pv", pv);
		light_shader->setMatrix4F("model", model);
		light_shader->setVec3("lightColor", light1.specular);

		glBindVertexArray(VAO_polygon);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(window); // Смена буферов
		glfwPollEvents(); // Обработка сообщений от ОС (нажатие кнопок, изменить окно)
	}

	delete polygon_shader;
	delete light_shader;

	glfwTerminate();
	return 0;
}