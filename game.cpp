#include "game.h"

Game::Game(Dialog& dialog,int numLives)
{
    score = 0;
    currentLevel = new Level(dialog,1);
    this->numLives = numLives;
    this->userStarted = false;
    this->userName = "";
    this->board = new LeaderBoard();
    this->m_stickman = *dialog.m_stickman;
    this->m_stickman.state = NULL;

    if (m_stickman.getSize() == "tiny") {
        this->m_stickman.setState(new TinyMode(0,dialog.m_jumpHeight, dialog.gravity));
    } else  if (m_stickman.getSize() == "normal") {
        this->m_stickman.setState(new NormalMode(1,dialog.m_jumpHeight, dialog.gravity));
    } else if (m_stickman.getSize() == "large") {
        this->m_stickman.setState(new LargeMode(2,dialog.m_jumpHeight, dialog.gravity));
    } else  {
        this->m_stickman.setState(new GiantMode(3,dialog.m_jumpHeight, dialog.gravity));
    }

}

Game::~Game()
{
    delete currentLevel;
    delete board;
}

void Game::welcome(QPainter& painter) {
    QMessageBox msgBox;
    msgBox.setText("Welcome to Stickman game!");
    msgBox.setInformativeText("Do you want to play?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    if (msgBox.exec() == QMessageBox::Yes ) {
        userStarted = true;
        bool ok;
        QInputDialog* inputDialog = new QInputDialog();
        inputDialog->setOptions(QInputDialog::NoButtons);
        do {
            QString text =  inputDialog->getText(NULL ,"QInputDialog::getText() Example",
                                                  "User name:", QLineEdit::Normal,
                                                  QDir::home().dirName(), &ok);

             if (ok && !text.isEmpty())
             {
                   userName = text.toStdString();
                   break;
             }
        } while (true);

    } else {
        board->show(painter);
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
