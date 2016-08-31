#include "leaderboard.h"
#include <QDebug>

LeaderBoard::LeaderBoard()
{
    std::string line;
    std::ifstream myfile ("leaderboard.txt");
    int index = 0;
    if (myfile.is_open()) {

        while ( getline (myfile,line) ) {
            if (index>=10) break;
            int pos = line.find(" ");

            try {
                int score = std::stoi(line.substr(0,pos));

                std::string name = line.substr(pos+1, line.length() - pos );

                scores[index] = score;
                names[index] = name;
                index++;
            } catch ( std::exception& e ){
                break;

            }

        }

        myfile.close();
    }
}

LeaderBoard::~LeaderBoard()
{

}

void LeaderBoard::show(QPainter& painter) {
    painter.setPen( Qt::black );
    QFont font("times", 24);
    painter.setFont(font);
    QPoint point = QPoint(10,30 );
    QString s = "Score         Name\n";
    painter.drawText( point, s );

    for (int i=0;i<10;i++) {
        if (scores[i]>0) {
            QFont font("times", 18);

            QPoint point = QPoint(10, i*30 +70  );
            painter.setFont(font);
            QString s = QString::number(scores[i]);
            s.append("            ");
            s.append(QString::fromStdString(names[i]));
            painter.drawText( point, s );

        } else
            break;
    }
}

