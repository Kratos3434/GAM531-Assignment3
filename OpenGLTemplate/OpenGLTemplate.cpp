#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shaderinit.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "camera.h"

void processInput(GLFWwindow* window);
void init(void);
void render(glm::mat4, glm::mat4);
void transformations(Shader& ourShader);

void mouse_callback(GLFWwindow*, double, double);
void scroll_callback(GLFWwindow*, double, double);

void renderTexture();
unsigned int loadCubemap(std::vector<std::string>);

//Screen Dimensions Variables
const unsigned int screen_width = 1200;
const unsigned int screen_height = 800;
//Number of Vertices
const GLuint NumVertices = 6;
int shaderProgram;
GLuint VBO, VBO2, geoVBO, geoVAO;
GLuint VAO, VAO2;
GLuint EBO, EBO2;

unsigned int skyboxVAO, skyboxVBO;

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
//glm::vec3 cameraPos = glm::vec3(0.5f, 0.0f, 3.0f);
//glm::vec3 cameraTarget = glm::vec3(0.1f, 0.1f, -1.0f);
//glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

//glm::vec3 cameraPos2 = glm::vec3(5.0f, 0.0f, 3.0f);
//glm::vec3 cameraTarget2 = glm::vec3(0.1f, 0.1f, -1.0f);
//glm::vec3 cameraUp2 = glm::vec3(0.0f, 1.0f, 0.0f);

Camera camera1(glm::vec3(0.5f, 0.0f, 3.0f));
Camera camera2(glm::vec3(5.0f, 0.0f, 3.0f));
Camera cameras[] = {camera1, camera2};

//texture
unsigned int texture;
int width, height, nrChannels;

unsigned int currentCamera = 0;
unsigned int cubemapTexture = 0;

//Shader shader("cubemapshader.vs", "cubemapshader.fs");
//Shader skyboxShader("skyboxshader.vs", "skyboxshader.fs");

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

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	// Specify the depth test function
	glDepthFunc(GL_LESS); // Passes if the incoming depth value is less than the stored one
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//This loop renders the window we created above 
	Shader ourShader("shader.vs", "shader.fs");

	glPointSize(20.0f);
	//Set the model
	modelLocation = glGetUniformLocation(ourShader.ID, "model");

	init();

	renderTexture();

	//ourShader.use();

	//glUniform1i(glGetUniformLocation(ourShader.ID, "texture"), 0);

	transformations(ourShader);
	Shader skyboxShader("skyboxshader.vs", "skyboxshader.fs");
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	//Shader geometryShader("geometryShader.vs", "geometryShader.fs", "geometryShader.gs");

	while (!glfwWindowShouldClose(window))

	{
		double currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);

		glm::mat4 view = cameras[currentCamera].GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(cameras[currentCamera].Zoom), (float)screen_width / (float)screen_height, 0.1f, 100.0f);
		ourShader.setMat4("view", view);


		//geometryShader.use();


		render(view, projection);
		ourShader.use();
		transformations(ourShader);




		//Swap buffers 

		glfwSwapBuffers(window);

		static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };

		glClearBufferfv(GL_COLOR, 0, black);
		glClear(GL_COLOR_BUFFER_BIT);

		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

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
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameras[currentCamera].ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameras[currentCamera].ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameras[currentCamera].ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameras[currentCamera].ProcessKeyboard(RIGHT, deltaTime);

}

void init(void)

