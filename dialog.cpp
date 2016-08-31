#include "dialog.h"
#include "ui_dialog.h"
#include "object.h"
#include "iofile.h"
#include <QDebug>

void Dialog::gameOver(QPainter& painter) {
    //if the new score > the lowest score in the board, add it to the end
    if (currentGame->score > currentGame->board->scores[9]) {
        currentGame->board->scores[9] = currentGame->score;
        currentGame->board->names[9] = currentGame->userName.c_str();
    }

    // sort the board
    for (int i = 0; i <= 8; i++) {
        for (int j = i + 1;j <= 9; j++) {
            if (currentGame->board->scores[i]<currentGame->board->scores[j]) {
                long temp = currentGame->board->scores[i];
                currentGame->board->scores[i] = currentGame->board->scores[j];
                currentGame->board->scores[j] = temp;
                std::string temp1 = currentGame->board->names[i].c_str();
                currentGame->board->names[i] = currentGame->board->names[j].c_str();
                currentGame->board->names[j] = temp1.c_str();
            }
        }
    }

    //write the new board to file
    std::ofstream file("leaderboard.txt");
    if (file.is_open()) {
        for (int i = 0; i < 10; i++) {
            if (currentGame->board->scores[i] > 0) {
                file << currentGame->board->scores[i] << " ";
                file << currentGame->board->names[i];
                file <<"\n";
            }
        }
        file.close();
    }

    //diplay score and ask user whether he wants to play a new game
    QString s = "Your score is ";
    s.append(QString::number(currentGame->score));
    s.append(". Do you want to start a new game?");

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Game Over", s,
                                QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        delete currentGame;
        currentGame = new Game(*this, numLives);
        pressedKeys.clear();
        m_counter = 0;
    } else {
        //if not, show the leaderboard
        currentGame->board->show(painter);
        QMessageBox msgBox;
        msgBox.setText("Leader Board");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        if (msgBox.exec() == QMessageBox::Yes ) {
        } else {
            QApplication::quit();
        }
    }
}

