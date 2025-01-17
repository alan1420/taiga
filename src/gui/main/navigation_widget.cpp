/**
 * Taiga
 * Copyright (C) 2010-2024, Eren Okka
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "navigation_widget.hpp"

#include <QMouseEvent>

#include "gui/main/main_window.hpp"
#include "gui/main/navigation_item_delegate.hpp"
#include "gui/utils/format.hpp"
#include "gui/utils/theme.hpp"
#include "media/anime.hpp"
#include "media/anime_db.hpp"
#include "media/anime_list.hpp"

namespace gui {

NavigationWidget::NavigationWidget(QWidget* parent) : QTreeWidget(parent) {
  setObjectName("navigation");
  setFixedWidth(200);
  setFrameShape(QFrame::Shape::NoFrame);
  setItemDelegate(new NavigationItemDelegate(this));
  setHeaderHidden(true);
  setIndentation(0);

  viewport()->setMouseTracking(true);

  refresh();

  connect(this, &QTreeWidget::currentItemChanged, this, [this](QTreeWidgetItem* current) {
    if (!current) return;

    constexpr auto pageIndexRole = static_cast<int>(NavigationItemDataRole::PageIndex);
    const auto page = current->data(0, pageIndexRole).value<MainWindowPage>();
    emit currentPageChanged(page);

    if (page != MainWindowPage::List) return;

    constexpr auto statusRole = static_cast<int>(NavigationItemDataRole::ListStatus);
    const auto status = current->data(0, statusRole).value<anime::list::Status>();
    emit currentListStatusChanged(status);
  });
}

void NavigationWidget::refresh() {
  setUpdatesEnabled(false);
  clear();

  addItem("Home", "home", MainWindowPage::Home);
  addItem("Search", "search", MainWindowPage::Search);
  addSeparator();

  auto listItem = addItem("Anime List", "list_alt", MainWindowPage::List);
  listItem->setExpanded(true);
  setItemData(listItem, NavigationItemDataRole::HasChildren, true);

  const auto statusCounts = []() {
    QMap<anime::list::Status, int> statuses;
    for (const auto& entry : anime::db.entries()) {
      statuses[entry.status] += 1;
    }
    return statuses;
  }();
  for (const auto status : anime::list::kStatuses) {
    auto item = addChildItem(listItem, formatListStatus(status));
    setItemData(item, NavigationItemDataRole::PageIndex, static_cast<int>(MainWindowPage::List));
    setItemData(item, NavigationItemDataRole::IsLastChild,
                status == anime::list::Status::PlanToWatch);
    setItemData(item, NavigationItemDataRole::ListStatus, static_cast<int>(status));
    setItemData(item, NavigationItemDataRole::Counter, statusCounts[status]);
  }

  addItem("History", "history", MainWindowPage::History);
  addSeparator();
  addItem("Library", "folder", MainWindowPage::Library);
  addItem("Torrents", "rss_feed", MainWindowPage::Torrents);

  setUpdatesEnabled(true);
}

void NavigationWidget::mouseMoveEvent(QMouseEvent* event) {
  auto cursor = Qt::CursorShape::ArrowCursor;

  if (const auto item = itemAt(event->pos())) {
    const int role = static_cast<int>(NavigationItemDataRole::IsSeparator);
    const bool isSeparator = item->data(0, role).toBool();
    if (!isSeparator) cursor = Qt::CursorShape::PointingHandCursor;
  }

  setCursor(cursor);

  QTreeView::mouseMoveEvent(event);
}

QTreeWidgetItem* NavigationWidget::addItem(const QString& text, const QString& icon,
                                           MainWindowPage page) {
  auto item = new QTreeWidgetItem(this);

  item->setFont(0, [item]() {
    auto font = item->font(0);
    font.setPointSize(10);
    font.setWeight(QFont::Weight::DemiBold);
    return font;
  }());

  item->setIcon(0, theme.getIcon(icon));
  item->setSizeHint(0, QSize{0, 32});
  item->setText(0, text);

  setItemData(item, NavigationItemDataRole::PageIndex, static_cast<int>(page));

  return item;
}

QTreeWidgetItem* NavigationWidget::addChildItem(QTreeWidgetItem* parent, const QString& text) {
  auto item = new QTreeWidgetItem(parent);

  item->setIcon(0, theme.getIcon("empty"));  // for indentation
  item->setSizeHint(0, QSize{0, 32});
  item->setText(0, text);

  setItemData(item, NavigationItemDataRole::IsChild, true);

  return item;
}

void NavigationWidget::addSeparator() {
  auto item = new QTreeWidgetItem(this);

  item->setDisabled(true);
  item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
  item->setSizeHint(0, QSize{0, 16});

  setItemData(item, NavigationItemDataRole::IsSeparator, true);
}

void NavigationWidget::setItemData(QTreeWidgetItem* item, NavigationItemDataRole role,
                                   const QVariant& value) {
  item->setData(0, static_cast<int>(role), value);
}

QTreeWidgetItem* NavigationWidget::findItemByPage(MainWindowPage page) const {
  static const auto find = [page](this auto const& find,
                                  QTreeWidgetItem* item) -> QTreeWidgetItem* {
    const auto role = static_cast<int>(NavigationItemDataRole::PageIndex);
    const int data = item->data(0, role).toInt();
    if (data == static_cast<int>(page)) return item;
    for (int i = 0; i < item->childCount(); ++i) {
      if (const auto child = find(item->child(i))) return child;
    }
    return nullptr;
  };

  for (int i = 0; i < topLevelItemCount(); ++i) {
    if (const auto item = find(topLevelItem(i))) return item;
  }

  return nullptr;
}

}  // namespace gui
