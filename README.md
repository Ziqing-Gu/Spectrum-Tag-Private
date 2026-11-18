# Spectrum Tag（私人） · 子青版

## 产品介绍 / About SpectrumTag

基于 [sweetorange1 / SpectrumTag](https://github.com/sweetorange1/SpectrumTag)，原作者 **iisaacbeats.cn**。本仓库是 **Ziqing-Gu 维护的 Fork**，保留原项目署名与 AGPL-3.0 许可。

Based on [sweetorange1 / SpectrumTag](https://github.com/sweetorange1/SpectrumTag), originally by **iisaacbeats.cn**. This is the **Ziqing-Gu fork**, retaining the original attribution and AGPL-3.0 license.

### 这是什么？

SpectrumTag 将 Logo 转换为黑白掩模，或将复杂图像转换为灰度掩模，并通过处理音频的频率成分，在频谱中留下图形。可以实时试听，也可以在录制或加载音频后编辑 Print，再导出处理结果。独立版还支持直接在空白时间轴上打印，并导出声音或纯频谱动画。

### 子青版图片编辑

右键切换原图显隐。在原图上拖动参数时显示预测频谱，松手恢复黑白或灰度图。在已打印图案上调整参数时保留预测，点击「打印」才写入音频。可在渲染过程中取消。

### 三个版本怎么选？

| 版本 | 适合的工作方式 |
| --- | --- |
| **实时 VST3 / AU** | 插入 DAW 轨道，边播放边体验 Print。Record 后可在录制的时间线上编辑、删除、撤销 Print；Clear Record 返回未录制模式。 |
| **ARA** | 从支持 ARA 的宿主入口使用，Load 获取宿主音频，同轨事件保留工程位置。无录音按钮；支持请求宿主定位，Clear 后可重新加载。 |
| **独立版** | Load 或拖入音频/视频音轨，本地播放、Print 并另存导出。不需要 DAW，也可以不导入文件直接开始。 |

实时插件内部点击试听不会改变 DAW 指针。正常跟随宿主时，已录区域输出处理结果，未录区域保留宿主输入；内部独立试听进入未录空隙时静音，停止后重新跟随宿主。ARA 和独立版则使用各自的定位接口。

### 主要功能

- **图像转换**：自动阈值 Otsu、局部自适应、手动阈值和透明度剪影；支持黑白强度、反转及效果预览。
- **可编辑 Print**：移动、右下角等比例缩放、修改参数、删除、撤销与重做。拖动时先显示预览，点击打印后再后台计算；同位置重新 Print 不叠加加深，删除同时移除对应补偿噪声。
- **噪声补偿**：为静音或能量不足的频段合成目标频谱，补偿量可调，仅作用于 Print 范围。独立版空白打印需要提高 Noise Fill 才会生成声音。
- **频谱视图**：默认 Mel (RX)、20 kHz；提供 Linear、Bark、Log、Extended Log 和 insight。共用时间尺度，insight 不改变显示上限；实际频率受采样率限制。
- **操作与外观**：8 种记忆配色、中英文界面和悬停说明、原图显示/隐藏、录音/加载范围全览。原图始终保持正确长宽比。
- **恢复与缓存**：保存 Print 编辑及处理缓存；独立版可恢复上次会话。可用缓存能够避免重复渲染，缓存缺失或内容变化时会重新分析；无法恢复的媒体静默留空。

### 独立版的音视频导出

支持常见音频及 MP4、M4V、MOV、MKV、WebM、AVI 的音轨。导入视频后，可选择保留原画面、替换处理后的音轨，或仅导出音频。它不包含视频画面编辑器。原格式导出保留受支持的编码族和适合的容器；压缩音频会重新编码，不承诺与原文件逐字节相同。

Print 可以超出原始文件的开始或结尾，导出时长会相应延长。视频超出原范围的部分补黑画面，原音画保持同步。源文件不被覆盖。

**导出频谱视频**生成满屏纯频谱，无标题、刻度和时间文字，从最右侧逐渐显现并向左滚动。画面按 Tag 的时间、比例和频率刻度平滑调整，声音保持正常速度；重叠 Tag 优先跟随较早开始者。可选 MP4/MOV/MKV/WebM、30/60 fps，提供进度弹窗及取消。1.4.28 新增 4:3、3:4、16:9、9:16；分辨率按短边 720/1080/1440/2160 四档联动，菜单显示实际宽×高，默认 16:9 并记住上次选择。导出默认打开桌面目录，也可自行选择位置。

### 常用操作

| 操作 | 功能 |
| --- | --- |
| 鼠标滚轮 | 左右平移频谱 |
| Ctrl / Shift + 滚轮 | 调整横向视图缩放；选中 Print 时只改变视图，不重新渲染 |
| 选中 Print 后拖动 Speed 拉杆 | 实时预览打印时长，点击「打印」后应用 |
| 单击已打印图形 | 选中并恢复其参数和正常显示比例，不移动播放指针 |
| 拖动已选 Print / 右下角 | 移动位置 / 等比例缩放 |
| 双击原图 | 选择新图片 |
| 频谱右键 / 眼睛图标 | 显示或隐藏原图 |
| 频谱右上角全览图标 | 适应已录制/加载的范围 |
| Delete | 删除选中的 Print |
| Ctrl+Z / Ctrl+Shift+Z | 撤销 / 重做 |
| Shift 拖动参数 / Alt+左键 | 微调 / 恢复默认值 |

### What is SpectrumTag?

SpectrumTag converts an image to a monochrome mask and processes audio frequencies so the image appears in a spectrogram. Use it as a realtime effect, edit Prints in recorded or loaded audio, or start with an empty standalone timeline and export sound or a spectrum animation.

### Ziqing image editing

Right-click to toggle the source. While it is visible, drag a control for a spectrum prediction and release to return to the converted artwork. With the source hidden, changes appear on the selected printed pattern. Click Print to commit. You can cancel a render in progress.

### Choose your edition

| Edition | Workflow |
| --- | --- |
| **Realtime VST3 / AU** | Print while your DAW plays. Record captures a timeline for editable Prints; Clear Record returns to live operation. |
| **ARA** | Load host-provided audio with same-track events at their project positions. No Record button; host seeking and Clear/reload are available. |
| **Standalone** | Load or drop audio/video soundtracks, play locally, edit and export. No DAW or source file is required to begin. |

Realtime internal audition does not move the DAW cursor. When following the host, recorded areas play processed audio and unrecorded areas pass through the host input. Independent internal audition is silent in unrecorded gaps; stopping restores host follow. ARA and standalone use their own seeking interfaces.

### Features

- **Image conversion:** Otsu, adaptive, manual threshold and alpha silhouette, with amount, invert and processing previews.
- **Editable Prints:** move, resize proportionally from the corner, change parameters, delete, undo and redo. Edits preview immediately. Click Print to commit them to audio; printing again at the same position replaces the prior operation, and deletion removes its Noise Fill too.
- **Noise Fill:** adjustable target-spectrum synthesis for silent or weak frequency content, confined to the Print area. Source-free standalone Prints need nonzero Noise Fill to produce sound.
- **Spectrum display:** Mel (RX) and 20 kHz by default; Linear, Bark, Log, Extended Log and insight are available. All share a time scale; insight leaves Range unchanged, and the source sample rate limits usable frequencies.
- **Interface:** eight remembered palettes, Chinese/English controls and tooltips, source-image visibility and recorded/loaded-range overview. The original image retains its aspect ratio.
- **Persistence:** saved edits and content-checked render caches, plus standalone session restoration. Missing or changed caches require analysis; unavailable media restores to a blank workspace without a missing-file prompt.

### Standalone export

Load common audio formats or the soundtrack of MP4/M4V/MOV/MKV/WebM/AVI files. Export processed audio alone or preserve the original video with a replaced soundtrack. There is no video picture editor. Original-format export retains supported codec families and suitable containers; compressed audio is re-encoded and is not byte-identical to the original.

Prints may extend before or after the source. Export duration expands accordingly; video receives black padding while the original audio and picture stay aligned. Source files are not overwritten.

**Spectrum video export** fills the frame with the actual processed spectrum, without titles, axes or time labels. New content enters from the right edge and scrolls left. The view smoothly follows each Tag's time, proportions and frequency scale without changing audio playback speed; earlier-starting Tags take priority during overlaps. Choose MP4/MOV/MKV/WebM and 30/60 fps. Version 1.4.28 adds 4:3, 3:4, 16:9 and 9:16, with four short-edge quality levels (720/1080/1440/2160) and actual width-by-height dimensions in the menu. It defaults to 16:9 and remembers the last choice. A progress dialog supports cancellation. Export dialogs start at the Desktop and allow another destination.

### Controls

| Action | Result |
| --- | --- |
| Mouse wheel | Pan horizontally |
| Ctrl / Shift + wheel | Change horizontal view zoom; selected Prints do not re-render |
| Drag Speed with a Print selected | Preview the new duration. Click Print to apply |
| Click a printed shape | Select it and recall its parameters and normal display proportions without seeking |
| Drag a selected Print / its lower-right corner | Move / resize proportionally |
| Double-click the original image | Choose another image |
| Right-click the spectrum / eye icon | Toggle the original image |
| Top-right overview icon | Fit the recorded/loaded range |
| Delete | Remove the selected Print |
| Ctrl+Z / Ctrl+Shift+Z | Undo / redo |
| Shift-drag a parameter / Alt-click | Fine adjustment / reset to default |

### 构建与开发记录

使用 CMake 3.22+、C++17、JUCE 8.0.15；Windows 使用 Visual Studio 2022 C++ 工具，Mac 使用 Xcode 命令行工具。ARA SDK 通过 CMake 的固定版本配置获取，也可指定本地 SDK。

```powershell
.\build-windows.ps1 -BuildDir D:\Codex\Temp\SpectrumTag-Windows
.\build-ara-windows.ps1 -BuildDir D:\Codex\Temp\SpectrumTag-ARA -AraSdkDir D:\Dependencies\ARA_SDK
.\build-standalone-windows.ps1 -BuildDir D:\Codex\Temp\SpectrumTag-Standalone -RuntimeDir D:\Dependencies\SpectrumTagRuntime
```

Windows 独立版的 RuntimeDir 需要 `ffmpeg.exe`、`COPYING.GPLv3` 和 `FFmpeg-NOTICE.txt`，构建时嵌入 EXE。Mac 的 [Cross-platform installers](.github/workflows/cross-platform.yml) 工作流分别构建两个架构，再合并并制作 PKG。当前安装入口位于 [packaging](packaging)，旧安装脚本和旧 Standalone 实现仅作历史参考。

- [项目总览与完整开发过程](PROJECT_OVERVIEW.md)
- [逐版更新](CHANGELOG.md)
- [Windows 架构与历史交接](docs/WINDOWS_HANDOFF.md)
- [Mac / AU 历史交接](docs/MAC_AU_HANDOFF.md)

Build with CMake 3.22+, C++17 and JUCE 8.0.15, using Visual Studio 2022 on Windows or Xcode command-line tools on Mac. See the Windows commands above. The standalone Windows codec input contains `ffmpeg.exe`, `COPYING.GPLv3` and `FFmpeg-NOTICE.txt`; it is embedded at build time. [Cross-platform installers](.github/workflows/cross-platform.yml) builds both Mac architectures and merges them into a PKG. Current packaging scripts are in [packaging](packaging).

See the [development overview](PROJECT_OVERVIEW.md), [changelog](CHANGELOG.md), [Windows handoff](docs/WINDOWS_HANDOFF.md) and [Mac/AU handoff](docs/MAC_AU_HANDOFF.md) for implementation details and historical validation boundaries.

## 署名与许可 / Credits and license

原项目与资源 / Original project and assets: **iisaacbeats.cn / sweetorange1**. Fork development: **Ziqing-Gu**.

项目许可保持 **GNU Affero General Public License v3.0 (AGPL-3.0)**，以 [LICENSE](https://github.com/Ziqing-Gu/Spectrum-Tag-Private/blob/654cfb47cee71aa8046a392fd77f3533f8e273b7/LICENSE) 完整文本为准。第三方依赖及媒体编解码器保留各自许可与声明，随相应安装组件提供。

The project remains under **GNU Affero General Public License v3.0 (AGPL-3.0)**; see [LICENSE](https://github.com/Ziqing-Gu/Spectrum-Tag-Private/blob/654cfb47cee71aa8046a392fd77f3533f8e273b7/LICENSE) for the full terms. Third-party dependencies and media codecs retain their own licenses and notices, supplied with the corresponding components.

© iisaacbeats.cn. 原作者署名与版权信息保留。 / Original author attribution and copyright notices retained.

## 最新版本下载与安装说明 / Downloads and installation

**2.0.10 · 2026-11-18**　[最新版 / Latest release](https://github.com/Ziqing-Gu/Spectrum-Tag-Private/releases/latest)　[2.0.10](https://github.com/Ziqing-Gu/Spectrum-Tag-Private/releases/tag/v2.0.10)

| 文件 / File | 适用范围 / Contents |
| --- | --- |
| [Windows 安装向导 / Windows Setup](https://github.com/Ziqing-Gu/Spectrum-Tag-Private/releases/download/v2.0.10/SpectrumTag-2.0.10-Windows-Setup.exe) | Windows 10/11 x64 · VST3、ARA VST3、独立程序 / standalone |
| [macOS 通用安装包 / Universal PKG](https://github.com/Ziqing-Gu/Spectrum-Tag-Private/releases/download/v2.0.10/SpectrumTag-2.0.10-macOS.pkg) | macOS 15+ · Intel + Apple Silicon · VST3、AU（实时与 ARA / realtime and ARA）、独立 App / standalone |
| [安装说明（中文 TXT）](https://github.com/Ziqing-Gu/Spectrum-Tag-Private/releases/download/v2.0.10/default.txt) | Windows 与 macOS 的安装、扫描与排错 |
| [Installation Guide (English TXT)](https://github.com/Ziqing-Gu/Spectrum-Tag-Private/releases/download/v2.0.10/Installation-Guide.txt) | Installation, plug-in scanning and troubleshooting for both platforms |
| [完整安装包与说明 / Both installers and guides (ZIP)](https://github.com/Ziqing-Gu/Spectrum-Tag-Private/releases/download/v2.0.10/SpectrumTag-2.0.10-Installers.zip) | 保留中文文件名「安装说明.txt」/ includes the original Chinese guide filename |

GitHub 页面底部的 Source code 压缩包用于编译源码。安装时请使用上表中的 Setup 或 PKG。Use Setup or PKG to install. GitHub’s Source code archives are for building from source.

- **Windows**：运行 `SpectrumTag-2.0.10-Windows-Setup.exe` 安装向导。VST3 安装到系统标准目录，独立程序目录可选择；包含所需微软 x64 运行库和媒体编解码器。
- **macOS**：运行 `SpectrumTag-2.0.10-macOS.pkg`，安装到标准的 Applications / Audio Plug-Ins 目录，App 内含双架构媒体依赖，无需另装 Homebrew 或 FFmpeg。
- 当前没有 Windows 发布者签名或 Apple Developer ID 签名/公证。macOS bundle 使用 ad-hoc 签名。系统提示、重扫插件、旧版本冲突、Logic ARA/Rosetta 和字体等问题均见安装说明。
- ARA 需要宿主支持及正确的扩展入口。OnlyARA 标记不能保证所有宿主都从普通插入菜单隐藏它。自动化构建/验证通过也不代表所有 DAW 工程都已实测。

- **Windows:** `SpectrumTag-2.0.10-Windows-Setup.exe` is an installation wizard. It uses the standard VST3 location, allows a standalone destination, and includes the required Microsoft x64 runtime and media codec.
- **macOS:** `SpectrumTag-2.0.10-macOS.pkg` installs to standard Applications / Audio Plug-Ins locations. Bundled media dependencies require neither Homebrew nor a separate FFmpeg installation.
- There is no Windows publisher signature or Apple Developer ID signing/notarization. Mac bundles are ad-hoc signed. The guides cover system prompts, rescanning, duplicate versions, Logic ARA/Rosetta and fonts.
- ARA requires a compatible host and the correct extension entry point. OnlyARA metadata cannot guarantee hiding from every host's ordinary insert menu. Automated validation is distinct from testing real projects in every DAW.

## 逐版本更新 / Version history

| 版本 / Version | 中文 | English |
| --- | --- | --- |
| 2.0.10 | 实时 VST3、AU 和 ARA 加入独立版两种编辑模式。参数调整可预测预览，点击打印才应用，渲染期间可取消。ARA 撤销和重做保留事件选择，新实例默认 Mel (RX) 和 20 kHz。 | Realtime VST3, AU and ARA gain the standalone’s two editing modes. Changes preview immediately and commit on Print. Renders can be cancelled. ARA undo and redo preserve selection. New instances default to Mel (RX) and 20 kHz. |
| 2.0.9 · Stable | 子青版独立程序。图片模式松手恢复黑白或灰度图，图案模式松手保留预测。在空白处右键可居中显示原图，隐藏后保留参数。 | User-designated Stable standalone. Releasing a control restores the source artwork in image mode and retains the prediction in Print mode. Right-clicking empty space centres the artwork. Hiding it retains its settings. |
| 2.0.8 | 独立版切换图像风格时先显示转换后的原图。Logo 显示黑白，复杂图像显示连续灰度，调节控件时开始预测。 | Switching styles in the standalone first shows the converted source in binary black and white for Logo or continuous grayscale for Complex. Adjusting a control starts the prediction. |
| 2.0.7 | 独立版区分拖动图案与拖动调出的原图。前者移动该 Print，后者可在新位置再印一份，原处保留。 | The standalone distinguishes dragging a Print to move it from dragging its recalled source to stamp a separate copy at another position. |
| 2.0.6 | 独立版在移动或替换图案时显示底下原有的频谱。Logo 支持预测预览，打开图片会记住上次使用的文件夹。 | The standalone reveals the underlying spectrum when moving or replacing a Print. Logo gains a spectrum prediction and the image chooser remembers its folder. |
| 2.0.5 | 独立版调整已印图案时保留草稿。确认打印才渲染，可取消进行中的编辑，连续放置多个图案时保留旧图。 | Editing a printed pattern in the standalone creates a draft. Print commits it, cancellation restores the previous result, and earlier stamps stay visible during repeated placements. |
| 2.0.4 | 子青版独立程序加入直接在频谱上预测打印效果的预览，覆盖灰度对比、亮度、保留音量及噪声补偿。 | The Ziqing standalone previews the estimated print directly on the spectrum for grayscale contrast, brightness, retained amplitude and Noise Fill. |
| 2.0.3 | 上游将两种风格命名为 Logo 和复杂图像，隐藏与所选风格无关的控件，更新阈值名称与悬停说明。 | Upstream named the styles Logo and Complex image, hid controls unused by the selected style, and revised threshold labels and tooltips. |
| 2.0.2 | 上游增加按像素灰度衰减的复杂图像模式。载入彩色图像时自动拟合对比度和亮度，打印参数可随编辑保存。 | Upstream added per-pixel grayscale attenuation, automatic contrast and brightness fitting for colour images, and storage of the settings with each Print. |
| 2.0.1 | 上游区分未打印、已打印和聚焦对象，增加复制粘贴与图案导航，更新图片下方的打印按钮。 | Upstream introduced unprinted, printed and focused object states, copy and paste, Print navigation, and an updated Print button below the image. |
| 2.0.0 | 上游统一按钮样式，更新中文字体与独立版播放图标，将清空操作移到垃圾桶图标。 | Upstream unified button styling, updated Chinese typography and the standalone transport icon, and moved Clear to the trash icon. |
| 1.4.28 · Stable | 上一次跨平台公开安装包。频谱视频支持 4:3、3:4、16:9、9:16 四种画幅。 | Previous public cross-platform release. Spectrum video export added 4:3, 3:4, 16:9 and 9:16 aspect ratios. |

[完整开发记录 / Development changelog](CHANGELOG.md)

## 历史版本下载 / Previous releases

[历史安装包 / Earlier installers](https://github.com/Ziqing-Gu/Spectrum-Tag-Private/releases)
