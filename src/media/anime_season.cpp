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

#include "anime_season.hpp"

#include <chrono>

namespace anime {

using enum anime::SeasonName;

namespace season {

using Month = std::chrono::month;

constexpr SeasonName from_month(const Month month) noexcept {
  // clang-format off
  switch (static_cast<unsigned int>(month)) {
    default: return Unknown;
    case 12: case  1: case  2: return Winter;
    case  3: case  4: case  5: return Spring;
    case  6: case  7: case  8: return Summer;
    case  9: case 10: case 11: return Fall;
  }
  // clang-format on
}

constexpr SeasonName next(const SeasonName name) noexcept {
  // clang-format off
  switch (name) {
    default:     return Unknown;
    case Winter: return Spring;
    case Spring: return Summer;
    case Summer: return Fall;
    case Fall:   return Winter;
  }
  // clang-format on
}

constexpr SeasonName prev(const SeasonName name) noexcept {
  // clang-format off
  switch (name) {
    default:     return Unknown;
    case Winter: return Fall;
    case Spring: return Winter;
    case Summer: return Spring;
    case Fall:   return Summer;
  }
  // clang-format on
}

constexpr std::pair<Month, Month> to_months(const SeasonName name) noexcept {
  using namespace std::chrono;

  // clang-format off
  switch (name) {
    default:     return {};
    case Winter: return {December, February};
    case Spring: return {March, May};
    case Summer: return {June, August};
    case Fall:   return {September, November};
  }
  // clang-format on
}

std::pair<Date, Date> to_date_range(const Season& season) noexcept {
  if (!season) return {};

  const auto months = season::to_months(season.name);

  const Date date_first{
      season.name == Winter ? season.year - std::chrono::years{1} : season.year,
      months.first,
      std::chrono::day{1},
  };

  const Date date_last =
      std::chrono::year_month_day_last{season.year, std::chrono::month_day_last(months.second)};

  return {date_first, date_last};
}

}  // namespace season

////////////////////////////////////////////////////////////////////////////////

Season::Season(SeasonName name, std::chrono::year year) : name{name}, year{year} {}

Season::Season(const Date& date) : Season{FuzzyDate{date}} {}

Season::Season(const FuzzyDate& date) {
  if (date.month()) {
    name = season::from_month(std::chrono::month{date.month()});
  }

  if (date.year()) {
    year = std::chrono::year{date.year()};
    if (date.month() && std::chrono::month{date.month()} == std::chrono::December) {
      ++year;  // e.g. December 2018 -> Winter 2019
    }
  }
}

Season::operator bool() const {
  return name != Unknown && static_cast<int>(year);
}

Season& Season::operator++() {
  name = season::next(name);
  if (name == Winter) ++year;
  return *this;
}

Season& Season::operator--() {
  name = season::prev(name);
  if (name == Fall) --year;
  return *this;
}

std::strong_ordering Season::operator<=>(const Season& season) const {
  using cmp = std::strong_ordering;

  if (year != season.year) {
    if (!static_cast<int>(year)) return cmp::greater;
    if (!static_cast<int>(season.year)) return cmp::less;
    return year < season.year ? cmp::less : cmp::greater;
  }

  if (name != season.name) {
    if (name == Unknown) return cmp::greater;  // Unknown seasons are in the future
    if (season.name == Unknown) return cmp::less;
    return name < season.name ? cmp::less : cmp::greater;
  }

  return cmp::equal;
}

}  // namespace anime