Dialog::Dialog(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::Dialog),
      m_counter(0),
      m_hasError(false)
{
    std::ostringstream errMsg;

    // If config file does not exist, display message detailing such and cleanly exit program.
    if (!std::ifstream("config.ini")) {
        errMsg << "The following file is missing: config.ini";
        QMessageBox::critical(this, "Error!", tr(errMsg.str().c_str()), QMessageBox::Ok);
        exit(0);
    }

    //Builds objects required.
    IOFile io;
    StageBuilder sb;
    errMsg << "There are errors in the structure of the config file.\n";

    m_stickman = sb.constructObject(new StickmanBuilder(), io);
    m_background = sb.constructObject(new BackgroundBuilder(), io);


    // UPDATED: Error-proofed the creation of these objects
    if (m_background->getErrorMsg().length() > 0) {
        errMsg << m_background->getErrorMsg();
        m_hasError = true;
    }
    if (m_stickman->getErrorMsg().length() > 0) {
        errMsg << m_stickman->getErrorMsg();
        m_hasError = true;
    }

    // START ADDED

    // Backwards Compatibility. Checks for if Obstacles->Enabled is true in config file
    if (io.getElement("[Obstacles]","Enabled=") == "true") {
        // Initialisation of variables that are independent of config input
        m_movesUp = 0, m_movesDown = 0, m_jumpsMade = 0, totalObstacleCount = 0, m_inJump = false;

        // Initialising configurable variables from config file, capturing error message if appropriate.
        // If error occured, variable is not set to the input.
        errMsg << SetIntFromFile("Stickman", "JumpHeight", &m_jumpHeight);
        errMsg << SetIntFromFile("Stickman", "MaxJumps", &m_maxJumps);
        errMsg << SetIntFromFile("Background", "WindowHeight", &m_windowHeight);
        errMsg << SetIntFromFile("Background", "WindowWidth", &m_windowWidth);
        errMsg << SetIntFromFile("Obstacles", "Count", &totalObstacleCount);

        m_stage3Enabled = false;
        m_stage2Enabled = true;


        for (int i = 0; i < totalObstacleCount; i++) {
            m_obstacleArray.push_back(sb.constructObject(new ObstacleBuilder(), io, i+1));
            // Detects if error message was given when constructing obstacles.
            if (m_obstacleArray.at(i)->getErrorMsg().length() > 0) {
                errMsg << m_obstacleArray.at(i)->getErrorMsg();
                m_hasError = true;
            }
        }

        gravity = m_jumpHeight;
    } else if (io.getElement("[Obstacles]","Enabled=") == "false"
               || io.getElement("[Obstacles]","Enabled=") == "nothing found") {
        // Retains values from base version. This is specified if enabled is set as false,
        // or if not included at all (such as backwards compatibility for config file).

        m_stage2Enabled = false;
        m_stage3Enabled = false;
        m_windowHeight = 600;
        m_windowWidth = 1000;

    } else {
        // Error where user has included the Obstacles->Enabled field, but has an invalid input
        errMsg << "\n- The input of '" + io.getElement("[Obstacles]","Enabled=")
                  + "' for [Obstacles]->Enabled is not valid.";
        m_hasError = true;
    }

    if (io.getElement("[Stage3]","Enabled=") == "true") {
        m_stage3Enabled = true;
        distanceMove = 0;
        bothKeys = false;
        checkPoint = new Object();
        checkPoint->setFile("checkpoint.jpg");
        checkPoint->setHeight(100);
        checkPoint->setWidth(100);
        checkPoint->setSpacing(200);
        m_obstacleArray.push_back(checkPoint);
        totalObstacleCount++;

        errMsg << SetIntFromFile("Stage3", "PowerUp", &powerUp);
        errMsg << SetIntFromFile("Stage3", "Lives", &numLives);

        int g;
        errMsg << SetIntFromFile("Stage3", "Gravity", &g);
        gravity = g * 1.0;

    }


    // If error has occurs, display message detailing errors and cleanly exit program.


    //create arrays of image file names and images
    QImage image= QImage(m_background->getFile().c_str());
    img_vector[0] =  image;
    img_name[0] =m_background->getFile().c_str();

    image = QImage(m_stickman->getFile().c_str());
    img_vector[1] =  image;
    img_name[1] = m_stickman->getFile().c_str();

    std::string shapes[] = {"rectangle","circle","triangle"};
    std::string colours[] = {"yellow","green","white","red"};

    for (int i=0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            std::string fileName = "obstacle_"+shapes[i]+"_"+colours[j]+".png";

            // If file does not exist, display message detailing such and cleanly exit program.
            if (!std::ifstream(fileName)) {
                std::string errMsg = "The following file is missing: " + fileName;
                QMessageBox::critical(this, "Error!", tr(errMsg.c_str()), QMessageBox::Ok);
                exit(0);
            }
            img_vector[2 + i * 4 + j] = QImage(fileName.c_str());
            img_name[2 + i * 4 + j] = fileName.c_str();

        }
        if (m_stage3Enabled) {
            image = QImage(checkPoint->getFile().c_str());
            img_vector[14] =  image;
            img_name[14] = "checkpoint.jpg";
         }

    }


    levelSetting = NULL;
    if (m_stage3Enabled) {
        numLevels = 0;
        errMsg << SetIntFromFile("Stage3", "NumLevels", &numLevels);
        if (numLevels > 0) {
            levelSetting = new std::vector<int>[numLevels];
            for (int i=0;i<numLevels;i++) {
                std::string s = "Level" + std::to_string((long double)(i+1));
                if (io.getElement("[Stage3]",s) =="nothing found") {
                    m_hasError = true;
                    errMsg << "\n- The input for [Stage3]->" + s + " is missing.";
                    continue;
                }
                split(io.getElement("[Stage3]",s) , ' ', levelSetting[i], totalObstacleCount);
            }
        }
     }

    if (m_hasError) {
        QMessageBox::critical(this, "Error!", tr(errMsg.str().c_str()), QMessageBox::Ok);
        delete m_stickman;
        for (int i = 0; i< m_obstacleArray.size(); i++)
            delete m_obstacleArray.at(i);
        m_obstacleArray.clear();
        delete m_background;
        if (levelSetting)
            delete[] levelSetting;
        exit(0);
    }

    ui->setupUi(this);
    this->resize(m_windowWidth, m_windowHeight);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(nextFrame()));
    this->installEventFilter(this);
    timer->start(32);

    if (m_stickman->getSize() == "tiny") {
        this->m_stickman->setState(new TinyMode(0,this->m_jumpHeight, this->gravity));
    } else  if (m_stickman->getSize() == "normal") {
        this->m_stickman->setState(new NormalMode(1,this->m_jumpHeight, this->gravity));
    } else if (m_stickman->getSize() == "large") {
        this->m_stickman->setState(new LargeMode(2,this->m_jumpHeight, this->gravity));
    } else  {
        this->m_stickman->setState(new GiantMode(3,this->m_jumpHeight, this->gravity));
    }


    currentGame = new Game(*this,numLives);
}

