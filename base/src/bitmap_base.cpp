#include <QIODevice>
#include <QFile>
#include <QBuffer>
#include <memory>         // for make_unique, unique_ptr
#include <string.h>
#include <QPainter>

#include "bitmap_base.hxx"
#include "color4d.hxx"
//#include "gr_basic.hxx"
#include "util.hxx"    // for KiROUND
//#include <plotters/plotter.h>
//#include "richio.hxx"




BITMAP_BASE::BITMAP_BASE( const VECTOR2I& pos )
{
    m_scale  = 1.0;                     // 1.0 = original bitmap size
    m_imageType = Image_Type::Image_Type_INVALID;
    m_bitmap = nullptr;
    m_image  = nullptr;
    m_originalImage = nullptr;
    m_ppi    = 300;                     // the bitmap definition. the default is 300PPI
    m_pixelSizeIu = 254000.0 / m_ppi;   // a pixel size value OK for bitmaps using 300 PPI
                                        // for Eeschema which uses currently 254000PPI
    m_isMirroredX = false;
    m_isMirroredY = false;
    m_rotation   = ANGLE_0;
}


BITMAP_BASE::BITMAP_BASE( const BITMAP_BASE& aSchBitmap )
{
    m_scale = aSchBitmap.m_scale;
    m_ppi   = aSchBitmap.m_ppi;
    m_pixelSizeIu = aSchBitmap.m_pixelSizeIu;
    m_isMirroredX = aSchBitmap.m_isMirroredX;
    m_isMirroredY = aSchBitmap.m_isMirroredY;
    m_rotation = aSchBitmap.m_rotation;
    m_imageType = aSchBitmap.m_imageType;

    m_image = nullptr;
    m_bitmap = nullptr;
    m_originalImage = nullptr;

    if( aSchBitmap.m_image )
    {
        m_image   = new QImage( *aSchBitmap.m_image );
        m_bitmap = new QPixmap(QPixmap::fromImage(*m_image));
        m_originalImage = new QImage( *aSchBitmap.m_originalImage );
        m_imageType = aSchBitmap.m_imageType;
        m_imageData = aSchBitmap.m_imageData;
        m_imageId = aSchBitmap.m_imageId;
    }
}


void BITMAP_BASE::rebuildBitmap( bool aResetID )
{
    if( m_bitmap )
        delete m_bitmap;

    m_bitmap = new QPixmap(QPixmap::fromImage(*m_image));

    if( aResetID )
        m_imageId = KIID();

}


void BITMAP_BASE::updatePPI()
{
    // Todo: eventually we need to support dpi / scaling in both dimensions
    int dpiX = m_originalImage->dotsPerMeterX() * 0.0254;

    if( dpiX > 1 )
    {
       m_ppi = dpiX;
    }
}


void BITMAP_BASE::ImportData( BITMAP_BASE& aItem )
{
    *m_image = *aItem.m_image;
    *m_bitmap = *aItem.m_bitmap;
    *m_originalImage = *aItem.m_originalImage;
    m_imageId = aItem.m_imageId;
    m_scale = aItem.m_scale;
    m_ppi = aItem.m_ppi;
    m_pixelSizeIu = aItem.m_pixelSizeIu;
    m_isMirroredX = aItem.m_isMirroredX;
    m_isMirroredY = aItem.m_isMirroredY;
    m_rotation = aItem.m_rotation;
    m_imageType = aItem.m_imageType;
    m_imageData = aItem.m_imageData;
}


bool BITMAP_BASE::ReadImageFile( QIODevice& aInStream )
{
    // Store the original image data in m_imageData
    QByteArray data = aInStream.readAll();
    m_imageData = data;


    // Load the image from the stream into new_image
    QImage newImage;
    if (!newImage.loadFromData(reinterpret_cast<const uchar*>(m_imageData.constData()), m_imageData.size()))
        return false;

    return SetImage(newImage);
}


bool BITMAP_BASE::ReadImageFile( QByteArray& aBuf )
{
    // Store the original image data in m_imageData
    m_imageData = aBuf;


    // Load the image from the buffer into new_image
    QImage newImage;
    if (!newImage.loadFromData(reinterpret_cast<const uchar*>(m_imageData.constData()), m_imageData.size()))
        return false;

    return SetImage(newImage);
}


