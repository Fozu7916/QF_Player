# QF_player — Музыкальный плеер на Qt

![Qt](https://img.shields.io/badge/Qt-6.x-green)
![C++](https://img.shields.io/badge/C%2B%2B-17-blue)

## Описание

TextRedactor — это простой, но функциональный музыкальный плеер на Qt с поддержкой mpv. Приложение позволяет:
- Добавлять и удалять треки
- Воспроизводить, ставить на паузу, перематывать и переключать треки
- Управлять громкостью
- Сохранять и загружать плейлист между сессиями
- Автоматически подстраивать интерфейс под размер окна

## Скриншот
![Скриншот](https://github.com/user-attachments/assets/d4934e01-f8f6-4576-bd9a-130eba39b1db)
)

## Как собрать

1. Установите Qt 6.x и CMake
2. Клонируйте репозиторий:
   ```bash
   git clone https://github.com/yourname/TextRedactor.git
   cd TextRedactor
   ```
3. Убедитесь, что в папке `mpv/` лежат файлы `libmpv-2.dll` и `libmpv.dll.a`
4. Соберите проект:
   ```bash
   mkdir build
   cd build
   cmake .. -G "Ninja Multi-Config" -DCMAKE_BUILD_TYPE=Release
   cmake --build . --config Release
   ```
5. Запустите `TextRedactor.exe`

## Зависимости
- [Qt 6.x](https://www.qt.io/download)
- [mpv](https://mpv.io/)

## Структура проекта
```
TextRedactor/
├── src/
│   ├── model/         # Track, TrackListModel
│   ├── view/          # MainWindow, делегаты
│   ├── controller/    # PlayerController
│   └── player/        # Player (mpv)
├── mpv/               # mpv dll и заголовки
├── CMakeLists.txt
├── README.md
└── ...
```

## Тестирование

Тесты находятся в папке `tests/` и написаны с использованием Qt Test.

### Как запустить тесты

1. Соберите проект с помощью CMake, как указано выше.
2. В папке `build/tests/` (или аналогичной для вашей сборки) будут созданы исполняемые файлы тестов:
   - `test_player.exe`
   - `test_playercontroller.exe`
   - `test_track.exe`
3. Запустите каждый тестовый исполняемый файл вручную или с помощью скрипта:
   ```bash
   ./test_player.exe
   ./test_playercontroller.exe
   ./test_track.exe
   ```
   или на Linux/Mac:
   ```bash
   ./test_player
   ./test_playercontroller
   ./test_track
   ```

## Лицензия

Этот проект распространяется под лицензией MIT. См. файл [LICENSE](LICENSE) для подробностей.


