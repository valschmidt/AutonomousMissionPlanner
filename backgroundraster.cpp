#include "backgroundraster.h"
#include <QPainter>
#include <QJsonObject>
#include <gdal_priv.h>
#include <QModelIndex>
#include <QDebug>

BackgroundRaster::BackgroundRaster(const QString &fname, QObject *parent, QGraphicsItem *parentItem)
    : MissionItem(parent), QGraphicsItem(parentItem), m_filename(fname),m_valid(false)
{
    GDALDataset * dataset = reinterpret_cast<GDALDataset*>(GDALOpen(fname.toStdString().c_str(),GA_ReadOnly));
    if (dataset)
    {
        extractGeoreference(dataset);

        int width = dataset->GetRasterXSize();
        int height = dataset->GetRasterYSize();
        
        QGeoCoordinate p1 = pixelToGeo(QPointF(width/2,height/2));
        QGeoCoordinate p2 = pixelToGeo(QPointF((width/2)+1,height/2));
        m_pixel_size = p1.distanceTo(p2);
        qDebug() << "pixel size: " << m_pixel_size;
        
        QImage image(width,height,QImage::Format_ARGB32);
        image.fill(Qt::black);
        
        for(int bandNumber = 1; bandNumber <= dataset->GetRasterCount(); bandNumber++)
        {
            GDALRasterBand * band = dataset->GetRasterBand(bandNumber);
            

            GDALColorTable *colorTable = band->GetColorTable();

            std::vector<uint32_t> buffer(width);


            for(int j = 0; j<height; ++j)
            {
                band->RasterIO(GF_Read,0,j,width,1,&buffer.front(),width,1,GDT_UInt32,0,0);
                uchar *scanline = image.scanLine(j);
                for(int i = 0; i < width; ++i)
                {
                    if(colorTable)
                    {
                        GDALColorEntry const *ce = colorTable->GetColorEntry(buffer[i]);
                        scanline[i*4] = ce->c3;
                        scanline[i*4+1] = ce->c2;
                        scanline[i*4+2] = ce->c1;
                        scanline[i*4+3] = ce->c4;
                    }
                    else
                    {
                        if(band->GetColorInterpretation() == GCI_GrayIndex)
                        {
                            scanline[i*4+0] = buffer[i];
                            scanline[i*4+1] = buffer[i];
                            scanline[i*4+2] = buffer[i];
                        }
                        if(band->GetColorInterpretation() == GCI_RedBand)
                            scanline[i*4+2] = buffer[i];
                        if(band->GetColorInterpretation() == GCI_GreenBand)
                            scanline[i*4+1] = buffer[i];
                        if(band->GetColorInterpretation() == GCI_BlueBand)
                            scanline[i*4+0] = buffer[i];
                        if(band->GetColorInterpretation() == GCI_AlphaBand)
                            scanline[i*4+3] = buffer[i];

                        //scanline[i*4+3] = 255; // hack to make sure alpha is solid in case no alpha channel is present
                        //scanline[i*4 + 2-(bandNumber-1)] = buffer[i];
                    }
                }
            }
        }
        
        backgroundImages[1] = QPixmap::fromImage(image);
        for(int i = 2; i < 128; i*=2)
        {
            backgroundImages[i] = QPixmap::fromImage(image.scaledToWidth(width/float(i),Qt::SmoothTransformation));
        }
        m_valid = true;
    }
}

bool BackgroundRaster::valid() const
{
    return m_valid;
}

QRectF BackgroundRaster::boundingRect() const
{
    auto ret = backgroundImages.cbegin();
    return QRectF(QPointF(0.0,0.0), ret->second.size());
}


void BackgroundRaster::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
{
    painter->save();
    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    double scale = painter->transform().m11();
    if(!backgroundImages.empty())
    {
        QPixmap selectedBackground;
        int selectedScale;
        auto l = backgroundImages.lower_bound(1/scale);
        if (l == backgroundImages.end())
        {
            auto last = backgroundImages.rbegin();
            selectedBackground = last->second;
            selectedScale = last->first;
        }
        else
        {
            selectedBackground = l->second;
            selectedScale = l->first;
        }
        painter->scale(selectedScale,selectedScale);

        painter->drawPixmap(0,0,selectedBackground);
    }
    painter->restore();

}

QPixmap BackgroundRaster::topLevelPixmap() const
{
    auto ret = backgroundImages.cbegin();
    return ret->second;
}

QString const &BackgroundRaster::filename() const
{
    return m_filename;
}

void BackgroundRaster::write(QJsonObject &json) const
{
    json["type"] = "BackgroundRaster";
    json["filename"] = m_filename;
}

void BackgroundRaster::writeToMissionPlan(QJsonArray& navArray) const
{
}


void BackgroundRaster::read(const QJsonObject &json)
{

}

qreal BackgroundRaster::pixelSize() const
{
    return m_pixel_size;
}

qreal BackgroundRaster::scaledPixelSize() const
{
    return m_pixel_size/m_map_scale;
}

void BackgroundRaster::updateMapScale(qreal scale)
{
    m_map_scale = scale;
}

qreal BackgroundRaster::mapScale() const
{
    return m_map_scale;
}

bool BackgroundRaster::canAcceptChildType(const std::string& childType) const
{
    return false;
}
