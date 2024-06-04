/*
Niniejszy program jest wolnym oprogramowaniem; możesz go
rozprowadzać dalej i / lub modyfikować na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundację Wolnego
Oprogramowania - według wersji 2 tej Licencji lub(według twojego
wyboru) którejś z późniejszych wersji.

Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
ZASTOSOWAŃ.W celu uzyskania bliższych informacji sięgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <random>
#include "lodepng.h"
#include "shaderprogram.h"

#include "Camera.h"
#include "ObjLoader.h"
#include "Animation.h"

int screenWidth = 1280, screenHeight = 720;
float aspectRatio = (float)screenWidth / (float)screenHeight; // tangens do perspektywy

ShaderProgram *sp;
Camera camera(glm::vec3(0, 7, -4), 90, 0);

ObjLoader *layer;
ObjLoader *figure;
ObjLoader *plasmid;
ObjLoader *sphere;
ObjLoader *poster;
ObjLoader *mesosom;
ObjLoader *nucleoid;

vector<glm::vec3> *spherePositions;
float sphereSize = 0.125f;
glm::vec3 randomColor = { 1, 0, 0 };

Animation<float>* tailAnimation;
Animation<glm::vec3>* plasmidAnimation;
Animation<glm::vec3>* sphereAnimation;

bool layerOn[3] = { true, true, true };
bool renderTail = true;
bool tailAnimationOn = true;
bool plasmidAnimationOn = true;
bool sphereAnimationOn = true;
bool nuclTexture = true;
bool renderPlasmid = true;
bool renderSpheres = true;
bool renderMesosom = true;
bool renderNucleoid = true;

GLuint tailTexture;
GLuint plasmidTexture;
GLuint nucleoidTexture1;
GLuint nucleoidTexture2;
GLuint keysTexture;

void error_callback(int error, const char* description) 
{
	fputs(description, stderr);
}

bool isPink(const glm::vec3 &color) 
{
	return (color.r > 0.4) && (color.g < 1.) && (color.b > 0.1);
}

glm::vec3 generateRandomColor() 
{
	glm::vec3 color;

	std::random_device rd;  // urządzenie losujące do seeda
	std::mt19937 gen(rd()); // generator Mersenne Twister z seedem
	std::uniform_int_distribution<> distrib(0, 255);

	do {
		color.r = ((float)distrib(gen) / 255.f);
		color.g = ((float)distrib(gen) / 255.f);
		color.b = ((float)distrib(gen) / 255.f);
	} while (isPink(color));

	return color;
}

void keyCallback(GLFWwindow* window,int key,int scancode,int action,int mods) 
{
	if (action==GLFW_PRESS) {
		if (key == GLFW_KEY_1)
			layerOn[0] = !layerOn[0];
		if (key == GLFW_KEY_2)
			layerOn[1] = !layerOn[1];
		if (key == GLFW_KEY_3)
			layerOn[2] = !layerOn[2];
		if (key == GLFW_KEY_4)
			renderTail = !renderTail;
		if (key == GLFW_KEY_5)
			renderPlasmid = !renderPlasmid;
		if (key == GLFW_KEY_6)
			renderSpheres = !renderSpheres;
		if (key == GLFW_KEY_7)
			renderMesosom = !renderMesosom;
		if (key == GLFW_KEY_8)
			renderNucleoid = !renderNucleoid;
		if (key == GLFW_KEY_F)
			tailAnimationOn = !tailAnimationOn;
		if (key == GLFW_KEY_G)
			plasmidAnimationOn = !plasmidAnimationOn;
		if (key == GLFW_KEY_H)
			sphereAnimationOn = !sphereAnimationOn;
		if (key == GLFW_KEY_R)
			randomColor = generateRandomColor();
		if (key == GLFW_KEY_T)
			nuclTexture = !nuclTexture;
    }

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		sphereSize = glm::max(0.05f, sphereSize - 0.01f);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		sphereSize = glm::min(0.20f, sphereSize + 0.01f);
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void windowResizeCallback(GLFWwindow* window,int width,int height) 
{
    if (height==0) return;
    aspectRatio=(float)width/(float)height;
    glViewport(0,0,width,height);
	screenWidth = width;
	screenHeight = height;
}


GLuint readTexture(const char* filename) 
{
    GLuint tex;
    glActiveTexture(GL_TEXTURE0);

    //Wczytanie do pamięci komputera
    std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
    unsigned width, height;   //Zmienne do których wczytamy wymiary obrazka
    //Wczytaj obrazek
    unsigned error = lodepng::decode(image, width, height, filename);

    //Import do pamięci karty graficznej
    glGenTextures(1, &tex); //Zainicjuj jeden uchwyt
    glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
    //Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
    glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return tex;
}

void initOpenGLProgram(GLFWwindow* window) 
{
	glClearColor(0,0,0,1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(window,windowResizeCallback);
	glfwSetKeyCallback(window,keyCallback);

	sp = new ShaderProgram("v_multiple_lights.glsl", nullptr, "f_multiple_lights.glsl");

	tailTexture = readTexture("textures\\tail_texture.png");
	plasmidTexture = readTexture("textures\\plasmid_texture.png");
	nucleoidTexture1 = readTexture("textures\\nucleoid_texture_1.png");
	nucleoidTexture2 = readTexture("textures\\nucleoid_texture_2.png");
	keysTexture = readTexture("textures\\keys2.png");

	layer = new ObjLoader("Models\\layer3.obj");
	figure = new ObjLoader("Models\\figure.obj");
	plasmid = new ObjLoader("Models\\plasmid.obj");
	sphere = new ObjLoader("Models\\sphere.obj");
	poster = new ObjLoader("Models\\poster.obj");
	mesosom = new ObjLoader("Models\\mesosom.obj");
	nucleoid = new ObjLoader("Models\\nucleoid.obj");

	spherePositions = new vector<glm::vec3>({
		{  0.40 , -2.00  ,  0.50  },        // srodek srodek dol
		{ -0.20 ,  1.75  , -0.20  },        // srodek srodek gora
		{  0.05 , -0.80  ,  1.60  },        // srodek lewo dol
		{  0.00 ,  1.40  ,  1.35  },        // srodek lewo gora
		{ -0.10 , -0.80  , -1.45  },        // srodek prawo dol
		{ -0.05 ,  1.10  , -1.55  },        // srodek prawo gora

		{  1.55 , -1.60  ,  0.00  },        // dol srodek dol
		{  1.66 ,  1.25  ,  0.00  },        // dol srodek gora
		{  1.52 , -1.05  ,  1.45  },        // dol lewo dol
		{  1.46 ,  0.70  ,  1.55  },        // dol lewo gora
		{  1.35 , -1.10  , -1.66  },        // dol prawo dol
		{  1.75 ,  0.50  , -1.75  },        // dol prawo gora

		{ -1.46 , -1.75  ,  0.00  },        // gora srodek dol
		{ -1.52 ,  1.55  ,  0.00  },        // gora srodek gora
		{ -1.66 , -0.90  ,  1.55  },        // gora lewo dol
		{ -1.55 ,  0.90  ,  1.66  },        // gora lewo gora
		{ -1.75 , -0.80  , -1.75  },        // gora prawo dol
		{ -1.35 ,  1.00  , -1.45  },        // gora prawo gora

		{ -2.46 , -1.75  ,  0.00  },        // ekstragora srodek dol
		{ -2.52 ,  1.55  ,  0.00  },        // ekstragora srodek gora
		{ -2.66 , -0.90  ,  1.55  },        // ekstragora lewo dol
		{ -2.55 ,  0.90  ,  1.66  },        // ekstragora lewo gora
		{ -2.75 , -0.80  , -1.75  },        // ekstragora prawo dol
		{ -2.35 ,  1.00  , -1.45  },        // ekstragora prawo gora

		{  2.55 , -1.60  ,  0.00  },        // ekstradol srodek dol
		{  2.66 ,  1.25  ,  0.00  },        // ekstradol srodek gora
		{  2.52 , -1.05  ,  1.45  },        // ekstradol lewo dol
		{  2.46 ,  0.70  ,  1.55  },        // ekstradol lewo gora
		{  2.35 , -1.10  , -1.66  },        // ekstradol prawo dol
		{  2.75 ,  0.50  , -1.75  },        // ekstradol prawo gora

		{ -3.46 , -1.75  ,  0.00  },        // ekstraekstragora srodek dol
		{ -3.52 ,  1.55  ,  0.00  },        // ekstraekstragora srodek gora
		{ -3.66 , -0.80  ,  1.55  },        // ekstraekstragora lewo dol
		{ -3.55 ,  0.80  ,  1.66  },        // ekstraekstragora lewo gora
		{ -3.75 , -0.75  , -1.75  },        // ekstraekstragora prawo dol
		{ -3.35 ,  0.90  , -1.45  },        // ekstraekstragora prawo gora

		{  3.55 , -1.60  ,  0.00  },        // ekstraekstradol srodek dol
		{  3.66 ,  1.25  ,  0.00  },        // ekstraekstradol srodek gora
		{  3.52 , -0.95  ,  1.45  },        // ekstraekstradol lewo dol
		{  3.46 ,  0.60  ,  1.55  },        // ekstraekstradol lewo gora
		{  3.35 , -1.00  , -1.66  },        // ekstraekstradol prawo dol
		{  3.75 ,  0.45  , -1.75  },        // ekstraekstradol prawo gora
	});
	
	tailAnimation = new Animation<float>(2, 0, 2.f);
	tailAnimation->m_vec[0] = -45;
	tailAnimation->m_vec[1] = 45;

	plasmidAnimation = new Animation<glm::vec3>(4, glm::vec3(0), 1.f);
	plasmidAnimation->m_vec[0] = glm::vec3(-2, 0, -2);
	plasmidAnimation->m_vec[1] = glm::vec3(2, 0, -2);
	plasmidAnimation->m_vec[2] = glm::vec3(2, 0, 2);
	plasmidAnimation->m_vec[3] = glm::vec3(-2, 0, 2);

	sphereAnimation = new Animation<glm::vec3>(4, glm::vec3(0), 1.5f);
	sphereAnimation->m_vec[0] = glm::vec3(-0.125, 0, -0.125);
	sphereAnimation->m_vec[1] = glm::vec3(0.125, 0, -0.125);
	sphereAnimation->m_vec[2] = glm::vec3(0.125, 0, 0.125);
	sphereAnimation->m_vec[3] = glm::vec3(-0.125, 0, 0.125);
}

void freeOpenGLProgram(GLFWwindow* window) 
{
	delete sp;
	delete layer;
	delete figure;
	delete plasmid;
	delete sphere;
	delete poster;
	delete mesosom;
	delete spherePositions;
	delete tailAnimation;
	delete plasmidAnimation;
	delete sphereAnimation;
	delete nucleoid;
}

void drawFibriaSegment(float x, float startRot)
{
	for (float rot = 0.f; rot < 360.f; rot += 30.f)
	{
		glm::mat4 M = glm::mat4(1);
		M = glm::scale(M, glm::vec3(0.15));
		M = glm::rotate(M, glm::radians(startRot + rot), glm::vec3(1, 0, 0));
		M = glm::translate(M, glm::vec3(x, 0.f, 22.5f));

		glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
		glUniform1i(sp->u("textureOn"), 1);

		glUniform1i(sp->u("material.diffuse"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tailTexture);

		figure->draw();
	}
}

void drawFullFibria()
{
	for (int i = 0; i < 7; ++i)
	{ 
		drawFibriaSegment(-30 + i * 10, i * 10);
	}
}

void drawMesosom()
{
	glm::mat4 M = glm::mat4(1);
	M = glm::translate(M, glm::vec3(0.f, 0.f, -2.4f));
	M = glm::scale(M, glm::vec3(0.0625f));
	M = glm::rotate(M, glm::radians(90.f), glm::vec3(1, 0, 0));

	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
	glUniform1i(sp->u("textureOn"), 1);

	glUniform1i(sp->u("material.diffuse"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, (!nuclTexture ? nucleoidTexture1 : nucleoidTexture2));

	mesosom->draw();
}

void drawSpheres()
{
	float time = glfwGetTime();

	for (auto sphrPos : *spherePositions)
	{
		glm::vec3 movePos(0.f);

		if (sphereAnimationOn)
			movePos = sphereAnimation->getVal(time);

		glm::mat4 M = glm::translate(glm::mat4(1.f), sphrPos + movePos);
		M = glm::scale(M, glm::vec3(sphereSize));

		glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
		glUniform1i(sp->u("textureOn"), 0);
		glUniform3f(sp->u("color"), randomColor.r, randomColor.g, randomColor.b);

		sphere->draw();

		time += 0.2664;
	}
}

void drawNucleoid()
{
	for (float rot = 0.f, rot2 = 0.f; rot <= 135.f; rot += 45.f, rot2 += 180.f)
	{
		glm::mat4 M = glm::mat4(1);
		M = glm::scale(M, glm::vec3(0.078));
		M = glm::rotate(M, glm::radians(rot), glm::vec3(1, 0, 0));
		M = glm::rotate(M, glm::radians(rot2), glm::vec3(0, 1, 0));

		glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
		glUniform1i(sp->u("textureOn"), 1);

		glUniform1i(sp->u("material.diffuse"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, (nuclTexture ? nucleoidTexture1 : nucleoidTexture2));

		nucleoid->draw();
	}
}

void drawPlasmid(glm::vec3 vec)
{
	glm::mat4 M = glm::scale(glm::mat4(1.f), glm::vec3(0.1f));
	M = glm::translate(M, vec);
	if (plasmidAnimationOn)
	{
		M = glm::translate(M, plasmidAnimation->getVal(glfwGetTime()));
	}

	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
	glUniform1i(sp->u("textureOn"), 1);

	glUniform1i(sp->u("material.diffuse"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, plasmidTexture);

	plasmid->draw();
}

void drawTail()
{
	glm::mat4 M = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
	M = glm::rotate(M, glm::radians(90.f), glm::vec3(0, 1, 0));
	M = glm::translate(M, glm::vec3(-0.5, 0, 10));

	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
	glUniform1i(sp->u("textureOn"), 1);

	glUniform1i(sp->u("material.diffuse"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tailTexture);

	figure->draw();

	M = glm::translate(M, glm::vec3(0, 0, 10));
	if (tailAnimationOn)
		M = glm::rotate(M, (float)glm::radians(tailAnimation->getVal(glfwGetTime())), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
	figure->draw();
}

void drawLayer(float scale, glm::vec3 color = glm::vec3(0)) //  1 - texture, 0 - color
{
	glm::mat4 M = glm::mat4(1.0f);
	M = glm::scale(M, glm::vec3(scale));
	M = glm::translate(M, glm::vec3(-12.f, 0.f, 0.f));

	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
	glUniform1i(sp->u("textureOn"), 0);
	glUniform3f(sp->u("color"), color.r, color.g, color.b);

	layer->draw();
}

void drawCell()
{
	if (layerOn[0])
	{
		drawLayer(0.35, glm::vec3(0, 0, 1));
		drawFullFibria();
	}

	if (layerOn[1])
		drawLayer(0.30, glm::vec3(0, 1, 0));
	if (layerOn[2])
		drawLayer(0.25, glm::vec3(1, 0, 0));

	if (renderTail)
	{
		drawTail();
	}

	if (renderPlasmid)
	{
		drawPlasmid(glm::vec3(-45, 0, -3));
		drawPlasmid(glm::vec3(40, 0, -3));
	}
	if (renderNucleoid)
		drawNucleoid();
	if (renderSpheres)
		drawSpheres();
	if (renderMesosom)
		drawMesosom();
}

void drawPoster()
{
	glm::mat4 M(1);
	M = glm::translate(M, glm::vec3(6, 0, 10));
	M = glm::scale(M, glm::vec3(12.f));
	M = glm::rotate(M, glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));

	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
	glUniform1i(sp->u("textureOn"), -1);

	glUniform1i(sp->u("material.diffuse"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, keysTexture);

	poster->draw();
}

void drawScene(GLFWwindow* window)
{
	float num = 0.76470588235f;
	glClearColor(num, num, num, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 V = camera.getViewMatrix();
	glm::mat4 P = glm::perspective(glm::radians(45.f), aspectRatio, 0.01f, 400.0f);

	glm::vec3 cameraPos = camera.getPos();
	glm::vec3 cameraFront = camera.getFront();

	sp->use();
	glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));


	glUniform1i(sp->u("material.diffuse"), 0);
	glUniform1f(sp->u("material.shininess"), 32.0f);
	glUniform3f(sp->u("viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glUniform3f(sp->u("pointLights[0].position"), 0, 0, 0);
	glUniform3f(sp->u("pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
	glUniform3f(sp->u("pointLights[0].diffuse"), 0.8f, 0.8f, 0.8f);
	glUniform1f(sp->u("pointLights[0].constant"), 1.0f);
	glUniform1f(sp->u("pointLights[0].linear"), 0.09f);
	glUniform1f(sp->u("pointLights[0].quadratic"), 0.032f);
	
	glUniform3f(sp->u("pointLights[1].position"), 12.5, 3, 0);
	glUniform3f(sp->u("pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
	glUniform3f(sp->u("pointLights[1].diffuse"), 0.8f, 0.8f, 0.8f);
	glUniform1f(sp->u("pointLights[1].constant"), 1.0f);
	glUniform1f(sp->u("pointLights[1].linear"), 0.09f);
	glUniform1f(sp->u("pointLights[1].quadratic"), 0.032f);


	glUniform3f(sp->u("spotLight.position"), cameraPos.x, cameraPos.y, cameraPos.z);
	glUniform3f(sp->u("spotLight.direction"), cameraFront.x, cameraFront.y, cameraFront.z);
	glUniform3f(sp->u("spotLight.ambient"), 0.05f, 0.05f, 0.05f);
	glUniform3f(sp->u("spotLight.diffuse"), 0.8f, 0.8f, 0.8f);
	glUniform1f(sp->u("spotLight.constant"), 1.0f);
	glUniform1f(sp->u("spotLight.linear"), 0.015f);
	glUniform1f(sp->u("spotLight.quadratic"), 0.01f);

	glUniform1f(sp->u("spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
	glUniform1f(sp->u("spotLight.outerCutOff"), glm::cos(glm::radians(15.0f)));

	drawCell();
	drawPoster();

	glfwSwapBuffers(window); //Przerzuć tylny bufor na przedni
}


int main(void)
{
	std::srand(std::time(0)); // Inicjalizacja generatora liczb losowych

	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(screenWidth, screenHeight, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjujące

	glfwSetTime(0); //Zeruj timer
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
		camera.update(window, 1.f/144.f);
		drawScene(window); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}
