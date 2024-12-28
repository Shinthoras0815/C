// gcc snake_game.c -o snake_game gcc snake_game.c -o snake_game -I(Path:/to/SDL_2/headers) -L(Path:/to/SDL2/libs) -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -mwindows
// need SDL2.dll and SDL2_ttf.dll in source directionary

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

// X
#define WINDOW_WIDTH 640
// Y
#define WINDOW_HEIGHT 480
#define CELL_SIZE 20
#define BODY_LENGTH 255
#define Fontpath "C:/Windows/Fonts/Arial.ttf"
#define startdelay 300
#define MAX_SCORE 5
#define NAME_LENGTH 10

char textscore[20];
struct HighscoreEntry {
    char name[NAME_LENGTH];
    int hscore;
};

struct HighscoreEntry highscores[MAX_SCORE];

int score = 0;
int delay = 300;
int faster = 10;
int maxspeed = 50;
static bool isMouseOverButton1 = false;
static bool isMouseOverButton2 = false;
static bool isMouseOverButton3 = false;
static bool isMouseOverButton4 = false;
bool isPaused = false;

struct button{
int Bx;
int By;
int Bradx;
int Brady;
char BText1[20];
char BText2[20];
};

struct button startbutton;
struct button endbutton;
struct button scorebutton;
struct button leavebutton;

struct COORD2D{
    int x;
    int y;
};

enum Direction{
    UP,
    DOWN,
    LEFT,
    RIGHT,
};
enum Direction nextDirection = RIGHT; // Startwert für die nächste Richtung der Schlange
struct SNAKE{
    struct COORD2D position;
    struct COORD2D body[BODY_LENGTH];
    int lenght;
    enum Direction direction;
};

struct FRUIT{
    struct COORD2D fruitposition;
};

SDL_Texture* renderText(const char* text, TTF_Font* font, SDL_Color color, SDL_Renderer* renderer) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void initSnake(struct SNAKE *snake);
void moveSnake(struct SNAKE *snake);
void handleInput(struct SNAKE *snake, bool *inMenu);
void handlecollision(struct SNAKE *snake, bool *inScore, struct FRUIT *fruit, bool *newHighscore, bool *inMenu);
void drawSnake(SDL_Renderer *renderer, struct SNAKE *snake, struct FRUIT *fruit);
void fruits(struct FRUIT *fruit);
void rendertext(SDL_Renderer *renderer,TTF_Font **font,SDL_Texture **TexturName, const char *text, int x, int y);
void Fontsize(TTF_Font **fontname, char *fontPath, int fontSize);
void drawMenu(SDL_Renderer *renderer);
void drawEllipse(SDL_Renderer* renderer, int x, int y, int radiusX, int radiusY);
void drawFilledEllipse(SDL_Renderer* renderer, int x, int y, int radiusX, int radiusY);
void drawdEllipseBorder(SDL_Renderer* renderer, int x, int y, int radiusX, int radiusY, int border);
bool isMouseOverEllipse(int mouseX, int mouseY, int ellipseCenterX, int ellipseCenterY, int radiusX, int radiusY);
void saveHighscores(struct HighscoreEntry highscores[]);
void loadHighscores(struct HighscoreEntry highscores[]);
void drawScore(SDL_Renderer *renderer);
void updateHighscores(struct HighscoreEntry highscores[], int newScore, char *newName);
int compareHighscores(const void *a, const void *b);
void enterName(SDL_Renderer *renderer, bool *newHighscore);

