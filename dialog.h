#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QTimer>
#include <Qpainter>
#include <QImage>
#include <iostream>
#include "object.h"
#include "level.h"
#include "game.h"
#include <QLabel>
#include <QMovie>
#include <QKeyEvent>
#include <math.h>
#include <QMessageBox>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QTextEdit>


namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public slots:
    void nextFrame();

public:
	explicit Dialog(QWidget *parent = 0);

	~Dialog();
	void draw(QPainter &painter, Object* object, int x, int y, int width, int height);
	std::string SetIntFromFile(std::string section, std::string key, int *value); // Added


	Ui::Dialog *ui;
	int m_counter;
    Object* m_stickman;
	Object* m_background;
    std::vector<Object*> m_obstacleArray;

	// All below are added
	int m_windowHeight;
	int m_windowWidth;
	int m_movesUp;
	int m_movesDown;
	bool m_inJump;
	int m_jumpsMade;
	int m_maxJumps;
    int m_jumpHeight;
    int distanceMove;
	bool m_hasError;
    bool m_stage2Enabled;
    bool bothKeys;
    bool m_stage3Enabled;
	int totalObstacleCount;
    QImage img_vector[15];
    std::string img_name[15];
    QSet<int> pressedKeys;
    double gravity;
    int numberOfLevels;
    std::vector<int>* levelSetting;
    std::vector<int>& split( std::string &s, char delim, std::vector<int> &elems, int max);
    int powerUp;
    int numLives;
    int numLevels;
    class Game* currentGame;

    Object* checkPoint;
    void gameOver(QPainter &);
    void drawBackground(QPainter &painter);
    void drawStickmanAndObstacles(QPainter& painter);
    void stickmanDropping();
    void showDetails(QPainter& painter);
    void stickmanJump();
    bool checkIfStickmanCanMove(QPainter &);
    void updateObstaclePosition(bool keepStill);
    void updateStickman() ;
    QTimer* timer;
protected:
	void paintEvent(QPaintEvent *);
    bool eventFilter(QObject * obj, QEvent * event);
};

#endif // DIALOG_H
