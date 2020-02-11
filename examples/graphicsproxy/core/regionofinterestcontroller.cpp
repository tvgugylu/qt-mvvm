// ************************************************************************** //
//
//  Model-view-view-model framework for large GUI applications
//
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @authors   see AUTHORS
//
// ************************************************************************** //

#include "regionofinterestcontroller.h"
#include "regionofinterestview.h"
#include "sceneitems.h"
#include <mvvm/plotting/sceneadapterinterface.h>

using namespace ModelView;

struct RegionOfInterestController::RegionOfInterestControllerImpl {
    RegionOfInterestItem* roi_item{nullptr};
    RegionOfInterestView* roi_view{nullptr};
    const SceneAdapterInterface* scene_adapter{nullptr};
    bool block_on_property_changed{false};
    QRectF roi_rectangle;
    std::unique_ptr<QMetaObject::Connection> xpos_conn;
    std::unique_ptr<QMetaObject::Connection> ypos_conn;
    RegionOfInterestControllerImpl(RegionOfInterestItem* item, RegionOfInterestView* view)
        : roi_item(item), roi_view(view)
    {
        xpos_conn = std::make_unique<QMetaObject::Connection>();
        ypos_conn = std::make_unique<QMetaObject::Connection>();
    }

    //! Connects signals related to QGraphicsObject position change. Will update
    //! properties of RegionOfInterestItem on any move performed by the user on scene.

    void connect_graphics_view()
    {
        auto on_scene_x_changed = [this]() {
            block_on_property_changed = true;
            roi_item->setProperty(RegionOfInterestItem::P_XLOW,
                                  scene_adapter->fromSceneX(roi_view->x()));
            roi_item->setProperty(RegionOfInterestItem::P_XUP,
                                  scene_adapter->fromSceneX(roi_view->x() + roi_rectangle.width()));
            block_on_property_changed = false;
        };
        *xpos_conn =
            QObject::connect(roi_view, &RegionOfInterestView::xChanged, on_scene_x_changed);

        auto on_scene_y_changed = [this]() {
            block_on_property_changed = true;
            roi_item->setProperty(
                RegionOfInterestItem::P_YLOW,
                scene_adapter->fromSceneY(roi_view->y() + roi_rectangle.height()));
            roi_item->setProperty(RegionOfInterestItem::P_YUP,
                                  scene_adapter->fromSceneY(roi_view->y()));
            block_on_property_changed = false;
        };
        *ypos_conn =
            QObject::connect(roi_view, &RegionOfInterestView::yChanged, on_scene_y_changed);
    }

    //! Calculates view rectangle in scene coordinates from item properties.

    void set_view_rectangle_from_item() { roi_rectangle = QRectF(0.0, 0.0, width(), height()); }

    //! Sets view position in scene from item properties.

    void set_view_position_from_item()
    {
        roi_view->setX(scene_adapter->toSceneX(par(RegionOfInterestItem::P_XLOW)));
        roi_view->setY(scene_adapter->toSceneY(par(RegionOfInterestItem::P_YUP)));
    }

    //! Sets view appearance from item properties and current state of scene adapter.

    void update_geometry()
    {
        set_view_rectangle_from_item();
        set_view_position_from_item();
    }

    double par(const std::string& name) const { return roi_item->property(name).value<double>(); }
    double width() const { return right() - left(); }
    double height() const { return bottom() - top(); }
    double left() const { return scene_adapter->toSceneX(par(RegionOfInterestItem::P_XLOW)); }
    double right() const { return scene_adapter->toSceneX(par(RegionOfInterestItem::P_XUP)); }
    double top() const { return scene_adapter->toSceneY(par(RegionOfInterestItem::P_YUP)); }
    double bottom() const { return scene_adapter->toSceneY(par(RegionOfInterestItem::P_YLOW)); }
};

RegionOfInterestController::RegionOfInterestController(RegionOfInterestItem* item,
                                                       RegionOfInterestView* view)
    : p_impl(std::make_unique<RegionOfInterestControllerImpl>(item, view))
{
    setItem(item);
}

QRectF RegionOfInterestController::roi_rectangle() const
{
    return p_impl->roi_rectangle;
}

void RegionOfInterestController::update_geometry()
{
    p_impl->update_geometry();
}

RegionOfInterestController::~RegionOfInterestController() = default;

void RegionOfInterestController::subscribe()
{
    p_impl->update_geometry();

    auto on_property_change = [this](SessionItem*, std::string) {
        if (p_impl->block_on_property_changed)
            return;

        p_impl->update_geometry();
    };
    currentItem()->mapper()->setOnPropertyChange(on_property_change, this);

    p_impl->connect_graphics_view();
}
