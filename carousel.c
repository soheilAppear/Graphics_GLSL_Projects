/** @file Demonstrates the Carousel Transparent and non Transparent objects.
 *
 * @author Soheil Sepahyar
 */

#include "libkuhl.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

static GLuint program = 0; /**< id value for the GLSL program */

static kuhl_geometry quad;

static int n = 5;

GLuint texId [5] = {0,1,2,3,4};
float aspectRatio [5];

char **p;

 int size_of_array = 0;



float modelview_total [5][16];  // storing Model View Matrix into our array of each quad

float distance [5]; // stroing z value of each quad


//////////////////////////  Duck Part

kuhl_geometry *modelgeom = NULL;

float model[16] = { 1,0,0,0,0,1,0,0,0,0,1,0,-.081,-.525,.022,1};

// Load the model from the file
const char *modelFile = "../models/duck/duck.dae";

static GLuint program_duck = 1; // Duck GLSL Program


/* Called by GLFW whenever a key is pressed. */
void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)  // get the user argument here
{
	/* If the library handles this keypress, return */
	if (kuhl_keyboard_handler(window, key, scancode, action, mods))
		return;
	/* custom key handling code here */
}



/** Draws the 3D scene. */
void display()
{
	/* Render the scene once for each viewport. Frequently one
	 * viewport will fill the entire screen. However, this loop will
	 * run twice for HMDs (once for the left eye and once for the
	 * right). */

	viewmat_begin_frame();
	for(int viewportID=0; viewportID <viewmat_num_viewports(); viewportID++)
	{
		viewmat_begin_eye(viewportID);

		/* Where is the viewport that we are drawing onto and what is its size? */
		int viewport[4]; // x,y of lower left corner, width, height
		viewmat_get_viewport(viewport, viewportID);
		/* Tell OpenGL the area of the window that we will be drawing in. */
		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

		/* Clear the current viewport. Without glScissor(), glClear()
		 * clears the entire screen. We could call glClear() before
		 * this viewport loop---but in order for all variations of
		 * this code to work (Oculus support, etc), we can only draw
		 * after viewmat_begin_eye(). */
		glScissor(viewport[0], viewport[1], viewport[2], viewport[3]);
		glEnable(GL_SCISSOR_TEST);
		
		
		//glClearColor(.2,.2,.2,0); // set clear color to grey
		glClearColor(1 , 1, 1, 1); // set clear color to white and opacity is one RE ****


		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glDisable(GL_SCISSOR_TEST);
		glEnable(GL_DEPTH_TEST); // turn on depth testing
		kuhl_errorcheck();

		/* Turn on blending (note, if you are using transparent textures,
		   the transparency may not look correct unless you draw further
		   items before closer items. This program always draws the
		   geometry in the same order.). */
		glEnable(GL_BLEND);
		glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

//////////////////////////////////////////////////////////////////////////////////////////////////

		float viewMatD[16], perspectiveD[16];
		viewmat_get(viewMatD, perspectiveD, viewportID);


		float scaleMatrixD[16];
		mat4f_scale_new(scaleMatrixD, 1, 1, 1);

		float translate_matD[16];
		mat4f_translate_new(translate_matD, 0 , -1 , 0);

		float modelviewD[16];
		mat4f_mult_mat4f_new(modelviewD, viewMatD, model);
		kuhl_errorcheck();
		
		//mat4f_mult_mat4f_new(modelviewD, viewMatD, scaleMatrixD);   // why scale matrix is not working fro Duck Model ??!!
		mat4f_mult_mat4f_new(modelviewD, viewMatD, translate_matD);


		kuhl_errorcheck();	
		glUseProgram(program_duck);	 
		kuhl_errorcheck();

		/* Set the uniform variable in the shader that is named "red" to the value 1. */
		//glUniform1i(kuhl_get_uniform("red"), 1);
		
		/* Send the perspective projection matrix to the vertex program. */
		glUniformMatrix4fv(kuhl_get_uniform("Projection"),
		                   1, // number of 4x4 float matrices
		                   0, // transpose
		                   perspectiveD); // value
		/* Send the modelview matrix to the vertex program. */
		glUniformMatrix4fv(kuhl_get_uniform("ModelView"),
		                   1, // number of 4x4 float matrices
		                   0, // transpose
		                   modelviewD); // value	
	

	
		kuhl_errorcheck();	
		kuhl_geometry_draw(modelgeom);
		
		//glUseProgram(0); // stop using a GLSL program.   // I'm not sure about this part!

		///////////////////////////////////////////////////////////////////////////////////  // z = -2

		/* Get the view or camera matrix; update the frustum values if needed. */
		float viewMat[16], perspective[16];
		viewmat_get(viewMat, perspective, viewportID);

		/* Calculate an angle to rotate the object. glfwGetTime() gets
		 * the time in seconds since GLFW was initialized. Rotates 45 degrees every second. */
		float angle = fmod(glfwGetTime()*45, 360);

		/* Make sure all computers/processes use the same angle */
		dgr_setget("angle", &angle, sizeof(GLfloat));

		/* Create a scale matrix. */
		float scaleMatrix[16];
		mat4f_scale_new(scaleMatrix, 2, 2, 2);

		/* create a translate matrix */
		float translate_mat[16];
		
		mat4f_translate_new(translate_mat, 0 , 0 , -2);

		/* create a rotate matrix */
		float rotateMat[16];
		mat4f_rotateAxis_new(rotateMat, angle, 0,1,0);
		

		/* create a rotate matrix */
		float rotateMat1[16];
		mat4f_rotateAxis_new(rotateMat1, 0,0 ,1 ,0);

		// Modelview = (viewMatrix * scaleMatrix) * rotationMatrix
		float modelview[16];
		mat4f_mult_mat4f_new(modelview, viewMat, scaleMatrix);
		mat4f_mult_mat4f_new(modelview, modelview, rotateMat1);  // for rotation part
		mat4f_mult_mat4f_new(modelview, modelview, rotateMat);  // for rotation part
		mat4f_mult_mat4f_new(modelview, modelview, translate_mat);  // for translation part	
	
		for (int i = 0; i < 16; i++)
		{
			modelview_total[0][i] = modelview[i];
		}
		
		float result1 [4];
		const float v1[4] = {0,0,0,1};

		mat4f_mult_vec4f_new (result1 , modelview, v1);

		distance[0] = result1[2];
		
		
		//printf("  %d  ", distance[0]);
		


		///////////////////////////////////////////////////////////////////////////////////


		/* Get the view or camera matrix; update the frustum values if needed. */
		float viewMat2[16], perspective2[16];
		viewmat_get(viewMat2, perspective2, viewportID);

		float translate_mat2[16];
		mat4f_translate_new(translate_mat2, 0 , 0 , 2);

		float scaleMatrix2[16];
		mat4f_scale_new(scaleMatrix2, 2, 2, 2);

		/* create a rotate matrix */
		float rotateMat2[16];
		mat4f_rotateAxis_new(rotateMat2, 108, 0,1,0);
		

		float modelview2[16];
		mat4f_mult_mat4f_new(modelview2, viewMat2, scaleMatrix2);
		mat4f_mult_mat4f_new(modelview2, modelview2, rotateMat2);  // for rotation part
		mat4f_mult_mat4f_new(modelview2, modelview2, rotateMat);  // for rotation part
		mat4f_mult_mat4f_new(modelview2, modelview2, translate_mat2);  // for translation part	

		for (int i = 0; i < 16; i++)
		{
			modelview_total[1][i] = modelview2[i];
		}
		
		float result2 [4];
		const float v2[4] = {0,0,0,1};

		mat4f_mult_vec4f_new (result2 , modelview2, v2);

		distance[1] = result2[2];
		

		//printf("  %d  ", result1[3]);



////////////////////////////////////////////////

		/* Get the view or camera matrix; update the frustum values if needed. */
		float viewMat3[16], perspective3[16];
		viewmat_get(viewMat3, perspective3, viewportID);

		/* create a rotate matrix */
		float rotateMat3[16];
		mat4f_rotateAxis_new(rotateMat3, -108, 0,1,0);

		float translate_mat3[16];
		mat4f_translate_new(translate_mat3, 0 , 0 , 2);

		/* Create a scale matrix. */
		float scaleMatrix3[16];
		mat4f_scale_new(scaleMatrix3, 2, 2, 2);
		

		float modelview3[16];
		mat4f_mult_mat4f_new(modelview3, viewMat3, scaleMatrix3);
		mat4f_mult_mat4f_new(modelview3, modelview3, rotateMat);  // for rotation part
		mat4f_mult_mat4f_new(modelview3, modelview3, rotateMat3);  // for rotation part
		mat4f_mult_mat4f_new(modelview3, modelview3, translate_mat3);  // for translation part	

		//modelview_total [2][0] = modelview3[0];

		for (int i = 0; i < 16; i++)
		{
			modelview_total[2][i] = modelview3[i];
		}
		
		
		float result3 [4];
		const float v3[4] = {0,0,0,1};

		mat4f_mult_vec4f_new (result3 , modelview3, v3);

		distance[2] = result3[2];
		

		//printf("  %d  ", result1[3]);



////////////////////////////////////////////////

		/* Get the view or camera matrix; update the frustum values if needed. */
		float viewMat4[16], perspective4[16];
		viewmat_get(viewMat4, perspective4, viewportID);

		/* create a rotate matrix */
		float rotateMat4[16];
		mat4f_rotateAxis_new(rotateMat4, -216, 0,1,0);

		float translate_mat4[16];
		mat4f_translate_new(translate_mat4, 0 , 0 , -2);

		/* Create a scale matrix. */
		float scaleMatrix4[16];
		mat4f_scale_new(scaleMatrix4, 2, 2, 2);
		

		float modelview4[16];
		mat4f_mult_mat4f_new(modelview4, viewMat4, scaleMatrix4);
		mat4f_mult_mat4f_new(modelview4, modelview4, rotateMat4);  // for rotation part
		mat4f_mult_mat4f_new(modelview4, modelview4, rotateMat);  // for rotation part
		mat4f_mult_mat4f_new(modelview4, modelview4, translate_mat4);  // for translation part	

		for (int i = 0; i < 16; i++)
		{
			modelview_total[3][i] = modelview4[i];
		}
		
		float result4 [4];
		const float v4[4] = {0,0,0,1};

		mat4f_mult_vec4f_new (result4 , modelview4, v4);

		distance[3] = result4[2];
		

		//printf("  %d  ", result1[3]);



////////////////////////////////////////////////

		/* Get the view or camera matrix; update the frustum values if needed. */
		float viewMat5[16], perspective5[16];
		viewmat_get(viewMat5, perspective5, viewportID);

		/* create a rotate matrix */
		float rotateMat5[16];
		mat4f_rotateAxis_new(rotateMat5, -324, 0,1,0);

		float translate_mat5[16];
		mat4f_translate_new(translate_mat5, 1 , 0 , 2);

		/* Create a scale matrix. */
		float scaleMatrix5[16];
		mat4f_scale_new(scaleMatrix5, 2, 2, 2);
		

		float modelview5[16];
		mat4f_mult_mat4f_new(modelview5, viewMat5, scaleMatrix5);
		mat4f_mult_mat4f_new(modelview5, modelview5, rotateMat);  // for rotation part
		mat4f_mult_mat4f_new(modelview5, modelview5, translate_mat5);  // for translation part	
		mat4f_mult_mat4f_new(modelview5, modelview5, rotateMat5);  // for rotation part

		for (int i = 0; i < 16; i++)
		{
			modelview_total[4][i] = modelview5[i];
		}
		
		float result5 [4];
		const float v5[4] = {0,0,0,1};

		mat4f_mult_vec4f_new (result5 , modelview5, v5);

		distance[4] = result5[2];
		

		//printf("  %d  ", result1[3]);  // test case.



///////////////////////////////////////////////////////////////////////////////////  // Drawing part

	float tmp;
		
    for(int i=0; i<n; i++)
    {
        for(int j=i+1; j<n; j++)
        {
            if(distance[j] < distance[i])
            {
                tmp = distance[i];
                distance[i] = distance[j];
                distance[j] = tmp;
            }
        }
    }

//    printf("\nElements of array in sorted ascending order:\n");   // for showing the correct order based on sorting.
//    for(int i=0; i<n; i++)
//    {
//        printf("%d  ", distance[i]);
//    }
		


	 for (int i = 0; i < 5; i++)
	 {

		 if( distance[i] == result1[2])
		 {
	 			/* Tell OpenGL which GLSL program the subsequent
	 			 * glUniformMatrix4fv() calls are for. */
	 			kuhl_errorcheck();
	 			glUseProgram(program);
	 			kuhl_errorcheck();
	 			/* Send the perspective projection matrix to the vertex program. */
	 			glUniformMatrix4fv(kuhl_get_uniform("Projection"),
	 			                   1, // number of 4x4 float matrices
	 			                   0, // transpose
	 			                   perspective); // value
	 			/* Send the modelview matrix to the vertex program. */
	 			glUniformMatrix4fv(kuhl_get_uniform("ModelView"),
	 			                   1, // number of 4x4 float matrices
	 			                   0, // transpose
	 			                   modelview); // value
	 			kuhl_errorcheck();
	 			/* Draw the geometry using the matrices that we sent to the
	 			 * vertex programs immediately above */
	
	
	 			kuhl_geometry_texture(&quad, texId[0], "tex", KG_WARN);

				if (size_of_array == 2 || size_of_array == 3 || size_of_array == 4 || size_of_array == 5) 
				{
	 				kuhl_geometry_draw(&quad);
				}
				

				
				 

		 }

		 if( distance[i] == result2[2])
		 {
	 			/* Tell OpenGL which GLSL program the subsequent
	 			 * glUniformMatrix4fv() calls are for. */
	 			kuhl_errorcheck();
	 			glUseProgram(program);
	 			kuhl_errorcheck();
	 			/* Send the perspective projection matrix to the vertex program. */
	 			glUniformMatrix4fv(kuhl_get_uniform("Projection"),
	 			                   1, // number of 4x4 float matrices
	 			                   0, // transpose
	 			                   perspective); // value
	 			/* Send the modelview matrix to the vertex program. */
	 			glUniformMatrix4fv(kuhl_get_uniform("ModelView"),
	 			                   1, // number of 4x4 float matrices
	 			                   0, // transpose
	 			                   modelview2); // value
	 			kuhl_errorcheck();
	 			/* Draw the geometry using the matrices that we sent to the
	 			 * vertex programs immediately above */
	
	
	 			kuhl_geometry_texture(&quad, texId[1], "tex", KG_WARN);

				if (size_of_array == 2 || size_of_array == 3 || size_of_array == 4 || size_of_array == 5) 
				{
	 				kuhl_geometry_draw(&quad);
				}
				
		 }

		 if( distance[i] == result3[2])
		 {
	 			/* Tell OpenGL which GLSL program the subsequent
	 			 * glUniformMatrix4fv() calls are for. */
	 			kuhl_errorcheck();
	 			glUseProgram(program);
	 			kuhl_errorcheck();
	 			/* Send the perspective projection matrix to the vertex program. */
	 			glUniformMatrix4fv(kuhl_get_uniform("Projection"),
	 			                   1, // number of 4x4 float matrices
	 			                   0, // transpose
	 			                   perspective); // value
	 			/* Send the modelview matrix to the vertex program. */
	 			glUniformMatrix4fv(kuhl_get_uniform("ModelView"),
	 			                   1, // number of 4x4 float matrices
	 			                   0, // transpose
	 			                   modelview3); // value
	 			kuhl_errorcheck();
	 			/* Draw the geometry using the matrices that we sent to the
	 			 * vertex programs immediately above */
	
	
	 			kuhl_geometry_texture(&quad, texId[2], "tex", KG_WARN);

				if (size_of_array == 3 || size_of_array == 4 || size_of_array == 5) 
				{
	 				kuhl_geometry_draw(&quad);
				}
				
		 }

		 if( distance[i] == result4[2])
		 {
	 			/* Tell OpenGL which GLSL program the subsequent
	 			 * glUniformMatrix4fv() calls are for. */
	 			kuhl_errorcheck();
	 			glUseProgram(program);
	 			kuhl_errorcheck();
	 			/* Send the perspective projection matrix to the vertex program. */
	 			glUniformMatrix4fv(kuhl_get_uniform("Projection"),
	 			                   1, // number of 4x4 float matrices
	 			                   0, // transpose
	 			                   perspective); // value
	 			/* Send the modelview matrix to the vertex program. */
	 			glUniformMatrix4fv(kuhl_get_uniform("ModelView"),
	 			                   1, // number of 4x4 float matrices
	 			                   0, // transpose
	 			                   modelview4); // value
	 			kuhl_errorcheck();
	 			/* Draw the geometry using the matrices that we sent to the
	 			 * vertex programs immediately above */
	
	
	 			kuhl_geometry_texture(&quad, texId[3], "tex", KG_WARN);
				if (size_of_array == 4 || size_of_array == 5) 
				{
	 				kuhl_geometry_draw(&quad);
				}
				
		 }

		 if( distance[i] == result5[2])
		 {
	 			/* Tell OpenGL which GLSL program the subsequent
	 			 * glUniformMatrix4fv() calls are for. */
	 			kuhl_errorcheck();
	 			glUseProgram(program);
	 			kuhl_errorcheck();
	 			/* Send the perspective projection matrix to the vertex program. */
	 			glUniformMatrix4fv(kuhl_get_uniform("Projection"),
	 			                   1, // number of 4x4 float matrices
	 			                   0, // transpose
	 			                   perspective); // value
	 			/* Send the modelview matrix to the vertex program. */
	 			glUniformMatrix4fv(kuhl_get_uniform("ModelView"),
	 			                   1, // number of 4x4 float matrices
	 			                   0, // transpose
	 			                   modelview5); // value
	 			kuhl_errorcheck();
	 			/* Draw the geometry using the matrices that we sent to the
	 			 * vertex programs immediately above */
	
	
	 			kuhl_geometry_texture(&quad, texId[4], "tex", KG_WARN);

				if (size_of_array == 5) 
				{
	 				kuhl_geometry_draw(&quad);
				}
				
		 }

	  }

		glUseProgram(0); // stop using a GLSL program.
		viewmat_end_eye(viewportID);
	} // finish viewport loop
	viewmat_end_frame();

	/* Check for errors. If there are errors, consider adding more
	 * calls to kuhl_errorcheck() in your code. */
	kuhl_errorcheck();

}