bool BITMAP_BASE::ReadImageFile(const QString& aFullFilename)
{
    QFile file(aFullFilename);
    if (!file.open(QIODevice::ReadWrite))
        return false;

    // Check if the file could be opened successfully


    return ReadImageFile(file);
}


bool BITMAP_BASE::SetImage( const QImage& aImage )
{
    if( !aImage.isNull() || aImage.width() == 0 || aImage.height() == 0 )
        return false;

    delete m_image;
    m_image = new QImage( aImage );

    // Create a new wxImage object from m_image
    delete m_originalImage;
    m_originalImage = new QImage( *m_image );

    rebuildBitmap();
    updatePPI();

    return true;
}


bool BITMAP_BASE::SaveImageData( QIODevice& aOutStream ) const
{
    if( m_imageData.isEmpty() )
    {
        // If m_imageData is empty, use wxImage::Save() method to write m_image contents to
        // the stream.
        const char* format = (m_imageType == Image_Type::Image_Type_JPEG) ? "JPEG" : "PNG";

        if (!m_image->save(&aOutStream, format))
        {
            return false;
        }
    }
    else
    {
        // Write the contents of m_imageData to the stream.
        aOutStream.write( m_imageData.constData(), m_imageData.size() );
    }

    return true;
}


bool BITMAP_BASE::LoadLegacyData( LINE_READER& aLine, QString& aErrorMsg )
{
    QByteArray stream;
    char* line;

    while( true )
    {
        //if( !aLine.ReadLine() )
        //{
        //    aErrorMsg = "Unexpected end of data";
        //    return false;
        //}

        //line = aLine.Line();

        if( strnicmp( line, "EndData", 4) == 0 )
        {
            // all the PNG date is read.
            // We expect here m_image and m_bitmap are void
            m_image = new QImage();
            QBuffer istream( &stream );
            istream.open(QIODevice::ReadOnly);
            m_image->load(&istream, "PNG");
            m_bitmap = new QPixmap( QPixmap::fromImage(*m_image) );
            m_originalImage = new QImage( *m_image );
            updateImageDataBuffer();
            break;
        }

        // Read PNG data, stored in hexadecimal,
        // each byte = 2 hexadecimal digits and a space between 2 bytes
        // and put it in memory stream buffer
        int len = strlen( line );

        for( ; len > 0; len -= 3, line += 3 )
        {
            int value = 0;

            if( sscanf( line, "%X", &value ) == 1 )
                stream.append( (char) value );
            else
                break;
        }
    }

    return true;
}


const BOX2I BITMAP_BASE::GetBoundingBox() const
{
    BOX2I    bbox;
    VECTOR2I size = GetSize();

    bbox.Inflate( size.x / 2, size.y / 2 );

    return bbox;
}


