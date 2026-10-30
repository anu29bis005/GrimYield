# GrimYield



\# GrimYield (formerly "Slash")



\*\*GrimYield\*\* is an indie dark fantasy action-adventure game developed in Unreal Engine 5.6 under the moniker WolfWail. Set in a bleak, atmospheric world, the game follows a Norse Viking warrior who ventures deep into a haunted forest to hunt down demonic monsters and claim their souls.



\*(Insert a link to a YouTube gameplay overview or a high-quality GIF of your game here)\*



\## Technical Overview



This repository serves as a programming portfolio showcasing the core C++ architecture and systems engineering behind the game. To maintain a lightweight and accessible repository, heavy engine assets (such as 3D meshes, animations, and uncompressed audio) have been deliberately excluded.



\### Core C++ Mechanics \& Systems



\* Pre-Compilation & UI State Management: Engineered a custom HUD initialization sequence that verifies shader compilation, sequences splash screens, and transitions seamlessly into the Main Menu.

\* Advanced Player Locomotion & Combat: Built state-driven movement mechanics encompassing multi-directional navigation, combat dodging, self-healing, and precise fall damage calculations that trigger hard-landing animations upon exceeding height thresholds.

* Dynamic Inventory & Overlap Logic: Programmed automatic item acquisition for health potions, spawned chest treasures, and defeated enemy souls using overlap events, alongside input-driven weapon equipping.

\* Quest & Objective Engineering: Developed a time-sensitive quest acceptance system featuring a custom UI prompt (holding TAB for 2 seconds within a 15-second window), paired with dynamic objective tracking and an interactive Journal system to read and store discovered notes.

\* World Interaction Systems: Programmed robust interaction logic for a diverse range of environmental actors, including ladders, levers, gates, letter notes, old machines, and chests.

\* Gated Event Logic & Side Missions: Implemented complex dependency chains for optional side quests, such as a Blacksmith rescue sequence that dynamically unlocks specific environmental interactable (like the Crate Platform) only when prerequisite steps are fulfilled.

\* Audio & Cinematic Integration: Engineered trigger systems to manage environmental audio, voice lines, and custom Unreal Engine cutscenes without interrupting the main gameplay loop.



\## Repository Structure



\* `Config/`: Stores the core initialization files (`DefaultEngine.ini`, `DefaultGame.ini`, etc.) defining project settings and input mappings.

\* `Source/`: Contains all custom C++ classes (BaseCharacter, Protagonist, GrimNPC, Enemy, etc.) handling state machines, combat calculations, and gameplay logic.

\* `Slash.uproject`: The core Unreal Engine project descriptor (maintaining the original working title "Slash" to preserve internal engine dependencies).



\## How to Review the Code



This is a code-only repository optimized for technical review. Because heavy engine assets (meshes, textures, audio, and blueprints) are excluded to keep the repository lightweight, launching the `.uproject` in the Unreal Editor will result in missing asset errors.

To review the systems engineering:

1\. Navigate to the `Source/` directory directly on GitHub to read the C++ classes.

2\. Alternatively, clone the repository locally, right-click `Slash.uproject` to \*\*Generate Visual Studio project files\*\*, and open `Slash.sln` to explore the architecture and class hierarchy directly within your IDE.

