#include "geographicsmissionitem.h"

#include "backgroundraster.h"
#include <QDebug>
#include <QVector2D>

GeoGraphicsMissionItem::GeoGraphicsMissionItem(MissionItem* parent):MissionItem(parent),m_lockedColor(50,200,50),m_unlockedColor(Qt::red), m_locked(false)
{
    if(parent)
    {
        QGraphicsItem *parentItem = parent->findParentGraphicsItem();
        setParentItem(parentItem);
        setAcceptHoverEvents(true);
        setOpacity(.5);
        setFlag(QGraphicsItem::ItemIsMovable);
        setFlag(QGraphicsItem::ItemIsSelectable);
        setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    }
}

void GeoGraphicsMissionItem::updateBackground(BackgroundRaster* bg)
{
    setParentItem(bg);
    updateProjectedPoints();
}


void GeoGraphicsMissionItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    //qDebug() << "Enter item!";
    if(!m_locked)
        setOpacity(1.0);
}

void GeoGraphicsMissionItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    //qDebug() << "Leave item!";
    setOpacity(.5);
}

QGraphicsItem * GeoGraphicsMissionItem::findParentGraphicsItem()
{
    return this;
}

QList<GeoGraphicsMissionItem *> GeoGraphicsMissionItem::childrenGeoGraphicsMissionItems() const
{
    QList<GeoGraphicsMissionItem *> ret;
    for(auto cmi: childMissionItems())
    {
        GeoGraphicsMissionItem * gmi = qobject_cast<GeoGraphicsMissionItem*>(cmi);
        if(gmi)
            ret.append(gmi);
    }
    return ret;
}


void GeoGraphicsMissionItem::lock()
{
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setOpacity(.5);
    m_locked = true;
    for(auto childItem: childrenGeoGraphicsMissionItems())
        childItem->lock();
}

void GeoGraphicsMissionItem::unlock()
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    m_locked = false;
    for(auto childItem: childrenGeoGraphicsMissionItems())
        childItem->unlock();
}

bool GeoGraphicsMissionItem::locked() const
{
    return m_locked;
}

void GeoGraphicsMissionItem::drawArrow(QPainterPath& path, const QPointF& from, const QPointF& to) const
{
    qreal scale = 1.0;
    auto bgr = autonomousVehicleProject()->getBackgroundRaster();
    if(bgr)
        scale = 1.0/bgr->mapScale();// scaledPixelSize();
    //qDebug() << "scale: " << scale;
    scale = std::max(0.05,scale);
    
    path.moveTo(to);
    QVector2D v(to-from);
    v.normalize();
    QVector2D left(-v.y(),v.x());
    QVector2D right(v.y(),-v.x());
    QVector2D back = -v;
    path.lineTo(to+(left+back*2).toPointF()*10*scale);
    path.moveTo(to);
    path.lineTo(to+(right+back*2).toPointF()*10*scale);
    path.moveTo(to);
    
}
