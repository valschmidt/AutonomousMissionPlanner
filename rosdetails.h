#ifndef ROSNODEDETAILS_H
#define ROSNODEDETAILS_H

#include <QWidget>

namespace Ui
{
class ROSDetails;
}

namespace ros
{
    class Time;
}

class ROSLink;

class ROSDetails : public QWidget
{
    Q_OBJECT
    
public:
    explicit ROSDetails(QWidget *parent =0);
    ~ROSDetails();
    
    void setROSLink(ROSLink *rosLink);
    
public slots:
    void heartbeatDelay(double seconds);
    void rangeAndBearingUpdate(double range, ros::Time const &range_timestamp, double bearing, ros::Time const &bearing_timestamp);
    void sogUpdate(qreal sog, qreal sog_avg);

private slots:
    void on_activeCheckBox_stateChanged(int state);

    void on_standbyPushButton_clicked(bool checked);
    void on_surveyPushButton_clicked(bool checked);
    void on_loiterPushButton_clicked(bool checked);

    void on_stopPingingPushButton_clicked(bool checked);
    void on_startPingingPushButton_clicked(bool checked);
    void on_pingAndLogPushButton_clicked(bool checked);

    void updateVehicleStatus(QString const &status);
    void on_sendWaypointIndexPushButton_clicked(bool checked);
    
    
private:
    Ui::ROSDetails* ui;
#ifdef AMP_ROS
    ROSLink *m_rosLink;
#endif
};

#endif // ROSNODEDETAILS_H
