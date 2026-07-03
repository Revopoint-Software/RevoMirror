<div align="center">
  <img src="RevoMirror.png" alt="RevoMirror icon" width="120"/>
  <h1 align="center">RevoMirror</h1>
  <h4 align="center">跨平台远程投屏与 PC 远程控制工具（局域网内）</h4>
</div>

<div align="center">
  <a href="https://github.com/Revopoint-Software/RevoMirror/stargazers"><img src="https://img.shields.io/github/stars/Revopoint-Software/RevoMirror.svg?logo=github&style=for-the-badge" alt="GitHub stars"></a>
  <a href="https://github.com/Revopoint-Software/RevoMirror/releases/latest"><img src="https://img.shields.io/github/downloads/Revopoint-Software/RevoMirror/total.svg?style=for-the-badge&logo=github" alt="GitHub Releases"></a>
  <a href="https://github.com/Revopoint-Software/RevoMirror/actions"><img src="https://img.shields.io/github/actions/workflow/status/Revopoint-Software/RevoMirror/ci.yml.svg?branch=main&label=CI%20build&logo=github&style=for-the-badge" alt="CI"></a>
  <a href="https://github.com/Revopoint-Software/RevoMirror/blob/main/LICENSE"><img src="https://img.shields.io/github/license/Revopoint-Software/RevoMirror.svg?style=for-the-badge" alt="License"></a>
</div>

<div align="center">
  <a href="README.md">English</a> | <b>简体中文</b>
</div>

## ℹ️ 项目简介

