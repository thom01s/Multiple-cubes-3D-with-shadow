/* Hello Triangle - código adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gráfico - Jogos Digitais - Unisinos
 * Versão inicial: 7/4/2017
 * Última atualização em 02/03/2022
 *
 */

#include <iostream>
#include <string>
#include <assert.h>
#include <fstream>
#include <sstream>
#include <vector>
#include "stb_image.h"

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "objloader.h"


// Protótipo da função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 800, HEIGHT = 600;


bool view1 = false, view2 = false, view3 = false, view4 = false, view5 = false, firstMouse = true, rotatex = false, rotatey = false, rotatez = false, modelx = false, modely = false, modelz = false;
float x = 0.0;
float y = 0.0;
float z = 0.0;
float t = 0.0;
float r = 0.0;
float f = 0.0;
float g = 0.0;

glm::vec3 cameraPos;
glm::vec3 cameraFront;
glm::vec3 cameraUp;
glm::vec3 front;

float lastX = WIDTH/2.0 , lastY = HEIGHT/2.0;
float yaw = -90.0f;
float pitch = 0.0f;

float fov = 45.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;


//struct Objeto
//{
//	// glm::mat4 model
//	// vector <Grupo> grupos;
//
//	;;// vec3 posicao
//	//vec3 escala
//	//float angulo, vec3 eixo
//};
//
//struct Grupo
//{
//	//VAO
//	//texID
//	//ka, kd, ks,n
//};
//
//vector <Objeto> objetos;

//opcionalmente
//vector <int> texIDs;

