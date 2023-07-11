#include "Board.h"
#include "Consts.h"
#include "Item.h"

#include "qdebug"




Board::Board(QGraphicsScene* scene)
	: _scene(scene)
	, _gen(_device())
	, _moveCount(0)
	, _score(0)
{
	_scene->addItem(&_root);
	_root.setX(_scene->sceneRect().width() / 2 - 
		Consts:: BOARD_ITEM_SIZE * Consts::BOARD_LENGTH / 2);
	_root.setY(_scene->sceneRect().height() / 2 -
		Consts::BOARD_ITEM_SIZE * Consts::BOARD_LENGTH / 2);

	_scene->addItem(&_text);
	_text.setX(_scene->sceneRect().width() / 2 - 
		Consts::BOARD_ITEM_SIZE * Consts::BOARD_LENGTH / 2);
	_text.setY(_scene->sceneRect().height() / 2 +
		Consts::BOARD_ITEM_SIZE * Consts::BOARD_LENGTH / 2);
	_text.setText("Score : 0");
	_text.setScale(2.5);

	for (int row = 0; row < Consts::BOARD_LENGTH; ++row)
	{
		std::vector<Item*> rowItems(Consts:: BOARD_LENGTH);
		_items.push_back(rowItems);
		for (int column = 0; column < Consts::BOARD_LENGTH; ++column)
		{
			addItem(row, column);
		}
	}

	refresh();
}

Board::~Board()
{
	for (int row = 0; row < _items.size(); ++row)
	{
		for (int column = 0; column < _items[row].size(); ++column)
		{
			removeItem(row, column);
		}
	}
}

// 주어진 row와 column 위치에 새로운 Item을 생성하고, 위치와 경로를 설정한 후 _items 배열에 저장.
// 게임 보드에 아이템을 추가하는 기능.
void Board::addItem(int row, int column)

{ 
	std::uniform_int_distribution<int> dis(0, 11);
	auto& path = Consts::paths[dis(_gen)];

	Item * item = new Item(this, path, row, column, &_root);
	item->setPos(column * Consts::BOARD_ITEM_SIZE, row * Consts::BOARD_ITEM_SIZE);

	_items[row][column] = item;
}

// 열과 행에 있는 아이템을 가져와서 해당 아이템이 존재하는 경우, 아이템을 제거하고 메모리에서 해제
// 아이템의 부모를 nullptr로 설정하여 부모와의 연결을 끊습니다.
void Board::removeItem(int row, int column)
{
	auto* item = _items[row][column];
	if (item == nullptr)
		return;
	_items[row][column] = nullptr;
	item->setParentItem(nullptr);
	_scene->removeItem(item);

	delete item;
}

// 정 위치(fromRow, fromColumn)에 있는 아이템을 가져와서 해당 아이템이 존재하는 경우,
// 다른 오버로드된 moveItem 함수를 호출하여 아이템을 새로운 위치(toRow, toColumn)로 이동. 
// 이동할 아이템이 존재하지 않는 경우 함수를 종료
void Board::moveItem(int fromRow, int fromColumn, int toRow, int toColumn)
{
	Item* item = _items[fromRow][fromColumn];
	if (item == nullptr)
		return;
	moveItem(item, toRow, toColumn);
}

// Item 객체를 지정된 행(toRow)과 열(toColumn)로 이동, 해당 위치에 아이템을 할당. 
// 이동 횟수(_moveCount)를 증가. 
// Consts::BOARD_ITEM_SIZE는 아이템의 크기를 나타내는 상수로 가정되며, 
// 이를 통해 이동할 위치를 계산.
void Board::moveItem(Item* item, int toRow, int toColumn)
{
	item->setRow(toRow);
	item->setColumn(toColumn);
	item->moveTo(toColumn * Consts::BOARD_ITEM_SIZE, toRow * Consts::BOARD_ITEM_SIZE);
	

	_items[toRow][toColumn] = item;
	_moveCount++;
}

//  두 개의 아이템을 교환하는 함수. 
// 주어진 두 개의 위치(row0, column0, row1, column1)에 있는 아이템들을 가져와서 서로의 위치를 교환
// 각 아이템의 행(row)과 열(column)을 변경하고, 아이템들을 새로운 위치에 할당. 
// 이후에는 아이템들을 서로의 위치로 이동시킵니다.
// 이동 횟수(_moveCount)를 2만큼 증가시킵니다. 
// canRevert 매개변수는 이동을 되돌릴 수 있는지 여부를 나타내며, 이동을 되돌릴 수 있는 경우 해당 아이템들의 이동을 취소할 수 있습니다.
void Board::exchangeItems(int row0, int column0, int row1, int column1, bool canRevert)
{
	Item* item0 = _items[row0][column0];
	Item* item1 = _items[row1][column1];

	item0->setRow(row1);
	item1->setRow(row0);

	item0->setColumn(column1);
	item1->setColumn(column0);

	_items[row0][column0] = item1;
	_items[row1][column1] = item0;

	item0->moveTo(item1, canRevert);
	item1->moveTo(item0, canRevert);
	
	_moveCount += 2;
}