int main(int argc, char* argv[]) {

    // Überprüfen, ob die Datei bereits existiert
    FILE *existingFile = fopen("highscores.txt", "r");
    if (existingFile == NULL) {
        // Die Datei existiert noch nicht, also erstellen wir sie
        FILE *highscoreFile = fopen("highscores.txt", "a");
            strcpy(highscores[0].name, "Spieler1");
            highscores[0].hscore = 30;
            strcpy(highscores[1].name, "Spieler2");
            highscores[1].hscore = 20;
            strcpy(highscores[2].name, "Spieler3");
            highscores[2].hscore = 10;
            strcpy(highscores[3].name, "Spieler4");
            highscores[3].hscore = 5;
            strcpy(highscores[4].name, "Spieler5");
            highscores[4].hscore = 0;
            saveHighscores(highscores);
            printf("Test\n");
        // Schließen Sie die Datei nach dem Öffnen
        if (highscoreFile != NULL) {
            fclose(existingFile);
        } else {
            printf("Fehler beim Erstellen der Datei!1\n");
            return 1;
        }
    } else {
        FILE *highscoreFile = fopen("highscores.txt", "r");
        if (highscoreFile != NULL) {
        loadHighscores(highscores);
        fclose(existingFile);
        }
    }

    srand(time(NULL));
    struct SNAKE snake;
    struct FRUIT fruit;
    snake.lenght = 0;

    startbutton.Bx = WINDOW_WIDTH / 2;
    startbutton.By = WINDOW_HEIGHT / 4;
    startbutton.Bradx = 100;
    startbutton.Brady = 50;

    endbutton.Bx = WINDOW_WIDTH / 2;
    endbutton.By = 3 * WINDOW_HEIGHT / 4;
    endbutton.Bradx = 100;
    endbutton.Brady = 50;

    scorebutton.Bx = WINDOW_WIDTH / 2;
    scorebutton.By = 2 * WINDOW_HEIGHT / 4;
    scorebutton.Bradx = 100;
    scorebutton.Brady = 50;

    leavebutton.Bx = WINDOW_WIDTH / 2;
    leavebutton.By = 7 * WINDOW_HEIGHT / 8;
    leavebutton.Bradx = 60;
    leavebutton.Brady = 30;
    char fieldtext[20] = "Leave";
    strcpy(leavebutton.BText1, fieldtext);

    // Initialisiere SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL konnte nicht initialisiert werden! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }
    TTF_Init();

    // Erstelle ein Fenster
    SDL_Window* window = SDL_CreateWindow("Snake Spiel", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Fenster konnte nicht erstellt werden! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Erstelle einen Renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer konnte nicht erstellt werden! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    TTF_Font *Title;
    Fontsize(&Title, Fontpath, 36);
    TTF_Font *By;
    Fontsize(&By, Fontpath, 15);

    SDL_Texture* TitleTexture;
    rendertext(renderer, &Title, &TitleTexture, "Snake Game", 200, 200);
    SDL_RenderPresent(renderer);
    SDL_Delay(500); // Wartezeit in Millisekunden
    SDL_Texture* ByTexture;
    rendertext(renderer, &By, &ByTexture, "von Shinthoras0815", 380, 280);

    // Fenster anzeigen und auf Benutzerinteraktion warten
    SDL_RenderPresent(renderer);
    SDL_Delay(500); // Wartezeit in Millisekunden

    SDL_DestroyTexture(TitleTexture);
    SDL_DestroyTexture(ByTexture);
    TTF_CloseFont(Title);
    TTF_CloseFont(By);
    
    //initiere Snakekörper
    // Spiel-Schleife
    bool start = false;
    bool inMenu = true;
    bool quit = false;
    bool startscore = false;
    bool inscore = false;
    bool newHighscore = false;
    SDL_Event e;

    while (!quit) {
        if(!inMenu && !inscore){
            start = false;
            handleInput(&snake, &inMenu);
            if (!isPaused) {
                // Hier sollte die Spiellogik ausgeführt werden
                moveSnake(&snake);
                handlecollision(&snake, &inscore, &fruit, &newHighscore, &inMenu);
            }
        }
        // Event-Handling
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            int mouseX = e.motion.x;
            int mouseY = e.motion.y;
            if (e.type == SDL_KEYDOWN) {
                  // Überprüfen Sie, ob die Alt-Taste gedrückt wurde
                   const Uint8 *state = SDL_GetKeyboardState(NULL);
                if (state[SDL_SCANCODE_LALT] || state[SDL_SCANCODE_RALT]) {
                     // Überprüfen Sie, ob die F4-Taste gedrückt wurde
                     if (e.key.keysym.sym == SDLK_F4) {
                        // Beenden Sie das Spiel
                        quit = true;
                    }
                }
             }    
                if (inscore){
                    isMouseOverButton2 = false;
                    if (e.type == SDL_KEYDOWN) {
                    switch(e.key.keysym.sym){
                    case SDLK_ESCAPE: // Wenn die Escape-Taste gedrückt wird
                    inscore = false;
                    e.type = 0;
                    inMenu = true;
                    break;
                    default:
                    break;    
                    }  
                    } 
                    else if (e.type == SDL_MOUSEMOTION) {
                    if (isMouseOverEllipse(mouseX, mouseY, leavebutton.Bx, leavebutton.By, leavebutton.Bradx, leavebutton.Brady)) {
                        isMouseOverButton4 = true;
                    }
                    else{
                        isMouseOverButton4 = false;
                    }
                    }
                    else if (e.type == SDL_MOUSEBUTTONDOWN) {
                    // Hier Mausklicks im Menü behandeln
                        if (isMouseOverButton4) {
                        inscore = false;
                        e.type = 0;
                        inMenu = true;
                        }
                    }
                }
                if (inMenu) {
                    isMouseOverButton4 = false;
                if (e.type == SDL_KEYDOWN) {
                    // Überprüfen Sie die Benutzereingabe im Menü
                    // Zum Beispiel, ob der Benutzer die Pfeiltasten nach oben/unten oder die Eingabetaste drückt
                    // Basierend auf der Auswahl des Benutzers können Sie die Variable inMenu aktualisieren, um das Spiel zu starten oder zu beenden
                        switch(e.key.keysym.sym) {
                            case SDLK_DOWN:
                            if (!isMouseOverButton1 && !isMouseOverButton2 && !isMouseOverButton3){
                                isMouseOverButton1 = true;
                                break;
                            }
                            else if (isMouseOverButton1)
                            {
                               isMouseOverButton1 = false;
                               isMouseOverButton2 = true;
                               isMouseOverButton3 = false;
                                break;
                            }
                            else if (isMouseOverButton2){
                                isMouseOverButton1 = false;
                                isMouseOverButton2 = false;
                                isMouseOverButton3 = true;
                                break;
                            }
                            else if(isMouseOverButton3){
                                isMouseOverButton1 = true;
                                isMouseOverButton2 = false;
                                isMouseOverButton3 = false;
                                break;
                            }
                            case SDLK_s:
                            if (!isMouseOverButton1 && !isMouseOverButton2 && !isMouseOverButton3){
                                isMouseOverButton1 = true;
                                break;
                            }
                            else if (isMouseOverButton1)
                            {
                               isMouseOverButton1 = false;
                               isMouseOverButton2 = true;
                               isMouseOverButton3 = false;
                                break;
                            }
                            else if (isMouseOverButton2){
                                isMouseOverButton1 = false;
                                isMouseOverButton2 = false;
                                isMouseOverButton3 = true;
                                break;
                            }
                            else if(isMouseOverButton3){
                                isMouseOverButton1 = true;
                                isMouseOverButton2 = false;
                                isMouseOverButton3 = false;
                                break;
                            }
                            case SDLK_UP:
                            if (!isMouseOverButton1 && !isMouseOverButton2 && !isMouseOverButton3){
                                isMouseOverButton3 = true;
                                break;
                            }
                            else if (isMouseOverButton1)
                            {
                               isMouseOverButton1 = false;
                               isMouseOverButton2 = false;
                               isMouseOverButton3 = true;
                                break;
                            }
                            else if (isMouseOverButton2){
                                isMouseOverButton1 = true;
                                isMouseOverButton2 = false;
                                isMouseOverButton3 = false;
                                break;
                            }
                            else if (isMouseOverButton3){
                                isMouseOverButton1 = false;
                                isMouseOverButton2 = true;
                                isMouseOverButton3 = false;
                                break;
                            }
                            case SDLK_w:
                            if (!isMouseOverButton1 && !isMouseOverButton2 && !isMouseOverButton3){
                                isMouseOverButton3 = true;
                                break;
                            }
                            else if (isMouseOverButton1)
                            {
                               isMouseOverButton1 = false;
                               isMouseOverButton2 = false;
                               isMouseOverButton3 = true;
                                break;
                            }
                            else if (isMouseOverButton2){
                                isMouseOverButton1 = true;
                                isMouseOverButton2 = false;
                                isMouseOverButton3 = false;
                                break;
                            }
                            else if (isMouseOverButton3){
                                isMouseOverButton1 = false;
                                isMouseOverButton2 = true;
                                isMouseOverButton3 = false;
                                break;
                            }
                            case SDLK_ESCAPE: // Wenn die Escape-Taste gedrückt wird
                                quit = true; // Das Spiel wird beendet
                                break;
                            default:
                                break;
                        }
                            if(isMouseOverButton1){
                                switch(e.key.keysym.sym) {
                                case SDLK_RETURN: // Wenn die Eingabetaste gedrückt wird
                                start = true; // Das Spiel startet
                                break;
                                }
                            }
                            else if(isMouseOverButton2){
                                switch(e.key.keysym.sym) {    
                                case SDLK_RETURN: // Wenn die Eingabetaste gedrückt wird
                                inMenu = false;
                                inscore = true; // Scoremenu öffnen
                                break;
                                }
                            }
                            else if(isMouseOverButton3){
                                switch(e.key.keysym.sym) {    
                                case SDLK_RETURN: // Wenn die Eingabetaste gedrückt wird
                                quit = true; // Das Spiel wird beendet
                                break;
                                }
                            }
                }            
                else if (e.type == SDL_MOUSEMOTION) {
                    // Mausbewegung erkannt
                    if (isMouseOverEllipse(mouseX, mouseY, startbutton.Bx, startbutton.By, startbutton.Bradx, startbutton.Brady)) {
                        isMouseOverButton1 = true;
                        isMouseOverButton2 = false;
                        isMouseOverButton3 = false;
                    }
                    else if (isMouseOverEllipse(mouseX, mouseY, endbutton.Bx, endbutton.By, endbutton.Bradx, endbutton.Brady)) {
                        isMouseOverButton1 = false;
                        isMouseOverButton2 = false;
                        isMouseOverButton3 = true;
                    }
                    else if (isMouseOverEllipse(mouseX, mouseY, scorebutton.Bx, scorebutton.By, scorebutton.Bradx, scorebutton.Brady)) {
                        isMouseOverButton1 = false;
                        isMouseOverButton2 = true;
                        isMouseOverButton3 = false;
                    }
                    else{
                        isMouseOverButton1 = false;
                        isMouseOverButton2 = false;
                        isMouseOverButton3 = false;
                    }
                }
                else if (e.type == SDL_MOUSEBUTTONDOWN) {
                // Hier Mausklicks im Menü behandeln
                if (isMouseOverButton1) {
                    start = true; // Das Spiel starten
                } else if (isMouseOverButton2) {
                    inMenu = false;
                    inscore = true; // Das Spiel beenden
                } else if (isMouseOverButton3) {
                    quit = true; // Das Spiel beenden
                }
                }
                if(start){
                delay = startdelay;
                score = 0;
                snake.lenght = 0;
                initSnake(&snake);
                fruits(&fruit);
                inMenu = false; // Das Spiel starten
                }
            }
        }
            // Hintergrund löschen
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            if (inMenu) {
                // Zeichnen Sie das Menü, wenn der Benutzer sich im Menü befindet
                drawMenu(renderer);
            }
            else if (inscore) {
                if (newHighscore){
                    enterName(renderer, &newHighscore);
                }
                else{
                drawScore(renderer);    
                }
            }
            else {
                // Snake zeichnen
                drawSnake(renderer, &snake, &fruit);
                // Bildschirm aktualisieren
                SDL_RenderPresent(renderer);

                // Kurze Verzögerung, um das Spiel nicht zu schnell zu machen
                SDL_Delay(delay);
            }
        }

        // Aufräumen
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        TTF_Quit();

    return 0;
}

