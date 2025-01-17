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

#include "anime_list_proxy_model.hpp"

#include <ranges>

#include "gui/models/anime_list_model.hpp"
#include "media/anime.hpp"
#include "media/anime_list.hpp"
#include "media/anime_season.hpp"

namespace {

const Anime* getAnime(const QModelIndex& index) {
  const int role = static_cast<int>(gui::AnimeListItemDataRole::Anime);
  return index.data(role).value<const Anime*>();
}

const ListEntry* getListEntry(const QModelIndex& index) {
  const int role = static_cast<int>(gui::AnimeListItemDataRole::ListEntry);
  return index.data(role).value<const ListEntry*>();
}

}  // namespace

namespace gui {

AnimeListProxyModel::AnimeListProxyModel(QObject* parent) : QSortFilterProxyModel(parent) {
  setFilterKeyColumn(AnimeListModel::COLUMN_TITLE);
  setFilterRole(Qt::DisplayRole);

  setSortCaseSensitivity(Qt::CaseInsensitive);
  setSortRole(Qt::UserRole);
}

const AnimeListProxyModelFilter& AnimeListProxyModel::filters() const {
  return m_filter;
}

void AnimeListProxyModel::setFilters(const AnimeListProxyModelFilter& filters) {
  m_filter = filters;
  invalidateRowsFilter();
}

void AnimeListProxyModel::setYearFilter(std::optional<int> year) {
  m_filter.year = year;
  invalidateRowsFilter();
}

void AnimeListProxyModel::setSeasonFilter(std::optional<int> season) {
  m_filter.season = season;
  invalidateRowsFilter();
}

void AnimeListProxyModel::setTypeFilter(std::optional<int> type) {
  m_filter.type = type;
  invalidateRowsFilter();
}

void AnimeListProxyModel::setStatusFilter(std::optional<int> status) {
  m_filter.status = status;
  invalidateRowsFilter();
}

void AnimeListProxyModel::setListStatusFilter(AnimeListStatusFilter filter) {
  m_filter.listStatus = filter;
  invalidateRowsFilter();
}

void AnimeListProxyModel::setTextFilter(const QString& text) {
  m_filter.text = text;
  invalidateRowsFilter();
}

bool AnimeListProxyModel::filterAcceptsRow(int row, const QModelIndex& parent) const {
  const auto model = static_cast<AnimeListModel*>(sourceModel());
  if (!model) return false;
  const auto index = model->index(row, 0, parent);
  const auto anime = getAnime(index);
  if (!anime) return false;
  const auto entry = getListEntry(index);

  static const auto contains = [](const std::string& str, const QStringView view) {
    return QString::fromStdString(str).contains(view, Qt::CaseInsensitive);
  };

  static const auto list_contains = [](const std::vector<std::string>& list,
                                       const QStringView view) {
    return std::ranges::any_of(list,
                               [view](const std::string& str) { return contains(str, view); });
  };

  // Year
  if (m_filter.year) {
    if (anime->date_started.year() != *m_filter.year) return false;
  }

  // Season
  if (m_filter.season) {
    const anime::Season season{anime->date_started};
    if (static_cast<int>(season.name) != *m_filter.season) return false;
  }

  // Type
  if (m_filter.type) {
    if (static_cast<int>(anime->type) != *m_filter.type) return false;
  }

  // Status
  if (m_filter.status) {
    if (static_cast<int>(anime->status) != *m_filter.status) return false;
  }

  // List status
  if (m_filter.listStatus.status) {
    const auto status = entry ? entry->status : anime::list::Status::NotInList;
    if (m_filter.listStatus.anyStatus) {
      if (status == anime::list::Status::NotInList) return false;
    } else {
      if (static_cast<int>(status) != *m_filter.listStatus.status) return false;
    }
  }

  // Titles
  if (!m_filter.text.isEmpty()) {
    if (!contains(anime->titles.romaji, m_filter.text) &&
        !contains(anime->titles.english, m_filter.text) &&
        !contains(anime->titles.japanese, m_filter.text) &&
        !list_contains(anime->titles.synonyms, m_filter.text)) {
      return false;
    }
  }

  return true;
}

bool AnimeListProxyModel::lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const {
  const auto lhs_anime = getAnime(lhs);
  const auto rhs_anime = getAnime(rhs);
  if (!lhs_anime || !rhs_anime) return false;

  const auto lhs_entry = getListEntry(lhs);
  const auto rhs_entry = getListEntry(rhs);

  // @TODO: Remove conversion to `QString` once we have our own string functions in place.
  static const auto compare_strings = [](const std::string& a, const std::string& b) {
    return QString::fromStdString(a).compare(QString::fromStdString(b), Qt::CaseInsensitive);
  };

  switch (lhs.column()) {
    case AnimeListModel::COLUMN_TITLE:
      return compare_strings(lhs_anime->titles.romaji, rhs_anime->titles.romaji) < 0;

    case AnimeListModel::COLUMN_DURATION:
      return lhs_anime->episode_length < rhs_anime->episode_length;

    case AnimeListModel::COLUMN_AVERAGE:
      return lhs_anime->score < rhs_anime->score;

    case AnimeListModel::COLUMN_TYPE:
      return lhs_anime->type < rhs_anime->type;

    case AnimeListModel::COLUMN_PROGRESS:
      return (lhs_entry ? lhs_entry->watched_episodes : 0) <
             (rhs_entry ? rhs_entry->watched_episodes : 0);

    case AnimeListModel::COLUMN_REWATCHES:
      return (lhs_entry ? lhs_entry->rewatched_times : 0) <
             (rhs_entry ? rhs_entry->rewatched_times : 0);

    case AnimeListModel::COLUMN_SCORE:
      return (lhs_entry ? lhs_entry->score : 0) < (rhs_entry ? rhs_entry->score : 0);

    case AnimeListModel::COLUMN_SEASON:
      return lhs_anime->date_started < rhs_anime->date_started;

    case AnimeListModel::COLUMN_STARTED:
      return (lhs_entry ? lhs_entry->date_started : FuzzyDate{}) <
             (rhs_entry ? rhs_entry->date_started : FuzzyDate{});

    case AnimeListModel::COLUMN_COMPLETED:
      return (lhs_entry ? lhs_entry->date_completed : FuzzyDate{}) <
             (rhs_entry ? rhs_entry->date_completed : FuzzyDate{});

    case AnimeListModel::COLUMN_LAST_UPDATED:
      return (lhs_entry ? lhs_entry->last_updated : 0) < (rhs_entry ? rhs_entry->last_updated : 0);

    case AnimeListModel::COLUMN_NOTES:
      return (lhs_entry ? lhs_entry->notes : "") < (rhs_entry ? rhs_entry->notes : "");
  }

  return false;
}

}  // namespace gui
