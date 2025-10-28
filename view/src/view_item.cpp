#include "view_item.hxx"
#include "view.hxx"

using namespace KIGFX;

VIEW_ITEM::~VIEW_ITEM()
{
    VIEW::OnDestroy(this);
    m_viewPrivData = nullptr;
}


double VIEW_ITEM::lodScaleForThreshold(const VIEW* aView, int aWhatIu, int aThresholdIu)
{
    //if (aView->GetPainter()->GetSettings()->IsPrinting())
    //    return LOD_SHOW;

    //if (aWhatIu == 0)
    //    return LOD_HIDE;

    //return double(aThresholdIu) / aWhatIu;
    return LOD_SHOW;
}
