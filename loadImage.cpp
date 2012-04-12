/*
 *This C program is an exercise in texture loading
 *user rotation and translation enabled
 * Nathan Gossett
 *Spring 2012
 **/

#include <stdio.h>
#include <GL/Angel.h>
#include <stdlib.h>
#include <math.h>
#include <IL/il.h> //notice the OpenIL include

#pragma comment(lib, "glew32.lib")
//We have additional libraries to link to now as well
#pragma comment(lib,"ILUT.lib")
#pragma comment(lib,"DevIL.lib")
#pragma comment(lib,"ILU.lib")


#define WIDTH 500
#define HEIGHT 400
#define TRUE 1
#define FALSE 0


int right_button_down = FALSE;
int left_button_down = FALSE;
int middle_button_down = FALSE;

int prevMouseX;
int prevMouseY;

double view_rotx = 180.0;
double view_roty = 0.0;
double view_rotz = 0.0;
double z_distance;

//We need three texture files
static GLuint * texName;


GLuint * vao;
GLuint * vbo;

// EARTH 
GLuint * spherevao;
GLuint * spherevbo;

// CLOUD
GLuint * cloudvao;
GLuint * cloudvbo;


//our modelview and perspective matrices
mat4 mv, p;

//and we'll need pointers to our shader variables
GLuint model_view;
GLuint projection;
GLuint vPosition;
GLuint texCoord;
GLuint texMap;

GLuint program;

int multiflag = 0;




/////// EARTH
int space = 10; //10; // 259200
int VertexCount = (180 / space) * (360 / space) * 4; // 2592

vec4 squareverts[6];
vec2 texcoords[6];


int rotateYEarth;
int rotateXEarth;


///////////////
// CLOUD
//////////////



//Modified slightly from the OpenIL tutorials
ILuint loadTexFile(const char* filename){
	
	ILboolean success; /* ILboolean is type similar to GLboolean and can equal GL_FALSE (0) or GL_TRUE (1)
    it can have different value (because it's just typedef of unsigned char), but this sould be
    avoided.
    Variable success will be used to determine if some function returned success or failure. */


	/* Before calling ilInit() version should be checked. */
	  if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
	  {
		/* wrong DevIL version */
		printf("Wrong IL version");
		exit(1);
	  }
 
	  success = ilLoadImage((LPTSTR)filename); /* Loading of image from file */
	if (success){ /* If no error occured: */
		//We need to figure out whether we have an alpha channel or not
		  if(ilGetInteger(IL_IMAGE_BPP) == 3){
			success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE); /* Convert every color component into
		  unsigned byte. If your image contains alpha channel you can replace IL_RGB with IL_RGBA */
		  }else if(ilGetInteger(IL_IMAGE_BPP) == 4){
			  success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		  }else{
			  success = false;
		  }
		if (!success){
		  /* Error occured */
		 printf("failed conversion to unsigned byte");
		 exit(1);
		}
	}else{
		/* Error occured */
	   printf("Failed to load image ");
	   printf(filename);
		exit(1);
	}
}



void CreateSphere (vec4 verts[2592], vec2 texcoords[2592], double R, double H, double K, double Z) {
    
	int n;
    double a;
    double b;
    n = 0;
    
	for( b = 0; b <= 180 - space; b+=space){
       
		for( a = 0; a <= 360 - space; a+=space){
            
			verts[n] = vec4( R * sin((a) / 180 * M_PI) * sin((b) / 180 * M_PI) - H,
								   R * cos((a) / 180 * M_PI) * sin((b) / 180 * M_PI) + K,
								   R * cos((b) / 180 * M_PI) - Z, 
								   1);
            texcoords[n] = vec2((a) / 360 , (2 * b) / 360);
            

			n++;
            
			verts[n] = vec4(R * sin((a) / 180 * M_PI) * sin((b + space) / 180 * M_PI) - H,
					             R * cos((a) / 180 * M_PI) * sin((b + space) / 180 * M_PI) + K,
								 R * cos((b + space) / 180 * M_PI) - Z,
								 1);

			texcoords[n] = vec2((a) / 360 , 
							(2 *  (b + space)) / 360);
            
            
			n++;
            

            
			verts[n] = vec4(R * sin((a + space) / 180 * M_PI) * sin((b) / 180 * M_PI) - H,
								R * cos((a + space) / 180 * M_PI) * sin((b) / 180 * M_PI) + K,
								R * cos((b) / 180 * M_PI) - Z,
								1);
         
			texcoords[n] = vec2((a + space) / 360 ,  (2 *  (b)) / 360 );


			n++;
            
			verts[n] = vec4( R * sin((a + space) / 180 * M_PI) * sin((b + space) /180 * M_PI) - H,
									R * cos((a + space) / 180 * M_PI) * sin((b + space) / 180 * M_PI) + K,
									 R * cos((b + space) / 180 * M_PI) - Z,
									 1);
            
			texcoords[n] = vec2((a + space) / 360, (2 *  (b + space)) / 360);

			n++;
            

            
		}
    
	}
}