void BITMAP_BASE::DrawBitmap( QPainter* aPainter, const VECTOR2I& aPos,
                              const KIGFX::COLOR4D& aBackgroundColor ) const
{
//    if( m_bitmap == nullptr )
//        return;
//
//    VECTOR2I pos = aPos;
//    VECTOR2I size = GetSize();
//
//    // This fixes a bug in OSX that should be fixed in the 3.0.3 version or later.
//    if( ( size.x == 0 ) || ( size.y == 0 ) )
//        return;
//
//    // To draw the bitmap, pos is the upper left corner position
//    pos.x -= size.x / 2;
//    pos.y -= size.y / 2;
//
//    aPainter->save();
//
//    double scale;
//    int    logicalOriginX, logicalOriginY;
//    scale = GetScalingFactor();
//    //aDC->GetUserScale( &scale, &scale );
//    aDC->GetLogicalOrigin( &logicalOriginX, &logicalOriginY );
//
//    // We already have issues to draw a bitmap on the wxDC, depending on wxWidgets version.
//    // Now we have an issue on wxWidgets 3.1.6 to fix the clip area
//    // and the bitmap position when using TransformMatrix
//    // So for version == 3.1.6  do not use it
//    // Be careful before changing the code.
//    bool useTransform = aDC->CanUseTransformMatrix();
//
//    wxAffineMatrix2D init_matrix = aDC->GetTransformMatrix();
//
//    // Note: clipping bitmap area was made to fix a minor issue in old versions of
//    // KiCad/wxWidgets (5.1 / wx 3.0)
//    // However SetClippingRegion creates a lot of issues (different ways to fix the
//    // position and size of the area, depending on wxWidgets version)because it changes with
//    // each versions of wxWidgets, so it is now disabled
//    // However the code is still here, just in case
//    // #define USE_CLIP_AREA
//
//    QPoint clipAreaPos;
//
//    if( useTransform )
//    {
//        wxAffineMatrix2D matrix = aDC->GetTransformMatrix();
//        matrix.Translate( pos.x, pos.y );
//        matrix.Scale( GetScalingFactor(), GetScalingFactor() );
//        aDC->SetTransformMatrix( matrix );
//
//        // Needed on wx <= 3.1.5, and this is strange...
//        // Nevertheless, this code has problem (the bitmap is not seen)
//        // with wx version > 3.1.5
//        clipAreaPos.setX(pos.x);
//        clipAreaPos.setY(pos.y);
//
//        pos.x = pos.y = 0;
//    }
//    else
//    {
//        aDC->SetUserScale( scale * GetScalingFactor(), scale * GetScalingFactor() );
//        aDC->SetLogicalOrigin( logicalOriginX / GetScalingFactor(),
//                               logicalOriginY / GetScalingFactor() );
//
//        pos.x  = KiROUND( pos.x / GetScalingFactor() );
//        pos.y  = KiROUND( pos.y / GetScalingFactor() );
//        size.x = KiROUND( size.x / GetScalingFactor() );
//        size.y = KiROUND( size.y / GetScalingFactor() );
//        clipAreaPos.setX(pos.x);
//        clipAreaPos.setY(pos.y);
//    }
//
//#ifdef USE_CLIP_AREA
//    aDC->DestroyClippingRegion();
//    aDC->SetClippingRegion( clipAreaPos, wxSize( size.x, size.y ) );
//#endif
//
//    if( aBackgroundColor != COLOR4D::UNSPECIFIED && m_bitmap->hasAlpha() )
//    {
//        // Most printers don't support transparent images properly,
//        // so blend the image with background color.
//
//        int w = m_bitmap->width();
//        int h = m_bitmap->height();
//
//        QImage  image( w, h );
//        QColor bgColor = aBackgroundColor.ToColour();
//
//        image.setColor( QRect( 0, 0, w, h ), bgColor.red(), bgColor.green(), bgColor.blue() );
//        image.Paste( m_bitmap->ConvertToImage(), 0, 0, wxIMAGE_ALPHA_BLEND_COMPOSE );
//
//        if( GetGRForceBlackPenState() )
//            image = image.ConvertToGreyscale();
//
//        aDC->DrawBitmap(QPixmap( QPixmap::fromImage(image) ), pos.x, pos.y, true );
//    }
//    else if( GetGRForceBlackPenState() )
//    {
//        QPixmap result( m_bitmap->toImage().ConvertToGreyscale() );
//        aPainter->drawImage(pos, result.toImage());
//        aDC->DrawBitmap( result, pos.x, pos.y, true );
//    }
//    else
//    {
//        aDC->DrawBitmap( *m_bitmap, pos.x, pos.y, true );
//    }
//
//    if( useTransform )
//        aDC->SetTransformMatrix( init_matrix );
//    else
//    {
//        aDC->SetUserScale( scale, scale );
//        aDC->SetLogicalOrigin( logicalOriginX, logicalOriginY );
//    }
//
//#ifdef USE_CLIP_AREA
//    aDC->DestroyClippingRegion();
//#endif
}


VECTOR2I BITMAP_BASE::GetSize() const
{
    VECTOR2I size;

    if( m_bitmap )
    {
        size.x = m_bitmap->width();
        size.y = m_bitmap->height();

        size.x = KiROUND( size.x * GetScalingFactor() );
        size.y = KiROUND( size.y * GetScalingFactor() );
    }

    return size;
}