{	//Hexagon 1
	//float vertices1[] = {
	//	0.3f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,// Center 
	//	0.1f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,// Left
	//	0.2f, 0.7f, 0.0f, 1.0f, 0.0f, 0.0f,// Top-left
	//	0.4f, 0.7f, 0.0f, 1.0f, 0.0f, 0.0f,// Top-right
	//	0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,// Right
	//	0.4f, 0.3f, 0.0f, 1.0f, 0.0f, 0.0f,// Bottom-right
	//	0.2f, 0.3f, 0.0f, 1.0f, 0.0f, 0.0f,// Bottom-left
	//};
	float vertices1[] = {                   //Texture coords
		0.4f, 0.4f, 0.6f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.6f, 0.4f, 0.6f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.6f, 0.6f, 0.6f, 1.0f, 0.0f, 0.0f,	1.0f, 1.0f,
		0.4f, 0.6f, 0.6f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,

		0.4f, 0.4f, 0.4f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		0.6f, 0.4f, 0.4f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		0.6f, 0.6f, 0.4f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		0.4f, 0.6f, 0.4f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	};
	// 
	//Hexagon 2
	//float vertices2[] = {
	//	0.3f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,// Center
	//	0.1f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,// Left
	//	0.2f, 0.7f, 0.0f, 1.0f, 0.0f, 0.0f,// Top-left
	//	0.4f, 0.7f, 0.0f, 1.0f, 0.0f, 0.0f,// Top-right
	//	0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,// Right
	//	0.4f, 0.3f, 0.0f, 1.0f, 0.0f, 0.0f,// Bottom-right
	//	0.2f, 0.3f, 0.0f, 1.0f, 0.0f, 0.0f, // Bottom-left
	//};
	float vertices2[] = {                   //Texture coords
		0.4f, 0.4f, 0.6f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.6f, 0.4f, 0.6f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.6f, 0.6f, 0.6f, 1.0f, 0.0f, 0.0f,	1.0f, 1.0f,
		0.4f, 0.6f, 0.6f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,

		0.4f, 0.4f, 0.4f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		0.6f, 0.4f, 0.4f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		0.6f, 0.6f, 0.4f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		0.4f, 0.6f, 0.4f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	};

	//int indices[] = {
	//	0, 1, 2,
	//	0, 2, 3,
	//	0, 3, 4,
	//	0, 4, 5,
	//	0, 5, 6,
	//	0, 6, 1
	//};

	int indices[] = {
		0, 1, 2,
		2, 3, 0,

		4, 5, 6,
		6, 7, 4,

		0, 4, 7,
		7, 3, 0,

		1, 5, 6,
		6, 2, 1,

		3, 2, 6,
		6, 7, 3,

		0, 1, 5,
		5, 4, 0
	};

	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	float points[] = {
		-0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // top-left
		 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // top-right
		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // bottom-right
		-0.5f, -0.5f, 1.0f, 1.0f, 0.0f  // bottom-left
	};

	glGenBuffers(1, &geoVBO);
	glGenVertexArrays(1, &geoVAO);
	glBindVertexArray(geoVAO);
	glBindBuffer(GL_ARRAY_BUFFER, geoVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
	glBindVertexArray(0);

	//Hexagon 1
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glGenBuffers(1, &EBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
	glBindVertexArray(VAO);

	//position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// texture coord attribute information
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// texture coord attribute information
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	std::vector<std::string> faces
	{
		("assets/right.jpg"),
		("assets/left.jpg"),
		("assets/top.jpg"),
		("assets/bottom.jpg"),
		("assets/front.jpg"),
		("assets/back.jpg")
	};
	cubemapTexture = loadCubemap(faces);

	//glEnableVertexAttribArray(0);
}

void render(glm::mat4 view, glm::mat4 projection) {
	static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glClearBufferfv(GL_COLOR, 0, black);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Assignment 1 functionality
	if (leftMove) {
		shape1X -= 0.001f;
		shape2X -= 0.001f;
	}
	if ((!shape1Hidden && !shape2Hidden && shape1X <= -1.1f) || (shape1X <= -1.1f && shape2Hidden) || (shape2X <= -1.1f && shape1Hidden)) {
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
	model1 = glm::translate(model1, glm::vec3(shape1X + 0.2, 0.0f, 0.0f));
	model1 = glm::rotate(model1, shape1Z, glm::vec3(0.0f, 0.0f, 1.0f));
	model1 = glm::rotate(model1, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model1));

	if (!shape1Hidden) {
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	//Shape 2
	glm::mat4 model2 = glm::mat4(1.0f);
	model2 = glm::scale(model2, glm::vec3(1.5f, 1.5f, 1.5f));
	model2 = glm::translate(model2, glm::vec3(shape2X + 0.2, 0.0f, 0.0f));
	model2 = glm::rotate(model2, shape2Z, glm::vec3(0.0f, 0.0f, 1.0f));
	model2 = glm::rotate(model2, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model2));

	if (!shape2Hidden) {
		glBindVertexArray(VAO2);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	Shader geometryShader("geometryShader.vs", "geometryShader.fs", "geometryShader.gs");
	geometryShader.use();
	geometryShader.setMat4("view", view);
	geometryShader.setMat4("projection", projection);
	glBindVertexArray(geoVAO);
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);

	// draw skybox as last
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	Shader skyboxShader("skyboxshader.vs", "skyboxshader.fs");
	skyboxShader.use();
	view = glm::mat4(glm::mat3(cameras[currentCamera].GetViewMatrix())); // remove translation from the view matrix

	skyboxShader.setMat4("view", view);
	transformations(skyboxShader);

	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
}
void transformations(Shader& ourShader) {
	glm::mat4 projection = glm::perspective(glm::radians(cameras[currentCamera].Zoom), (float)screen_width / (float)screen_height, 0.1f, 100.0f);
	ourShader.use();
	ourShader.setMat4("projection", projection);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	cameras[currentCamera].ProcessMouseMovement(xoffset, yoffset);
}

// glfw: callback called, when the mouse scroll wheel scrolls
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	cameras[currentCamera].ProcessMouseScroll(static_cast<float>(yoffset));
}

void renderTexture() {
	// texture 1
	// ---------
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image (smilie.png) and create the texture 
	unsigned char* data = stbi_load("assets/mc_view.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}