/*spilit array of string to an array of int */
std::vector<int>& Dialog::split(std::string &s, char delim, std::vector<int> &elems, int max) {
    s=s.substr(1);
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        try {
            if (std::stoi(item)<=max && std::stoi(item)>0)
                elems.push_back(std::stoi(item)-1);
        }
        catch( std::exception& e) {

        }
    }
    return elems;
}

Dialog::~Dialog()
{
    delete ui;
    delete m_stickman;
    delete m_background;
    delete timer;
    delete currentGame;
    for (int i = 0; i< m_obstacleArray.size(); i++)
        delete m_obstacleArray.at(i);
    m_obstacleArray.clear();

    if (levelSetting)
        delete[] levelSetting;
}

/*draw the stickman and obstacles */
void Dialog::drawStickmanAndObstacles(QPainter& painter) {
    // Draw stickman
    draw(painter, &currentGame->m_stickman, currentGame->m_stickman.getX(),
         -currentGame->m_stickman.getY() + m_windowHeight - currentGame->m_stickman.getHeight(),
         currentGame->m_stickman.getWidth(), currentGame->m_stickman.getHeight());

    // Render Obstacles only if they do not overlap
    if (currentGame->currentLevel->objectList.size() > 0) {
        draw(painter, &currentGame->currentLevel->objectList[0], currentGame->currentLevel->objectList[0].getX(),
                -currentGame->currentLevel->objectList[0].getY() + m_windowHeight - currentGame->currentLevel->objectList[0].getHeight(),
                currentGame->currentLevel->objectList[0].getWidth(), currentGame->currentLevel->objectList[0].getHeight());
    }

    for (int i = 1; i < currentGame->currentLevel->objectList.size(); i++) {
        if (currentGame->currentLevel->objectList.at(i).getSpacing() >= 0) {
            // X-Position of obstacles are based on the previous obstacle's X-Position + Spacing
            currentGame->currentLevel->objectList.at(i).overrideX(currentGame->currentLevel->objectList[i - 1].getX()
                    + currentGame->currentLevel->objectList[i - 1].getWidth() + currentGame->currentLevel->objectList.at(i).getSpacing());

            draw(painter, &currentGame->currentLevel->objectList.at(i), currentGame->currentLevel->objectList.at(i).getX(),
                 -currentGame->currentLevel->objectList.at(i).getY() + m_windowHeight - currentGame->currentLevel->objectList.at(i).getHeight(),
                 currentGame->currentLevel->objectList.at(i).getWidth(), currentGame->currentLevel->objectList.at(i).getHeight());
        }
    }
}

/*draw the background */
void Dialog::drawBackground(QPainter &painter) {
    if (m_background->getX()<-m_windowWidth)
        m_background->overrideX(0);

    int currentpos = m_background->getX();
    draw(painter, m_background, m_background->getX(), m_background->getY(), m_windowWidth, m_windowHeight);
    draw(painter, m_background, currentpos + m_windowWidth, m_background->getY(), m_windowWidth, m_windowHeight);
    draw(painter, m_background, currentpos - m_windowWidth, m_background->getY(), m_windowWidth, m_windowHeight);
}

