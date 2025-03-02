#include <iostream>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <conio.h>
#include <ctime>
#include <algorithm>
#include <fstream>

using namespace std;

struct Player {
    string name;
    int score;
};


void ShowMenu(int selectedOption);
void MoveD(int &selectedOption, int numOptions);
void MoveU(int &selectedOption, int numOptions);
void MainGame();
void moveInvaders(vector<pair<int, int>>& invaders, int& direction, const int width);
void movePlayer(int& playerX, char move);
void moveBullet(int& bulletX, int& bulletY, bool& bulletActive);
void moveEnemyBullets(vector<pair<int, int>>& enemyBullets);
void checkBulletHitAndLives(vector<pair<int, int>>& invaders, int bulletX, int bulletY, int& lives, bool& bulletActive, int playerX, int playerY, vector<pair<int, int>>& enemyBullets, int& score, vector<pair<int, int>>& aliens, int& spaceshipX, int& spaceshipY, bool& spaceshipActive);
void draw(int playerX, int playerY, const vector<pair<int, int>>& aliens, const vector<pair<int, int>>& invaders, int lives, int score, int bulletX, int bulletY, bool bulletActive, const vector<pair<int, int>>& enemyBullets, int spaceshipX, int spaceshipY, bool spaceshipActive);
void HowToPlay();
void ShowLeaderboard();
void savePlayerScore(const Player& player);
void LoadGame(Player &player);
void saveGameState(int playerX, int playerY, int score, int lives, vector<pair<int, int>>& invaders, vector<pair<int, int>>& enemyBullets);

int main() {
    int selectedOption = 0;
    const int numOptions = 5;
    bool running = true;
    Player player;

    while (running) {
        system("cls");
        ShowMenu(selectedOption);

        char input = _getch();
        if (input == 's' || input == 'S') {
            MoveD(selectedOption, numOptions);
        } else if (input == 'w' || input == 'W') {
            MoveU(selectedOption, numOptions);
        } else if (input == 'e' || input == 'E') {
            system("cls");
            cout << "You selected option: ";
            switch (selectedOption) {
                case 0:
                    cout << "New Game";
                    MainGame();
                    break;
                case 1:
                    LoadGame(player);
                    MainGame();
                    break;
                case 2:
                    HowToPlay();
                    break;
                case 3:
                    ShowLeaderboard();
                    break;
                case 4:
                    cout << "Exit";
                    running = false;
                    break;
            }
            cout << endl;
        }
    }

    return 0;
}

void ShowMenu(int selectedOption) {
    string options[] = {
            "1. New Game",
            "2. Load Game",
            "3. How to Play",
            "4. Leaderboard",
            "5. Exit"
    };

    const int width = 30;

    cout << "\u250C" << string(width - 2, '-') << "\u2510" << endl;

    for (int i = 0; i < 5; i++) {
        cout << "\u2502 ";
        if (i == selectedOption) {
            cout << "[" << options[i] << "]";
        } else {
            int spaces = width - 4 - options[i].length();
            cout << options[i];
            for (int j = 0; j < spaces; j++) {
                cout << " ";
            }
        }
        cout << " \u2502" << endl;
    }

    cout << "\u2514" << string(width - 2, '-') << "\u2518" << endl;
}

void MoveD(int &selectedOption, int numOptions) {
    selectedOption = (selectedOption + 1) % numOptions;
}

void MoveU(int &selectedOption, int numOptions) {
    selectedOption = (selectedOption - 1 + numOptions) % numOptions;
}


