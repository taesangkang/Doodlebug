#include <iostream>
#include <ctime>
#include <vector>
#include <cstdlib>
#include <random>

const int NUM_OF_DOODLEBUGS = 5;
const int NUM_OF_ANTS = 100;
const int WORLD_SIZE = 20;
const char EMPTY_SPACE = '-';
const char DOODLE_BUG = 'X';
const char ANT = 'O';
const int DIRECTIONS[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};


class World{
public:
    World() : world(WORLD_SIZE, std::vector<char>(WORLD_SIZE, EMPTY_SPACE)) {}
    std::vector<std::vector<char>> world;

    void displayWorld(){
        for(const std::vector<char>& row : world){
            for(char cell : row){
                std::cout << cell << "  ";
            }
            std::cout << std::endl;
        }
    }
    bool isCellEmpty(int x, int y){
        return world[y][x] == EMPTY_SPACE;
    }
    void placeOrganism(int x, int y, char symbol){
        world[y][x] = symbol;
    }
};
class Organism{
public:
    virtual char symbol() = 0;
    virtual void move() = 0;
    virtual void breed() = 0;
protected:
    int x,y;
    World& world;
    Organism(int X, int Y, World& reference) : x(X), y(Y), world(reference){}
};
class DoodleBug : public Organism{
public:
    int breedingCounter = 0;
    int starvationCounter = 0;
    DoodleBug(int X, int Y, World& reference) : Organism(X,Y, reference){}
    char symbol() override{
        return DOODLE_BUG;
    }
    void move() override {
        bool hasMoved = false;
        bool hasEaten = false;

        for(auto i : DIRECTIONS) {
            int newX = x + i[0];
            int newY = y + i[1];

            if (newX >= 0 && newX < WORLD_SIZE && newY >= 0 && newY < WORLD_SIZE && world.world[newY][newX] == ANT) {
                world.world[y][x] = EMPTY_SPACE;
                world.world[newY][newX] = DOODLE_BUG;
                x = newX;
                y = newY;
                hasEaten = true;
                hasMoved = true;
                break;
            }
        }
        if (!hasMoved) {
            int attempts = 0;
            while (attempts < 4 && !hasMoved) {
                int randomIndex = rand() % 4;
                int newX = x + DIRECTIONS[randomIndex][0];
                int newY = y + DIRECTIONS[randomIndex][1];

                if (newX >= 0 && newX < WORLD_SIZE && newY >= 0 && newY < WORLD_SIZE && world.world[newY][newX] == EMPTY_SPACE) {
                    world.world[y][x] = EMPTY_SPACE;
                    world.world[newY][newX] = DOODLE_BUG;
                    x = newX;
                    y = newY;
                    hasMoved = true;
                }
                attempts++;
            }
        }
        if (!hasEaten) {
            starvationCounter++;
        } else {
            starvationCounter = 0;
        }
        breedingCounter++;
    }

    void breed() override{
        if(breedingCounter == 8){
            bool bred = false;
            for(auto i : DIRECTIONS) {
                int newX = x + i[0];
                int newY = y + i[1];
                if (newX >= 0 && newX < WORLD_SIZE && newY >= 0 && newY < WORLD_SIZE && world.world[newY][newX] == EMPTY_SPACE) {
                    world.world[y][x] = EMPTY_SPACE;
                    world.world[newY][newX] = DOODLE_BUG;
                    x = newX;
                    y = newY;
                    bred = true;
                }
                if(bred){
                    break;
                }
            }
        }
    }
    void starve(){
        if(starvationCounter >= 3){
            world.world[y][x] = EMPTY_SPACE;
        }
    }
};

class Ant : public Organism{
public:
    Ant(int X, int Y, World& reference) : Organism(X,Y, reference){}
    int breedingCounter = 0;

    char symbol() override{
        return ANT;
    }
    void move() override{
        bool hasMoved = false;
        int attempts = 0;
        while(!hasMoved || attempts <= 4){
            int randomIndex = rand() % 4;
            int newX = x + DIRECTIONS[randomIndex][0];
            int newY = y + DIRECTIONS[randomIndex][1];

            if (newX >= 0 && newX < WORLD_SIZE && newY >= 0 && newY < WORLD_SIZE && world.world[newY][newX] == EMPTY_SPACE) {
                world.world[y][x] = EMPTY_SPACE;
                world.world[newY][newX] = ANT;
                x = newX;
                y = newY;
                hasMoved = true;
            }
            attempts++;
            breedingCounter++;
        }
    }
    void breed() override{
        if(breedingCounter == 3){
            bool bred = false;
            for(auto i : DIRECTIONS) {
                int newX = x + i[0];
                int newY = y + i[1];
                if (newX >= 0 && newX < WORLD_SIZE && newY >= 0 && newY < WORLD_SIZE && world.world[newY][newX] == EMPTY_SPACE) {
                    world.world[y][x] = EMPTY_SPACE;
                    world.world[newY][newX] = DOODLE_BUG;
                    x = newX;
                    y = newY;
                    bred = true;
                }
                if(bred){
                    break;
                }
            }
        }
    }
};

void initializeOrganisms(World& world, std::vector<Organism*>& organisms){
    for(int i = 0; i < NUM_OF_DOODLEBUGS; i++){
        int x, y;
        do {
            x = rand() % WORLD_SIZE;
            y = rand() % WORLD_SIZE;
        } while (!world.isCellEmpty(x, y));
        organisms.push_back(new DoodleBug(x, y, world));
        world.placeOrganism(x, y, DOODLE_BUG);
    }
    for(int i = 0; i < NUM_OF_ANTS; i++){
        int x, y;
        do {
            x = rand() % WORLD_SIZE;
            y = rand() % WORLD_SIZE;
        } while (!world.isCellEmpty(x, y));
        organisms.push_back(new Ant(x, y, world));
        world.placeOrganism(x, y, ANT);
    }
}

void simulate(World& world, std::vector<Organism*>& organisms){
    std::string input;
    int timeCounter = 0;
    do{
        std::cout << "World at time " << timeCounter << ":" << std::endl;
        for(Organism* org : organisms){
            org->move();
            org->breed();
        }
        world.displayWorld();
        std::cout << "Press enter to continue, enter in any other key to stop: ";
        getline(std::cin, input);
        timeCounter++;
    }   while(input.empty());
}

int main(){
    World world;
    std::vector<Organism*> organisms;

    initializeOrganisms(world, organisms);
    simulate(world, organisms);

    for(Organism* org: organisms){
        delete org;
    }
    organisms.clear();

    return 0;
}