# Modifications to Sunshine

RevoMirror-PC is a derivative work based on
[Sunshine](https://github.com/LizardByte/Sunshine) (licensed under GPL-3.0).

This document records the modifications made to the original Sunshine source
code, in compliance with Section 5 of the GNU General Public License v3.0,
which requires modified versions to carry prominent notices stating that the
files were changed and the date of any change.

> **Note:** The RevoMirror-PC application code located in `tools/RevoMirror` is
> new code added by Revopoint Software, not a modification of existing Sunshine
> files. This document focuses on changes made to the **original Sunshine
> source files**.

---

## Base Version

- **Upstream project:** Sunshine (LizardByte)
- **Based on version / commit:** `Commits on Jun 29, 2026(#5354)` 
- **Fork date:** 2026-06-29

---

## Summary of Modifications

| Date | Modified File(s) | Description of Change | Reason / Intent |
|------|------------------|-----------------------|-----------------|
| 2026-10-15 | src/main.cpp | 添加权限申请处理 | 处理使用中部分权限申请要重启才生效问题 |
| 2026-08-22 | camke/packaging/windows_innosetup.cmake | 支持win平台下innosetup打包 | 支持win下更丰富的打包样式 |
| 2026-08-22 | src/file_handler.cpp |处理安装存在中文路径时sunshine打开程序报文件目录异常失败问题 | 修复文件路径sunshine启动失败问题 |
| 2026-08-22 | src/confighttp.cpp |添加退出和连接状态获取的http接口/隐藏托盘和启动webserver的命令行接口 | 添加通过http获取退出和连接状态共功能 |

<!-- 按实际修改逐条填写 -->

---

## Detailed Notes

### 1、src/confighttp.cpp
- **改动内容：** 添加退出/连接状态获取的http接口和隐藏托盘和启动webserver的命令（新增quit、getSessionState接口）
- **改动原因：** 需要通过http请求实时获取连接状态和响应退出请求和新增通过QT的http客户端代替webserver，不用在启动webserver
- **影响范围：** 新增功能，不影响原有功能

### 2、sr/file_handler.cpp
- **改动内容：** 处理中文路径下配置文件读取崩溃问题（新增read_json、write_json、exists接口，修改make_directory、read_file、write_file接口对路径的处理）
- **改动原因：** 软件读取程序安装目录下的配置文件时，如果是中文路径，会闪退
- **影响范围：** 影响之前的配置文件读写功能

### 3、src/main.cpp
- **改动内容：** 添加mac平台启动权限申请和循环检测（在main中添加mac平台启动需要权限申请功能）
- **改动原因：** 之前权限是运行中动态申请，部分权限申请后不生效，重启后才生效，改成启动之初就申请所有需要权限
- **影响范围：** 影响mac平台权限申请功能

### 4、camke/packaging/windows_innosetup.cmake
- **改动内容：** 新增win平台innosetup方式打包（支持更丰富的自定义打包样式）
- **改动原因：** 之前的nins打包可自定义内容太少
- **影响范围：** 新增打包方式，不影响之前功能


---

## Copyright of Modifications

All modifications described above are:

Copyright (C) 2026 Revopoint Software
(Xi'an Chishine Optoelectronics Technology Co., Ltd.)

These modifications are licensed under the GNU General Public License v3.0,
consistent with the license of the original Sunshine project.
