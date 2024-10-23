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

#include "settings.hpp"

#include <QFile>
#include <QVariant>
#include <QXmlStreamReader>

namespace compat::v1 {

QVariantMap read_settings(const std::string& path) {
  QFile file(QString::fromStdString(path));

  if (!file.open(QIODevice::ReadOnly)) return {};

  QXmlStreamReader xml(&file);

  if (!xml.readNextStartElement()) return {};
  if (xml.name() != u"settings") return {};

  QVariantMap settings;
  QStringList libraryFolders;

  while (xml.readNextStartElement()) {
    if (xml.name() == u"account") {
      while (xml.readNextStartElement()) {
        if (xml.name() == u"update") {
          settings["service"] = xml.attributes().value(u"activeservice").toString();
        } else if (xml.name() == settings["service"].toString()) {
          settings["username"] = xml.attributes().value(u"username").toString();
        }
        xml.skipCurrentElement();
      }
    } else if (xml.name() == u"anime") {
      while (xml.readNextStartElement()) {
        if (xml.name() == u"folders") {
          while (xml.readNextStartElement()) {
            if (xml.name() == u"root") {
              libraryFolders.append(xml.attributes().value(u"folder").toString());
            }
            xml.skipCurrentElement();
          }
        }
        xml.skipCurrentElement();
      }
    } else {
      xml.skipCurrentElement();
    }
  }

  settings["library.folders"] = libraryFolders;

  return settings;
}

}  // namespace compat::v1
