#include "application.h"
#include "utils.h"
#include "includes.h"
#include "utils.h"

#include "image.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"
#include "camera.h"
#include "material.h"
#include "light.h"

#define num_lights 3

Camera* camera = NULL;
Mesh* mesh = NULL;
Shader* shader = NULL;

//might be useful...
Material* material = NULL;
Light* light = NULL;
Light* lights[num_lights] = {NULL};
Shader* phong_shader = NULL;
Shader* gouraud_shader = NULL;
int type_shader = 0;

Vector3 ambient_light(0.1,0.2,0.3); //here we can store the global ambient light of the scene

float angle = 0;

Application::Application(const char* caption, int width, int height)
{
	this->window = createWindow(caption, width, height);

	// initialize attributes
	// Warning: DO NOT CREATE STUFF HERE, USE THE INIT 
	// things create here cannot access opengl
	int w,h;
	SDL_GetWindowSize(window,&w,&h);

	this->window_width = w;
	this->window_height = h;
	this->keystate = SDL_GetKeyboardState(NULL);
}

//Here we have already GL working, so we can create meshes and textures
void Application::init(void)
{
	std::cout << "initiating app..." << std::endl;
	
	//here we create a global camera and set a position and projection properties
	camera = new Camera();
	camera->lookAt(Vector3(0,20,20),Vector3(0,10,0),Vector3(0,1,0));
	camera->setPerspective(60,window_width / window_height,0.1,10000);

	//then we load a mesh
	mesh = new Mesh();
	if( !mesh->loadOBJ( "lee.obj" ) )
		std::cout << "FILE Lee.obj NOT FOUND " << std::endl;

	//we load one or several shaders...
	shader = Shader::Get( "simple.vs", "simple.ps" );

	//load your Gouraud and Phong shaders here and stored them in some global variables
	gouraud_shader = new Shader();
	phong_shader = new Shader();

	gouraud_shader = Shader::Get("gouraud.vs", "gouraud.ps");
	phong_shader = Shader::Get("phong.vs", "phong.ps");

	//CODE HERE:
	//create a light (or several) and and some materials
	light = new Light();
	material = new Material();

	//Array of lights, init [0] 
	lights[0] = light;
}

//render one frame
void Application::render(void){
	//update the aspect of the camera acording to the window size
	camera->aspect = window_width / window_height;
	camera->updateProjectionMatrix();
	//Get the viewprojection matrix from our camera
	Matrix44 viewprojection = camera->getViewProjectionMatrix();

	//set the clear color of the colorbuffer as the ambient light so it matches
	glClearColor(ambient_light.x, ambient_light.y, ambient_light.z, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear framebuffer and depth buffer 
	glEnable( GL_DEPTH_TEST ); //enable depth testing for occlusions
	glDepthFunc(GL_LEQUAL); //Z will pass if the Z is LESS or EQUAL to the Z of the pixel

	//choose a shader and enable it
	shader->enable();

	Matrix44 model_matrix;
	model_matrix.setIdentity();
	model_matrix.translate(0,0,0); //example of translation
	model_matrix.rotate(angle, Vector3(0, 1, 0));
	shader->setMatrix44("model", model_matrix); //upload the transform matrix to the shader
	shader->setMatrix44("viewprojection", viewprojection); //upload viewprojection info to the shader
	//
	//choose a shader and enable it
	if (type_shader == 1) {
		shader = phong_shader;
	}
	if (type_shader == 0) {
		shader = gouraud_shader;
	}
	//

	//CODE HERE: pass all the info needed by the shader to do the computations
	//send the material and light uniforms to the shader
	shader->setVector3("Ka", material->ambient);
	shader->setVector3("Kd", material->diffuse);
	shader->setVector3("Ks", material->specular);
	shader->setFloat("alpha", material->shininess);
	
	shader->setVector3("Ia",ambient_light);
	shader->setVector3("Id", light->diffuse_color);
	shader->setVector3("Is", light->specular_color);
	shader->setVector3("lp", light->position);

	shader->setFloat("cont",1);
	Vector3 m(20, 0, 30);
	shader->setVector3("lp1", m);
	shader->setVector3("eye", camera->eye);
	shader->setFloat("blend", 1);
	//---------------------------------------------------------------------------------
	//do the draw call into the GPU
	mesh->render(GL_TRIANGLES);

	//disable shader when we do not need it any more
	shader->disable();

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

//called after render
void Application::update(double seconds_elapsed)
{
	if (keystate[SDL_SCANCODE_SPACE])
		angle += seconds_elapsed;

	if (keystate[SDL_SCANCODE_P]) {
		type_shader = 1;

	}

	if (keystate[SDL_SCANCODE_G]) {
		type_shader = 0;

	}
	//EYE
	if (keystate[SDL_SCANCODE_RIGHT]) {
		camera->eye = camera->eye + Vector3(1, 0, 0) * seconds_elapsed * 10.0;

	}
	if (keystate[SDL_SCANCODE_LEFT]) {
		camera->eye = camera->eye + Vector3(-1, 0, 0) * seconds_elapsed * 10.0;
	}
	if (keystate[SDL_SCANCODE_UP]) {
		camera->eye = camera->eye + Vector3(0, 1, 0) * seconds_elapsed * 10.0;
	}
	
	if (keystate[SDL_SCANCODE_DOWN]) {
		camera->eye = camera->eye + Vector3(0, -1, 0) * seconds_elapsed * 10.0;
	}

	//CENTER
	/*if (keystate[SDL_SCANCODE_W]) {
			camera->center = camera->center + Vector3(0, 1, 0) * seconds_elapsed * 10.0;
		}
	if (keystate[SDL_SCANCODE_S]) {
			camera->center = camera->center + Vector3(0, -1, 0) * seconds_elapsed * 10.0;
		}
	if (keystate[SDL_SCANCODE_A]) {
			camera->center = camera->center + Vector3(-1, 0, 0) * seconds_elapsed * 10.0;
		}
	if (keystate[SDL_SCANCODE_D]) {
			camera->center = camera->center + Vector3(1, 0, 0) * seconds_elapsed * 10.0;

		}*/
	

	//Control array of lights using keyboard
	if (keystate[SDL_SCANCODE_D]) {
		lights[0]->position = lights[0]->position + Vector3(5, 0, 0) * seconds_elapsed * 10.0;

	}
	if (keystate[SDL_SCANCODE_A]) {
		lights[0]->position = lights[0]->position + Vector3(-5, 0, 0) * seconds_elapsed * 10.0;
	}
	if (keystate[SDL_SCANCODE_W]) {
		lights[0]->position = lights[0]->position + Vector3(0, 5, 0) * seconds_elapsed * 10.0;
	}
	if (keystate[SDL_SCANCODE_S]) {
		lights[0]->position = lights[0]->position + Vector3(0, -5, 0) * seconds_elapsed * 10.0;
	}


}

//keyboard press event 
void Application::onKeyPressed( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		case SDLK_ESCAPE: exit(0); break; //ESC key, kill the app
		case SDLK_r: 
			Shader::ReloadAll();
			break; //ESC key, kill the app
	}
}

//mouse button event
void Application::onMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) //left mouse pressed
	{
	}

}

void Application::onMouseButtonUp( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) //left mouse unpressed
	{

	}
}

//when the app starts
void Application::start()
{
	std::cout << "launching loop..." << std::endl;
	launchLoop(this);
}
