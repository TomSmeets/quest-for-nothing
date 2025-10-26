set -euo pipefail

echo $1

rm -vrf out/release
mkdir -p out/release

./build build src/qfn/qfn.c out/release/quest_for_nothing.elf  release linux
./build build src/qfn/qfn.c out/release/quest_for_nothing.exe  release windows
./build build src/qfn/qfn.c out/release/quest_for_nothing.wasm release wasm

# Download SDL3
if [ ! -f out/SDL3.dll ]; then
  curl -L -o out/SDL3.zip https://github.com/libsdl-org/SDL/releases/download/release-3.2.16/SDL3-3.2.16-win32-x64.zip
  ( cd out/ && unzip SDL3.zip SDL3.dll )
fi

cp out/SDL3.dll       out/release/SDL3.dll
cp src/lib/*.js       out/release/
cp src/gfx/*.js       out/release/
cp src/qfn/index.html out/release/index.html

# Publish release
# butler push out/release tsmeets/quest-for-nothing:release --userversion $(date +'%F')
# butler push out/release tsmeets/quest-for-nothing:release-web --userversion $(date +'%F')
# rclone copy out/release fastmail:tsmeets.fastmail.com/files/tsmeets.nl/qfn/