void CreateCloudSphere(vec4 verts[2592], vec2 texcoords[2592], double R, double H, double K, double Z)
{
	CreateSphere (verts, texcoords, 5.5, 0, 0, 0);
}

void createSquare(vec4 squareverts[6], vec2 texcoords[6])
{
   
   squareverts[0] = vec4(-1, -1, 0, 1); 
   texcoords[0] = vec2(0, 0);
   squareverts[1] = vec4(1, -1, 0, 1);
   texcoords[1] = vec2(1, 0);
   squareverts[2] = vec4(1, 1, 0, 1);
   texcoords[2] = vec2(1, 1);
   squareverts[3] = vec4(1, 1, 0, 1);
   texcoords[3] = vec2(1, 1);
   squareverts[4] = vec4(-1, 1, 0, 1);
   texcoords[4] = vec2(0, 1);
   squareverts[5] = vec4(-1, -1, 0, 1);
   texcoords[5] = vec2(0, 0);
}

void SetupShader(GLuint vao[1], GLuint vbo[3], vec4 squareverts[6], vec2 texcoords[2],ILuint ilTexID[3],
					GLuint texName[3])
{
	// Create a vertex array object
    glGenVertexArrays( 1, &vao[0] );

    // Create and initialize any buffer objects
	glBindVertexArray( vao[0] );
	glGenBuffers( 2, &vbo[0] );
    glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );
    glBufferData( GL_ARRAY_BUFFER, sizeof(squareverts), squareverts, GL_STATIC_DRAW);

	glBindBuffer( GL_ARRAY_BUFFER, vbo[1] );
    glBufferData( GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);

	


	ilBindImage(ilTexID[0]); /* Binding of IL image name */
	loadTexFile("images/Earth.png");
	glBindTexture(GL_TEXTURE_2D, texName[0]); //bind OpenGL texture name

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   //Note how we depend on OpenIL to supply information about the file we just loaded in
    glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),0,
	ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
}
void init(void)
{    
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glEnable(GL_DEPTH_TEST);

	rotateYEarth = 0;
	rotateXEarth = 0;

     
   //squareverts = new vec4[6];

   //createSquare(squareverts, texcoords);

	////////////////////////////
	// Creating EARTH
	///////////////////////////
	vec4 sphereverts[2592];
	vec2 spheretexcoords[2592]; // 2592


	CreateSphere(sphereverts, spheretexcoords, 2, 0,0,0);


	////////////////////////////
	// Creating CLOUD
	///////////////////////////
	vec4 cloudverts[2592];
	vec2 cloudtexcoords[2592]; 

	CreateSphere(cloudverts, cloudtexcoords, 2.1, 0,0,0);



	//////////////////////////////////////
	// Initialize Shader
	//////////////////////////////////////
   program = InitShader( "vshader-texture.glsl", "fshader-texture.glsl" );


   ILuint * ilTexID = new ILuint[3]; /* ILuint is a 32bit unsigned integer.

    //Variable texid will be used to store image name. */
   texName = new GLuint[3];


	ilInit(); /* Initialization of OpenIL */
	ilGenImages(3, ilTexID); /* Generation of three image names for OpenIL image loading */
	glGenTextures(3, texName); //and we eventually want the data in an OpenGL texture
 
	vao = new GLuint[1];
	vbo = new GLuint[3];
	

	if (true)
	{
		//SetupShader(vao, vbo, squareverts, texcoords,ilTexID,texName);
	}
	else 
	{
		// Create a vertex array object
		glGenVertexArrays( 1, &vao[0] );

		// Create and initialize any buffer objects
		glBindVertexArray( vao[0] );
		glGenBuffers( 2, &vbo[0] );
		glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );
		glBufferData( GL_ARRAY_BUFFER, sizeof(squareverts), squareverts, GL_STATIC_DRAW);

		glBindBuffer( GL_ARRAY_BUFFER, vbo[1] );
		glBufferData( GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);

		ilBindImage(ilTexID[0]); /* Binding of IL image name */
		loadTexFile("images/Earth.png");
		glBindTexture(GL_TEXTURE_2D, texName[0]); //bind OpenGL texture name

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	   //Note how we depend on OpenIL to supply information about the file we just loaded in
		glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),0,
		ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
	}

	/////////////////////////////////////////
	// EARTH BUFFERS
	////////////////////////////////////////////////////

	if ( true )
	{
		/////////////////////////////////////////
		// Create a vertex array object
		spherevao = new GLuint[1];
		spherevbo = new GLuint[3];


		glGenVertexArrays( 1, &spherevao[0] );

		// Create and initialize any buffer objects
		glBindVertexArray( spherevao[0] );
		glGenBuffers( 2, &spherevbo[0] );
		glBindBuffer( GL_ARRAY_BUFFER, spherevbo[0] );
		glBufferData( GL_ARRAY_BUFFER, VertexCount*sizeof(vec4), sphereverts, GL_STATIC_DRAW);

		glBindBuffer( GL_ARRAY_BUFFER, spherevbo[1] );
		glBufferData( GL_ARRAY_BUFFER, VertexCount*sizeof(vec2), spheretexcoords, GL_STATIC_DRAW);

		ilBindImage(ilTexID[0]); /* Binding of IL image name */
		loadTexFile("images/Earth.png");
		glBindTexture(GL_TEXTURE_2D, texName[0]); //bind OpenGL texture name

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	   //Note how we depend on OpenIL to supply information about the file we just loaded in
		glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),0,
		ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());

	}

	/////////////////////////////////////////
	// CLOUD BUFFERS
	////////////////////////////////////////////////////

	if ( true )
	{
		/////////////////////////////////////////
		// Create a vertex array object
		cloudvao = new GLuint[1];
		cloudvbo = new GLuint[3];


		glGenVertexArrays( 1, &cloudvao[0] );

		// Create and initialize any buffer objects
		glBindVertexArray( cloudvao[0] );
		glGenBuffers( 2, &cloudvbo[0] );
		glBindBuffer( GL_ARRAY_BUFFER, cloudvbo[0] );
		glBufferData( GL_ARRAY_BUFFER, VertexCount*sizeof(vec4), cloudverts, GL_STATIC_DRAW);

		glBindBuffer( GL_ARRAY_BUFFER, cloudvbo[1] );
		glBufferData( GL_ARRAY_BUFFER, VertexCount*sizeof(vec2), cloudtexcoords, GL_STATIC_DRAW);

		ilBindImage(ilTexID[1]); /* Binding of IL image name */
		loadTexFile("images/earthcloudmap.png");
		glBindTexture(GL_TEXTURE_2D, texName[1]); //bind OpenGL texture name

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	   //Note how we depend on OpenIL to supply information about the file we just loaded in
		glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),0,
		ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());

	}

	/////////////////////////////////////////
   ////////////////////////////////////////////////////
	//Now repeat the process for the second image
	/*ilBindImage(ilTexID[1]);
	
	glBindTexture(GL_TEXTURE_2D, texName[1]);
	loadTexFile("images/alpaca.png");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),0,
	   ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
*/
	////////////////////////////////////////////////////
	//And the third image

	/*ilBindImage(ilTexID[2]);
	glBindTexture(GL_TEXTURE_2D, texName[2]);
	loadTexFile("images/opengl.png");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),0,
	   ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
*/

	////////////////////////////////////////////////
    ilDeleteImages(3, ilTexID); //we're done with OpenIL, so free up the memory

	////////////////////////////////////////////////////

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	model_view = glGetUniformLocation(program, "model_view");
	projection = glGetUniformLocation(program, "projection");
	
	texMap = glGetUniformLocation(program, "texture");
	glUniform1i(texMap, 0);//assign this one to texture unit 0
	/////////////////////////////////////////////////////////////////
	///// bind VAO and VBO


	if ( false )
	{
		glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );
		vPosition = glGetAttribLocation(program, "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer( GL_ARRAY_BUFFER, vbo[1] );
		texCoord = glGetAttribLocation(program, "texCoord");
		glEnableVertexAttribArray(texCoord);
		glVertexAttribPointer(texCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}
	else
	{
		glBindBuffer( GL_ARRAY_BUFFER, spherevbo[0] );
		vPosition = glGetAttribLocation(program, "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer( GL_ARRAY_BUFFER, spherevbo[1] );
		texCoord = glGetAttribLocation(program, "texCoord");
		glEnableVertexAttribArray(texCoord);
		glVertexAttribPointer(texCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}


	if (true)
	{
		glBindBuffer( GL_ARRAY_BUFFER, cloudvbo[0] );
		vPosition = glGetAttribLocation(program, "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer( GL_ARRAY_BUFFER, cloudvbo[1] );
		texCoord = glGetAttribLocation(program, "texCoord");
		glEnableVertexAttribArray(texCoord);
		glVertexAttribPointer(texCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}
}

void display(void)
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
	
    //mat4 camera = mv =  LookAt(vec4(0,0,5.0+z_distance,1),vec4(0,0,0,1),vec4(0,1,0,0))* RotateX(view_rotx) * RotateY(view_roty) * RotateZ(view_rotz);

    mat4 camera = mv =  LookAt(vec4(0,0,5.0+z_distance,1),vec4(0,0,0,1),vec4(0,1,0,0))* RotateX(rotateXEarth) * RotateY(rotateYEarth)* RotateX(-90.0); //RotateX(view_rotx) * RotateY(view_roty);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv*Translate(0,0,1));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texName[0]); //which texture do we want?
	glDrawArrays( GL_QUAD_STRIP, 0, VertexCount );
	
	/*
	mv = camera * Translate(0,0,0);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv*Translate(0,0,1));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texName[1]); //which texture do we want?
	glDrawArrays( GL_QUAD_STRIP, 0, VertexCount );
	*/

	/*
	mv = camera * RotateY(90)* Translate(0,0,1);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	
	glBindTexture(GL_TEXTURE_2D, texName[1]); //which texture do we want?
	glDrawArrays( GL_TRIANGLES, 0, 6 );

	mv = camera* RotateY(-90) * Translate(0,0,1);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	glBindTexture(GL_TEXTURE_2D, texName[2]); //which texture do we want?
	glDrawArrays( GL_TRIANGLES, 0, 6 );
	*/

   glutSwapBuffers();


}

void reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);

	 p = Perspective(60.0, (float)w/(float)h, 1.0, 30.0);
	glUniformMatrix4fv(projection, 1, GL_TRUE, p);
}

void keyboard (unsigned char key, int x, int y)
{
   switch (key) {
      case 27:
         exit(0);
         break;
      default:
         break;
   }
}

void mouse_dragged(int x, int y) {
	double thetaY, thetaX;
	if (left_button_down) {

		rotateYEarth += 1;

		/*thetaY = 360.0 *(x-prevMouseX)/WIDTH;    
		thetaX = 360.0 *(prevMouseY - y)/HEIGHT;
		prevMouseX = x;
		prevMouseY = y;
		view_rotx += thetaX;
		view_roty += thetaY;*/
	}
	else if (right_button_down) {

		rotateXEarth += 1;
		// z_distance = 5.0*(prevMouseY-y)/HEIGHT;
	}

	else if (middle_button_down)
	{
		z_distance += 1;
	}
    glutPostRedisplay();
}


void mouse(int button, int state, int x, int y) {
  //establish point of reference for dragging mouse in window
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
      left_button_down = TRUE;
	  prevMouseX= x;
      prevMouseY = y;
    }

	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
      right_button_down = TRUE;
      prevMouseX = x;
      prevMouseY = y;
    }
    else if (state == GLUT_UP) {
      left_button_down = FALSE;
	  right_button_down = FALSE;
	}
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
   glutInitWindowSize(WIDTH, HEIGHT);
   glutInitWindowPosition(100, 100);
   glutCreateWindow("Texturing");
   glewExperimental = GL_TRUE;

	glewInit();
   init();
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(mouse_dragged);    
   glutMainLoop();
   return 0; 
}