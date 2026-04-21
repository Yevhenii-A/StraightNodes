<img width="1920" height="1080" alt="2026-04-22_00-48" src="https://github.com/user-attachments/assets/19d47370-befc-4cae-9795-ad199de32e2d" />
<img width="1920" height="1080" alt="2026-04-22_00-49" src="https://github.com/user-attachments/assets/7548e3e9-18c4-4ab9-a353-03c7a9dbe14f" />
# Straight Nodes

**by Yevhenii / Silent Symmetry Team**

Replaces Blueprint's default curved (Bézier) wires with clean **90-degree right-angle routed** wires — no more spaghetti.

---

## Features

- **Manhattan routing** — two 90° bends at the midpoint X of each connection
- **Subway routing** — short horizontal stub extruded from each pin before turning, ideal for dense graphs
- **Full Blueprint compatibility** — Alt+LMB wire cutting, hover detection, and exec bubble animations all work as expected
- **Per-project settings** — configure routing style and stub length in *Project Settings → Plugins → Straight Nodes*
- **Zero runtime cost** — Editor-only module, ships nothing to your game build

---

## Compatibility

| Engine Version | Status |
|---|---|
| UE 5.7 | ✅ Supported |

**Platforms:** Windows, Linux, macOS (Editor only)

---

## Installation

> ⚠️ This plugin ships as **source code** and requires a one-time compilation.
> Unreal handles this automatically — it takes about 30 seconds.

### Requirements

- **Visual Studio 2022** with **"Game development with C++"** workload
  → Download: https://visualstudio.microsoft.com/
  → During install, select: **Desktop development with C++** and **Game development with C++**
- Unreal Engine 5.7

### Steps

**1. Copy the plugin folder into your project**

```
YourProject/
├── Content/
├── Plugins/          ← create this folder if it doesn't exist
│   └── StraightNodes/   ← place the plugin folder here
└── YourProject.uproject
```

**2. Generate Visual Studio project files**

Right-click your `.uproject` file → **"Generate Visual Studio project files"**

> If this option is missing, run the following file:
> `C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealVersionSelector.exe`

**3. Open your project**

Open `YourProject.uproject` as usual. Unreal will detect the plugin and ask:
> *"The following modules are missing or built with a different engine version. Rebuild now?"*

Click **Yes**. Done.

---

## Settings

**Edit → Project Settings → Plugins → Straight Nodes**

| Setting | Default | Description |
|---|---|---|
| Routing Style | Manhattan | Manhattan = 2 bends at midpoint. Subway = stub + turn. |
| Subway Stub Length | 32 px | Horizontal stub length (Subway style only). |

---

## Troubleshooting

**"Could not be compiled. Try rebuilding from source manually."**
Open the `.sln` in Visual Studio → Build → Build Solution. Check the Output panel for the exact error.

**"Generate Visual Studio project files" is missing from the right-click menu**
Run as Administrator: `C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealVersionSelector.exe /register`

**Wires look the same**
Go to Edit → Plugins → search "Straight Nodes" → make sure it's enabled, then restart the editor.

---

## Support

Leave a comment on the product page or reach out to **Silent Symmetry Team**.

---

## Changelog

### 1.0.0
- Initial release
- Manhattan and Subway routing styles
- Alt+LMB wire cutting
- Hover and tooltip support
