// ************************************************************************** //
//
//  Model-view-view-model framework for large GUI applications
//
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @authors   see AUTHORS
//
// ************************************************************************** //

#ifndef STYLEUTILS_H
#define STYLEUTILS_H

class QSize;

//! Namespace for central access to all theme styling.

namespace StyleUtils
{

//! Size of tolbar icons for LayerEditor, MaterialEditor and similar.
QSize ToolBarIconSize();

//! Hint on size of docks on main reflectometry window.
QSize DockSizeHint();

//! Hint on minimum size of docks on main reflectometry window.
QSize DockMinimumSizeHint();

};

#endif // STYLEUTILS_H
