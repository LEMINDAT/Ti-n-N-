#include <SDL.h>
#include <bits/stdc++.h>

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

    Object Random_generate(){
        x = rand() % (SCREEN_WIDTH / CELL_SIZE) * CELL_SIZE;
        y = rand() % (SCREEN_HEIGHT / CELL_SIZE) * CELL_SIZE;
    }
};

// Function prototypes
void draw(SDL_Renderer *renderer, const std::vector<Object>& snake, const Object& food);
void update(std::vector<Object>& snake, Object& food, int& direction);
bool checkCollision(const std::vector<Object>& snake);
bool checkFoodCollision(const std::vector<Object>& snake, const Object& food);
Object generateFood(const std::vector<Object>& snake);

int main(int argc, char *argv[]) {
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create window
    window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Seed random number generator
    std::srand(std::time(NULL));

    // Initialize snake
    std::vector<Object> snake;
    snake.push_back({SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2});

    // Initialize food
    Object food = generateFood(snake);
    int direction = rand()%DIR_TOTAL;


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
                    if(direction != DIR_DOWN) direction = DIR_UP;
                    break;

                    case SDLK_DOWN:
                    if(direction != DIR_UP) direction = DIR_DOWN;
                    break;

                    case SDLK_LEFT:
                    if(direction != DIR_RIGHT) direction = DIR_LEFT;
                    break;

                    case SDLK_RIGHT:
                    if(direction != DIR_LEFT) direction = DIR_RIGHT;
                    break;

                }
            }
        }
        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Update and draw
        update(snake, food, direction);
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

void draw(SDL_Renderer *renderer, const std::vector<Object>& snake, const Object& food) {
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

void update(std::vector<Object>& snake, Object& food, int& direction) {
    // Move snake

    Object prv_back = snake.back();
    for (int i = snake.size() - 1; i > 0; --i) {
        snake[i] = snake[i - 1];
    }

    switch (direction) {
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

bool checkCollision(const std::vector<Object>& snake) {
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

bool checkFoodCollision(const std::vector<Object>& snake, const Object& food) {
    return snake.front().x == food.x && snake.front().y == food.y;
}

Object generateFood(const std::vector<Object>& snake) {
    Object food;
    bool valid = false;

    while (!valid) {
        food.Random_generate();

        valid = true;
        for (const auto& segment : snake) {
            if (segment.x == food.x && segment.y == food.y) {
                valid = false;
                break;
            }
        }
    }

    return food;
}