// Função MAIN
int main()
{
	// Inicialização da GLFW
	glfwInit();

	//Muita atenção aqui: alguns ambientes não aceitam essas configurações
	//Você deve adaptar para a versão do OpenGL suportada por sua placa
	//Sugestão: comente essas linhas de código para desobrir a versão e
	//depois atualize (por exemplo: 4.5 com 4 e 5)
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Essencial para computadores da Apple
//#ifdef __APPLE__
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif

	// Criação da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "trab GB", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);

	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	// Obtendo as informações de versão
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Compilando e buildando o programa de shader
	Shader shader = Shader("../shaders/hello.vs", "../shaders/hello.fs");

	// Gerando um buffer simples, com a geometria de um triângulo
	// Read our .obj file
	GLuint VAO = setupMesh("../Cube.obj");
	GLuint texID = loadTexture("../Cube.png");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texID);
	/*Objeto novoObjeto = setupMesh("....");
	objetos.push_back(novoObjeto);

	novoObjeto = setupMesh("....");
	... */


	glUseProgram(shader.ID);

	cameraPos = glm::vec3(0.0, 0.0, 3.0);
	cameraFront = glm::vec3(0.0, 0.0, -1.0);
	cameraUp = glm::vec3(0.0, 1.0, 0.0);

	// Propriedades do material dos objetos
		float ka, kd, ks, n;

	//Mandando para o shader as infos de iluminação
	GLint lightColorLoc = glGetUniformLocation(shader.ID, "lightColor");
	GLint lightPosLoc = glGetUniformLocation(shader.ID, "lightPos");
	GLint viewPosLoc = glGetUniformLocation(shader.ID, "viewPos");
	glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
	glUniform3f(lightPosLoc, 0.0 + f, 5.0 + g, 2.0 + r);
	glUniform3f(viewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);


	

	glm::mat4 model = glm::mat4(1); //matriz identidade;
	GLint modelLoc = glGetUniformLocation(shader.ID, "model");

	glm::mat4 view = glm::mat4(1); //matriz identidade;
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	GLint viewLoc = glGetUniformLocation(shader.ID, "view");
	glUniformMatrix4fv(viewLoc, 1, FALSE, glm::value_ptr(view));

	glm::mat4 projection = glm::mat4(1); //matriz identidade;
	GLint projectionLoc = glGetUniformLocation(shader.ID, "projection");
	projection = glm::perspective(glm::radians(fov), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
	glUniformMatrix4fv(projectionLoc, 1, FALSE, glm::value_ptr(projection));

	glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));


	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	glEnable(GL_DEPTH_TEST);

	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		model = glm::mat4(1);

		glm::mat4 view = glm::mat4(1); //matriz identidade;
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		glUniform3f(lightPosLoc, 0.0 + f, 5.0 + g, 2.0 + r);

		glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));

		projection = glm::perspective(glm::radians(fov), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

		if (view1)
		{
			view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else if (view2)
		{
			view = glm::lookAt(glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else if (view3)
		{
			view = glm::lookAt(glm::vec3(3.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else if (view4)
		{
			view = glm::lookAt(glm::vec3(-3.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else if (view5)
		{
			view = glm::lookAt(glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		}

		glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, FALSE, glm::value_ptr(projection));

		glUniform1i(glGetUniformLocation(shader.ID, "tex_coord"), 0);



		// Chamada de desenho - drawcall
		// Poligono Preenchido - GL_TRIANGLES
		glBindVertexArray(VAO);

		float xi = -2.0;
		float offsetx = 1;

		float yi = -1.5;
		float offsety = 1.5;

		shader.setFloat("ka", 0.5);
		shader.setFloat("kd", 0.45);
		shader.setFloat("ks", 0.25);

		float ns[5] = { 3.0, 5.0, 10.0, 30.0, 200.0 };
		float kss[3] = { 0.1, 0.25, 0.5 };

		glBindVertexArray(VAO);
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 5; j++)
			{
				if (j == 0) {

					texID = loadTexture("../0.png");
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, texID);
				}
				if (j == 1) {

					texID = loadTexture("../1.png");
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, texID);
				}
				if (j == 2) {

					texID = loadTexture("../2.png");
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, texID);
				}
				if (j == 3) {

					texID = loadTexture("../3.png");
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, texID);
				}
				if (j == 4) {

					texID = loadTexture("../4.png");
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, texID);
				}
				glUniform1i(glGetUniformLocation(shader.ID, "tex_coord"), 0);
				shader.setFloat("ks", kss[i]);
				shader.setFloat("n", ns[j]);
				glm::mat4 model = glm::mat4(1);
				model = glm::translate(model, glm::vec3((xi + j * offsetx) + x, (yi + i * offsety) + y, 0.0 + z));
				model = glm::scale(model, glm::vec3(0.4 + t, 0.4 + t, 0.4 + t));
				if (rotatex)
				{
					model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 0.0f));
				}
				else if (rotatey)
				{
					model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
				}
				else if (rotatez)
				{
					model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
				}

				glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));
				glDrawArrays(GL_TRIANGLES, 0, getMeshSize());
			}
		}

		

		// Chamada de desenho - drawcall
		// CONTORNO - GL_LINE_LOOP

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO);
	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	float cameraSpeed = 0.1f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		cameraPos += cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		cameraPos -= cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}


	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
	{
		rotatex = true;
		rotatey = false;
		rotatez = false;
	}
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
	{
		rotatex = false;
		rotatey = true;
		rotatez = false;
	}
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
	{
		rotatex = false;
		rotatey = false;
		rotatez = true;
	}


	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		view1 = true;
		view2 = false;
		view3 = false;
		view4 = false;
		view5 = false;
	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		view1 = false;
		view2 = true;
		view3 = false;
		view4 = false;
		view5 = false;
	}
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
	{
		view1 = false;
		view2 = false;
		view3 = true;
		view4 = false;
		view5 = false;
	}
	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
	{
		view1 = false;
		view2 = false;
		view3 = false;
		view4 = true;
		view5 = false;
	}
	if (key == GLFW_KEY_5 && action == GLFW_PRESS)
	{
		view1 = false;
		view2 = false;
		view3 = false;
		view4 = false;
		view5 = true;
	}


	if (key == GLFW_KEY_I && action == GLFW_PRESS) 
	{
		y = y + 1.0;
	}
	if (key == GLFW_KEY_K && action == GLFW_PRESS)
	{
		y = y - 1.0;
	}
	if (key == GLFW_KEY_J && action == GLFW_PRESS)
	{
		x = x - 1.0;
	}
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		x = x + 1.0;
	}
	if (key == GLFW_KEY_O && action == GLFW_PRESS)
	{
		z = z + 1.0;
	}
	if (key == GLFW_KEY_U && action == GLFW_PRESS)
	{
		z = z - 1.0;
	}

	if (key == GLFW_KEY_C && action == GLFW_PRESS)
	{
		t = t - 0.1;
	}
	if (key == GLFW_KEY_V && action == GLFW_PRESS)
	{
		t = t + 0.1;
	}

	if (key == GLFW_KEY_T && action == GLFW_PRESS)
	{
		g = g + 1.0;
	}
	if (key == GLFW_KEY_G && action == GLFW_PRESS)
	{
		g = g - 1.0;
	}
	if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		f = f - 1.0;
	}
	if (key == GLFW_KEY_H && action == GLFW_PRESS)
	{
		f = f + 1.0;
	}
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		r = r + 1.0;
	}
	if (key == GLFW_KEY_E && action == GLFW_PRESS)
	{
		r = r - 1.0;
	}
}



void mouse_callback(GLFWwindow * window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}

