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

#pragma once

#include <QSortFilterProxyModel>
#include <optional>

namespace gui {

struct ListProxyModelFilter {
  std::optional<int> listStatus;
  QString text;
};

class ListProxyModel final : public QSortFilterProxyModel {
  Q_OBJECT
  Q_DISABLE_COPY_MOVE(ListProxyModel)

public:
  ListProxyModel(QObject* parent);
  ~ListProxyModel() = default;

  void removeListStatusFilter();
  void setListStatusFilter(int status);
  void setTextFilter(const QString& text);

protected:
  bool filterAcceptsRow(int row, const QModelIndex& parent) const override;
  bool lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const override;

private:
  ListProxyModelFilter m_filter;
};

}  // namespace gui
