# GrimYield



\# GrimYield (formerly "Slash")



\*\*GrimYield\*\* is an indie dark fantasy action-adventure game developed in Unreal Engine 5.6 under the moniker WolfWail. Set in a bleak, atmospheric world, the game follows a Norse Viking warrior who ventures deep into a haunted forest to hunt down demonic monsters and claim their souls.



\*(Insert a link to a YouTube gameplay overview or a high-quality GIF of your game here)\*



\## Technical Overview



This repository serves as a programming portfolio showcasing the core C++ architecture and systems engineering behind the game. To maintain a lightweight and accessible repository, heavy engine assets (such as 3D meshes, animations, and uncompressed audio) have been deliberately excluded.



\### Core C++ Mechanics \& Systems







\## Repository Structure



\* `Config/`: Stores the core initialization files (`DefaultEngine.ini`, `DefaultGame.ini`, etc.) defining project settings and input mappings.

\* `Source/`: Contains all custom C++ classes (BaseCharacter, Protagonist, GrimNPC, Enemy, etc.) handling state machines, combat calculations, and gameplay logic.

\* `Slash.uproject`: The core Unreal Engine project descriptor (maintaining the original working title "Slash" to preserve internal engine dependencies).



\## How to Review the Code



This is a code-only repository optimized for technical review. Because heavy engine assets (meshes, textures, audio, and blueprints) are excluded to keep the repository lightweight, launching the `.uproject` in the Unreal Editor will result in missing asset errors.

To review the systems engineering:

1\. Navigate to the `Source/` directory directly on GitHub to read the C++ classes.

2\. Alternatively, clone the repository locally, right-click `Slash.uproject` to \*\*Generate Visual Studio project files\*\*, and open `Slash.sln` to explore the architecture and class hierarchy directly within your IDE.