void initSnake(struct SNAKE *snake){
    snake->position.x = WINDOW_WIDTH / 2;
    snake->position.y = WINDOW_HEIGHT / 2;

    // Setze die Standardbewegungsrichtung der Schlange
    snake->direction = RIGHT;
}

void moveSnake(struct SNAKE *snake){
    // Setze die Position des ersten Körperteils auf die Position des Kopfes
    snake->body[0] = snake->position;

    snake->direction = nextDirection;
    switch (snake->direction){
        case UP:
        snake->position.y -= CELL_SIZE;
        break;
        case DOWN:
        snake->position.y += CELL_SIZE;
        break;
        case RIGHT:
        snake->position.x += CELL_SIZE;
        break;
        case LEFT:
        snake->position.x -= CELL_SIZE;
        break;
    }

    for(int i = snake->lenght ; i > 0; i--) {
        snake->body[i] = snake->body[i - 1];
    }
}

void handleInput(struct SNAKE *snake, bool *inMenu) {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym) {
                    case SDLK_RETURN:
                    if (!isPaused){
                        isPaused = 1;
                        break;
                        }
                    else {
                        isPaused = 0;
                        break;
                    }
                    case SDLK_ESCAPE:
                        *inMenu = true;
                        break;           
                    case SDLK_UP:
                        if(snake->direction != DOWN) {
                            nextDirection = UP;
                        }
                        break;
                        case SDLK_w:
                        if(snake->direction != DOWN) {
                            nextDirection = UP;
                        }
                        break;
                    case SDLK_DOWN:
                        if(snake->direction != UP) {
                            nextDirection = DOWN;
                        }
                        break;
                    case SDLK_s:
                        if(snake->direction != UP) {
                            nextDirection = DOWN;
                        }
                        break;
                    case SDLK_LEFT:
                        if(snake->direction != RIGHT) {
                            nextDirection = LEFT;
                        }
                        break;
                        case SDLK_a:
                        if(snake->direction != RIGHT) {
                            nextDirection = LEFT;
                        }
                        break;
                    case SDLK_RIGHT:
                        if(snake->direction != LEFT) {
                            nextDirection = RIGHT;
                        }
                        break;
                    case SDLK_d:
                        if(snake->direction != LEFT) {
                            nextDirection = RIGHT;
                        }
                        break;
                    default:
                        break;
                }
                break;
            case SDL_QUIT:
                // Handle window close event
                break;
            default:
                break;
        }
    }
}

