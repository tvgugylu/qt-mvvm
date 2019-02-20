// ************************************************************************** //
//
//  Prototype of mini MVVM framework for bornagainproject.org
//
//! @homepage  http://www.bornagainproject.org
//! @license   GNU General Public License v3 or higher
//
// ************************************************************************** //

#ifndef MODELMAPPER_H
#define MODELMAPPER_H

#include "global.h"
#include "model_types.h"
#include <algorithm>
#include <functional>
#include <string>
#include <vector>

namespace ModelView
{

class SessionItem;
class SessionModel;

//! Provides notifications on various SessionModel changes.
//! Used to notify QAbstractItemModel.

class CORE_EXPORT ModelMapper
{
    friend class SessionModel;
    friend class SessionItem;
public:
    ModelMapper(SessionModel* item);

    void setOnDataChange(Callbacks::func_item_int_t f, Callbacks::caller_t caller = 0);
    void setOnRowInserted(Callbacks::func_item_int_t f, Callbacks::caller_t caller = 0);
    void setOnRowRemoved(Callbacks::func_item_int_t f, Callbacks::caller_t caller = 0);
    void setOnRowRemoved2(Callbacks::func_item_int_str_t f, Callbacks::caller_t caller = 0);

    void setActive(bool value);

    void unsubscribe(Callbacks::caller_t caller);

private:
    template <class U> void clean_container(U& v, Callbacks::caller_t caller);

    void callOnDataChange(SessionItem* item, int role);
    void callOnRowInserted(SessionItem* parent, int index);
    void callOnRowRemoved(SessionItem* parent, int index);
    void callOnRowRemoved2(SessionItem* parent, int index, std::string id);

    std::vector<std::pair<Callbacks::func_item_int_t, Callbacks::caller_t>> m_on_data_change;
    std::vector<std::pair<Callbacks::func_item_int_t, Callbacks::caller_t>> m_on_row_inserted;
    std::vector<std::pair<Callbacks::func_item_int_t, Callbacks::caller_t>> m_on_row_removed;
    std::vector<std::pair<Callbacks::func_item_int_str_t, Callbacks::caller_t>> m_on_row_removed2;

    bool m_active;
    SessionModel* m_model;
};

template <class U> inline void ModelMapper::clean_container(U& v, const void* caller)
{
    v.erase(std::remove_if(v.begin(), v.end(),
                           [caller](typename U::value_type const& x) -> bool {
                               return (x.second == caller ? true : false);
                           }),
            v.end());
}

} // ModelView

#endif // MODELMAPPER_H
