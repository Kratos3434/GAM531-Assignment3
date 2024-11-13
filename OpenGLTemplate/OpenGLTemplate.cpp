#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shaderinit.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void processInput(GLFWwindow* window);
void init(void);
void render();
void transformations(Shader& ourShader);

void mouse_callback(GLFWwindow*, double, double);
void scroll_callback(GLFWwindow*, double, double);

//Screen Dimensions Variables
const unsigned int screen_width = 1200;
const unsigned int screen_height = 800;
//Number of Vertices
const GLuint NumVertices = 6;
int shaderProgram;
GLuint VBO, VBO2;
GLuint VAO, VAO2;
GLuint EBO, EBO2;

bool shape1Hidden = false, shape2Hidden = false;
float angle = 0.0f;
bool tKeyPressed = false, hKeyPressed = false;
float shape1X = 0.0f;
float shape2X = 0.4f;
float shape1Z = 0.4f;
float shape2Z = 0.4f;
bool leftMove = false;
bool rightMove = false;

unsigned int modelLocation;

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 55.0f;

double deltaTime = 0.0f;
double lastFrame = 0.0f;

// camera 
glm::vec3 cameraPos = glm::vec3(0.5f, 0.0f, 3.0f);
glm::vec3 cameraTarget = glm::vec3(0.1f, 0.1f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 cameraPos2 = glm::vec3(5.0f, 0.0f, 3.0f);
glm::vec3 cameraTarget2 = glm::vec3(0.1f, 0.1f, -1.0f);
glm::vec3 cameraUp2 = glm::vec3(0.0f, 1.0f, 0.0f);

unsigned int currentCamera = 0;

int main() {
	// Initialize GLFW and check if it works
// ------------------------------
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initiate GLFW!\n");
		return -1;
	}
	//Set some Hints for the next glfwCreateWindow call.
	glfwWindowHint(GLFW_SAMPLES, 4); //Antializing set to x4
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); //Set the OpenGL version to 4.x
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4); //Set the OpenGL version to 4.x
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //Set compatibility with newer versions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Set OpenGL core profile
	//Apple compatibility
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	// Open a window and create its OpenGL context
	GLFWwindow* window;
	window = glfwCreateWindow(screen_width, screen_height, "SimpleWindowwithColor", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window); // Set the windows to render
	// Load all OpenGL function pointers and check if GLAD works… 

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))

	{

		std::cout << "Failed to initialize GLAD" << std::endl;

		return -1;

	}
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Capture the mouse cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	//This loop renders the window we created above 
	Shader ourShader("shader.vs", "shader.fs");
	//Set the model
	modelLocation = glGetUniformLocation(ourShader.ID, "model");

	init();

	transformations(ourShader);

	glm::mat4 cameras[2] = { glm::mat4(1.0f), glm::mat4(1.0f) }; // view matrix initialization

	while (!glfwWindowShouldClose(window))

	{
		double currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);


		render();
		ourShader.use();
		transformations(ourShader);


		cameras[0] = glm::lookAt(cameraPos, cameraTarget, cameraUp);
		cameras[1] = glm::lookAt(cameraPos2, cameraTarget2, cameraUp2);

		ourShader.setMat4("view", cameras[currentCamera]);


		//Swap buffers 

		glfwSwapBuffers(window);

		static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };

		glClearBufferfv(GL_COLOR, 0, black);

		//Poll for Input/Output Events such a key pressed, mouse clicked etc... 

		glfwPollEvents();

	}

	//Terminate, clearing all previously allocated GLFW resources. 

	glfwTerminate();

	return 0;
}


void processInput(GLFWwindow* window)

{
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		int random = rand() % 2;
		if (!shape1Hidden && !shape2Hidden) {
			if (random == 0) shape1Hidden = true;
			else shape2Hidden = true;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
		if (!tKeyPressed) {
			angle -= 35;
			tKeyPressed = true; 
		}
	}
	else {
		tKeyPressed = false;
	}
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
		if (!leftMove && !rightMove) {
			leftMove = true; 
		}
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		shape1Hidden = false;
		shape2Hidden = false;
		angle = 0;
		shape1X = 0.0f;
		shape2X = 0.4f; 
		leftMove = false; // Reset movement
		rightMove = false; // Reset movement
	}

	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
		if (!hKeyPressed) {
			hKeyPressed = true;
			if (currentCamera == 0) {
				currentCamera = 1;
			}
			else {
				currentCamera = 0;
			}
		}
	}
	else hKeyPressed = false;

	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	// increate the camera speed using the deltaTime
	float cameraSpeed = 3 * (float)deltaTime;
	if (currentCamera == 0) {
		// forward movement
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			cameraPos += cameraSpeed * cameraTarget;
		// backward movement
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			cameraPos -= cameraSpeed * cameraTarget;
		// left movement
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			cameraPos -= glm::normalize(glm::cross(cameraTarget, cameraUp)) * cameraSpeed;
		// right movement
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			cameraPos += glm::normalize(glm::cross(cameraTarget, cameraUp)) * cameraSpeed;
	}
	else if (currentCamera == 1) {
		// forward movement
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			cameraPos2 += cameraSpeed * cameraTarget2;
		// backward movement
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			cameraPos2 -= cameraSpeed * cameraTarget2;
		// left movement
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			cameraPos2 -= glm::normalize(glm::cross(cameraTarget2, cameraUp2)) * cameraSpeed;
		// right movement
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			cameraPos2 += glm::normalize(glm::cross(cameraTarget2, cameraUp2)) * cameraSpeed;
	}
}