void handlecollision (struct SNAKE *snake, bool *inScore, struct FRUIT *fruit, bool *newHighscore, bool *inMenu){

    int headx = snake->position.x;
    int heady = snake->position.y;
    bool collision = 0;
    // Wand kollision
    if (headx < 0 || headx > WINDOW_WIDTH-10 || heady < 0 || heady > WINDOW_HEIGHT-10){
            collision = 1;
    }
    // Kollision mit sich selbst
    for (int i = 0; i < (snake->lenght ); i++ ){
        if (snake->body[i].x == headx && snake->body[i].y == heady)
        collision = 1;
    }
    // Frucht berührt
    if (headx == fruit->fruitposition.x && heady == fruit->fruitposition.y) {
// Aktualisiere die Position des neuen Körperteils
    snake->body[snake->lenght +1 ].x = snake->body[snake->lenght - 1].x;
    snake->body[snake->lenght +1 ].y = snake->body[snake->lenght - 1].y;
            snake->lenght++;
            score++;
            fruits(fruit);
        if(delay > maxspeed){
            delay -= faster;
        }
    }

    if(collision == 1){
        if (score > highscores[4].hscore){
            *newHighscore = true;
            *inMenu = false;
            *inScore = true;
        }
        else{
            *inMenu = true;
        }
  
    }
}