/* This illustrates how to draw a quad by drawing two triangles and reusing vertices. */
void init_geometryQuad(kuhl_geometry *geom, GLuint prog)
{
	kuhl_geometry_new(geom, prog,
	                  4, // number of vertices
	                  GL_TRIANGLES); // type of thing to draw

	GLfloat texcoordData[] = {0, 0,
							  1, 0,
						      1, 1,
	                          0, 1};

	kuhl_geometry_attrib(geom, texcoordData, 2, "in_TexCoord", KG_WARN);
	// The 2 parameter above means each texture coordinate is a 2D coordinate.				  

	GLfloat vertexPositions[] = {-0.5, -0.5, 0,
	                             0.5, -0.5, 0,
	                             0.5, 0.5, 0,
	                             -0.5, 0.5, 0 };							 
	kuhl_geometry_attrib(geom, vertexPositions,
	                     3, // number of components x,y,z
	                     "in_Position", // GLSL variable
	                     KG_WARN); // warn if attribute is missing in GLSL program?

	/* The normals for each vertex */
	GLfloat normalData[] = {0, 0, 1,
	                        0, 0, 1,
	                        0, 0, 1,
	                        0, 0, 1};
	kuhl_geometry_attrib(geom, normalData, 3, "in_Normal", KG_WARN);
	
	/* A list of triangles that we want to draw. "0" refers to the
	 * first vertex in our list of vertices. Every three numbers forms
	 * a single triangle. */
	GLuint indexData[] = { 0, 1, 2,  
	                       0, 2, 3 };
	kuhl_geometry_indices(geom, indexData, 6);


	kuhl_errorcheck();
}



