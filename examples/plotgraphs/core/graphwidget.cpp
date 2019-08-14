// ************************************************************************** //
//
//  Prototype of mini MVVM framework for bornagainproject.org
//
//! @homepage  http://www.bornagainproject.org
//! @license   GNU General Public License v3 or higher
//
// ************************************************************************** //

#include "graphwidget.h"
#include <QBoxLayout>

using namespace ModelView;

GraphWidget::GraphWidget(GraphModel* model, QWidget* parent) : QWidget(parent)
{
    auto mainLayout = new QHBoxLayout();
    mainLayout->setSpacing(10);

    mainLayout->addLayout(create_left_layout(), 1);
    mainLayout->addLayout(create_right_layout(), 3);

    setLayout(mainLayout);
    setModel(model);
}

void GraphWidget::setModel(GraphModel* model)
{
    if (!model)
        return;
}

GraphWidget::~GraphWidget() = default;

QBoxLayout* GraphWidget::create_left_layout()
{
    auto result = new QVBoxLayout;
    result->addWidget(new QWidget);
    return result;
}

QBoxLayout* GraphWidget::create_right_layout()
{
    auto result = new QVBoxLayout;
    result->addWidget(new QWidget);
    return result;
}