void drawSnake(SDL_Renderer *renderer, struct SNAKE *snake, struct FRUIT *fruit){
    // Kopf zeichnen
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Grün für den Kopf
    SDL_Rect headRect = {snake->position.x-5, snake->position.y-5, CELL_SIZE + 10, CELL_SIZE + 10};
    SDL_RenderFillRect(renderer, &headRect);
    // Körperteile zeichnen
    SDL_SetRenderDrawColor(renderer, 150, 238, 150, 100); // Hellgrün für den Körper

        for (int i = 1; i < snake->lenght+1; i++) {
            SDL_Rect bodyRect = {snake->body[i].x, snake->body[i].y, CELL_SIZE, CELL_SIZE};
            SDL_RenderFillRect(renderer, &bodyRect);
        }
    
    // Früchte zeichnen
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Rot für den Kopf
    SDL_Rect fruitRect = {fruit->fruitposition.x, fruit->fruitposition.y, CELL_SIZE, CELL_SIZE};
    SDL_RenderFillRect(renderer, &fruitRect);

    // score zeichnen
    TTF_Font *scorefont;
    Fontsize (&scorefont, Fontpath, 28);
    sprintf(textscore, "%d", score);
    SDL_Texture* ScoreText;
    rendertext(renderer, &scorefont, &ScoreText, textscore, 20, 20);

}

void fruits(struct FRUIT *fruit){
fruit->fruitposition.x = (rand() % (WINDOW_WIDTH / CELL_SIZE)) * CELL_SIZE; // Zufällige x-Position im Raster
fruit->fruitposition.y = (rand() % (WINDOW_HEIGHT / CELL_SIZE)) * CELL_SIZE; // Zufällige y-Position im Raster
}

void Fontsize(TTF_Font **fontname, char *fontPath, int fontSize){
   *fontname = TTF_OpenFont(fontPath, fontSize);
    if (!*fontname) {
        printf("Failed to load font: %s\n", TTF_GetError());
    }
}

void rendertext(SDL_Renderer *renderer,TTF_Font **font,SDL_Texture **TexturName, const char *text, int x, int y){
    SDL_Color textColor = {255, 255, 255}; // Weißer Text
    *TexturName = renderText(text, *font, textColor, renderer);

    SDL_Rect destRect = {x, y, 0, 0}; // Position des Textes
    SDL_QueryTexture(*TexturName, NULL, NULL, &destRect.w, &destRect.h);
    SDL_RenderCopy(renderer, *TexturName, NULL, &destRect);
}

