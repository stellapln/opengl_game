#include "../include/graphic.h"

void resizeViewport(int w, int h) {
    WINDOW_WIDTH = w;
    WINDOW_HEIGHT = h;
    SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, BIT_PER_PIXEL, SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_RESIZABLE);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION); // PROJECTION?
    glLoadIdentity();
    gluOrtho2D(0., 60., 0.,40.);
}

void createImgPNG(GLuint *textureID, SDL_Surface* img){
	glGenTextures(1,&(*textureID));
	glBindTexture(GL_TEXTURE_2D,*textureID);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		img->w,
		img->h,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		img->pixels);
	glBindTexture(GL_TEXTURE_2D,0);
    SDL_FreeSurface(img);
}

void loadImgPNG(char imgSrc[], ListeElements elements){
	SDL_Surface *img = IMG_Load(imgSrc);
	if(img == NULL){
		printf("ERROR at loading image from %s\n", imgSrc);
		exit(1);
	}
	GLuint id;
	createImgPNG(&id, img);
	elements->textureID = id;
}

void loadImgNb(char imgSrc[], GLuint texture[], int index){
    SDL_Surface *img = IMG_Load(imgSrc);
    if(img == NULL){
        printf("ERROR at loading image from %s\n", imgSrc);
        exit(1);
    }
    GLuint id;
    createImgPNG(&id, img);
    texture[index] = id;
}

void drawScore(GLuint texture, int posX, int nb){
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    /* Brackground drawing */
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
        glPushMatrix();
            glTranslatef(posX+50, 39, 0);
            if(nb == 1) glScalef(0.5,0.5,1.0);
            else if(nb == 0) glScalef(1.5,1.5,0);
            else if(nb == 2) glScalef(1.2,1.2,0);
            glBegin(GL_QUADS);
            glTexCoord2f(0,0);
            glVertex2f(-1,1);

            glTexCoord2f(1,0);
            glVertex2f(1,1);

            glTexCoord2f(1,1);
            glVertex2f(1,-1);

            glTexCoord2f(0,1);
            glVertex2f(-1,-1);
            glEnd();
        glPopMatrix();

    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void drawList(ListeElements list, float width, float height){
	while(list != NULL){
		drawElement(list, width, height);
		list=list->next;
	}
}

void drawElement(Element *e, float width, float height){
    /* Transparency */
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        /* Brackground drawing */
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, e->textureID);
            glPushMatrix();
            	glTranslatef(e->pos.x, e->pos.y, 0);
                glScalef(width,height,1.0);
                glBegin(GL_QUADS);
                glTexCoord2f(0,0);
                glVertex2f(-1,1);

                glTexCoord2f(1,0);
                glVertex2f(1,1);

                glTexCoord2f(1,1);
                glVertex2f(1,-1);

                glTexCoord2f(0,1);
                glVertex2f(-1,-1);
                glEnd();
            glPopMatrix();

        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
}

void drawShipInMove(Element *e, float width, float height, int collision){
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); /* Transparency */
    /* Brackground drawing */   
    if(collision > 0){
        glColor3ub(255,255,100);
    }
    else if(collision < 0){
        glColor3ub(255,100,100);
    }
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, e->textureID);
        glPushMatrix();
            glScalef(width,height,1.0);
            glBegin(GL_QUADS);
            glTexCoord2f(0,0);
            glVertex2f(-1,1);

            glTexCoord2f(1,0);
            glVertex2f(1,1);

            glTexCoord2f(1,1);
            glVertex2f(1,-1);

            glTexCoord2f(0,1);
            glVertex2f(-1,-1);
            glEnd();
        glPopMatrix();

    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void moveElementsVertical(World world){
    float speed;
    while(world.mobs!=NULL){
        if(world.mobs->type == 'd' && world.mobs->pos.x <= 60.0){
            speed = world.mobs->speed - 0.06;
            if (world.mobs->pos.y > world.ship->pos.y){
                world.mobs->pos.y -= speed;
                world.mobs->pmax.y -= speed;
                world.mobs->pmin.y -= speed;
            }
            else{
                world.mobs->pos.y += speed;
                world.mobs->pmax.y += speed;
                world.mobs->pmin.y += speed;
            }
        }
        world.mobs = world.mobs->next;
    }
}

void moveElements(ListeElements l){
    while(l!=NULL){
        float speed = l->speed;

        //Delete arrows
        if (l->type == 'a'){
            if (l->pos.x > 60){
                l->life = 0;
            }
        }
        l->pos.x -= speed;
        l->pmax.x -= speed;
        l->pmin.x -= speed;
        l=l->next;
    }
}

void moveShip(Element *e, int move){
    /* Up */
    e->speed = 0.3;
    if (move == 1){
        if(e->pos.y + e->speed <= 39.0){
            e->pos.y += e->speed;
            e->pmin.y += e->speed;
            e->pmax.y += e->speed;
        }
    }
    /* Down */
    else if (move == -1){
        if(e->pos.y - e->speed >= 1.0){
            e->pos.y -= e->speed;
            e->pmin.y -= e->speed;
            e->pmax.y -= e->speed;
        }
    }
}

void moveBackground(Element *e){
    if(e->pos.x <= -30.0) e->pos.x = 90.0;
    else e->pos.x -= e->speed;
}

/********** DEBUG ***********/
void drawBB(ListeElements l){
    
    glColor3ub(0, 255, 0);
        glBegin(GL_QUADS);
    while(l!=NULL){ 


            glVertex2f(l->pmin.x,l->pmax.y);

            glVertex2f(l->pmax.x,l->pmax.y);

            glVertex2f(l->pmax.x,l->pmin.y);

            glVertex2f(l->pmin.x,l->pmin.y);

        l=l->next;
    }

        glEnd();
    glColor3ub(255,255,255);
}
void drawLandmark(){
    int i, size;
    size = WINDOW_WIDTH;

    glColor3ub(255, 0, 0);
    for(i = 0 ; i < size ; i++){
        glBegin(GL_LINES);
            glVertex2f(i,-size);
            glVertex2f(i, size);
        glEnd();
    }
    for(i = 0 ; i < size ; i++){
        glBegin(GL_LINES);
            glVertex2f(-size, i);
            glVertex2f(size, i);
        glEnd();
    }
    glColor3ub(255,255,255);
}
