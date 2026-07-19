# MochiPocket 🍡

MochiPocket is a cozy, offline-first desktop personal finance manager built in Modern C++ using the Raylib GUI rendering library. 
<p align="center">
   <img width="988" height="740" alt="Screenshot 2026-07-19 203307" src="https://github.com/user-attachments/assets/1d868822-8b32-467e-8ab0-e8350f005a6e" />
</p>
Manage expenses, budgets, savings goals, and analytics while collecting animated Mochi companions.

## 🛠 Tech Stack

- **Language:** Modern C++17
- **Graphics:** Raylib
- **Storage:** JSON
- **Concepts:** Object-Oriented Programming, STL, File I/O
- **Build:** MinGW GCC

## 🎨 Features
* **Offline-First**: All data is stored locally in `data/wallet.json`. No accounts, server connections, or cloud databases.
* **Cozy Theme**: Mascots and interactive elements (swaying bonsai, steaming tea cup, clouds) are rendered procedurally using Raylib shapes and vector math. No bulky image assets are needed.
* **Analytics Dashboard**: View spending summaries, category distributions, savings progress, and recent financial activity.
* **Ledger Entries**: Add, filter, search, and delete transactions.
* **Budget Allocations**: Set category spending limits. Overspending warns you visually and changes your Mochi's mood.
* **Savings Goals**: Plan long-term projects and deposit/withdraw funds.
* **Sticker Book**: Track unlocked companions (Original, Matcha, Strawberry, Sakura, Golden) and customize your desk mascot.

## 🏗️ Project Architecture
The codebase follows standard modular organization:
* **UI & Rendering**: [main.cpp](src/main.cpp) and [UIComponents.cpp](src/UIComponents.cpp) handle mouse events, forms, progress bars, and rendering.
* **Core Logic**: Coordinates metrics, streak dates, category totals, and rules ([FinanceManager.cpp](src/FinanceManager.cpp), [MochiSystem.cpp](src/MochiSystem.cpp), [Analytics.cpp](src/Analytics.cpp)).
* **Persistence**: [FileManager.cpp](src/FileManager.cpp) serializes/deserializes states using a line-based state machine parser to read and write JSON.

## 📷 More Screenshots
<p align = "center">
<img width="25%"  alt="Screenshot 2026-07-19 203317" src="https://github.com/user-attachments/assets/7e1abb19-68dd-4763-996b-82cfd3d67515" />
<img width="25%"  alt="Screenshot 2026-07-19 203324" src="https://github.com/user-attachments/assets/a0f91beb-1260-4cc1-ad54-a8251dcc3d9f" />
<img width="25%"  alt="Screenshot 2026-07-19 203333" src="https://github.com/user-attachments/assets/8174d5f7-a419-4195-8d48-b8432892f479" />
<img width="25%"  alt="Screenshot 2026-07-19 203340" src="https://github.com/user-attachments/assets/298198ca-188c-4e81-80f0-5e2f983239b7" />
</p>

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