void drawMenu(SDL_Renderer *renderer) {
    // Zeichnen Sie das Menü auf den Renderer
    // Hintergrund löschen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Start Ellipse zeichnen
    if(isMouseOverButton1){
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // weiß
    }
    else{
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 150); // grau
    }
    drawFilledEllipse(renderer, startbutton.Bx, startbutton.By, startbutton.Bradx, startbutton.Brady);  

    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 150); // hellgrau
    drawFilledEllipse(renderer, startbutton.Bx-7, startbutton.By-3, startbutton.Bradx-20, startbutton.Brady-10);

    TTF_Font *menuStart;
    Fontsize(&menuStart, Fontpath, 20);
    SDL_Texture* menuStartText;
    rendertext(renderer, &menuStart, &menuStartText, "Start", startbutton.Bx-(8*5), startbutton.By-(3*5));

    // Score Ellipse zeichnen
    if(isMouseOverButton2){
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // weiß
    }
    else{
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 150); // grau
    }
    drawFilledEllipse(renderer, scorebutton.Bx, scorebutton.By, scorebutton.Bradx, scorebutton.Brady);
    
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 150); // hellgrau
    drawFilledEllipse(renderer, scorebutton.Bx-7, scorebutton.By-3, scorebutton.Bradx-20, scorebutton.Brady-10);

    TTF_Font *menuScore;
    Fontsize(&menuScore, Fontpath, 20);
    SDL_Texture* menuScoreText;
    rendertext(renderer, &menuScore, &menuScoreText, "Highscore", scorebutton.Bx-(12*5), scorebutton.By-(3*5));

    // End Ellipse zeichnen
    if(isMouseOverButton3){
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // weiß       
    }
    else{
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 150); // grau
    }
    drawFilledEllipse(renderer, endbutton.Bx, endbutton.By, endbutton.Bradx, endbutton.Brady); 

    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 150); // hellgrau
    drawFilledEllipse(renderer, endbutton.Bx-7, endbutton.By-3, endbutton.Bradx-20, endbutton.Brady-10);
    TTF_Font *menuEnd;
    Fontsize(&menuEnd, Fontpath, 20);
    SDL_Texture* menuEndText;
    rendertext(renderer, &menuEnd, &menuEndText, "Quit", endbutton.Bx-(8*5)+5, endbutton.By-(3*5));

    SDL_RenderPresent(renderer);
    SDL_Delay(100);
}

void drawEllipse(SDL_Renderer* renderer, int x, int y, int radiusX, int radiusY) {
    float angle = 0.0;
    float angleStep = 0.1; // Step size for angle

    SDL_Point points[3600]; // Array to store points of the ellipse

    // Calculate points of the ellipse based on the parametric equation
    for (int i = 0; i < 3600; i++) {
        points[i].x = x + radiusX * cos(angle);
        points[i].y = y + radiusY * sin(angle);
        angle += angleStep;
    }

    // Draw the ellipse using the calculated points
    SDL_RenderDrawPoints(renderer, points, 3600);
}

void drawFilledEllipse(SDL_Renderer* renderer, int x, int y, int radiusX, int radiusY) {
    int ry = radiusY;
    // Zeichne Kreise mit abnehmendem Radius
    for (int rx = radiusX; rx >= 0; rx--) {
        ry--;
        drawEllipse(renderer, x, y, rx, ry); // Hier benötigst du eine Funktion, um einen Kreis zu zeichnen
    }
}

void drawdEllipseBorder(SDL_Renderer* renderer, int x, int y, int radiusX, int radiusY, int border) {
    int ry = radiusY;
    int rx = radiusX;
    drawEllipse(renderer, x, y, rx, ry);
    // Zeichne Kreise mit abnehmendem Radius
    for (int borderwidth = border; borderwidth >= 0; borderwidth--) {
        if(borderwidth % 2){
            ry--;
        }
        rx--;
        drawEllipse(renderer, x, y, rx, ry); // Hier benötigst du eine Funktion, um einen Kreis zu zeichnen
    }
}

bool isMouseOverEllipse(int mouseX, int mouseY, int ellipseCenterX, int ellipseCenterY, int radiusX, int radiusY) {
    float normalizedX = (float)(mouseX - ellipseCenterX) / radiusX;
    float normalizedY = (float)(mouseY - ellipseCenterY) / radiusY;
    return (normalizedX * normalizedX) + (normalizedY * normalizedY) <= 1.0;
}

