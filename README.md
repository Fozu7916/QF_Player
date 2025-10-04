# QF_Player — Музыкальный плеер на Qt

![Qt](https://img.shields.io/badge/Qt-6.x-green)
![C++](https://img.shields.io/badge/C%2B%2B-17-blue)
![Windows](https://img.shields.io/badge/Windows-10%2B-blue)
![Linux](https://img.shields.io/badge/Linux-supported-green)

## Описание

QF_Player — это современный музыкальный плеер на Qt с поддержкой mpv. Приложение предоставляет:

### 🎵 Основные функции
- **Воспроизведение аудио**: поддержка MP3, WAV, FLAC и других форматов
- **Управление плейлистом**: добавление, удаление, переключение треков
- **Режим воспроизведения**: обычный и случайный порядок
- **Управление громкостью**: плавная регулировка от 0 до 100%
- **Сохранение состояния**: автоматическое сохранение плейлиста и настроек

### ⌨️ Системная интеграция
- **Медиа-клавиши Windows**: поддержка кнопок на клавиатуре и гарнитуре
- **Глобальные горячие клавиши**: работают даже когда окно не в фокусе
- **OSD уведомления**: визуальная обратная связь при нажатии медиа-клавиш
- **Автозагрузка**: восстановление последнего плейлиста при запуске

## Скриншот
<img width="659" height="832" alt="image" src="https://github.com/user-attachments/assets/7fa7e5b9-0736-42c8-8894-665916434e30" />


## 🚀 Быстрый старт

### Требования
- **Windows 10+** или **Linux** (Ubuntu 20.04+, Arch Linux)
- **Qt 6.x** с модулями: Widgets, Multimedia, Concurrent
- **CMake 3.16+**
- **mpv** (автоматическая установка через скрипты)

### Сборка

1. **Клонируйте репозиторий:**
   ```bash
   git clone https://github.com/yourname/TextRedactor.git
   cd TextRedactor
   ```

2. **Установите mpv зависимости:**
   
   **Windows (PowerShell):**
   ```powershell
   .\scripts\fetch_mpv_win.ps1
   ```
   
   **Linux (Ubuntu/Debian):**
   ```bash
   chmod +x scripts/fetch_mpv_linux.sh
   ./scripts/fetch_mpv_linux.sh
   ```
   
   **Linux (Arch/Manjaro):**
   ```bash
   chmod +x scripts/fetch_mpv_arch.sh
   ./scripts/fetch_mpv_arch.sh
   ```

3. **Соберите проект:**
   ```bash
   mkdir build && cd build
   cmake .. -G "Ninja Multi-Config" -DCMAKE_BUILD_TYPE=Release
   cmake --build . --config Release
   ```

4. **Запустите:**
   ```bash
   # Windows
   ./TextRedactor.exe
   
   # Linux
   ./TextRedactor
   ```

## 🎮 Управление

### Клавиатура и мышь
- **Добавить треки**: кнопка "Добавить" или перетаскивание файлов
- **Воспроизведение**: кнопка Play/Pause или пробел
- **Переключение треков**: кнопки "Следующий"/"Предыдущий"
- **Громкость**: ползунок громкости
- **Случайный режим**: переключатель "Random"

### Системные медиа-клавиши (Windows)
- **Play/Pause**: `VK_MEDIA_PLAY_PAUSE` (Fn+F11, кнопка на гарнитуре)
- **Следующий трек**: `VK_MEDIA_NEXT_TRACK` (Fn+F12)
- **Предыдущий трек**: `VK_MEDIA_PREV_TRACK` (Fn+F10)
- **Стоп**: `VK_MEDIA_STOP` (Fn+F9)

> 💡 **Совет**: Медиа-клавиши работают глобально, даже когда окно не в фокусе!

## 📁 Структура проекта

```
TextRedactor/
├── src/
│   ├── model/              # Модели данных
│   │   ├── track.h/cpp     # Класс трека
│   │   └── ...
│   ├── view/               # Пользовательский интерфейс
│   │   ├── mainwindow.h/cpp # Главное окно
│   │   └── mainwindow.ui   # UI файл
│   ├── controller/         # Бизнес-логика
│   │   ├── playercontroller.h/cpp    # Контроллер плеера
│   │   └── durationcontroller.h/cpp  # Получение длительности треков
│   ├── player/             # Плеер (mpv)
│   │   └── player.h/cpp
│   └── integration/        # Системная интеграция
│       └── mediaosd.h/cpp  # OSD уведомления
├── mpv/                    # mpv библиотеки и заголовки
├── resources/              # Ресурсы (изображения, иконки)
├── scripts/                # Скрипты установки зависимостей
├── tests/                  # Модульные тесты
├── CMakeLists.txt
└── README.md
```

## 🧪 Тестирование

Проект включает модульные тесты для всех основных компонентов:

### Запуск тестов

```bash
# После сборки проекта
cd build/tests

# Windows
./test_player.exe
./test_playercontroller.exe  
./test_track.exe

# Linux
./test_player
./test_playercontroller
./test_track
```

### Покрытие тестами
- ✅ **Player**: воспроизведение, пауза, громкость, позиция
- ✅ **PlayerController**: управление плейлистом, переключение треков
- ✅ **Track**: создание и валидация треков

## 🔧 Технические детали

### Архитектура
- **MVC паттерн**: четкое разделение Model-View-Controller
- **Qt сигналы/слоты**: асинхронная коммуникация между компонентами
- **Многопоточность**: получение длительности треков в отдельном потоке
- **Системная интеграция**: глобальные горячие клавиши Windows

### Производительность
- **Потокобезопасность**: корректная работа с Qt потоками
- **Память**: автоматическое управление через smart pointers
- **mpv интеграция**: эффективное воспроизведение через libmpv

### Скрипты установки mpv

| Дистрибутив | Скрипт | Метод установки |
|-------------|--------|-----------------|
| **Windows** | `fetch_mpv_win.ps1` | Скачивание готовых DLL из mpv-winbuild |
| **Ubuntu/Debian** | `fetch_mpv_linux.sh` | Установка через `apt-get` (libmpv-dev) |
| **Arch/Manjaro** | `fetch_mpv_arch.sh` | Установка через `pacman` + копирование библиотек |

> 💡 **Совет**: Если ваш дистрибутив не поддерживается, установите `libmpv-dev` вручную

## 📄 Лицензия

Этот проект распространяется под лицензией MIT. См. файл [LICENSE](LICENSE) для подробностей.

---

**Создано с ❤️ на Qt и C++**