bool Board::refresh()
{
	MatchSet matched = matchedItems();
	if (matched.size() < 3)   // 그림 3개 일치X  false
		return false;
	for (const auto& pair : matched)
	{
		removeItem(pair.first, pair.second);   // 일치한 그림 3개이상 삭제

		_score += 10;  
		_text.setText(("Score : " + std::to_string(_score)).c_str());   // 점수up
	} 
	// 삭제되면 아이템들을 내려서 빈 공간 채우기
	for (int column = 0; column < _items[0].size(); ++column)
	{
		for (int row = _items.size() - 1; row >= 0; --row)
		{
			if (_items[row][column] != nullptr)   // 아이템이 존재하는 공간은 건너뛰기
			{
				continue;
			}
			for (int i = row - 1; i >= 0; --i)
			{
				if (_items[i][column] != nullptr)  
				{
					moveItem(i, column, row, column);
					_items[i][column] = nullptr;
					break;
				}
			}
		}
	}

	// 빈 공간 갯수 계산후 emptyCounts 에 저장
	std::vector<int> emptyCounts;
	for (int column = 0; column < _items[0].size(); ++column)
	{
		int emptyCount = 0;
		for (int row = 0; row < _items.size(); ++row)
		{
			if (_items[row][column] == nullptr)
				emptyCount++;
			else
				break;
		}
		emptyCounts.push_back(emptyCount);
	}

	// fill new items
	for (int column = 0; column < _items[0].size(); ++column)
	{
		for (int row = 0; row < _items.size(); ++row)
		{
			if (_items[row][column] == nullptr)   // 빈 공간 아이템 채우기
			{
				addItem(row, column);   // 아이템 추가

				Item* item = _items[row][column];
				item->setY(item->y() - emptyCounts[column] * Consts::BOARD_ITEM_SIZE);
				moveItem(row, column, row, column);  // 아이템 새로운 위치에 이동
			}
			else
			{
				break;
			}
		}
	}
	return true;
}

MatchSet Board::matchedItems() const
{
	MatchSet matched;
	for (int row = 0; row < _items.size(); ++row)
	{
		for (int column = 0; column < _items[row].size(); ++column)
		{
			MatchSet m = matchedItems(row, column);
			if (m.size() >= 3)
			{
				matched.insert(m.begin(), m.end());  // c++라이브러리
			}
		}
	}
	return matched;
}

MatchSet Board::matchedItems(int row, int column) const
{
	MatchSet horizontalMatched = matchedItemsHorizontal(row, column);
	MatchSet verticalMatched = matchedItemsVertical(row, column);

	MatchSet matched;
	if (horizontalMatched.size() >= 3)
		matched.insert(horizontalMatched.begin(), horizontalMatched.end());
	if (verticalMatched.size() >= 3)
		matched.insert(verticalMatched.begin(), verticalMatched.end());
	return matched;
}

MatchSet Board::matchedItemsHorizontal(int row, int column) const
{
	Item* item = _items[row][column];
	if (item == nullptr)
		return {};

	MatchSet leftMatched;
	for (int i = column - 1; i >= 0; --i)
	{
		if (_items[row][i] && _items[row][i]->path() == item->path())
		{
			leftMatched.insert({ row, i });
		}
		else
		{
			break;
		}
	}

	MatchSet rightMatched;
	for (int i = column + 1; i < _items[row].size(); ++i)
	{
		if (_items[row][i] && _items[row][i]->path() == item->path())
		{
			rightMatched.insert({ row, i });
		}
		else
		{
			break;
		}
	}

	if (leftMatched.size() + rightMatched.size() + 1 >= 3)
	{
		leftMatched.insert(rightMatched.begin(), rightMatched.end());
		leftMatched.insert({ row, column });
		return leftMatched;
	}
	else
	{
		return {};
	}
}

MatchSet Board::matchedItemsVertical(int row, int column) const
{
	Item* item = _items[row][column];
	if (item == nullptr)
		return {};

	MatchSet upMatched;
	for (int i = row - 1; i >= 0; --i)
	{
		if (_items[i][column] && _items[i][column]->path() == item->path())
		{
			upMatched.insert({ i, column });
		}
		else
		{
			break;
		}
	}

	MatchSet downMatched;
	for (int i = row + 1; i < _items.size(); ++i)
	{
		if (_items[i][column] && _items[i][column]->path() == item->path())
		{
			downMatched.insert({ row, i });
		}
		else
		{
			break;
		}
	}

	if (upMatched.size() + downMatched.size() + 1 >= 3)
	{
		upMatched.insert(downMatched.begin(), downMatched.end());
		upMatched.insert({ row, column });
		return upMatched;
	}
	else
	{
		return {};
	}
}

void Board::itemDragEvent(Item* item, Item::Direction dir)
{
	int row0 = item->row();
	int column0 = item->column();
	int row1 = row0;
	int column1 = column0;

	switch (dir)
	{
	case Item::Direction::Left:
		column1--;
		break;
	case Item::Direction::Rigth:
		column1++;
		break;
	case Item::Direction::Up:
		row1--;
		break;
	case Item::Direction::Down:
		row1++;
		break;
	}

	if (row1 < 0 || column1 < 0)
		return;

	if (row1 >= Consts::BOARD_LENGTH || column1 >= Consts::BOARD_LENGTH)
		return;

	Item* item1 = _items[row1][column1];
	if (item1 == nullptr)
		return;

	exchangeItems(row0, column0, row1, column1, true);
	
}

void Board::itemMoveFinished(Item* item0, Item* item1, bool canRevert)
{
	if (--_moveCount > 0)
		return;

	if (refresh())
		return;

	if (!canRevert)
		return;

	if (item0 == nullptr || item1 == nullptr)
		return;

	// revert exchange
	exchangeItems(item0->row(), item0->column(), item1->row(), item1->column(), false);
}
