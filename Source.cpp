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

using std::cout;
using std::endl;

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
float cam_dist = 3.0f;

void onResize(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void inputHandle(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		background = { 1.0f, 0.0f, 0.0f, 1.0f };
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		background = { 0.0f, 1.0f, 0.0f, 1.0f };
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		background = { 0.0f, 0.0f, 1.0f, 1.0f };
	if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
		cam_dist += 0.05f;
	if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
		cam_dist -= 0.05f;
}

typedef unsigned char byte;

int main() {
#pragma region WINDOW INITIALIZATION
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "Window", NULL, NULL);
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

	glViewport(0, 0, 800, 600);
	glEnable(GL_DEPTH_TEST);
#pragma endregion

	int box_width, box_height, channels;
	byte* data = stbi_load("images\\arbuzman.png", &box_width, &box_height, &channels, 0);


	const int verts = 8;

	// Вершины треугольника
	// некорректное применение текстур
	float cube[verts * (3 + 4 + 2)] = {
/*0*/		-1.0f, 1.0f, -1.0f,		1.0f, 0.0f, 0.0f, 1.0f,		1.0f, 0.0f,
/*1*/		1.0f, 1.0f, -1.0f,		0.5f, 0.5f, 0.0f, 1.0f,		0.0f, 0.0f,
/*2*/		1.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f, 1.0f,		1.0f, 0.0f,
/*3*/		-1.0f, 1.0f, 1.0f,		0.0f, 0.5f, 0.5f, 1.0f,		0.0f, 0.0f,
/*4*/		-1.0f, -1.0f, -1.0f,	0.0f, 0.0f, 1.0f, 1.0f,		1.0f, 1.0f,
/*5*/		1.0f, -1.0f, -1.0f,		0.5f, 0.0f, 0.5f, 1.0f,		0.0f, 1.0f,
/*6*/		1.0f, -1.0f, 1.0f,		0.5f, 0.5f, 0.5f, 1.0f,		1.0f, 1.0f,
/*7*/		-1.0f, -1.0f, 1.0f,		1.0f, 1.0f, 1.0f, 1.0f,		0.0f, 1.0f
	};

	unsigned int indices[] = {
		0, 1, 2,
		0, 2, 3,
		0, 1, 4,
		1, 4, 5,
		0, 3, 7,
		0, 4, 7,
		1, 2, 6,
		1, 5, 6,
		2, 3, 7,
		2, 6, 7,
		4, 5, 7,
		5, 6, 7
	};

	ModelTransform polygonTrans1 = { glm::vec3(0.f, 0.f, 0.f), // position 
									glm::vec3(0.f, 0.f, 0.f), // rotation
									glm::vec3(1.f, 1.f, 1.f) }; // scale

	ModelTransform polygonTrans2 = { glm::vec3(0.f, 0.f, 0.f), // position 
									glm::vec3(0.f, 0.f, 0.f), // rotation
									glm::vec3(1.f, 1.f, 1.f) }; // scale

	ModelTransform polygonTrans3 = { glm::vec3(0.f, 0.f, 0.f), // position 
									glm::vec3(0.f, 0.f, 0.f), // rotation
									glm::vec3(1.f, 1.f, 1.f) }; // scale

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

	// Vertex Buffer Object, Vertex Array Object, Element Buffer Object
	unsigned int VBO_polygon, VAO_polygon, EBO_polygon; // Объект буфера вершин
	glGenBuffers(1, &VBO_polygon); // Генерируем буфер, в котором будут вершины
	glGenBuffers(1, &EBO_polygon);
	glGenVertexArrays(1, &VAO_polygon); // Генерируем вертексный массив

	glBindVertexArray(VAO_polygon);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_polygon); // Привязываем буфер к таргету

	// GL_STREAM_DRAW - данные записываются один раз, редко используются ГП
	// GL_STATIC_DRAW - данные записываются один раз, используются часто
	// GL_DYNAMIC_DRAW - данные могут перезаписываться
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verts * 9, cube, GL_STATIC_DRAW); // Загружает в видеопамять данные из ОЗУ

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_polygon);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 36, indices, GL_STATIC_DRAW);

	// Как читать данные из массива
	// 1 параметр - номер атрибута из шейдера (location = ?)
	// 2 - количество данных на 1 вершину (не менее 1 и не более 4)
	// 3 - тип данных
	// 4 - должны ли быть данные нормализованными 
	// 5 - шаг (через сколько байт начинается следующая область для второй вершины)
	// 6 - указатель для адреса смещения
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); // Включаем аттрибут
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1); // Включаем аттрибут
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));
	glEnableVertexAttribArray(2); // Включаем аттрибут

