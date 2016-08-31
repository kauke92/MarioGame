#ifndef LEVEL_H
#define LEVEL_H
#include "dialog.h"
#include <QMessageBox>
#include <QSpacerItem>
#include <QGridLayout>

class Level
{
public:
//    std::,
    std::vector<Object> objectList;
    int time;
    int levelIndex;
    Level(class Dialog& dialog, int index, int time=500);
    ~Level();
    void welcome();


};

#endif // LEVEL_H
