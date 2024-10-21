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

#include "list.hpp"

#include <QFile>
#include <QXmlStreamReader>

#include "compat/common.hpp"

namespace compat::v1 {

QList<ListEntry> read_list_entries(const std::string& path) {
  QFile file(QString::fromStdString(path));

  if (!file.open(QIODevice::ReadOnly)) return {};

  QString str{file.readAll()};
  str.remove(meta_element_regex);

  QXmlStreamReader xml(str);

  if (!xml.readNextStartElement()) return {};
  if (xml.name() != u"library") return {};

  QList<ListEntry> entries;

  while (xml.readNextStartElement()) {
    if (xml.name() != u"anime") break;

    ListEntry entry;

    while (xml.readNextStartElement()) {
      if (xml.name() == u"id") {
        entry.anime_id = xml.readElementText().toInt();
      } else if (xml.name() == u"library_id") {
        entry.id = xml.readElementText().toStdString();
      } else if (xml.name() == u"progress") {
        entry.watched_episodes = xml.readElementText().toInt();
      } else if (xml.name() == u"date_start") {
        entry.date_started = FuzzyDate(xml.readElementText().toStdString());
      } else if (xml.name() == u"date_end") {
        entry.date_completed = FuzzyDate(xml.readElementText().toStdString());
      } else if (xml.name() == u"score") {
        entry.score = xml.readElementText().toInt();
      } else if (xml.name() == u"status") {
        entry.status = static_cast<anime::list::Status>(xml.readElementText().toInt());
      } else if (xml.name() == u"private") {
        entry.is_private = xml.readElementText().toInt();
      } else if (xml.name() == u"rewatched_times") {
        entry.rewatched_times = xml.readElementText().toInt();
      } else if (xml.name() == u"rewatching") {
        entry.rewatching = xml.readElementText().toInt();
      } else if (xml.name() == u"rewatching_ep") {
        entry.rewatching_ep = xml.readElementText().toInt();
      } else if (xml.name() == u"notes") {
        entry.notes = xml.readElementText().toStdString();
      } else if (xml.name() == u"last_updated") {
        entry.last_updated = xml.readElementText().toLongLong();
      } else {
        xml.skipCurrentElement();
      }
    }

    entries.emplace_back(entry);
  }

  return entries;
}

}  // namespace compat::v1
