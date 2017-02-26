
# Auto Compile

将cpp文件复制到`/content/`下

打开`VS2012开发人员命令提示`，转到当前文件夹。

使用debug模式编译，则先建立 `/debug/`,`/compiled_debug/`两个文件夹，然后运行`/COMPILE_DEBUG`。结果在`/compiled_debug/`内。

使用release模式编译，则先建立 `/release/`,`/compiled_release/`两个文件夹，然后运行`/COMPILE_RELEASE`。结果在`/compiled_release/`内。