void saveHighscores(struct HighscoreEntry highscores[]){

FILE *highscoreFile = fopen("highscores.txt", "w");
    if (highscoreFile != NULL) {
        for (int i = 0; i < MAX_SCORE; i++) {
            fprintf(highscoreFile, "%s %d\n", highscores[i].name, highscores[i].hscore);
        }
        fclose(highscoreFile);
    } else {
        printf("Fehler beim Öffnen der Datei für den Highscore.2\n");
    }
}

void loadHighscores(struct HighscoreEntry highscores[]){

FILE *highscoreFile = fopen("highscores.txt", "r");
    if (highscoreFile != NULL) {
        for (int i = 0; i < MAX_SCORE; i++) {
            fscanf(highscoreFile, "%s %d\n", highscores[i].name, &highscores[i].hscore);
        }
        fclose(highscoreFile);
    } else {
        printf("Fehler beim Öffnen der Datei für den Highscore.3\n");
    }
}

void drawScore(SDL_Renderer *renderer) {
    TTF_Font *Fields;
    Fontsize(&Fields, Fontpath, 15);
    
    int fieldCount = 5; // Anzahl der Highscore-Felder
    
    int scorefield;
    struct button Scorefields[fieldCount];
    SDL_Texture* FieldTexts[fieldCount];
    SDL_Texture* FieldScores[fieldCount];
    
    int xOffset = WINDOW_WIDTH / 4;
    int yOffset = WINDOW_HEIGHT / 8;
    int fieldSpacing = WINDOW_HEIGHT / 8;

    for (int i = 0; i < fieldCount; ++i) {
        Scorefields[i].Bx = xOffset;
        scorefield = 3 * xOffset;
        Scorefields[i].By = yOffset + (i * fieldSpacing);
        Scorefields[i].Bradx = 60;
        Scorefields[i].Brady = 30;
        
        strcpy(Scorefields[i].BText1, highscores[i].name);
        sprintf(Scorefields[i].BText2,"%d", highscores[i].hscore); // Wandele den Score in eine Zeichenkette um

        // Ellipse zeichnen
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 150); // grau
        drawFilledEllipse(renderer, Scorefields[i].Bx, Scorefields[i].By, Scorefields[i].Bradx, Scorefields[i].Brady);  
        drawFilledEllipse(renderer, scorefield, Scorefields[i].By, Scorefields[i].Bradx, Scorefields[i].Brady);
        // Hintergrund für Score-Text
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 150); // hellgrau
        drawFilledEllipse(renderer, Scorefields[i].Bx - 3, Scorefields[i].By - 1, Scorefields[i].Bradx - 10, Scorefields[i].Brady - 5);
        drawFilledEllipse(renderer, scorefield - 3, Scorefields[i].By - 1, Scorefields[i].Bradx - 10, Scorefields[i].Brady - 5);
        // Text rendern
        SDL_Texture* FieldText;
        rendertext(renderer, &Fields, &FieldText, Scorefields[i].BText1, Scorefields[i].Bx - (8 * 5), Scorefields[i].By - (3 * 5));
        FieldTexts[i] = FieldText;
        
        SDL_Texture* FieldScore;
        rendertext(renderer, &Fields, &FieldScore, Scorefields[i].BText2, scorefield-(2*5), Scorefields[i].By - (3 * 5)); // Score-Text weiter rechts platzieren
        FieldScores[i] = FieldScore;
    }

// Leave field Ellipse zeichnen

    if (isMouseOverButton4){
    SDL_SetRenderDrawColor(renderer, 225, 225, 225, 225); // weiß    
    }
    else{                 
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 150); // grau
    }
    drawFilledEllipse(renderer, leavebutton.Bx, leavebutton.By, leavebutton.Bradx, leavebutton.Brady);  

    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 150); // hellgrau
    drawFilledEllipse(renderer, leavebutton.Bx-3, leavebutton.By-1, leavebutton.Bradx-10, leavebutton.Brady-5);

    SDL_Texture* LeaveText;
    rendertext(renderer, &Fields, &LeaveText, leavebutton.BText1, leavebutton.Bx-(6*5), leavebutton.By-(3*5));

    SDL_RenderPresent(renderer);
    SDL_Delay(0);
}

