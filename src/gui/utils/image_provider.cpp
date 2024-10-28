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

#include "image_provider.hpp"

#include <QImage>
#include <QImageReader>

#include "base/string.hpp"
#include "taiga/path.hpp"

namespace gui {

const QPixmap& ImageProvider::loadPoster(int id) {
  if (const auto it = m_pixmaps.find(id); it != m_pixmaps.end()) {
    return it.value();
  }

  const auto path = QString::fromStdString(taiga::get_data_path());
  QImageReader reader(u"%1/v1/db/image/%2.jpg"_s.arg(path).arg(id));
  const QImage image = reader.read();

  m_pixmaps[id] = !image.isNull() ? QPixmap::fromImage(image) : QPixmap{};

  return m_pixmaps[id];
}

void ImageProvider::reloadPoster(int id) {
  m_pixmaps.remove(id);
  loadPoster(id);
  emit posterChanged(id);
}

}  // namespace gui
