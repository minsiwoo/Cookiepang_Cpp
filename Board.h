#pragma once

#include <vector>
#include <set>
#include <random>
#include <QtWidgets/QGraphicsPixmapItem>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsRectItem>
#include <QtWidgets/QGraphicsTextItem>

#include "Item.h"
using MatchPair = std::pair<int, int>;
using MatchSet = std::set<MatchPair>;
class Board : public Item::EventListener
{
private:
	QGraphicsScene *_scene;
	QGraphicsRectItem _root;
	QGraphicsSimpleTextItem _text;


	std::vector<std::vector<Item*>> _items;
	std::random_device _device;
	std::mt19937 _gen;

	int _moveCount;
	int _score;
	
public:
	Board(QGraphicsScene* scene);
	~Board();
	void addItem(int row, int column);
	void removeItem(int row, int column);
	void moveItem(int fromRow, int fromColumn, int toRow, int toColumn);
	void moveItem(Item* item, int toRow, int toColumn);
	void exchangeItems(int row0, int column0, int row1, int column1, bool canRevert);
	bool refresh();
	MatchSet matchedItems() const;
	MatchSet matchedItems(int row, int column) const;
	MatchSet matchedItemsHorizontal(int row, int column) const;
	MatchSet matchedItemsVertical(int row, int column) const;

	virtual void itemDragEvent(Item* item, Item::Direction dir) override;
	virtual void itemMoveFinished(Item* item0, Item* item1, bool canRevert) override;
};