void init(void)

{	//Hexagon 1
	float vertices1[] = {
		0.3f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,// Center 
		0.1f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,// Left
		0.2f, 0.7f, 0.0f, 1.0f, 0.0f, 0.0f,// Top-left
		0.4f, 0.7f, 0.0f, 1.0f, 0.0f, 0.0f,// Top-right
		0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,// Right
		0.4f, 0.3f, 0.0f, 1.0f, 0.0f, 0.0f,// Bottom-right
		0.2f, 0.3f, 0.0f, 1.0f, 0.0f, 0.0f,// Bottom-left
	};
	//Hexagon 2
	float vertices2[] = {
		0.3f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,// Center
		0.1f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,// Left
		0.2f, 0.7f, 0.0f, 1.0f, 0.0f, 0.0f,// Top-left
		0.4f, 0.7f, 0.0f, 1.0f, 0.0f, 0.0f,// Top-right
		0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,// Right
		0.4f, 0.3f, 0.0f, 1.0f, 0.0f, 0.0f,// Bottom-right
		0.2f, 0.3f, 0.0f, 1.0f, 0.0f, 0.0f, // Bottom-left
	};

	int indices[] = {
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 5,
		0, 5, 6,
		0, 6, 1
	};


	//Hexagon 1
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glGenBuffers(1, &EBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
	glBindVertexArray(VAO);

	//position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//Hexagon 2
	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glBindVertexArray(VAO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);

	glGenBuffers(1, &EBO2);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
	glBindVertexArray(VAO2);

	//position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//glEnableVertexAttribArray(0);
}

void render() {
	static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glClearBufferfv(GL_COLOR, 0, black);
	//Assignment 1 functionality
	if (leftMove) {
		shape1X -= 0.001f; 
		shape2X -= 0.001f; 
	}
	if ((!shape1Hidden && !shape2Hidden && shape1X <= -1.1f)  || (shape1X <= -1.1f && shape2Hidden) || (shape2X <= -1.1f && shape1Hidden)) {
		leftMove = false;
		rightMove = true;
	}
	if (rightMove) {
		shape1X += 0.001f;
		shape2X += 0.001f; 
	}
	if ((!shape1Hidden && !shape2Hidden && shape2X >= 0.5f) || shape1X >= 0.5f && shape2Hidden || shape2X >= 0.5f && shape1Hidden) {
		rightMove = false;
	}
	///////
	///Shape 1
	glm::mat4 model1 = glm::mat4(1.0f);
	model1 = glm::scale(model1, glm::vec3(1.5f, 1.5f, 1.5f));
	model1 = glm::translate(model1, glm::vec3(shape1X+0.2, 0.0f, 0.0f));
	model1 = glm::rotate(model1, shape1Z, glm::vec3(0.0f, 0.0f, 1.0f));
	model1 = glm::rotate(model1, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model1));

	if (!shape1Hidden) {
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
	}
	//Shape 2
	glm::mat4 model2 = glm::mat4(1.0f);
	model2 = glm::scale(model2, glm::vec3(1.5f, 1.5f, 1.5f));
	model2 = glm::translate(model2, glm::vec3(shape2X+0.2, 0.0f, 0.0f));
	model2 = glm::rotate(model2, shape2Z, glm::vec3(0.0f, 0.0f, 1.0f));
	model2 = glm::rotate(model2, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model2));

	if (!shape2Hidden) {
		glBindVertexArray(VAO2);
		glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
	}
}

void transformations(Shader& ourShader) {
	glm::mat4 projection = glm::perspective(glm::radians(fov), (float)screen_width / (float)screen_height, 0.1f, 100.0f);
	ourShader.use();
	ourShader.setMat4("projection", projection);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}

	float xoffset = (float)xpos - lastX;
	float yoffset = lastY - (float)ypos; // reversed since y-coordinates go from bottom to top
	lastX = (float)xpos;
	lastY = (float)ypos;

	float sensitivity = 0.2f; // a ratio that define how sensitive the mouse movement will be
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	if (currentCamera == 0) {
		cameraTarget = glm::normalize(front);
	}
	else if (currentCamera == 1) {
		cameraTarget2 = glm::normalize(front);
	}
}

// glfw: callback called, when the mouse scroll wheel scrolls
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;
}
