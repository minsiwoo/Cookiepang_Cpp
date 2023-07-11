#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QGraphicsTextItem>
#include "Board.h"

// Qt �����ӿ�ũ�� ����Ͽ� �׷��� ����� ���ø����̼��� �����ϰ� �����ϴ� �⺻���� ���ø�
// QApplication, QGraphicsScene, QGraphicsView ���� Ŭ������ Ȱ���Ͽ� �׷��� ��ҵ��� �����ϰ� ȭ�鿡 ǥ���ϴ� ����� ����

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