void MainGame() {
    const int width = 30;
    const int height = 20;

    Player player;

    system("cls");
    cout << "Enter your name: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, player.name);
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    int playerX = 14, playerY = 18;
    int lives = 3;
    int score = 0;

    vector<pair<int, int>> aliens = {{5, 13}, {15, 13}, {25, 13}};
    vector<pair<int, int>> invaders;
    for (int i = 0; i < 10; i++) {
        invaders.push_back({i + 10, 2});
        invaders.push_back({i + 10, 3});
        invaders.push_back({i + 10, 4});
    }

    int direction = 1;
    auto lastMoveTime = chrono::steady_clock::now();
    auto lastGameTime = chrono::steady_clock::now();

    int bulletX = -1, bulletY = -1;
    bool bulletActive = false;

    vector<pair<int, int>> enemyBullets;

    int spaceshipX = width, spaceshipY = 0;
    bool spaceshipActive = false;
    auto lastSpaceshipTime = chrono::steady_clock::now();

    srand(time(0));

    bool gamePaused = false;

    while (true) {
        if (!gamePaused) {
            auto currentTime = chrono::steady_clock::now();

            if (chrono::duration_cast<chrono::seconds>(currentTime - lastMoveTime).count() >= 2) {
                moveInvaders(invaders, direction, width);
                lastMoveTime = currentTime;
            }

            if (rand() % 10 < 2) {
                int invader = rand() % invaders.size();
                enemyBullets.push_back({invaders[invader].first, invaders[invader].second + 1});
            }

            if (chrono::duration_cast<chrono::seconds>(currentTime - lastSpaceshipTime).count() >= 10) {
                spaceshipActive = true;
                spaceshipX = width - 1;
                lastSpaceshipTime = currentTime;
            }

            if (spaceshipActive) {
                spaceshipX--;
                if (spaceshipX < 0) {
                    spaceshipActive = false;
                }
            }

            if (_kbhit()) {
                char move = _getch();
                if (move == 'q') break;
                if (move == 'p' || move == 'P') {
                    gamePaused = true;
                    saveGameState(playerX, playerY, score, lives, invaders, enemyBullets);
                    ShowMenu(0);
                    break;
                }
                if (move == ' ') {
                    if (!bulletActive) {
                        bulletX = playerX;
                        bulletY = playerY - 1;
                        bulletActive = true;
                    }
                } else {
                    movePlayer(playerX, move);
                }
            }

            moveBullet(bulletX, bulletY, bulletActive);
            moveEnemyBullets(enemyBullets);
            checkBulletHitAndLives(invaders, bulletX, bulletY, lives, bulletActive, playerX, playerY, enemyBullets, score, aliens, spaceshipX, spaceshipY, spaceshipActive);
            draw(playerX, playerY, aliens, invaders, lives, score, bulletX, bulletY, bulletActive, enemyBullets, spaceshipX, spaceshipY, spaceshipActive);

            if (lives == 0) {
                player.score = score;
                savePlayerScore(player);
                cout << "Game Over!" << endl;
                break;
            }

            this_thread::sleep_for(chrono::milliseconds(1000));
        }
    }
}
void saveGameState(int playerX, int playerY, int score, int lives, vector<pair<int, int>>& invaders, vector<pair<int, int>>& enemyBullets) {
    ofstream saveFile("game_save.txt");
    if (saveFile.is_open()) {
        saveFile << playerX << " " << playerY << " " << score << " " << lives << endl;
        saveFile << invaders.size() << endl;
        for (auto& invader : invaders) {
            saveFile << invader.first << " " << invader.second << endl;
        }
        saveFile << enemyBullets.size() << endl;
        for (auto& bullet : enemyBullets) {
            saveFile << bullet.first << " " << bullet.second << endl;
        }
    }
}

void LoadGame(Player &player) {
    ifstream saveFile("game_save.txt");
    if (saveFile.is_open()) {
        saveFile >> player.score;
        int playerX, playerY, score, lives, invaderCount, bulletCount;
        saveFile >> playerX >> playerY >> score >> lives;
        saveFile >> invaderCount;
        vector<pair<int, int>> invaders;
        for (int i = 0; i < invaderCount; i++) {
            int x, y;
            saveFile >> x >> y;
            invaders.push_back({x, y});
        }
        saveFile >> bulletCount;
        vector<pair<int, int>> enemyBullets;
        for (int i = 0; i < bulletCount; i++) {
            int x, y;
            saveFile >> x >> y;
            enemyBullets.push_back({x, y});
        }
    }
}

void savePlayerScore(const Player& player) {
    ofstream outFile("leaderboard.txt", ios::app);
    if (outFile.is_open()) {
        outFile << player.name << " " << player.score << endl;
        outFile.close();
    }
}

void ShowLeaderboard() {
    ifstream inFile("leaderboard.txt");
    if (!inFile) {
        cout << "No leaderboard found!" << endl;
        return;
    }

    vector<Player> leaderboard;
    string name;
    int score;
    while (inFile >> name >> score) {
        leaderboard.push_back({name, score});
    }

    sort(leaderboard.begin(), leaderboard.end(), [](const Player& a, const Player& b) {
        return a.score > b.score;
    });

    cout << "Leaderboard:" << endl;
    for (const auto& p : leaderboard) {
        cout << p.name << " - " << p.score << endl;
    }
    inFile.close();
}

void moveInvaders(vector<pair<int, int>>& invaders, int& direction, const int width) {
    for (auto& invader : invaders) {
        invader.first += direction;
    }

    for (const auto& invader : invaders) {
        if (invader.first <= 1 || invader.first >= width - 2) {
            direction *= -1;
            for (auto& inv : invaders) {
                inv.second += 1;
            }
            break;
        }
    }
}

void movePlayer(int& playerX, char move) {
    if (move == 'a' && playerX > 1) {
        playerX--;
    } else if (move == 'd' && playerX < 28) {
        playerX++;
    }
}

