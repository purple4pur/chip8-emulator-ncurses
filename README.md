# Chip-8 Emulator in Terminal

参考 <https://austinmorlan.com/posts/chip8_emulator/> 实现的 Chip-8 模拟器。

主要不同点：

- 在 terminal 中运行（使用 ncurses 库）
- 增加了 ROM 的选择页，并自带了一些 ROM 文件
- （可能）更详细的注释

ROM 来自于 [corax89/chip8-test-rom](https://github.com/corax89/chip8-test-rom) 和 [dmatlack/chip8](https://github.com/dmatlack/chip8/tree/master/roms)，版权归原作者。

## Build and Run

> 编译需要用到 ncurses 库和支持 c++17 的 g++

Tested on Linux 20.04:

```
$ sudo apt install g++ libncurses5-dev
$ make run
```

`make help` 查看帮助菜单

## Usage

1. 运行时可以指定每个 cycle 的时延（即模拟器速度），默认为 3ms，然而实际上速度也受限于终端打印字符的速度

```
$ ./chip8_emulator [delay_in_ms]
  or
$ make run t=[delay_in_ms]
```

2. 终端至少需要 H64 * W32 的大小，为了更好的显示效果，可以适当减小行间距
3. 运行中任何时候都可以按 ESC 退出
4. 在 ROM 选择界面用上下键选择、 ENTER 确认
5. 按键映射沿用了所参考网页的配置，如下：

```
 Chip-8       KeyBoard
|1|2|3|C|     |1|2|3|4|
|4|5|6|D| ==> |Q|W|E|R|
|7|8|9|E|     |A|S|D|F|
|A|0|B|F|     |Z|X|C|V|
```
