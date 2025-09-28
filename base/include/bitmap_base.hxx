#pragma once


#include <QImage>
#include <QBitmap>
#include "mirror.hxx"
#include "kiid.hxx"
#include "box2.hxx"
#include "color4d.hxx"

class LINE_READER;
class PLOTTER;


enum Image_Type {
    Image_Type_INVALID,          // should be == 0 for compatibility!
    Image_Type_BMP,
    Image_Type_BMP_RESOURCE,
    Image_Type_RESOURCE = Image_Type_BMP_RESOURCE,
    Image_Type_ICO,
    Image_Type_ICO_RESOURCE,
    Image_Type_CUR,
    Image_Type_CUR_RESOURCE,
    Image_Type_XBM,
    Image_Type_XBM_DATA,
    Image_Type_XPM,
    Image_Type_XPM_DATA,
    Image_Type_TIFF,
    Image_Type_TIF = Image_Type_TIFF,
    Image_Type_TIFF_RESOURCE,
    Image_Type_TIF_RESOURCE = Image_Type_TIFF_RESOURCE,
    Image_Type_GIF,
    Image_Type_GIF_RESOURCE,
    Image_Type_PNG,
    Image_Type_PNG_RESOURCE,
    Image_Type_JPEG,
    Image_Type_JPEG_RESOURCE,
    Image_Type_PNM,
    Image_Type_PNM_RESOURCE,
    Image_Type_PCX,
    Image_Type_PCX_RESOURCE,
    Image_Type_PICT,
    Image_Type_PICT_RESOURCE,
    Image_Type_ICON,
    Image_Type_ICON_RESOURCE,
    Image_Type_ANI,
    Image_Type_IFF,
    Image_Type_TGA,
    Image_Type_MACCURSOR,
    Image_Type_MACCURSOR_RESOURCE,
    Image_Type_WEBP,
    Image_Type_WEBP_RESOURCE,

    Image_Type_MAX,
    Image_Type_ANY = 50
};

/**
 * This class handle bitmap images in KiCad.
 *
 * It is not intended to be used alone, but inside another class so all methods are protected
 * or private.  It is used in #SCH_BITMAP class, #DS_DRAW_ITEM_BITMAP, and possibly others in
 * the future.
 *
 * @warning Not all plotters are able to plot a bitmap.  Mainly GERBER plotters cannot.
 */
class BITMAP_BASE
{
public:
    BITMAP_BASE( const VECTOR2I& pos = VECTOR2I( 0, 0 ) );

    BITMAP_BASE( const BITMAP_BASE& aSchBitmap );

    ~BITMAP_BASE()
    {
        delete m_bitmap;
        delete m_image;
        delete m_originalImage;
    }

    /*
     * Accessors:
     */
    double GetPixelSizeIu() const { return m_pixelSizeIu; }
    void SetPixelSizeIu( double aPixSize ) { m_pixelSizeIu = aPixSize; }

    QImage* GetImageData() { return m_image; }
    const QImage* GetImageData() const { return m_image; }

    const QImage* GetOriginalImageData() const { return m_originalImage; }

    double GetScale() const { return m_scale; }
    void SetScale( double aScale ) { m_scale = aScale; }

    KIID GetImageID() const { return m_imageId; }

    /**
     * Copy aItem image to this object and update #m_bitmap.
     */
    void ImportData( BITMAP_BASE& aItem );

    /**
     * This scaling factor depends on #m_pixelSizeIu and #m_scale.
     *
     * #m_pixelSizeIu gives the scaling factor between a pixel size and the internal units.
     * #m_scale is an user dependent value, and gives the "zoom" value.
     *  - #m_scale = 1.0 = original size of bitmap.
     *  - #m_scale < 1.0 = the bitmap is drawn smaller than its original size.
     *  - #m_scale > 1.0 = the bitmap is drawn bigger than its original size.
     *
     * @return The scaling factor from pixel size to actual draw size.
     */
    double GetScalingFactor() const
    {
        return m_pixelSizeIu * m_scale;
    }

    /**
     * @return the actual size (in user units, not in pixels) of the image
     */
    VECTOR2I GetSize() const;

    /**
     * @return the size in pixels of the image
     */
    VECTOR2I GetSizePixels() const
    {
        if( m_image )
            return VECTOR2I( m_image->width(), m_image->height() );
        else
            return VECTOR2I( 0, 0 );
    }

    /**
     * @return the bitmap definition in ppi, the default is 300 ppi.
     */
    int GetPPI() const
    {
        return m_ppi;
    }

    /**
     * Return the orthogonal, bounding box of this object for display purposes.
     *
     * This box should be an enclosing perimeter for visible components of this object,
     * and the units should be in the pcb or schematic coordinate system.  It is OK to
     * overestimate the size by a few counts.
     */
    const BOX2I GetBoundingBox() const;

    void DrawBitmap( QPainter* aDC, const VECTOR2I& aPos,
                     const KIGFX::COLOR4D& aBackgroundColor = KIGFX::COLOR4D::UNSPECIFIED ) const;

