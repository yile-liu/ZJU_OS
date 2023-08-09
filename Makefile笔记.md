# .PHONY

```css
clean:
    rm -f *.o temp
```

因为rm命令不产生任何clean文件。

- 如果目录下不存在名为"clean"的文件，则运行`make clean`时，命令都会正常执行；
- **如果目录下存在名为"clean"的文件，则运行`make clean`时，命令规则会失效**，因为"clean"没有依赖文件，makefile会认为当前名为"clean"的文件是“最新的”，于是`make clean`不会被执行。

为了解决上述问题，可以使用`.PHONY clean`指明该clean命令。如下：

```css
.PHONY clean
clean:
    rm -f *.o temp
```

这样执行命令`make clean`会自动忽略名为"clean"文件的存在。**`.PHONY`配置项的目标并不是其他文件生成的实际文件，make命令会自动绕过隐含规则搜索过程**，因此声明.PHONY会改善性能，且不需要担心实际同名文件存在与否。