void updateHighscores(struct HighscoreEntry highscores[], int newScore, char *newName) {
    // Füge den neuen Highscore in die Liste ein
    strcpy(highscores[MAX_SCORE - 1].name, newName);
    highscores[MAX_SCORE - 1].hscore = newScore;
    
    // Sortiere die Liste nach absteigenden Highscores
    qsort(highscores, MAX_SCORE, sizeof(struct HighscoreEntry), compareHighscores);
    
    // Speichere die aktualisierte Liste
    saveHighscores(highscores);
}

int compareHighscores(const void *a, const void *b) {
    struct HighscoreEntry *entryA = (struct HighscoreEntry *)a;
    struct HighscoreEntry *entryB = (struct HighscoreEntry *)b;
    
    // Sortiere absteigend nach den Punktzahlen
    return entryB->hscore - entryA->hscore;
}

// Funktion zur Eingabe des Spielernamens
void enterName(SDL_Renderer *renderer, bool *newHighscore) {
    SDL_StartTextInput(); // Aktiviere die Texteingabe

    bool quit = false;
    bool escape = false;
    SDL_Event e;
    int cursor = 0; // Aktuelle Position des Cursors im Namen
    char name[NAME_LENGTH] = { '\0' };
    int i;
    // Schleife zur Behandlung von Benutzereingaben
    while (!quit) {
        i++;
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                // Überprüfe die gedrückte Taste
                switch (e.key.keysym.sym) {
                    case SDLK_RETURN:
                        quit = true; // Beende die Eingabe bei Drücken der Eingabetaste
                        break;
                    case SDLK_ESCAPE:
                        quit = true; // Beende die Eingabe bei Drücken der Eingabetaste
                        escape = true;
                        break;
                    case SDLK_BACKSPACE:
                        // Lösche das Zeichen links vom derzeitigen Cursor
                        if (cursor > 0) {
                            name[cursor - 1] = '\0';
                            cursor--;
                        }
                        break;
                }
            } else if (e.type == SDL_TEXTINPUT) {
                // Behandle eingegebene Zeichen
                if (cursor < NAME_LENGTH - 1) { // Überprüfe, ob der Name nicht die maximale Länge überschreitet
                    strcat(name, e.text.text);
                    cursor++;
                }
            }
        }
        int textWidth, textHeight;
        SDL_Rect textRect = {((1 * WINDOW_WIDTH / 3 )- textWidth + 10), ((6 * WINDOW_HEIGHT / 8) - textHeight+5), textWidth, textHeight};
        SDL_Rect textReset = {((1 * WINDOW_WIDTH / 3 )- textWidth - 50), ((6 * WINDOW_HEIGHT / 8) - textHeight+5), textWidth+100, textHeight};
        //SDL_Rect cursorRect = {textRect.x + textWidth, textRect.y, 2, textHeight}; // Annahme: Der Cursor ist 2 Pixel breit
        SDL_Rect cursorRect = {223, 344, 2, 21};
        // Den Bereich löschen, in dem der Text und der Cursor gezeichnet werden
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Schwarz
        SDL_RenderFillRect(renderer, &textReset); // Hier musst du den Bereich ersetzen, in dem der Text gezeichnet wird
        SDL_RenderFillRect(renderer, &cursorRect); // Hier musst du den Bereich ersetzen, in dem der Cursor gezeichnet wird

        // Den Namen auf den Bildschirm zeichnen
        TTF_Font *font = TTF_OpenFont(Fontpath, 15);
        SDL_Color textColor = {255, 255, 255, 255}; // Weiß
        SDL_Surface *textSurface = TTF_RenderText_Solid(font, name, textColor);
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

        // den Score zeichnen
        char scoreText[10];
        SDL_Texture* Score;
        sprintf(scoreText,"%d", score); // Wandele den Score in eine Zeichenkette um
        rendertext(renderer, &font, &Score, scoreText, (2 * WINDOW_WIDTH / 3)-20, (6 * WINDOW_HEIGHT / 8)-15);

        // Den Cursor zeichnen
        // nur in jeder 4. Schleife
            if(i % 4){
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Weiß
        SDL_RenderFillRect(renderer, &cursorRect);
            }
        drawScore(renderer);
            // Schleife resetten
            if (i == 100){
                i = 0;
            }
        
    }
    SDL_StopTextInput(); // Deaktiviere die Texteingabe nach der Eingabe
    *newHighscore = false;
    if (!escape){
    updateHighscores(highscores, score, name);
    }

}