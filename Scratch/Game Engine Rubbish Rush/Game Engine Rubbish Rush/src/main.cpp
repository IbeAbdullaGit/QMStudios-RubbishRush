#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <iostream>
//always include glad before glfw

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"	FragColor = vec4(0.8f,0.3f,0.02f,1.0f);\n"
"}\n\0";


int main() {

	//always write glfw before glad

	glfwInit();

	//will be using opengl 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* For program to run on apple/mac

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPACT, GL_TRUE);

	*/

	//Creating vertices point to create a shape
	GLfloat vertices[] =
	{
		-0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,
		0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,
		0.0f, 0.5f * float(sqrt(3)) * 2 / 3, 0.0f
	};


	GLFWwindow* window = glfwCreateWindow(1920, 1080, "Rubbish Rush", NULL, NULL);

	glfwMakeContextCurrent(window);

	//Loads out glad 
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);


	//ViewPort
	glViewport(0, 0, 1920, 1080);


	//Use unassigned integers to refrence Shaders object into the window
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);


	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);


	//Use a GLSHADER Program through a refrence value so the Shaders actually work in the window, they must be attached, use glLinkProgram to link the Shader into the window.

	GLuint ShaderProgram = glCreateProgram();

	glAttachShader(ShaderProgram, vertexShader);
	glAttachShader(ShaderProgram, fragmentShader);

	glLinkProgram(ShaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);



	//Creating the Object using Buffers VAO and VBO **VAO Allows to change between objects refrenced by VBO**  **VBO IS AN ARRAY OF REFRENCES|| CAN BE USED FOR MULTILPE OBJECTS** ORDER MATTERS

	GLuint VAO, VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//We store the vertces using DataBuffer... There are Three different bodies that can be made Stream, Static, Dynamic. 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	//VertexAttribute Pointers allows the communication between vertex shaders from outside
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);



	//All the render calculations are in this loop 

	float ptime = float(glfwGetTime());
	float angle = 0.0f;

	while (glfwWindowShouldClose(window) == 0) {

		//Escapes window with press the "x" button
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) //GLFW_PRESS OR GLFW_RELEASE both work similar
		{
			glfwSetWindowShouldClose(window, 1);
		}

		//glclearcolor and glclear focuses on changing, color, depth and stencil buffers
		//glClearColor changes the color using RGB and float numerals.


		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(ShaderProgram);
		glBindVertexArray(VAO);
		
		/*What draw the tirangle

		glDrawArrays(GL_TRIANGLES, 0, 3);
		*/
		
		/*
		Changing colors using time

		float time = float(glfwGetTime());

		if (time - ptime >= 0.1f) {
			angle += 0.1f;
			ptime = time;
		}

		glClearColor(float(sin(angle)), float(cos(angle)), float(tan(angle)), 1.0f);

		glClear(GL_COLOR_BUFFER_BIT);*/

		//always at the bottom of render loop
		glfwSwapBuffers(window);
		glfwPollEvents();
	};

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(ShaderProgram);

	glfwTerminate();
	return 0;
}
