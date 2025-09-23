#!/usr/bin/env bash
set -euo pipefail

# Папка, куда складываем библиотеки и заголовки
OUT_DIR="${1:-mpv}"
LIB_DIR="$OUT_DIR/lib"
INCLUDE_DIR="$OUT_DIR/include"

echo "Создаём папки..."
mkdir -p "$LIB_DIR"
mkdir -p "$INCLUDE_DIR"

# Проверяем, установлен ли mpv
if ! command -v mpv >/dev/null 2>&1; then
    echo "mpv не найден, устанавливаем через pacman..."
    sudo pacman -Syu --needed mpv
fi

# Копируем динамическую библиотеку libmpv
MPV_LIB=$(find /usr/lib -maxdepth 1 -type f -name "libmpv.so*" | head -n1)
if [ -z "$MPV_LIB" ]; then
    echo "libmpv.so не найдена в /usr/lib"
    exit 1
fi
cp "$MPV_LIB" "$LIB_DIR/"

# Копируем заголовки C/C++ mpv
if [ -d "/usr/include/mpv" ]; then
    cp -r /usr/include/mpv/* "$INCLUDE_DIR/"
else
    echo "Заголовки mpv не найдены в /usr/include/mpv"
    exit 1
fi

echo "Готово! libmpv и заголовки находятся в $OUT_DIR"