/*show the time, score, level and remaining lives */
void Dialog::showDetails(QPainter& painter) {
    painter.setPen( Qt::black );
    QPoint point = QPoint(10, 30 );
    QFont font("times", 24);

    painter.setFont(font);
    QString s = "Level: ";
    s.append(QString::number(currentGame->currentLevel->levelIndex));
    s.append("   Remaining Lives: ");
    s.append(QString::number(currentGame->numLives));
    s.append("   Time: ");
    s.append(QString::number(currentGame->currentLevel->time));
    s.append("   Score: ");
    s.append(QString::number(currentGame->score));
    painter.drawText( point, s );
}

/*make the stickman drop after jumpping */
void Dialog::stickmanDropping() {
    bool hasCollision = false;
    if (m_jumpsMade==0) {
        // check if there is obstacle on the way dropping
        currentGame->m_stickman.setY(-currentGame->m_stickman.state->gravity);
        for (int i = 0; i < currentGame->currentLevel->objectList.size(); i++) {
            if (currentGame->currentLevel->objectList.at(i).getSpacing() >= 0) {
                if (currentGame->currentLevel->objectList.at(i).collisionChecker(&currentGame->m_stickman, 0)) {
                    hasCollision = true;
                }
            }
        }

        if (hasCollision) {
            currentGame->m_stickman.setY(currentGame->m_stickman.state->gravity);
            m_jumpsMade = 0;
        }

        if (currentGame->m_stickman.getY()<0) currentGame->m_stickman.setY(-currentGame->m_stickman.getY()); //make 0;
  }
}

/* check whether the stickman can jump and make it jump*/
void Dialog::stickmanJump() {
    int hasCollision = false;
    if (m_jumpsMade > 0) {

        currentGame->m_stickman.setY(this->currentGame->m_stickman.state->jumpHeight);


        for (int i = 0; i < currentGame->currentLevel->objectList.size(); i++) {
            if (currentGame->currentLevel->objectList.at(i).getSpacing() >= 0) {
                if (currentGame->currentLevel->objectList.at(i).collisionChecker(&currentGame->m_stickman,distanceMove)) {
                    hasCollision = true;
                }
            }
        }

        if (hasCollision) {
            currentGame->m_stickman.setY(-this->currentGame->m_stickman.state->jumpHeight);
        }

        if (!pressedKeys.contains(Qt::Key_Space))
            m_jumpsMade--;

        if (m_jumpsMade==0) m_inJump=false;

        if (currentGame->m_stickman.getY()+currentGame->m_stickman.getHeight()>m_windowHeight)
               currentGame->m_stickman.setY(-currentGame->m_stickman.getY()+ m_windowHeight - currentGame->m_stickman.getHeight());
    }
}

/* update the stickman's state after losing life or power up */
void Dialog::updateStickman() {

    if (currentGame->m_stickman.state->stateIndex==0) {
         currentGame->m_stickman.setSize( "tiny");
    } else if (currentGame->m_stickman.state->stateIndex==1) {
        currentGame->m_stickman.setSize( "normal");
    } else if (currentGame->m_stickman.state->stateIndex==2) {
        currentGame->m_stickman.setSize( "large");
    } else  {
        currentGame->m_stickman.setSize( "giant");
    }

    if (currentGame->m_stickman.getSize() == "tiny") {
        currentGame->m_stickman.setHeight(50);
        currentGame->m_stickman.setWidth(50);
    } else if(currentGame->m_stickman.getSize() == "normal") {
        currentGame->m_stickman.setHeight(80);
        currentGame->m_stickman.setWidth(80);
    } else if(currentGame->m_stickman.getSize() == "large") {
        currentGame->m_stickman.setHeight(100);
        currentGame->m_stickman.setWidth(100);
    } else if(currentGame->m_stickman.getSize() == "giant") {
        currentGame->m_stickman.setHeight(150);
        currentGame->m_stickman.setWidth(150);
    }
}

