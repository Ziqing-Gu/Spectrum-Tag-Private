# SpectrumTag 1.4.28 — cross-platform installers

2026-11-09. User-designated Stable. New Plan B completed before publishing: source `b762eb9ef12a5010d16cc9cd8107d6bad320db06`, 479 files, 74,914,433 bytes, 17 key hashes matched. The backup is frozen; subsequent work only cites its completion record.

## Scope and inherited evidence

The 1.4.28 Windows standalone EXE and video/UI validation are reused from [WINDOWS_1.4.28.md](WINDOWS_1.4.28.md); its SHA-256 is `D1A9D74C3A1D7A86F820C8AE45543A79938196BC56E8114B5A3109216012FA1F`. New feature: selectable 4:3, 3:4, 16:9, 9:16 spectrum-video canvases. Existing plugin audio processing is unchanged. Version-matched Windows realtime/ARA VST3, Mac native VST3/ARA/AU/standalone and installation packages are built under the user's new request.

The user requests a Windows Setup and Universal Mac PKG, with a Chinese-named Chinese TXT and an English TXT. `docs/安装说明.txt` and `docs/INSTALL-EN.txt` are bundled with the products; the final lean delivery uses `安装说明.txt`, `Installation-Guide.txt`, `Win/` and `Mac/`. Source licensing and third-party notices remain unchanged.

GitHub repository: `Ziqing-Gu/Spectrum-Tag-Private` (the existing user-named public Fork; visibility is unchanged). Upstream PR #3 already merged. An incremental PR is authorized after packaging; there is no authority to merge it automatically.

## Current validation

Windows plugin builds and Mac/installation validation are in progress. Run IDs, result boundaries and final hashes will be recorded after actual completion. Do not interpret this document as a completed build.

Local evidence: `D:\Codex\Temp\SpectrumTag-1.4.28`.

Final delivery: `D:\Codex\Outputs\SpectrumTag-1.4.28-Installers` and a real-file copy at `C:\Users\86189\Desktop\SpectrumTag-1.4.28-Installers`.

No commercial Windows publisher signature or Apple Developer ID/notarization is provided. Mac bundles retain ad-hoc signing. Automated validation is separate from actual DAW/project field acceptance. The previous 1.4.27 products and frozen backups remain intact.
