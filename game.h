#ifndef GAME_H
#define GAME_H
#include "dialog.h"
#include "leaderboard.h"
#include <QApplication>
#include <QInputDialog>
#include <QtGui>

class Game
{
public:
    Game(class Dialog& dialog,int numLives);
    ~Game();
    int numLives;
    int score;
    bool userStarted;
    std::string userName;
    class Level* currentLevel;
    LeaderBoard* board;
    void setCurrent(Level* s) {
        currentLevel = s;
    }
    void welcome(QPainter& painter);
    Object m_stickman;

};

#endif // GAME_H