void moveBullet(int& bulletX, int& bulletY, bool& bulletActive) {
    if (bulletActive) {
        bulletY--;
        if (bulletY < 0) {
            bulletActive = false;
        }
    }
}

void moveEnemyBullets(vector<pair<int, int>>& enemyBullets) {
    for (auto& bullet : enemyBullets) {
        bullet.second++;
    }
    enemyBullets.erase(remove_if(enemyBullets.begin(), enemyBullets.end(),
                                 [](const pair<int, int>& bullet) { return bullet.second >= 20; }), enemyBullets.end());
}

void checkBulletHitAndLives(vector<pair<int, int>>& invaders, int bulletX, int bulletY, int& lives, bool& bulletActive, int playerX, int playerY, vector<pair<int, int>>& enemyBullets, int& score, vector<pair<int, int>>& aliens, int& spaceshipX, int& spaceshipY, bool& spaceshipActive) {
    for (auto it = invaders.begin(); it != invaders.end(); ) {
        if (it->first == bulletX && it->second == bulletY) {
            it = invaders.erase(it);
            bulletActive = false;
            score += 50;
            return;
        } else {
            ++it;
        }
    }

    if (bulletX == playerX && bulletY == playerY) {
        lives--;
        bulletActive = false;
        if (lives == 0) {
            cout << "Game Over!" << endl;
            exit(0);
        }
    }

    for (auto it = enemyBullets.begin(); it != enemyBullets.end(); ) {
        if (it->first == playerX && it->second == playerY) {
            lives--;
            it = enemyBullets.erase(it);
            if (lives == 0) {
                cout << "Game Over!" << endl;
                exit(0);
            }
        } else {
            ++it;
        }
    }

    for (auto it = aliens.begin(); it != aliens.end(); ) {
        if ((it->first == bulletX && it->second == bulletY) ||
            (it->first + 1 == bulletX && it->second == bulletY) ||
            (it->first == bulletX && it->second + 1 == bulletY) ||
            (it->first + 1 == bulletX && it->second + 1 == bulletY)) {
            it = aliens.erase(it);
            bulletActive = false;
            return;
        } else {
            ++it;
        }
    }

    if (spaceshipActive && spaceshipX <= bulletX && spaceshipX + 3 >= bulletX && spaceshipY == bulletY) {
        spaceshipActive = false;
        score += 100;
    }
}


void draw(int playerX, int playerY, const vector<pair<int, int>>& aliens, const vector<pair<int, int>>& invaders, int lives, int score, int bulletX, int bulletY, bool bulletActive, const vector<pair<int, int>>& enemyBullets, int spaceshipX, int spaceshipY, bool spaceshipActive) {
    const int width = 30;
    const int height = 20;

    system("cls");

    cout << "Lives: ";
    for (int i = 0; i < lives; i++) {
        cout << "* ";
    }
    cout << "   ";

    cout << "Score: " << score << endl;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            bool printed = false;

            for (const auto& invader : invaders) {
                if (invader.first == j && invader.second == i) {
                    cout << "X";
                    printed = true;
                    break;
                }
            }

            for (const auto& alien : aliens) {
                if ((alien.first == j && alien.second == i) ||
                    (alien.first + 1 == j && alien.second == i) ||
                    (alien.first == j && alien.second + 1 == i) ||
                    (alien.first + 1 == j && alien.second + 1 == i)) {
                    cout << "@";
                    printed = true;
                    break;
                }
            }

            if (playerX == j && playerY == i) {
                cout << "^";
                printed = true;
            }

            if (bulletActive && bulletX == j && bulletY == i) {
                cout << "|";
                printed = true;
            }

            for (const auto& bullet : enemyBullets) {
                if (bullet.first == j && bullet.second == i) {
                    cout << "!";
                    printed = true;
                }
            }

            if (spaceshipActive && spaceshipX <= j && spaceshipX + 2 >= j && spaceshipY == i) {
                cout << "$";
                printed = true;
            }

            if (i == 0 || i == height - 1 || j == 0 || j == width - 1) {
                cout << "#";
            } else if (!printed) {
                cout << " ";
            }
        }
        cout << endl;
    }
}
void HowToPlay() {
    system("cls");
    cout << "==============================" << endl;
    cout << "       How to Play" << endl;
    cout << "==============================" << endl;
    cout << "Space Invaders is a classic shooting game.\n";
    cout << "You control a spaceship and must defeat the invaders.\n";
    cout << "\nControls:" << endl;
    cout << " - Move Left:   A" << endl;
    cout << " - Move Right:  D" << endl;
    cout << " - Shoot:       Spacebar" << endl;
    cout << " - Exit Game:   Q" << endl;
    cout << "\nDestroy all enemies and survive as long as possible!\n";
    cout << "Press any key to go back to the menu...";
    _getch();
}
