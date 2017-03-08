#ifndef SURVEYPATTERN_H
#define SURVEYPATTERN_H

#include "geographicsitem.h"

class Waypoint;

class SurveyPattern : public GeoGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    SurveyPattern(QObject *parent = 0, QGraphicsItem *parentItem =0);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QPainterPath shape() const;

    void write(QJsonObject &json) const;
    void read(const QJsonObject &json);

    QGeoCoordinate const &startLocation() const;
    void setStartLocation(QGeoCoordinate const &location);
    void setEndLocation(QGeoCoordinate const &location);
    void setSpacingLocation(QGeoCoordinate const &location);

    bool hasSpacingLocation() const;

public slots:
    void waypointHasChanged();

protected:
    Waypoint * createWaypoint();

private:
    Waypoint * m_startLocation;
    Waypoint * m_endLocation;
    Waypoint * m_spacingLocation;

    QList<QGeoCoordinate> getPath() const;

};

#endif // SURVEYPATTERN_H