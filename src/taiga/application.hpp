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

#include <QApplication>
#include <QCommandLineParser>
#include <QPointer>
#include <QSharedMemory>

namespace gui {
class MainWindow;
}

namespace taiga {

class Application final : public QApplication {
  Q_OBJECT
  Q_DISABLE_COPY_MOVE(Application)

public:
  Application(int argc, char* argv[]);
  ~Application() override;

  int run();

  bool isDebug() const;
  bool isVerbose() const;

  gui::MainWindow* mainWindow() const;

private:
  bool hasPreviousInstance();
  void initLogger() const;
  void parseCommandLine();

  struct Options {
    bool debug = false;
    bool verbose = false;
  } options_;

  QCommandLineParser parser_;
  QSharedMemory shared_memory_;
  QPointer<gui::MainWindow> window_;
};

inline Application* app() {
  return static_cast<Application*>(qApp);
}

}  // namespace taiga