#pragma endregion

	Shader* polygon_shader = new Shader("shaders\\basic.vert", "shaders\\basic.frag");

	while (!glfwWindowShouldClose(window)) {

		inputHandle(window);

		polygonTrans1.rotation.z = glfwGetTime() * 60.0;
		polygonTrans1.rotation.x = glfwGetTime() * 45.0;
		polygonTrans1.position.x = 0.3f * cos(glfwGetTime());
		polygonTrans1.position.y = 0.3f * sin(glfwGetTime());
		polygonTrans1.setScale(0.2f);

		polygonTrans2.rotation.z = glfwGetTime() * 30.0;
		//polygonTrans2.rotation.y = glfwGetTime() * 45.0;
		polygonTrans2.position.x = 0.6f * cos(glfwGetTime() + 3.1415);
		polygonTrans2.position.y = 0.6f * sin(glfwGetTime() + 3.1415);
		polygonTrans2.setScale(0.2f);

		polygonTrans3.setScale(0.2f);
		//polygonTrans3.rotation.x = glfwGetTime() * 90.0;
		//polygonTrans3.rotation.y = glfwGetTime() * 60.0;

		glClearColor(background.r, background.g, background.b, background.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		polygon_shader->use();

		glm::vec3 pos_vec = glm::vec3(cam_dist * cos(glfwGetTime()* 0.3), 0.0f, cam_dist * sin(glfwGetTime() * 0.3)); // Вектор позиции камеры
		glm::vec3 target_vec = glm::vec3(0.0f, 0.0f, 0.0f); // Вектор куда смотрит камера
		glm::vec3 up_vec = glm::vec3(0.0f, 1.0f, 0.0f); // Вектор направления взгляда

		glm::mat4 camera = glm::lookAt(pos_vec, target_vec, up_vec);
		//glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.f, 1.f, 0.01f, 100.f);
		glm::mat4 projection = glm::perspective(45.f, 1.f, 0.01f, 100.f);

		// 1
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, polygonTrans1.position);
		model = glm::rotate(model, glm::radians(polygonTrans1.rotation.x), glm::vec3(1.f, 0.f, 0.f));
		model = glm::rotate(model, glm::radians(polygonTrans1.rotation.y), glm::vec3(0.f, 1.f, 0.f));
		model = glm::rotate(model, glm::radians(polygonTrans1.rotation.z), glm::vec3(0.f, 0.f, 1.f));
		model = glm::scale(model, polygonTrans1.scale);

		glm::mat4 pvm = projection * camera * model;
		polygon_shader->setMatrix4F("pvm", pvm);

		glBindTexture(GL_TEXTURE_2D, box_texture);
		glBindVertexArray(VAO_polygon);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		// 2
		model = glm::mat4(1.0f);
		model = glm::translate(model, polygonTrans2.position);
		model = glm::rotate(model, glm::radians(polygonTrans2.rotation.x), glm::vec3(1.f, 0.f, 0.f));
		model = glm::rotate(model, glm::radians(polygonTrans2.rotation.y), glm::vec3(0.f, 1.f, 0.f));
		model = glm::rotate(model, glm::radians(polygonTrans2.rotation.z), glm::vec3(0.f, 0.f, 1.f));
		model = glm::scale(model, polygonTrans2.scale);

		/*pvm = projection * camera * model;
		polygon_shader->setMatrix4F("pvm", pvm);*/

		glBindVertexArray(VAO_polygon);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		// 3
		model = glm::mat4(1.0f);
		model = glm::translate(model, polygonTrans3.position);
		model = glm::rotate(model, glm::radians(polygonTrans3.rotation.x), glm::vec3(1.f, 0.f, 0.f));
		model = glm::rotate(model, glm::radians(polygonTrans3.rotation.y), glm::vec3(0.f, 1.f, 0.f));
		model = glm::rotate(model, glm::radians(polygonTrans3.rotation.z), glm::vec3(0.f, 0.f, 1.f));
		model = glm::scale(model, polygonTrans3.scale);

		/*pvm = projection * camera * model;
		polygon_shader->setMatrix4F("pvm", pvm);*/

		glBindVertexArray(VAO_polygon);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window); // Смена буферов
		glfwPollEvents(); // Обработка сообщений от ОС (нажатие кнопок, изменить окно)
	}

	delete polygon_shader;

	glfwTerminate();
	return 0;
}