**RevoMirror** 是由 **Revopoint Software（西安知象光电科技有限公司）** 开发的跨平台远程投屏与远程控制解决方案。它基于开源项目 [Sunshine](https://github.com/LizardByte/Sunshine)（遵循 GPL-3.0 许可证）二次开发，充分利用其高性能、低延迟的串流能力。

RevoMirror 的核心功能包括：

- 🖥️ **屏幕投屏** —— 将 PC 桌面实时投屏到移动设备。
- 🎮 **远程控制** —— 通过移动设备直接控制 PC 桌面，实现对主机的完整远程操作。
- 🌐 **局域网互联** —— 需在同一局域网（LAN）或同一网段内使用，保证低延迟与连接安全。

通过集成 Sunshine 的硬件加速编码与屏幕采集管线，RevoMirror 为多平台设备提供流畅、响应迅速的远程桌面体验。

> **说明：** RevoMirror 是基于 Sunshine 的衍生项目。核心串流引擎由 Sunshine 提供，RevoMirror 应用（位于 `tools/RevoMirror` 目录）在此基础上增加了投屏、远程控制及跨平台客户端能力。

## 🚀 主要特性

- **跨平台支持**
  - 主机端（被控/被投屏方）：Windows、macOS、Linux
  - 客户端（观看/控制方）：Windows、macOS、Linux、Android、iOS
- **PC → 移动端投屏** —— 将 PC 桌面画面投送至移动设备。
- **移动端 → PC 远程控制** —— 通过移动设备远程操作 PC，支持键盘、鼠标及触控输入。
- **低延迟** —— 采用硬件加速视频编码，接近实时的操作体验。
- **同网段运行** —— 专为同一局域网 / 同一网段场景设计。
- **多 GPU 编码** —— 支持 AMD、Intel、NVIDIA 硬件编码，并提供软件编码兜底（继承自 Sunshine）。

## 🧩 工作原理

RevoMirror 集成于 Sunshine 源码树的 `tools/RevoMirror` 目录中，复用了 Sunshine 的以下能力：

- **屏幕采集** 管线（DXGI、ScreenCaptureKit、KMS/DRM、X11、Wayland 等）
- **硬件编码** 接口（NVENC、QuickSync、AMF、VideoToolbox、VAAPI 等）
- **串流协议**，实现低延迟视频传输

在此基础上，RevoMirror 实现了投屏流程以及面向移动端和桌面端客户端的双向远程控制输入处理。

## 🖥️ 平台支持

| 角色 | Windows | macOS | Linux | Android | iOS |
|------|:-------:|:-----:|:-----:|:-------:|:---:|
| 主机端（被投屏 / 被控制） | ✅ | ✅ | ✅ | ➖ | ➖ |
| 客户端（观看 / 控制） | ✅ | ✅ | ✅ | ✅ | ✅ |

**图例：** ✅ 支持 ｜ ➖ 不适用

## 🎮 编码与采集兼容性

RevoMirror 继承了 Sunshine 的编码与屏幕采集能力。

<table>
    <caption id="encoding_api">编码 API</caption>
    <tr>
        <th>编码 API</th>
        <th>GPU 厂商</th>
        <th>Linux</th>
        <th>macOS</th>
        <th>Windows</th>
    </tr>
    <tr><td>AMF</td><td>AMD</td><td>➖</td><td>➖</td><td>✅</td></tr>
    <tr><td>NVENC</td><td>NVIDIA</td><td>✅</td><td>➖</td><td>✅</td></tr>
    <tr><td>QuickSync</td><td>Intel</td><td>➖</td><td>➖</td><td>✅</td></tr>
    <tr><td>VAAPI</td><td>AMD / Intel / NVIDIA</td><td>✅</td><td>➖</td><td>➖</td></tr>
    <tr><td>Video Toolbox</td><td>Apple / Intel</td><td>➖</td><td>✅</td><td>➖</td></tr>
    <tr><td>软件编码</td><td>任意</td><td>✅</td><td>✅</td><td>✅</td></tr>
</table>

<table>
    <caption id="screen_capture">屏幕采集</caption>
    <tr>
        <th>采集方式</th>
        <th>Linux</th>
        <th>macOS</th>
        <th>Windows</th>
    </tr>
    <tr><td>DXGI Desktop Duplication</td><td>➖</td><td>➖</td><td>✅</td></tr>
    <tr><td>Windows.Graphics.Capture</td><td>➖</td><td>➖</td><td>✅</td></tr>
    <tr><td>ScreenCaptureKit</td><td>➖</td><td>✅</td><td>➖</td></tr>
    <tr><td>KMS/DRM</td><td>✅</td><td>➖</td><td>➖</td></tr>
    <tr><td>X11</td><td>✅</td><td>➖</td><td>➖</td></tr>
    <tr><td>Wayland (wlroots)</td><td>✅</td><td>➖</td><td>➖</td></tr>
    <tr><td>KWin Screencast</td><td>✅</td><td>➖</td><td>➖</td></tr>
</table>

**图例：** ✅ 支持 ｜ 🟡 部分支持 ｜ ❌ 暂不支持 ｜ ➖ 不适用

## 🖥️ 系统要求

> [!WARNING]
> 以下表格仍在完善中，请勿仅依据此信息选购硬件。相关要求继承自 Sunshine。

<table>
    <caption id="minimum_requirements">最低配置要求（主机端）</caption>
    <tr>
        <th>组件</th>
        <th>要求</th>
    </tr>
    <tr>
        <td rowspan="3">GPU</td>
        <td>AMD：VCE 1.0 或更高</td>
    </tr>
    <tr>
        <td>Intel：Windows 需 Skylake 或更新且支持 QuickSync；Linux 需兼容 VAAPI</td>
    </tr>
    <tr>
        <td>NVIDIA：支持 NVENC 的显卡</td>
    </tr>
    <tr>
        <td rowspan="2">CPU</td>
        <td>AMD：Ryzen 3 或更高</td>
    </tr>
    <tr>
        <td>Intel：Core i3 或更高</td>
    </tr>
    <tr>
        <td>内存</td>
        <td>4GB 或以上</td>
    </tr>
    <tr>
        <td rowspan="4">操作系统</td>
        <td>Windows：11 及以上</td>
    </tr>
    <tr>
        <td>macOS：14.2 及以上</td>
    </tr>
    <tr>
        <td>Linux/Ubuntu：22.04+（jammy）</td>
    </tr>
    <tr>
        <td>Linux/Fedora：43+ / Debian：13+</td>
    </tr>
    <tr>
        <td rowspan="2">网络</td>
        <td>主机端：5GHz 802.11ac 无线或有线（需与客户端处于同一局域网/网段）</td>
    </tr>
    <tr>
        <td>客户端：5GHz 802.11ac 无线或有线（需与主机端处于同一局域网/网段）</td>
    </tr>
</table>

> 关于 4K 与 HDR 的硬件建议，请参考 [Sunshine 官方文档](https://docs.lizardbyte.dev/projects/sunshine)，RevoMirror 的底层编码要求与其一致。

## 📦 快速开始

> [!NOTE]
> 详细的编译与使用说明请参见 [docs](docs/getting_started.md) 目录。

1. 确保 **主机 PC** 与 **客户端设备** 处于 **同一局域网或同一网段**。
2. 在主机 PC 上安装并运行 RevoMirror。
3. 在你的设备上安装 RevoMirror 客户端（Windows / macOS / Linux / Android / iOS）。
4. 将客户端与主机进行配对。
5. 开始投屏和/或远程控制。

## 📖 文档

- 快速开始：[docs/getting_started.md](docs/getting_started.md)
- 底层引擎相关细节，请参考 [Sunshine 官方文档](https://docs.lizardbyte.dev/projects/sunshine)。

## 📜 许可证

RevoMirror 遵循 **GNU 通用公共许可证 v3.0（GPL-3.0）**。

本项目是基于 [Sunshine](https://github.com/LizardByte/Sunshine) 的衍生作品，Sunshine 同样遵循 GPL-3.0 许可证。根据 GPL 的相关条款，RevoMirror 以相同许可证进行分发。

完整许可证文本请参见 [LICENSE](./LICENSE) 文件。

## 🙏 致谢与声明

RevoMirror 基于以下优秀的开源项目构建：

- **[Sunshine](https://github.com/LizardByte/Sunshine)** —— 面向 Moonlight 的自托管游戏串流主机。
  版权所有 © LizardByte，遵循 GPL-3.0 许可证。

在此向 LizardByte 团队及所有 Sunshine 贡献者致以诚挚的感谢。本仓库完整保留了 Sunshine 的原始许可证与版权声明。

## 🏢 关于 Revopoint Software

RevoMirror 由 **Revopoint Software（西安知象光电科技有限公司，Xi'an Chishine Optoelectronics Technology Co., Ltd.）** 开发并维护。
