#ifndef LEADERBOARD_H
#define LEADERBOARD_H
#include <QString>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <exception>
#include <QPainter>
class LeaderBoard
{
public:
    LeaderBoard();
    ~LeaderBoard();
    long scores[10];
    std::string names[10];
    void show(QPainter& painter);

};

#endif // LEADERBOARD_H
