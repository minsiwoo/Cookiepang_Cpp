#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QGraphicsTextItem>
#include "Board.h"

// Qt 프레임워크를 사용하여 그래픽 기반의 애플리케이션을 구성하고 실행하는 기본적인 템플릿
// QApplication, QGraphicsScene, QGraphicsView 등의 클래스를 활용하여 그래픽 요소들을 구성하고 화면에 표시하는 기능을 구현

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QRect geometry = QApplication::desktop()->geometry();
    geometry.setY(0);

    QGraphicsScene scene;
    scene.setSceneRect(geometry);

    Board board(&scene);


    
    QGraphicsView view(&scene);
    view.showFullScreen();


    return a.exec();
}
