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

#include "myanimelist_parsers.hpp"

#include <QDateTime>
#include <QMap>

#include "base/chrono.hpp"
#include "base/string.hpp"
#include "media/anime.hpp"
#include "media/anime_list.hpp"

namespace sync::myanimelist {

anime::AgeRating parseAgeRating(const QString& value) {
  using anime::AgeRating;
  // clang-format off
  static const QMap<QString, AgeRating> table{
      {"g", AgeRating::G},
      {"pg", AgeRating::PG},
      {"pg_13", AgeRating::PG13},
      {"r", AgeRating::R17},
      {"r+", AgeRating::R17},
      {"rx", AgeRating::R18},
  };
  // clang-format on
  return table.value(value.toLower(), AgeRating::Unknown);
}

FuzzyDate parseFuzzyDate(const QString& value) {
  // YYYY-MM-DD
  if (value.size() >= 10) return FuzzyDate(value.toStdString());
  // YYYY-MM
  if (value.size() == 7) return FuzzyDate(u"%1-00"_s.arg(value).toStdString());
  // YYYY
  if (value.size() == 4) return FuzzyDate(u"%1-00-00"_s.arg(value).toStdString());
  return FuzzyDate{};
}

int parseEpisodeLength(int value) {
  const auto seconds = std::chrono::seconds{value};
  return std::chrono::duration_cast<std::chrono::minutes>(seconds).count();
}

anime::Status parseStatus(const QString& value) {
  using anime::Status;
  static const QMap<QString, Status> table{
      {"currently_airing", Status::Airing},
      {"finished_airing", Status::FinishedAiring},
      {"not_yet_aired", Status::NotYetAired},
  };
  return table.value(value.toLower(), Status::Unknown);
}

anime::Type parseType(const QString& value) {
  using anime::Type;
  // clang-format off
  static const QMap<QString, Type> table{
    {"unknown", Type::Unknown},
    {"tv", Type::Tv},
    {"ova", Type::Ova},
    {"movie", Type::Movie},
    {"special", Type::Special},
    {"ona", Type::Ona},
    {"music", Type::Music},
    {"cm", Type::Special},
    {"pv", Type::Special},
    {"tv_special", Type::Special},
  };
  // clang-format on
  return table.value(value.toLower(), Type::Unknown);
}

std::time_t parseListLastUpdated(const QString& value) {
  return QDateTime::fromString(value, Qt::DateFormat::ISODate).toSecsSinceEpoch();
}

int parseListScore(int value) {
  return value * (anime::list::kScoreMax / 10);
}

anime::list::Status parseListStatus(const QString& value) {
  using anime::list::Status;
  static const QMap<QString, Status> table{
      {"watching", Status::Watching},
      {"completed", Status::Completed},
      {"on_hold", Status::OnHold},
      {"dropped", Status::Dropped},
      {"plan_to_watch", Status::PlanToWatch},
  };
  return table.value(value.toLower(), Status::NotInList);
}

}  // namespace sync::myanimelist
