# MochiPocket 🍡

MochiPocket is a cozy, offline-first desktop personal finance manager built in Modern C++ using the Raylib GUI rendering library. 

Instead of typing numbers into clinical corporate spreadsheets, MochiPocket combines budget tracking with a cute, relaxing desk environment. You can manage your ledger, set savings targets, track categories, and collect animated Mochi companions as rewards for staying under budget and keeping streaks.

## 🎨 Features
* **Offline-First**: All data is stored locally in `data/wallet.json`. No accounts, server connections, or cloud databases.
* **Cozy Theme**: Mascots and interactive elements (swaying bonsai, steaming tea cup, clouds) are rendered procedurally using Raylib shapes and vector math. No bulky image assets are needed.
* **Ledger Entries**: Add, filter, search, and delete transactions.
* **Budget Allocations**: Set category spending limits. Overspending warns you visually and changes your Mochi's mood.
* **Savings Goals**: Plan long-term projects and deposit/withdraw funds.
* **Sticker Book**: Track unlocked companions (Original, Matcha, Strawberry, Sakura, Golden) and customize your desk mascot.

## 🏗️ Project Architecture
The codebase follows standard modular organization:
* **UI & Rendering**: [main.cpp](src/main.cpp) and [UIComponents.cpp](src/UIComponents.cpp) handle mouse events, forms, progress bars, and rendering.
* **Core Logic**: Coordinates metrics, streak dates, category totals, and rules ([FinanceManager.cpp](src/FinanceManager.cpp), [MochiSystem.cpp](src/MochiSystem.cpp), [Analytics.cpp](src/Analytics.cpp)).
* **Persistence**: [FileManager.cpp](src/FileManager.cpp) serializes/deserializes states using a line-based state machine parser to read and write JSON.

## ⚙️ How to Compile & Run (Windows)
1. Ensure MinGW GCC (from Code::Blocks) is installed at `C:\Program Files\CodeBlocks\MinGW\bin\g++.exe`.
2. Open PowerShell in the root directory:
   ```powershell
   powershell.exe -ExecutionPolicy Bypass -File .\compile.ps1
   ```
3. Launch the standalone program:
   ```powershell
   .\MochiPocket.exe
   ```