void BITMAP_BASE::Mirror( FLIP_DIRECTION aFlipDirection )
{
    //if( m_image )
    //{
    //    // wxImage::Mirror() clear some parameters of the original image.
    //    // We need to restore them, especially resolution and unit, to be
    //    // sure image parameters saved in file are the right parameters, not
    //    // the default values
    //    int resX = m_image->GetOptionInt( wxIMAGE_OPTION_RESOLUTIONX );
    //    int resY = m_image->GetOptionInt( wxIMAGE_OPTION_RESOLUTIONY );
    //    int unit = m_image->GetOptionInt( wxIMAGE_OPTION_RESOLUTIONUNIT );

    //    *m_image = m_image->Mirror( aFlipDirection == FLIP_DIRECTION::LEFT_RIGHT );

    //    m_image->SetOption( wxIMAGE_OPTION_RESOLUTIONUNIT , unit);
    //    m_image->SetOption( wxIMAGE_OPTION_RESOLUTIONX, resX);
    //    m_image->SetOption( wxIMAGE_OPTION_RESOLUTIONY, resY);

    //    if( aFlipDirection == FLIP_DIRECTION::TOP_BOTTOM )
    //        m_isMirroredY = !m_isMirroredY;
    //    else
    //        m_isMirroredX = !m_isMirroredX;

    //    rebuildBitmap( false );
    //    updateImageDataBuffer();
    //}
}


void BITMAP_BASE::Rotate( bool aRotateCCW )
{
    //if( m_image )
    //{
    //    // wxImage::Rotate90() clear some parameters of the original image.
    //    // We need to restore them, especially resolution and unit, to be
    //    // sure image parameters saved in file are the right parameters, not
    //    // the default values
    //    int resX = m_image->GetOptionInt( wxIMAGE_OPTION_RESOLUTIONX );
    //    int resY = m_image->GetOptionInt( wxIMAGE_OPTION_RESOLUTIONY );
    //    int unit = m_image->GetOptionInt( wxIMAGE_OPTION_RESOLUTIONUNIT );

    //    *m_image = m_image->Rotate90( aRotateCCW );

    //    m_image->SetOption( wxIMAGE_OPTION_RESOLUTIONUNIT, unit );
    //    m_image->SetOption( wxIMAGE_OPTION_RESOLUTIONX, resX );
    //    m_image->SetOption( wxIMAGE_OPTION_RESOLUTIONY, resY );

    //    m_rotation += ( aRotateCCW ? ANGLE_90 : -ANGLE_90 );
    //    rebuildBitmap( false );
    //    updateImageDataBuffer();
    //}
}


void BITMAP_BASE::ConvertToGreyscale()
{
    //if( m_image )
    //{
    //    *m_image  = m_image->ConvertToGreyscale();
    //    *m_originalImage = m_originalImage->ConvertToGreyscale();
    //    rebuildBitmap();
    //    updateImageDataBuffer();
    //}
}


//void BITMAP_BASE::PlotImage( PLOTTER*       aPlotter, const VECTOR2I& aPos,
//                             const COLOR4D& aDefaultColor,
//                             int            aDefaultPensize ) const
//{
//    if( m_image == nullptr )
//        return;
//
//    // These 2 lines are useful only for plotters that cannot plot a bitmap
//    // and plot a rectangle instead of.
//    aPlotter->SetColor( aDefaultColor );
//    aPlotter->SetCurrentLineWidth( aDefaultPensize );
//    aPlotter->PlotImage( *m_image, aPos, GetScalingFactor() );
//}


void BITMAP_BASE::updateImageDataBuffer()
{
    //if( m_image )
    //{
    //    wxMemoryOutputStream stream;
    //    wxBitmapType type = m_imageType == wxBITMAP_TYPE_JPEG ? wxBITMAP_TYPE_JPEG
    //                                                          : wxBITMAP_TYPE_PNG;

    //    if( !m_image->SaveFile( stream, type ) )
    //        return;

    //    m_imageData.GetWriteBuf( stream.GetLength() );
    //    stream.CopyTo( m_imageData.GetData(), stream.GetLength() );
    //    m_imageData.SetDataLen( stream.GetLength() );
    //}
}