/*check whether the stickman can move ahead i.e. not obstacles ahead */
bool Dialog::checkIfStickmanCanMove(QPainter& painter) {
    bool keepStill = false;
    for (int i = 0; i < currentGame->currentLevel->objectList.size(); i++) {
        if (currentGame->currentLevel->objectList.at(i).getSpacing() >= 0) {
            if (currentGame->currentLevel->objectList.at(i).collisionChecker(&currentGame->m_stickman, distanceMove)) {

                if (m_stage3Enabled) {
                    // if the obstacle touched is not the checkpoint, i.e. losing life
                    if (i!=currentGame->currentLevel->objectList.size()-1) {

                        if (!currentGame->m_stickman.state->destroyObstabcle) {
                            currentGame->numLives--;
                            if (currentGame->numLives==0) {
                                gameOver(painter); // if remaning lives = 0
                            }

                            // downgrade the state
                            currentGame->m_stickman.state->prevState(currentGame->m_stickman);

                            Level* t = currentGame->currentLevel;

                            //make the same Level Object, but keep the time
                            currentGame->currentLevel =  new Level(*this,
                                   currentGame->currentLevel->levelIndex, currentGame->currentLevel->time);
                            delete t; // delete the previouse Level Object

                            return keepStill;
                        }

                    } else {
                        // the stickman touches the checkpoint
                        currentGame->score += currentGame->currentLevel->time;

                        //check if power-up, i.e. score>=required score for power up
                        if (currentGame->score>=powerUp) {
                           currentGame->m_stickman.state->nextState(currentGame->m_stickman);
                            powerUp *= 2;
                        }

                        Level* t = currentGame->currentLevel;

                        //make next Level
                        currentGame->currentLevel = new Level(*this,
                                            currentGame->currentLevel->levelIndex+1);
                        delete t;

                        currentGame->currentLevel->welcome();
                        return keepStill;
                    }
                }

                if (!currentGame->m_stickman.state->destroyObstabcle)
                    keepStill = true;

                //explode the obstacle if the current state is GIANT
                else currentGame->currentLevel->objectList.erase(currentGame->currentLevel->objectList.begin());

                break;
            }
        }
    }
    return keepStill;
}

/*if the stickman can move ahead, update its position */
void Dialog::updateObstaclePosition(bool keepStill) {
    int currentpos = m_background->getX();
    if (m_stage3Enabled) {
       //
        if (currentpos <= -m_windowWidth) {
            m_background->setX(m_windowWidth);
        }
        if (currentpos >= m_windowWidth) {
            m_background->setX(-m_windowWidth);
        }


        // If no collision, all obstacles and background will continue moving.
        if (!keepStill) {
            for (int i = 0; i < currentGame->currentLevel->objectList.size(); i++) {
                currentGame->currentLevel->objectList.at(i).setX(-distanceMove);
            }
            m_background->setX(-distanceMove);



        }
        if (!pressedKeys.contains(Qt::Key_Right) && !pressedKeys.contains(Qt::Key_Left) )
                distanceMove = 0;


    }
    else if (m_stage2Enabled) {
        if (currentpos <= -m_windowWidth) {
            m_background->setX(m_windowWidth);
        }

        // If no collision, all obstacles and background will continue moving.
        if (!keepStill) {
            for (int i = 0; i < currentGame->currentLevel->objectList.size(); i++) {
                currentGame->currentLevel->objectList.at(i).setX(-currentGame->m_stickman.getV());
            }
            m_background->setX(-currentGame->m_stickman.getV());
        }

        // Once the last obstacle has left the screen, the first will appear again.

        if (currentGame->currentLevel->objectList.size() > 0) {
            if (currentGame->currentLevel->objectList.at(currentGame->currentLevel->objectList.size() - 1).getX() <= -currentGame->currentLevel->objectList.at(currentGame->currentLevel->objectList.size() - 1).getWidth()) {
                currentGame->currentLevel->objectList.at(0).overrideX(m_windowWidth);
            }
        }
    } else {

        m_background->setX(-currentGame->m_stickman.getV());
    }
}

