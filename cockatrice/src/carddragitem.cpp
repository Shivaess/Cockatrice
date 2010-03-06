#include "carddragitem.h"
#include "cardzone.h"
#include "tablezone.h"
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QCursor>

CardDragItem::CardDragItem(AbstractCardItem *_item, int _id, const QPointF &_hotSpot, bool _faceDown, AbstractCardDragItem *parentDrag)
	: AbstractCardDragItem(_item, _hotSpot, parentDrag), id(_id), faceDown(_faceDown), currentZone(0)
{
}

void CardDragItem::updatePosition(const QPointF &cursorScenePos)
{
	QList<QGraphicsItem *> colliding = scene()->items(cursorScenePos);

	CardZone *cursorZone = 0;
	for (int i = colliding.size() - 1; i >= 0; i--)
		if ((cursorZone = qgraphicsitem_cast<CardZone *>(colliding.at(i))))
			break;
	if (!cursorZone)
		return;
	currentZone = cursorZone;
	
	QPointF zonePos = currentZone->scenePos();
	QPointF cursorPosInZone = cursorScenePos - zonePos;
	QPointF cardTopLeft = cursorPosInZone - hotSpot;
	QPointF newPos = zonePos + cursorZone->closestGridPoint(cardTopLeft);
	
	if (newPos != pos()) {
		for (int i = 0; i < childDrags.size(); i++)
			childDrags[i]->setPos(newPos + childDrags[i]->getHotSpot());
		setPos(newPos);
	}
}

void CardDragItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	setCursor(Qt::OpenHandCursor);
	QGraphicsScene *sc = scene();
	QPointF sp = pos();
	sc->removeItem(this);

	if (currentZone) {
		CardZone *startZone = qgraphicsitem_cast<CardZone *>(item->parentItem());
		currentZone->handleDropEvent(id, startZone, (sp - currentZone->scenePos()).toPoint(), faceDown);
		for (int i = 0; i < childDrags.size(); i++) {
			CardDragItem *c = static_cast<CardDragItem *>(childDrags[i]);
			currentZone->handleDropEvent(c->id, startZone, (sp - currentZone->scenePos() + c->getHotSpot()).toPoint(), faceDown);
			sc->removeItem(c);
		}
	}

	event->accept();
}
