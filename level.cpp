#include "level.h"

Level::Level(Dialog& dialog, int index, int time)
{
    if (dialog.m_stage3Enabled) {
        //repeating the levels
        int l = (index-1) % dialog.numLevels;
        for (int i=0;i<dialog.levelSetting[l].size();i++) {
            objectList.push_back(*dialog.m_obstacleArray.at(dialog.levelSetting[l].at(i)));
        }

        objectList.push_back(*dialog.checkPoint);
        dialog.pressedKeys.clear();
        dialog.m_counter = 0;
        dialog.distanceMove = 0;
        if (dialog.m_stage3Enabled) {


        }
    } else {
        for (int i=0;i<dialog.m_obstacleArray.size();i++) {
                objectList.push_back(*dialog.m_obstacleArray.at(i));
        }
    }
    this->time=time;
    this->levelIndex = index;

    for (int i = 1; i < objectList.size(); i++) {
        if (objectList.at(i).getSpacing() >= 0) {
            // X-Position of obstacles are based on the previous obstacle's X-Position + Spacing
            objectList.at(i).overrideX(objectList[i - 1].getX()
                    + objectList[i - 1].getWidth() + objectList.at(i).getSpacing());

        }
    }
}
Level::~Level()
{
    objectList.clear();
}
void Level::welcome(){
    QMessageBox messageBox;
    QSpacerItem* horizontalSpacer = new QSpacerItem(500, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QString s = "You've finished level ";
    s.append(QString::number(this->levelIndex-1));
    s.append(". Are you ready for a new level?");
    messageBox.setText( s);
    QGridLayout* layout = (QGridLayout*)messageBox.layout();
    layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

    messageBox.exec();

    //memory management
    for (int i = 0; i < layout->count(); ++i) {
        QLayoutItem *layoutItem = layout->itemAt(i);
        if (layoutItem->spacerItem()) {
            layout->removeItem(layoutItem);
            // You could also use: layout->takeAt(i);
            delete layoutItem;
            --i;
        }
    }
   ;
}