void Dialog::paintEvent(QPaintEvent *event) {

    QPainter painter(this);

    // Draw background
    drawBackground(painter);

    if (m_stage3Enabled) {
        if (m_counter++ ==0) return ;
        if (currentGame->userStarted == false) {
            currentGame->welcome(painter);
            return;
        }

        if (currentGame->currentLevel->time<=0) {
            //show messaage box
            qDebug()<< currentGame->currentLevel->time << endl;
            gameOver(painter);
            return;
        }
        currentGame->currentLevel->time--;
    } else distanceMove = m_stickman->getV();

    if (m_stage2Enabled || m_stage3Enabled)
        stickmanJump();
    updateObstaclePosition(checkIfStickmanCanMove(painter));
    updateStickman();

    drawStickmanAndObstacles(painter);


    stickmanDropping();

    if (m_stage3Enabled)
        showDetails(painter);
    m_counter++;
}

void Dialog::nextFrame() {

    update();
}

/*filter the keyPress and keyRelease Events*/
bool Dialog::eventFilter(QObject * obj, QEvent * event)
{

    if(event->type()==QEvent::KeyPress) {
        pressedKeys += ((QKeyEvent*)event)->key();
        if (m_stage3Enabled) {
            if( pressedKeys.contains(Qt::Key_Right)) {
                distanceMove = currentGame->m_stickman.getV();
            }
            if( pressedKeys.contains(Qt::Key_Left)) {
                distanceMove = -currentGame->m_stickman.getV();
            }
        }

        if ( pressedKeys.contains(Qt::Key_Space)) {
            if (m_inJump) {
                if (m_jumpsMade < m_maxJumps) {
                    m_jumpsMade++;
                }
            } else {
                m_inJump = true;
                m_jumpsMade = 1;
            }
        }
        if (m_stage3Enabled)
            // Extension part: if the Alt key is pressed, the stickman will downgrade its state
            // e.g. Large --> Normal
            if (((QKeyEvent*)event)->key() == Qt::Key_Alt) {
                currentGame->m_stickman.state->prevState(currentGame->m_stickman);
                updateStickman();
            }
    }

    if(event->type()==QEvent::KeyRelease)
    {
        pressedKeys -= ((QKeyEvent*)event)->key();
        if (pressedKeys.contains(Qt::Key_Right)) {
            distanceMove = currentGame->m_stickman.getV();
        }
        if( pressedKeys.contains(Qt::Key_Space) ){
                if (m_inJump) {
                    if (m_jumpsMade < m_maxJumps) {
                        m_jumpsMade++;
                    }
                } else {
                    m_inJump = true;
                    m_jumpsMade = 1;
                }
        }
    }
    return false;
}


void Dialog::draw(QPainter &painter, Object* object, int x, int y, int width, int height) {
    //Draws object at specifies points.
    QRectF target(x, y, width, height);

    int index = 0;
    for (int i = 0; i <= 14; i++) {
        if (img_name[i]==object->getFile().c_str()) {
            index = i;
            break;
        }
    }

    painter.drawImage(target, img_vector[index]);

}

// Added
std::string Dialog::SetIntFromFile(std::string section, std::string key, int *value) {

    // Attempts to retrieve int from file using the specified ini section and key.
    // Captures and returns error message if retrieval is unsuccessful.

    IOFile io;
    std::string strToCheck = io.getElement("[" + section + "]", key + "=");

    if (strToCheck == "nothing found") {
        m_hasError = true;
        return "\n- The input for [" + section + "]->" + key + " is missing.";
    }

    long long intAsLong = atoi(strToCheck.c_str());
    std::stringstream ss;
    ss << intAsLong ;
    std::string parsedInt =ss.str();

    if (isdigit(strToCheck[0]) == 0 || strToCheck.length() > parsedInt.length()) {
        m_hasError = true;
        return "\n- The input of '" + strToCheck + "' for [" + section + "]->" + key + " is not valid.";
    }

    if (atoi(strToCheck.c_str()) < 0) {
        m_hasError = true;
        return "\n- The input of '" + strToCheck + "' for [" + section + "]->" + key + " is not valid.";
    }


    *value = atoi(strToCheck.c_str());

    return "";
}
