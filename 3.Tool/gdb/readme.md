# gdb + vscode

安装必要的扩展：在 VS Code 中安装 Remote Development 扩展，以便连接到远程 Linux 服务器。你也可以安装 C/C++ 扩展，以便在 VS Code 中编写和调试 C 代码。

连接到远程 Linux：使用 Remote Development 扩展连接到远程 Linux 服务器。你可以在 VS Code 的命令面板中输入 Remote-SSH: Connect to Host，然后输入远程 Linux 的地址和凭据来连接。

打开 C 项目：在 VS Code 中打开远程 Linux 上的 C 项目文件夹。

配置调试器：在 VS Code 中打开需要调试的 C 文件，并确保已经配置了调试器。你可以按下 F5 键或者点击编辑器左侧的调试按钮，然后选择配置调试器。在配置文件中，选择 C/C++: (gdb) Attach to Remote 进行远程调试。

设置断点：在需要设置断点的代码行上点击左侧编辑器的行号区域，以在那里设置断点。

开始调试：按下 F5 键或者点击编辑器左侧的调试按钮来开始调试。VS Code 将连接到远程 Linux 服务器，并在断点处停止执行程序。

通过这些步骤，你可以在 VS Code 中远程调试 Linux 上的 C 代码，并设置断点来检查程序的执行情况。需要注意的是，你可能需要在远程 Linux 上安装并配置调试器，例如 GDB，以便 VS Code 正常进行远程调试。




**补充说明**
> Linux下还有很多工具
> 如果你正在寻找类似于 Keil 或者 IAR Embedded Workbench 的图形界面断点调试工具，用于嵌入式系统开发和调试，那么在 Linux 环境下，可能没有直接的替代品。
> 