    /**
     * Reads and stores in memory an image file.
     *
     * Initialize the bitmap format used to draw this item.  Supported images formats are
     * format supported by wxImage if all handlers are loaded.  By default, .png, .jpeg
     * are always loaded.
     *
     * @param aFullFilename The full filename of the image file to read.
     * @return  true if success reading else false.
     */
    bool ReadImageFile( const QString& aFullFilename );

    /**
     * Reads and stores in memory an image file.
     *
     * Initialize the bitmap format used to draw this item.
     *
     * Supported images formats are format supported by wxImage if all handlers are loaded.
     * By default, .png, .jpeg are always loaded.
     *
     * @param aInStream an input stream containing the file data.
     * @return true if success reading else false.
     */
    bool ReadImageFile(QIODevice& aInStream );

    /**
     * Reads and stores in memory an image file.
     *
     * Initialize the bitmap format used to draw this item.
     *
     * Supported images formats are format supported by wxImage if all handlers are loaded.
     * By default, .png, .jpeg are always loaded.
     *
     * @param aBuf a memory buffer containing the file data.
     * @return true if success reading else false.
     */
    bool ReadImageFile( QByteArray& aBuf );

    /**
     * Set the image from an existing wxImage.
     */
    bool SetImage( const QImage& aImage );

    /**
    * Write the bitmap data to \a aOutStream.
    *
    * This writes binary data, not hexadecimal strings
    *
    * @param aOutStream The output stream to write to.
    * @return true if success writing else false.
    */
    bool SaveImageData(QIODevice& aOutStream ) const;

    /**
     * Load an image data saved by #SaveData.
     *
     * The file format must be png format in hexadecimal.
     *
     * @param aLine the LINE_READER used to read the data file.
     * @param aErrorMsg Description of the error if an error occurs while loading the
     *                  png bitmap data.
     * @return true if the bitmap loaded successfully.
     */
    bool LoadLegacyData( LINE_READER& aLine, QString& aErrorMsg );

    /**
     * Mirror image vertically (i.e. relative to its horizontal X axis ) or horizontally (i.e
     * relative to its vertical Y axis).
     * @param aFlipDirection the direction to flip the image.
     */
    void Mirror( FLIP_DIRECTION aFlipDirection );

    /**
     * Rotate image CW or CCW.
     *
     * @param aRotateCCW true to rotate CCW or false to rotate CW.
     */
    void Rotate( bool aRotateCCW );

    void ConvertToGreyscale();

    bool IsMirroredX() const { return m_isMirroredX; }
    bool IsMirroredY() const { return m_isMirroredY; }
    EDA_ANGLE Rotation() const { return m_rotation; }

    /**
     * Plot bitmap on plotter.
     *
     * If the plotter does not support bitmaps, plot a
     *
     * @param aPlotter the plotter to use.
     * @param aPos the position of the center of the bitmap.
     * @param aDefaultColor the color used to plot the rectangle when bitmap is not supported.
     * @param aDefaultPensize the pen size used to plot the rectangle when bitmap is not supported.
     */
    void PlotImage( PLOTTER* aPlotter, const VECTOR2I& aPos,
                    const KIGFX::COLOR4D& aDefaultColor, int aDefaultPensize ) const;

    /**
     * Return the bitmap type (png, jpeg, etc.)
     */
    Image_Type GetImageType() const { return m_imageType; }

    /**
     * Set the bitmap type (png, jpeg, etc.)
     */
    void SetImageType(Image_Type aType ) { m_imageType = aType; }

private:
    /**
     * Resets the image data buffer using the current image data.
     */
    void updateImageDataBuffer();

    /**
     * Rebuild the internal bitmap used to draw/plot image.
     *
     * This must be called after a #m_image change.
     *
     * @param aResetID is used to reset the cache ID used for OpenGL rendering.
     */
    void rebuildBitmap( bool aResetID = true );

    void updatePPI();

    double    m_scale;              ///< The scaling factor of the bitmap
                                    ///< with #m_pixelSizeIu, controls the actual draw size.
    QByteArray m_imageData;     ///< The original image data in its original format.
    Image_Type   m_imageType;     ///< The image type (png, jpeg, etc.).

    QImage*  m_image;              ///< The raw, uncompressed image data.
    QImage*  m_originalImage;      ///< Raw image data, not transformed by rotate/mirror.
    QPixmap* m_bitmap;             ///< The bitmap used to draw/plot image.
    double    m_pixelSizeIu;        ///< The scaling factor of the bitmap to convert the bitmap
                                    ///< size (in pixels) to internal KiCad units.  This usually
                                    ///< does not change.
    int       m_ppi;                ///< The bitmap definition. The default is 300PPI.
    KIID      m_imageId;
    bool      m_isMirroredX;        // Used for OpenGL rendering only
    bool      m_isMirroredY;        // Used for OpenGL rendering only
    EDA_ANGLE m_rotation;           // Used for OpenGL rendering only
};
