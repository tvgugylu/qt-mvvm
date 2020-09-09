// ************************************************************************** //
//
//  Model-view-view-model framework for large GUI applications
//
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @authors   see AUTHORS
//
// ************************************************************************** //

#include <QJsonArray>
#include <QJsonObject>
#include <mvvm/model/itemfactoryinterface.h>
#include <mvvm/model/sessionitem.h>
#include <mvvm/model/sessionitemdata.h>
#include <mvvm/model/sessionitemtags.h>
#include <mvvm/serialization/jsonitem_types.h>
#include <mvvm/serialization/jsonitemconverter_v2.h>
#include <mvvm/serialization/jsonitemdataconverter.h>
#include <mvvm/serialization/jsonitemformatassistant.h>
#include <mvvm/serialization/jsonitemtagsconverter.h>

using namespace ModelView;

struct JsonItemConverterV2::JsonItemConverterV2Impl {
    JsonItemConverterV2* m_parent{nullptr};
    const ItemFactoryInterface* m_factory{nullptr};
    bool m_is_new_id{false};
    std::unique_ptr<JsonItemDataConverter> m_itemdata_converter;
    std::unique_ptr<JsonItemTagsConverter> m_itemtags_converter;

    JsonItemConverterV2Impl(JsonItemConverterV2* parent) : m_parent(parent)
    {
        //! Callback to convert SessionItem to JSON object.
        auto to_json = [this](const SessionItem& item) { return m_parent->to_json(&item); };

        //! Callback to update SessionItem from JSON object
        auto update_item = [this](const QJsonObject& json, SessionItem* item) {
            populate_item(json, *item);
        };

        //! Simplified method to create SessionItem from JSON object
        auto create_item = [this](const QJsonObject& json) { return m_parent->from_json(json); };

        ConverterContext context{to_json, update_item, create_item};

        m_itemdata_converter = std::make_unique<JsonItemDataConverter>();
        m_itemtags_converter = std::make_unique<JsonItemTagsConverter>(context);
    }

    const ItemFactoryInterface* factory() { return m_factory; }

    void populate_item_data(const QJsonArray& json, SessionItemData& item_data)
    {
        m_itemdata_converter->from_json(json, item_data);
    }

    void populate_item_tags(const QJsonObject& json, SessionItemTags& item_tags)
    {
        m_itemtags_converter->from_json(json, item_tags);
    }

    void populate_item(const QJsonObject& json, SessionItem& item)
    {
        auto modelType = json[JsonItemFormatAssistant::modelKey].toString().toStdString();

        if (modelType != item.modelType())
            throw std::runtime_error("Item model mismatch");

        populate_item_tags(json[JsonItemFormatAssistant::itemTagsKey].toObject(), *item.itemTags());
        populate_item_data(json[JsonItemFormatAssistant::itemDataKey].toArray(), *item.itemData());
    }

    QJsonObject item_to_json(const SessionItem& item) const
    {
        QJsonObject result;
        result[JsonItemFormatAssistant::modelKey] = QString::fromStdString(item.modelType());
        result[JsonItemFormatAssistant::itemDataKey] =
            m_itemdata_converter->get_json(*item.itemData());
        result[JsonItemFormatAssistant::itemTagsKey] =
            m_itemtags_converter->to_json(*item.itemTags());

        return result;
    }
};

JsonItemConverterV2::JsonItemConverterV2(const ItemFactoryInterface* factory, bool new_id_flag)
    : p_impl(std::make_unique<JsonItemConverterV2Impl>(this))
{
    p_impl->m_factory = factory;
    p_impl->m_is_new_id = new_id_flag;
}

JsonItemConverterV2::~JsonItemConverterV2() = default;

QJsonObject JsonItemConverterV2::to_json(const SessionItem* item) const
{
    return item ? p_impl->item_to_json(*item) : QJsonObject();
}

std::unique_ptr<SessionItem> JsonItemConverterV2::from_json(const QJsonObject& json) const
{
    static JsonItemFormatAssistant assistant;

    if (!assistant.isSessionItem(json))
        throw std::runtime_error("JsonItemConverterV2::from_json() -> Error. Given json object "
                                 "can't represent an SessionItem.");

    auto modelType = json[JsonItemFormatAssistant::modelKey].toString().toStdString();
    auto result = p_impl->factory()->createItem(modelType);

    p_impl->populate_item(json, *result);

    return result;
}
