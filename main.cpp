#include <SDL.h>
#include <SDL_image.h>
#include <bits/stdc++.h>

using namespace std;

// Screen dimensions
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int CELL_SIZE = 20;

// Direction macros
enum KeyPressSurfaces
{
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT,
    DIR_TOTAL
};

// Object structure
struct Object {
    int x, y;

    bool operator == (const Object& o) const {
        return x == o.x && y == o.y;
    }

    void RandomGenerate(){
        x = rand() % (SCREEN_WIDTH / CELL_SIZE) * CELL_SIZE;
        y = rand() % (SCREEN_HEIGHT / CELL_SIZE) * CELL_SIZE;
    }
};

// Function prototypes
void draw(SDL_Renderer *renderer, vector<Object> snake, Object food);
void update(vector<Object>& snake, Object& food, int& DIR);
bool checkCollision(vector<Object> snake);
bool checkFoodCollision(vector<Object> snake, Object food);
Object generateFood(vector<Object> snake);

SDL_Texture* loadTexture(SDL_Renderer* renderer, const string& imagePath) {
    // Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load(imagePath.c_str());
    if (loadedSurface == NULL) {
        cerr << "Unable to load image " << imagePath << "! SDL_image Error: " << IMG_GetError() << endl;
        return NULL;
    }

    // Create texture from surface pixels
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    if (texture == NULL) {
        cerr << "Unable to create texture from " << imagePath << "! SDL Error: " << SDL_GetError() << endl;
        return NULL;
    }

    // Get rid of old loaded surface
    SDL_FreeSurface(loadedSurface);

    return texture;
}

void drawBackground(SDL_Renderer* renderer, SDL_Texture* backgroundTexture) {
    SDL_Rect destRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderCopy(renderer, backgroundTexture, NULL, &destRect);
}

int main(int argc, char *argv[]) {
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
        return 1;
    }

    // Create window
    window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
        return 1;
    }

    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << endl;
        return 1;
    }

    // Initialize SDL_image
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << endl;
        return 1;
    }

    // Load background image
    SDL_Texture* backgroundTexture = loadTexture(renderer, "vecteezy_background-of-green-grass-field-cartoon-drawing_14572097.jpg");
    if (backgroundTexture == NULL) {
        return 1;
    }

    // Seed random number generator
    srand(time(NULL));

    // Initialize snake
    vector<Object> snake;
    snake.push_back({SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2});

    // Initialize food
    Object food = generateFood(snake);
    int DIR = rand()%DIR_TOTAL;


    bool quit = false;
    SDL_Event e;

    // Game loop
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if(e.type == SDL_KEYDOWN){
                switch(e.key.keysym.sym){
                    case SDLK_UP:
                    if(DIR != DIR_DOWN) DIR = DIR_UP;
                    break;

                    case SDLK_DOWN:
                    if(DIR != DIR_UP) DIR = DIR_DOWN;
                    break;

                    case SDLK_LEFT:
                    if(DIR != DIR_RIGHT) DIR = DIR_LEFT;
                    break;

                    case SDLK_RIGHT:
                    if(DIR != DIR_LEFT) DIR = DIR_RIGHT;
                    break;

                }
            }
        }
        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Update and draw
        update(snake, food, DIR);
        // Draw background
        drawBackground(renderer, backgroundTexture);
        draw(renderer, snake, food);

        // Update screen
        SDL_RenderPresent(renderer);

        // Delay to control frame rate
        SDL_Delay(100);
    }

    // Clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void draw(SDL_Renderer *renderer, vector<Object> snake, Object food) {
    // Draw snake
    for (const auto& segment : snake) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        if(segment.x==snake.front().x && segment.y==snake.front().y) SDL_SetRenderDrawColor(renderer, 0, 255, 255, 0);
        SDL_Rect rect = {segment.x, segment.y, CELL_SIZE, CELL_SIZE};
        SDL_RenderFillRect(renderer, &rect);
    }

    // Draw food
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect rect = {food.x, food.y, CELL_SIZE, CELL_SIZE};
    SDL_RenderFillRect(renderer, &rect);
}

void update(vector<Object>& snake, Object& food, int& DIR) {
    // Move snake

    Object prv_back = snake.back();
    for (int i = snake.size() - 1; i > 0; --i) {
        snake[i] = snake[i - 1];
    }

    switch (DIR) {
        case DIR_UP:
            snake.front().y -= CELL_SIZE;
            break;
        case DIR_DOWN:
            snake.front().y += CELL_SIZE;
            break;
        case DIR_LEFT:
            snake.front().x -= CELL_SIZE;
            break;
        case DIR_RIGHT:
            snake.front().x += CELL_SIZE;
            break;
    }

    //snake goes through walls
    if(snake.front().x<0) snake.front().x = SCREEN_WIDTH - CELL_SIZE;
    else if(snake.front().x==SCREEN_WIDTH) snake.front().x = 0;

    if(snake.front().y<0) snake.front().y = SCREEN_HEIGHT - CELL_SIZE;
    else if(snake.front().y==SCREEN_HEIGHT) snake.front().y = 0;

    // Check collision with food
    if (checkFoodCollision(snake, food)) {
        snake.push_back(prv_back);
        food = generateFood(snake);
    }

    // Check collision with itself
    if (checkCollision(snake)) {
        snake.clear();
        snake.push_back({SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2});
    }
}

bool checkCollision(vector<Object> snake) {
    // Check collision with walls
//    if (snake.front().x < 0 || snake.front().x >= SCREEN_WIDTH || snake.front().y < 0 || snake.front().y >= SCREEN_HEIGHT) {
//        return true;
//    }

    // Check collision with itself
    for (int i = 1; i < snake.size(); ++i) {
        if (snake.front().x == snake[i].x && snake.front().y == snake[i].y) {
            return true;
        }
    }

    return false;
}

bool checkFoodCollision(vector<Object> snake, Object food) {
    for(Object segment: snake){
        if(segment == food){
            return true;
        }
    }
    return false;
}

Object generateFood(vector<Object> snake) {
    Object food;

    do{
        food.RandomGenerate();
    }while(checkFoodCollision(snake, food));

    return food;
}