void calling_texture()
{

	for(int i = 0; i < size_of_array; i++)
	{
		aspectRatio [i] = kuhl_read_texture_file(p[i], &texId[i]);
		msg(MSG_DEBUG, "Aspect ratio of image is %f\n", aspectRatio[i]); // write message to log.txt
		/* Tell this piece of geometry to use the texture we just loaded. */
	}
		
}

int main(int argc, char** argv)
{
	/* Initialize GLFW and GLEW */
	kuhl_ogl_init(&argc, argv, 512, 512, 32, 4);

	/* Specify function to call when keys are pressed. */
	glfwSetKeyCallback(kuhl_get_window(), keyboard);
	// glfwSetFramebufferSizeCallback(window, reshape);

	size_of_array = (argc -1);

	p = malloc( sizeof(char *) * ( size_of_array) );

	if (size_of_array == 0) 
	{
		printf("\nYou did not put any image as the input! :(\nYou have to have, 2 or more than that up to 5 images.\nTry it again..\n");
		exit(0);
	}

	else if (size_of_array == 1) 
	{
		printf("\nYou put only 1 image as the input! :(\nYou have to have, 2 or more than that up to 5 images.\nTry it again..\n");
		exit(0);
	}	

	else if (size_of_array == 2) 
	{
		p[0] = argv[1];
		p[1] = argv[2];
	}
	 
	else if (size_of_array == 3) 
	{
		p[0] = argv[1];
		p[1] = argv[2];
		p[2] = argv[3];		
	}

	else if (size_of_array == 4) 
	{
		p[0] = argv[1];
		p[1] = argv[2];
		p[2] = argv[3];
		p[3] = argv[4];
	}
	
	else if (size_of_array == 5) 
	{
		p[0] = argv[1];
		p[1] = argv[2];
		p[2] = argv[3];
		p[3] = argv[4];
		p[4] = argv[5];
	}

	else
	{
		printf("\nThe number of arguments (%d) is not correct and more than 5 images! :(\nTry it again with less input..\n",size_of_array);
		exit(0);
	}
	


	calling_texture();  // ready textures only once

	///////////////////////////////////////////////////////////////////////////////////  // Duck Model Calling Part
	
		program_duck = kuhl_create_program("carousel.vert", "carousel.frag");
		modelgeom = kuhl_load_model(modelFile, NULL, program_duck, NULL);

		kuhl_errorcheck();

    ////////////////////////////////////////////////////////////////////////////   // Quad Model Calling Part

	/* Compile and link a GLSL program composed of a vertex shader and
	 * a fragment shader. */

	program = kuhl_create_program("texture.vert", "texture.frag");
	glUseProgram(program);
	kuhl_errorcheck();
	/* Set the uniform variable in the shader that is named "red" to the value 1. */
	//glUniform1i(kuhl_get_uniform("red"), 1);

	init_geometryQuad(&quad, program);
	
	
	/* Good practice: Unbind objects until we really need them. */
	glUseProgram(0);

	dgr_init();     /* Initialize DGR based on environment variables. */

	float initCamPos[3]  = {0,0,10}; // location of camera
	float initCamLook[3] = {0,0,0}; // a point the camera is facing at
	float initCamUp[3]   = {0,1,0}; // a vector indicating which direction is up


	viewmat_init(initCamPos, initCamLook, initCamUp);
	
	while(!glfwWindowShouldClose(kuhl_get_window()))
	{
		display();
		kuhl_errorcheck();

		/* process events (keyboard, mouse, etc) */
		glfwPollEvents();
	}

	exit(EXIT_SUCCESS);
}
